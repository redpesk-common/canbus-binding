/*
 * Copyright (C) 2015, 2016 "IoT.bzh"
 * Author "Romain Forlot" <romain.forlot@iot.bzh>
 * Author "Loic Collignon" <loic.collignon@iot.bzh>
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *	 http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "low-can-hat.hpp"

#include <map>
#include <queue>
#include <mutex>
#include <vector>
#include <thread>
#include <json-c/json.h>
#include <systemd/sd-event.h>

#include "openxc.pb.h"
#include "application.hpp"
#include "../can/can-bus.hpp"
#include "../can/can-signals.hpp"
#include "../can/can-message.hpp"
#include "../utils/signals.hpp"
#include "../diagnostic/diagnostic-message.hpp"
#include "../utils/openxc-utils.hpp"


///******************************************************************************
///
///		SystemD event loop Callbacks
///
///*******************************************************************************/

void on_no_clients(std::shared_ptr<low_can_subscription_t> can_subscription, uint32_t pid, std::map<int, std::shared_ptr<low_can_subscription_t> >& s)
{
	if( ! can_subscription->get_diagnostic_message().empty() && can_subscription->get_diagnostic_message(pid) != nullptr)
	{
		DiagnosticRequest diag_req = can_subscription->get_diagnostic_message(pid)->build_diagnostic_request();
		active_diagnostic_request_t* adr = application_t::instance().get_diagnostic_manager().find_recurring_request(diag_req);
		if( adr != nullptr)
			application_t::instance().get_diagnostic_manager().cleanup_request(adr, true);
	}

	on_no_clients(can_subscription, s);
}

void on_no_clients(std::shared_ptr<low_can_subscription_t> can_subscription, std::map<int, std::shared_ptr<low_can_subscription_t> >& s)
{
	auto it = s.find(can_subscription->get_index());
	s.erase(it);
}

static void push_n_notify(const can_message_t& cm)
{
	can_bus_t& cbm = application_t::instance().get_can_bus_manager();
	{
		std::lock_guard<std::mutex> can_message_lock(cbm.get_can_message_mutex());
	 	cbm.push_new_can_message(cm);
	}
	cbm.get_new_can_message_cv().notify_one();
}

int read_message(sd_event_source *event_source, int fd, uint32_t revents, void *userdata)
{
	low_can_subscription_t* can_subscription = (low_can_subscription_t*)userdata;
	if ((revents & EPOLLIN) != 0)
	{
		can_message_t cm;
		utils::socketcan_bcm_t& s = can_subscription->get_socket();
		s >> cm;

		// Sure we got a valid CAN message ?
		if(! cm.get_id() == 0 && ! cm.get_length() == 0)
			{push_n_notify(cm);}
	}

	// check if error or hangup
	if ((revents & (EPOLLERR|EPOLLRDHUP|EPOLLHUP)) != 0)
	{
		sd_event_source_unref(event_source);
		can_subscription->get_socket().close();
	}
	return 0;
}

///******************************************************************************
///
///		Subscription and unsubscription
///
///*******************************************************************************/

static int make_subscription_unsubscription(struct afb_req request, std::shared_ptr<low_can_subscription_t>& can_subscription, std::map<int, std::shared_ptr<low_can_subscription_t> >& s, bool subscribe)
{
	/* Make the subscription or unsubscription to the event */
	if (((subscribe ? afb_req_subscribe : afb_req_unsubscribe)(request, s[can_subscription->get_index()]->get_event())) < 0)
	{
		AFB_ERROR("Operation goes wrong for signal: %s", can_subscription->get_name().c_str());
		return -1;
	}
	return 0;
}

static int create_event_handle(std::shared_ptr<low_can_subscription_t>& can_subscription, std::map<int, std::shared_ptr<low_can_subscription_t> >& s)
{
	int sub_index = can_subscription->get_index();
	can_subscription->set_event(afb_daemon_make_event(can_subscription->get_name().c_str()));
	s[sub_index] = can_subscription;
	if (!afb_event_is_valid(s[sub_index]->get_event()))
	{
		AFB_ERROR("Can't create an event for %s, something goes wrong.", can_subscription->get_name().c_str());
		return -1;
	}
	return 0;
}

/// @brief Will determine if it is needed or not to create the event handle and checks it to be sure that
/// we got a valid afb_event to get subscribe or unsubscribe. Then launch the subscription or unsubscription
/// against the application framework using that event handle.
static int subscribe_unsubscribe_signal(struct afb_req request, bool subscribe, std::shared_ptr<low_can_subscription_t>& can_subscription, std::map<int, std::shared_ptr<low_can_subscription_t> >& s)
{
	int ret = -1;
	int sub_index = can_subscription->get_index();

	if (can_subscription && s.find(sub_index) != s.end())
	{
		if (!afb_event_is_valid(s[sub_index]->get_event()) && !subscribe)
		{
			AFB_NOTICE("Event isn't valid, no need to unsubscribed.");
			ret = -1;
		}
		ret = 0;
	}
	else
	{
		/* Event doesn't exist , so let's create it */
		can_subscription->set_event({nullptr, nullptr});
		s[sub_index] = can_subscription;
		ret = create_event_handle(can_subscription, s);
	}

	// Check whether or not the event handler has been correctly created and
	// make the subscription/unsubscription operation is so.
	if (ret < 0)
		return ret;
	return make_subscription_unsubscription(request, can_subscription, s, subscribe);
}

static int add_to_event_loop(std::shared_ptr<low_can_subscription_t>& can_subscription)
{
		struct sd_event_source* event_source = nullptr;
		return ( sd_event_add_io(afb_daemon_get_event_loop(),
			&event_source,
			can_subscription->get_socket().socket(),
			EPOLLIN,
			read_message,
			can_subscription.get()));
}

static int subscribe_unsubscribe_diagnostic_messages(struct afb_req request, bool subscribe, std::vector<std::shared_ptr<diagnostic_message_t> > diagnostic_messages, struct event_filter_t& event_filter, std::map<int, std::shared_ptr<low_can_subscription_t> >& s)
{
	int rets = 0;
	application_t& app = application_t::instance();
	diagnostic_manager_t& diag_m = app.get_diagnostic_manager();

	for(const auto& sig : diagnostic_messages)
	{
		DiagnosticRequest* diag_req = new DiagnosticRequest(sig->build_diagnostic_request());
		event_filter.frequency = std::isnan(event_filter.frequency) ? sig->get_frequency() : event_filter.frequency;
		std::shared_ptr<low_can_subscription_t> can_subscription;

		auto it =  std::find_if(s.begin(), s.end(), [&sig](std::pair<int, std::shared_ptr<low_can_subscription_t> > sub){ return (! sub.second->get_diagnostic_message().empty());});
		can_subscription = it != s.end() ?
			it->second : 
			std::make_shared<low_can_subscription_t>(low_can_subscription_t(event_filter));
		// If the requested diagnostic message isn't supported by the car then unsubcribe it
		// no matter what we want, worse case will be a fail unsubscription but at least we don't
		// poll a PID for nothing.
		//TODO: Adding callback requesting ignition status:	diag_req, sig.c_str(), false, diagnostic_message_t::decode_obd2_response, diagnostic_message_t::check_ignition_status, frequency);
		if(sig->get_supported() && subscribe)
		{
			diag_m.add_recurring_request(diag_req, sig->get_name().c_str(), false, sig->get_decoder(), sig->get_callback(), event_filter.frequency);
			if(can_subscription->create_rx_filter(sig) < 0)
				{return -1;}
			AFB_DEBUG("Signal: %s subscribed", sig->get_name().c_str());
			if(it == s.end() && add_to_event_loop(can_subscription) < 0)
			{
				diag_m.cleanup_request(
					diag_m.find_recurring_request(*diag_req), true);
				AFB_WARNING("signal: %s isn't supported. Canceling operation.",  sig->get_name().c_str());
				return -1;
			}
		}
		else
		{
			if(sig->get_supported())
			{AFB_DEBUG("%s cancelled due to unsubscribe", sig->get_name().c_str());}
			else
			{
				AFB_WARNING("signal: %s isn't supported. Canceling operation.", sig->get_name().c_str());
				return -1;
			}
		}
		int ret = subscribe_unsubscribe_signal(request, subscribe, can_subscription, s);
		if(ret < 0)
			return ret;

		rets++;
	}

	return rets;
}

// TODO: Create separate subscrition object if event_filter isn't the same.
static int subscribe_unsubscribe_can_signals(struct afb_req request, bool subscribe, std::vector<std::shared_ptr<can_signal_t> > can_signals, struct event_filter_t& event_filter, std::map<int, std::shared_ptr<low_can_subscription_t> >& s)
{
	int rets = 0;
	for(const auto& sig: can_signals)
	{
		auto it =  std::find_if(s.begin(), s.end(), [&sig](std::pair<int, std::shared_ptr<low_can_subscription_t> > sub){ return sub.second->get_can_signal() == sig; });
		std::shared_ptr<low_can_subscription_t> can_subscription;
		if(it != s.end())
		{
			 can_subscription = it->second;
		}
		else
		{
			 can_subscription = std::make_shared<low_can_subscription_t>(low_can_subscription_t(event_filter));
			if(can_subscription->create_rx_filter(sig) < 0)
				{return -1;}
			if(add_to_event_loop(can_subscription) < 0)
				{return -1;}
		}

		if(subscribe_unsubscribe_signal(request, subscribe, can_subscription, s) < 0)
			{return -1;}

		rets++;
		AFB_DEBUG("signal: %s subscribed", sig->get_name().c_str());
	}
	return rets;
}

///
/// @brief subscribe to all signals in the vector signals
///
/// @param[in] afb_req request : contain original request use to subscribe or unsubscribe
/// @param[in] subscribe boolean value used to chose between a subscription operation or an unsubscription
/// @param[in] signals -  struct containing vectors with can_signal_t and diagnostic_messages to subscribe
///
/// @return Number of correctly subscribed signal
///
static int subscribe_unsubscribe_signals(struct afb_req request, bool subscribe, const struct utils::signals_found& signals, struct event_filter_t& event_filter)
{
	int rets = 0;
	utils::signals_manager_t& sm = utils::signals_manager_t::instance();

	std::lock_guard<std::mutex> subscribed_signals_lock(sm.get_subscribed_signals_mutex());
	std::map<int, std::shared_ptr<low_can_subscription_t> >& s = sm.get_subscribed_signals();

	rets += subscribe_unsubscribe_diagnostic_messages(request, subscribe, signals.diagnostic_messages, event_filter, s);
	rets += subscribe_unsubscribe_can_signals(request, subscribe, signals.can_signals, event_filter, s);

	return rets;
}

static int one_subscribe_unsubscribe(struct afb_req request, bool subscribe, const std::string& tag, json_object* args)
{
	int ret = 0;
	struct event_filter_t event_filter;
	struct json_object  *filter, *obj;
	struct utils::signals_found sf;

	// computes the filter
	if (json_object_object_get_ex(args, "filter", &filter))
	{
		if (json_object_object_get_ex(filter, "frequency", &obj)
		&& (json_object_is_type(obj, json_type_double) || json_object_is_type(obj, json_type_int)))
		{
			event_filter.frequency = (float)json_object_get_double(obj);
			ret += 1;
		}
		if (json_object_object_get_ex(filter, "min", &obj)
		&& (json_object_is_type(obj, json_type_double) || json_object_is_type(obj, json_type_int)))
		{
			event_filter.min = (float)json_object_get_double(obj);
			ret += 2;
		}
		if (json_object_object_get_ex(filter, "max", &obj)
		&& (json_object_is_type(obj, json_type_double) || json_object_is_type(obj, json_type_int)))
		{
			event_filter.max = (float)json_object_get_double(obj);
			ret += 4;
		}
	}

	// subscribe or unsubscribe
	openxc_DynamicField search_key = build_DynamicField(tag);
	sf = utils::signals_manager_t::instance().find_signals(search_key);
	if (sf.can_signals.empty() && sf.diagnostic_messages.empty())
		AFB_NOTICE("No signal(s) found for %s.", tag.c_str());
	else
		ret = subscribe_unsubscribe_signals(request, subscribe, sf, event_filter);

	return ret;
}

static void do_subscribe_unsubscribe(struct afb_req request, bool subscribe)
{
	int i, n, rc, rc2;
	struct json_object *args, *event, *x;

	args = afb_req_json(request);
	if (args == NULL || !json_object_object_get_ex(args, "event", &event))
	{
		rc = one_subscribe_unsubscribe(request, subscribe, "*", args);
	}
	else if (json_object_get_type(event) != json_type_array)
	{
		rc = one_subscribe_unsubscribe(request, subscribe, json_object_get_string(event), args);
	}
	else
	{
		rc = 0;
		n = json_object_array_length(event);
		for (i = 0 ; i < n ; i++)
		{
			x = json_object_array_get_idx(event, i);
			rc2 = one_subscribe_unsubscribe(request, subscribe, json_object_get_string(x), args);
			if (rc >= 0)
				rc = rc2 >= 0 ? rc + rc2 : rc2;
		}
	}

	if (rc >= 0)
		afb_req_success(request, NULL, NULL);
	else
		afb_req_fail(request, "error", NULL);
}

void subscribe(struct afb_req request)
{
	do_subscribe_unsubscribe(request, true);
}

void unsubscribe(struct afb_req request)
{
	do_subscribe_unsubscribe(request, false);
}
