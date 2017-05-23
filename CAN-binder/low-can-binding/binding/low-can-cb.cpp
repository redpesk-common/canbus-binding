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
#include "configuration.hpp"
#include "../can/can-bus.hpp"
#include "../can/can-signals.hpp"
#include "../can/can-message.hpp"
#include "../utils/timer.hpp"
#include "../utils/signals.hpp"
#include "../diagnostic/diagnostic-message.hpp"
#include "../utils/openxc-utils.hpp"

extern "C"
{
	#include <afb/afb-service-itf.h>
};

void on_no_clients(std::string message)
{
	DiagnosticRequest* diag_req = configuration_t::instance().get_request_from_diagnostic_message(message);
	if(diag_req != nullptr)
	{
		active_diagnostic_request_t* adr = configuration_t::instance().get_diagnostic_manager().find_recurring_request(diag_req);
		if( adr != nullptr)
			configuration_t::instance().get_diagnostic_manager().cleanup_request(adr, true);
	}
}

static void push_n_notify(const can_message_t cm)
{
	can_bus_t& cbm = configuration_t::instance().get_can_bus_manager();
	std::lock_guard<std::mutex> can_message_lock(cbm.get_can_message_mutex());
	{ cbm.push_new_can_message(cm); }
	cbm.get_new_can_message_cv().notify_one();
}

int read_message(sd_event_source *s, int fd, uint32_t revents, void *userdata)
{
	can_message_t cm;
	can_signal_t* sig;
	diagnostic_manager_t& diag_m = configuration_t::instance().get_diagnostic_manager();

	if(userdata != nullptr)
	{
		sig = (can_signal_t*)userdata;
		utils::socketcan_bcm_t s = sig->get_socket();
		s >> cm;
	}
	else
	{
		utils::socketcan_bcm_t s = diag_m.get_socket();
		s >> cm;
	}

	push_n_notify(cm);

	/* check if error or hangup */
	if ((revents & (EPOLLERR|EPOLLRDHUP|EPOLLHUP)) != 0)
	{
		sd_event_source_unref(s);
		if(userdata != nullptr)
		{
			sig->get_socket().close();
			sig->create_rx_filter();
			NOTICE(binder_interface, "%s: Recreated RX_SETUP BCM job for signal: %s", __FUNCTION__, sig->get_name().c_str());
		}
		else
		{
			diag_m.get_socket().close();
			diag_m.cleanup_active_requests(true);
			ERROR(binder_interface, "%s: Error on diagnostic manager socket, cancelling active requests.", __FUNCTION__);
		}
		return -1;
	}

	return 0;
}


///******************************************************************************
///
///		Subscription and unsubscription
///
///*******************************************************************************/

static int make_subscription_unsubscription(struct afb_req request, const std::string& sig_name, std::map<std::string, struct afb_event>& s, bool subscribe)
{
	/* Make the subscription or unsubscription to the event */
	if (((subscribe ? afb_req_subscribe : afb_req_unsubscribe)(request, s[sig_name])) < 0)
	{
		ERROR(binder_interface, "%s: Operation goes wrong for signal: %s", __FUNCTION__, sig_name.c_str());
		return 0;
	}
	return 1;
}

static int create_event_handle(const std::string& sig_name, std::map<std::string, struct afb_event>& s)
{
	s[sig_name] = afb_daemon_make_event(binder_interface->daemon, sig_name.c_str());
	if (!afb_event_is_valid(s[sig_name]))
	{
		ERROR(binder_interface, "%s: Can't create an event for %s, something goes wrong.", __FUNCTION__, sig_name.c_str());
		return 0;
	}
	return 1;
}

/// @brief Will determine if it is needed or not to create the event handle and checks it to be sure that
/// we got a valid afb_event to get subscribe or unsubscribe. Then launch the subscription or unsubscription
/// against the application framework using that event handle.
static int subscribe_unsubscribe_signal(struct afb_req request, bool subscribe, const std::string& sig)
{
	int ret;

	utils::signals_manager_t& sm = utils::signals_manager_t::instance();

	std::lock_guard<std::mutex> subscribed_signals_lock(sm.get_subscribed_signals_mutex());
	std::map<std::string, struct afb_event>& s = sm.get_subscribed_signals();
	if (s.find(sig) != s.end())
	{
		if (!afb_event_is_valid(s[sig]) && !subscribe)
		{
			NOTICE(binder_interface, "%s: Event isn't valid, no need to unsubscribed.", __FUNCTION__);
			ret = -1;
		}
		else
		{
			// Event it isn't valid annymore, recreate it
			ret = create_event_handle(sig, s);
		}
	}
	else
	{
		/* Event doesn't exist , so let's create it */
		struct afb_event empty_event = {nullptr, nullptr};
		s[sig] = empty_event;
		ret = create_event_handle(sig, s);
	}

	// Check whether or not the event handler has been correctly created and
	// make the subscription/unsubscription operation is so.
	if (ret <= 0)
		return ret;
	return make_subscription_unsubscription(request, sig, s, subscribe);
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
static int subscribe_unsubscribe_signals(struct afb_req request, bool subscribe, const struct utils::signals_found& signals)
{
	int ret,  rets = 0;

	//TODO: Implement way to dynamically call the right function no matter
	// how much signals types we have.
	configuration_t& conf = configuration_t::instance();

	for(const auto& sig : signals.diagnostic_messages)
	{
		diagnostic_manager_t& diag_m = conf.get_diagnostic_manager();
		DiagnosticRequest* diag_req = conf.get_request_from_diagnostic_message(sig->get_name());

		// If the requested diagnostic message isn't supported by the car then unsubcribe it
		// no matter what we want, worse case will be a fail unsubscription but at least we don't
		// poll a PID for nothing.
		if(sig->get_supported() && subscribe)
		{
			float frequency = sig->get_frequency();
			diag_m.add_recurring_request(diag_req, sig->get_name().c_str(), false, sig->get_decoder(), sig->get_callback(), (float)frequency);
				//TODO: Adding callback requesting ignition status:	diag_req, sig.c_str(), false, diagnostic_message_t::decode_obd2_response, diagnostic_message_t::check_ignition_status, frequency);
		}
		else
		{
			diag_m.cleanup_request(
				diag_m.find_recurring_request(diag_req), true);
			WARNING(binder_interface, "%s: signal: %s isn't supported. Canceling operation.", __FUNCTION__, sig->get_name().c_str());
			return -1;
		}

		ret = subscribe_unsubscribe_signal(request, subscribe, sig->get_name());
		if(ret <= 0)
			return ret;
		rets++;
		DEBUG(binder_interface, "%s: Signal: %s subscribed", __FUNCTION__, sig->get_name().c_str());
	}

	for(const auto& sig: signals.can_signals)
	{
		if(sig->create_rx_filter() <= 0 && 
			subscribe_unsubscribe_signal(request, subscribe, sig->get_name()) <=0)
		{
			return -1;
		}
		struct sd_event_source* e_source;
		sd_event_add_io(afb_daemon_get_event_loop(binder_interface->daemon), &e_source, sig->get_socket().socket(), EPOLLIN, read_message, sig.get());
		rets++;
		DEBUG(binder_interface, "%s: signal: %s subscribed", __FUNCTION__, sig->get_name().c_str());
	}
	return rets;
}

static int process_args(struct afb_req request, const std::map<std::string, struct event_filter_t>& args, bool subscribe)
{
	struct utils::signals_found sf;
	int ok = 0, total = 0;

	for(const auto& sig: args)
	{
		openxc_DynamicField search_key = build_DynamicField(sig.first);
		sf = utils::signals_manager_t::instance().find_signals(search_key);
		total = (int)sf.can_signals.size() + (int)sf.diagnostic_messages.size();

		if (sf.can_signals.empty() && sf.diagnostic_messages.empty())
			NOTICE(binder_interface, "%s: No signal(s) found for %s.", __FUNCTION__, sig.first.c_str());
		else
			ok = subscribe_unsubscribe_signals(request, subscribe, sf);
	}
	NOTICE(binder_interface, "%s: Subscribed/unsubscribe correctly to %d/%d signal(s).", __FUNCTION__, ok, total);
	return ok;
}

static int parse_filter(json_object* event, struct event_filter_t* event_filter)
{
	struct json_object  *filter, *obj;
	int ret = 0;

	if (json_object_object_get_ex(event, "filter", &filter))
	{
		event_filter->frequency = -1.0;
		event_filter->min = -1.0;
		event_filter->max = -1.0;
		if (json_object_object_get_ex(filter, "frequency", &obj)
		&& json_object_get_type(obj) == json_type_double)
		{
			event_filter->frequency = json_object_get_double(obj);
			ret += 1;
		}
		if (json_object_object_get_ex(filter, "min", &obj)
		&& json_object_get_type(obj) == json_type_double)
		{
			event_filter->min = json_object_get_double(obj);
			ret += 2;
		}
		if (json_object_object_get_ex(filter, "max", &obj)
		&& json_object_get_type(obj) == json_type_double)
		{
			event_filter->max = json_object_get_double(obj);
			ret += 4;
		}
	}

	return ret;
}

static const std::map<std::string, struct event_filter_t> parse_args_from_request(struct afb_req request)
{
	int i, n;
 	std::map<std::string, struct event_filter_t> ret;
	struct json_object *args, *event, *x;
	struct event_filter_t event_filter;
	
	/* retrieve signals to subscribe */
	args = afb_req_json(request);
	if (args == NULL || !json_object_object_get_ex(args, "event", &event))
	{
		ret["*"] = event_filter;
	}
	else if (json_object_get_type(event) != json_type_array)
	{
		const std::string event_pattern = std::string(json_object_get_string(event));
		parse_filter(event, &event_filter);
		ret[event_pattern] = event_filter;
	}
	else
	{
		n = json_object_array_length(event);
		for (i = 0 ; i < n ; i++)
		{
			x = json_object_array_get_idx(event, i);
			const std::string event_pattern = std::string(json_object_get_string(x));
			parse_filter(x, &event_filter);
			ret[event_pattern] = event_filter;
		}
	}

	return ret;
}

void subscribe(struct afb_req request)
{
	bool subscribe = true;

	const std::map<std::string, struct event_filter_t> args = parse_args_from_request(request);

	if (process_args(request, args, subscribe) > 0)
		afb_req_success(request, NULL, NULL);
	else
		afb_req_fail(request, "error", NULL);
}

void unsubscribe(struct afb_req request)
{
	bool subscribe = false;
	
	const std::map<std::string, struct event_filter_t> args = parse_args_from_request(request);

	if (process_args(request, args, subscribe) > 0)
		afb_req_success(request, NULL, NULL);
	else
		afb_req_fail(request, "error", NULL);
}