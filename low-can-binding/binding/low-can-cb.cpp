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
#include "low-can-apidef.h"

#include <map>
#include <queue>
#include <mutex>
#include <vector>
#include <thread>
#include <json-c/json.h>
#include <systemd/sd-event.h>

#include "openxc.pb.h"
#include "application.hpp"
#include "../can/can-encoder.hpp"
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
	bool is_permanent_recurring_request = false;

	if( ! can_subscription->get_diagnostic_message().empty() && can_subscription->get_diagnostic_message(pid) != nullptr)
	{
		DiagnosticRequest diag_req = can_subscription->get_diagnostic_message(pid)->build_diagnostic_request();
		active_diagnostic_request_t* adr = application_t::instance().get_diagnostic_manager().find_recurring_request(diag_req);
		if( adr != nullptr)
		{
			is_permanent_recurring_request = adr->get_permanent();

			if(! is_permanent_recurring_request)
				application_t::instance().get_diagnostic_manager().cleanup_request(adr, true);
		}
	}

	if(! is_permanent_recurring_request)
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

static int make_subscription_unsubscription(struct afb_req request,
											std::shared_ptr<low_can_subscription_t>& can_subscription,
											std::map<int, std::shared_ptr<low_can_subscription_t> >& s,
											bool subscribe)
{
	/* Make the subscription or unsubscription to the event (if request contents are not null) */
	if(request.itf && request.closure &&
	   ((subscribe ? afb_req_subscribe : afb_req_unsubscribe)(request, s[can_subscription->get_index()]->get_event())) < 0)
	{
		AFB_ERROR("Operation goes wrong for signal: %s", can_subscription->get_name().c_str());
		return -1;
	}
	return 0;
}

static int create_event_handle(std::shared_ptr<low_can_subscription_t>& can_subscription,
							std::map<int, std::shared_ptr<low_can_subscription_t> >& s)
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

/// @brief This will determine if an event handle needs to be created and checks if
/// we got a valid afb_event to get subscribe or unsubscribe. After that launch the subscription or unsubscription
/// against the application framework using that event handle.
static int subscribe_unsubscribe_signal(struct afb_req request,
										bool subscribe,
										std::shared_ptr<low_can_subscription_t>& can_subscription,
										std::map<int, std::shared_ptr<low_can_subscription_t> >& s)
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

	// Checks if the event handler is correctly created, if it is, it
	// performs the subscription or unsubscription operations.
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

static int subscribe_unsubscribe_diagnostic_messages(struct afb_req request,
													bool subscribe,
													std::vector<std::shared_ptr<diagnostic_message_t> > diagnostic_messages,
													struct event_filter_t& event_filter,
													std::map<int, std::shared_ptr<low_can_subscription_t> >& s,
													bool perm_rec_diag_req)
{
	int rets = 0;
	application_t& app = application_t::instance();
	diagnostic_manager_t& diag_m = app.get_diagnostic_manager();

	for(const auto& sig : diagnostic_messages)
	{
		DiagnosticRequest* diag_req = new DiagnosticRequest(sig->build_diagnostic_request());
		event_filter.frequency = event_filter.frequency == 0 ? sig->get_frequency() : event_filter.frequency;
		std::shared_ptr<low_can_subscription_t> can_subscription;

		auto it =  std::find_if(s.begin(), s.end(), [&sig](std::pair<int, std::shared_ptr<low_can_subscription_t> > sub){ return (! sub.second->get_diagnostic_message().empty());});
		can_subscription = it != s.end() ?
			it->second :
			std::make_shared<low_can_subscription_t>(low_can_subscription_t(event_filter));
		// If the requested diagnostic message is not supported by the car then unsubcribe it
		// no matter what we want, worst case will be a failed unsubscription but at least we won't
		// poll a PID for nothing.
		if(sig->get_supported() && subscribe)
		{
			if (!app.isEngineOn())
				AFB_WARNING("signal: Engine is off, %s won't received responses until it's on",  sig->get_name().c_str());

			diag_m.add_recurring_request(diag_req, sig->get_name().c_str(), false, sig->get_decoder(), sig->get_callback(), event_filter.frequency, perm_rec_diag_req);
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

static int subscribe_unsubscribe_can_signals(struct afb_req request,
											bool subscribe,
											std::vector<std::shared_ptr<can_signal_t> > can_signals,
											struct event_filter_t& event_filter,
											std::map<int, std::shared_ptr<low_can_subscription_t> >& s)
{
	int rets = 0;
	for(const auto& sig: can_signals)
	{
		auto it =  std::find_if(s.begin(), s.end(), [&sig, &event_filter](std::pair<int, std::shared_ptr<low_can_subscription_t> > sub){ return sub.second->is_signal_subscription_corresponding(sig, event_filter) ; });
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
/// @param[in] afb_req request : contains original request use to subscribe or unsubscribe
/// @param[in] subscribe boolean value, which chooses between a subscription operation or an unsubscription
/// @param[in] signals -  struct containing vectors with can_signal_t and diagnostic_messages to subscribe
///
/// @return Number of correctly subscribed signal
///
static int subscribe_unsubscribe_signals(struct afb_req request,
										bool subscribe,
										const struct utils::signals_found& signals,
										struct event_filter_t& event_filter)
{
	int rets = 0;
	utils::signals_manager_t& sm = utils::signals_manager_t::instance();

	std::lock_guard<std::mutex> subscribed_signals_lock(sm.get_subscribed_signals_mutex());
	std::map<int, std::shared_ptr<low_can_subscription_t> >& s = sm.get_subscribed_signals();

	rets += subscribe_unsubscribe_diagnostic_messages(request, subscribe, signals.diagnostic_messages, event_filter, s, false);
	rets += subscribe_unsubscribe_can_signals(request, subscribe, signals.can_signals, event_filter, s);

	return rets;
}

static int one_subscribe_unsubscribe(struct afb_req request,
									bool subscribe,
									const std::string& tag,
									json_object* args)
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
			{event_filter.frequency = (float)json_object_get_double(obj);}
		if (json_object_object_get_ex(filter, "min", &obj)
		&& (json_object_is_type(obj, json_type_double) || json_object_is_type(obj, json_type_int)))
			{event_filter.min = (float)json_object_get_double(obj);}
		if (json_object_object_get_ex(filter, "max", &obj)
		&& (json_object_is_type(obj, json_type_double) || json_object_is_type(obj, json_type_int)))
			{event_filter.max = (float)json_object_get_double(obj);}
	}

	// subscribe or unsubscribe
	openxc_DynamicField search_key = build_DynamicField(tag);
	sf = utils::signals_manager_t::instance().find_signals(search_key);
	if (sf.can_signals.empty() && sf.diagnostic_messages.empty())
	{
		AFB_NOTICE("No signal(s) found for %s.", tag.c_str());
		ret = -1;
	}
	else
		{ret = subscribe_unsubscribe_signals(request, subscribe, sf, event_filter);}

	return ret;
}
static int process_one_subscribe_args(struct afb_req request, bool subscribe, json_object *args)
{
	int rc = 0, rc2=0;
	json_object *x = nullptr, *event = nullptr;
	if(args == NULL ||
		!json_object_object_get_ex(args, "event", &event))
	{
		rc = one_subscribe_unsubscribe(request, subscribe, "*", args);
	}
	else if (json_object_get_type(event) != json_type_array)
	{
		rc = one_subscribe_unsubscribe(request, subscribe, json_object_get_string(event), args);
	}
	else
	{
		for (int i = 0 ; i < json_object_array_length(event); i++)
		{
			x = json_object_array_get_idx(event, i);
			rc2 = one_subscribe_unsubscribe(request, subscribe, json_object_get_string(x), args);
			if (rc >= 0)
				rc = rc2 >= 0 ? rc + rc2 : rc2;
		}
	}
	return rc;
}

static void do_subscribe_unsubscribe(struct afb_req request, bool subscribe)
{
	int rc = 0;
	struct json_object *args, *x;

	args = afb_req_json(request);
	if (json_object_get_type(args) == json_type_array)
	{
		for(int i = 0; i < json_object_array_length(args); i++)
		{
			x = json_object_array_get_idx(args, i);
			rc += process_one_subscribe_args(request, subscribe, x);
		}
	}
	else
	{
		rc += process_one_subscribe_args(request, subscribe, args);
	}

	if (rc >= 0)
		afb_req_success(request, NULL, NULL);
	else
		afb_req_fail(request, "error", NULL);
}

void auth(struct afb_req request)
{
	afb_req_session_set_LOA(request, 1);
	afb_req_success(request, NULL, NULL);
}

void subscribe(struct afb_req request)
{
	do_subscribe_unsubscribe(request, true);
}

void unsubscribe(struct afb_req request)
{
	do_subscribe_unsubscribe(request, false);
}

static int send_frame(const std::string& bus_name, const struct can_frame& cf)
{
	std::map<std::string, std::shared_ptr<low_can_socket_t> >& cd = application_t::instance().get_can_devices();

	if( cd.count(bus_name) == 0)
		{cd[bus_name] = std::make_shared<low_can_socket_t>(low_can_socket_t());}

	return cd[bus_name]->tx_send(cf, bus_name);
}

static int write_raw_frame(const std::string& bus_name, uint32_t can_id, uint8_t can_dlc, struct json_object* can_data)
{
	int rc = 0;
	struct can_frame cf;

	::memset(&cf, 0, sizeof(cf));

	cf.can_id = can_id;
	cf.can_dlc = can_dlc;

	struct json_object *x;
	size_t n = json_object_array_length(can_data);
	if(n <= 8)
	{
		for (int i = 0 ; i < n ; i++)
		{
			x = json_object_array_get_idx(can_data, i);
			cf.data[i] = json_object_get_type(x) == json_type_int ? (uint8_t)json_object_get_int(x) : 0;
		}
	}

	const std::string found_device = application_t::instance().get_can_bus_manager().get_can_device_name(bus_name);
	if( ! found_device.empty())
	{
		rc = send_frame(found_device, cf);
	}

	return rc;
}
static int write_signal(const std::string& name, uint64_t value)
{
	int rc = 0;
	struct can_frame cf;
	struct utils::signals_found sf;

	::memset(&cf, 0, sizeof(cf));

	openxc_DynamicField search_key = build_DynamicField(name);
	sf = utils::signals_manager_t::instance().find_signals(search_key);

	if (sf.can_signals.empty())
	{
		AFB_WARNING("No signal(s) found for %s. Message not sent.", name.c_str());
		rc = -1;
	}
	else
	{
		for(const auto& sig: sf.can_signals)
		{
			if(sig->get_writable())
			{
				cf = encoder_t::build_frame(sig, value);
				const std::string bus_name = sig->get_message()->get_bus_device_name();
				rc = send_frame(bus_name, cf);
			}
			else
			{
				AFB_WARNING("%s isn't writable. Message not sent.", sig->get_name().c_str());
				return -1;
			}
		}
	}

	return rc;
}

void write(struct afb_req request)
{
	int rc = 0;
	struct json_object* args = nullptr,
		*json_name = nullptr,
		*json_value = nullptr;

	args = afb_req_json(request);

	// Process about Raw CAN message on CAN bus directly
	if (args != NULL &&
		(json_object_object_get_ex(args, "bus_name", &json_name) && json_object_is_type(json_name, json_type_string) ) &&
		(json_object_object_get_ex(args, "frame", &json_value) && json_object_is_type(json_value, json_type_object) ))
	{
		struct json_object* json_can_id = nullptr,
			*json_can_dlc = nullptr,
			*json_can_data = nullptr;

		if( (json_object_object_get_ex(json_value, "can_id", &json_can_id) && (json_object_is_type(json_can_id, json_type_double) || json_object_is_type(json_can_id, json_type_int))) &&
			(json_object_object_get_ex(json_value, "can_dlc", &json_can_dlc) && (json_object_is_type(json_can_dlc, json_type_double) || json_object_is_type(json_can_dlc, json_type_int))) &&
			(json_object_object_get_ex(json_value, "can_data", &json_can_data) && json_object_is_type(json_can_data, json_type_array) ))
		{
			rc = write_raw_frame(json_object_get_string(json_name),
				json_object_get_int(json_can_id),
				(uint8_t)json_object_get_int(json_can_dlc),
				json_can_data);
		}
		else
		{
			AFB_ERROR("Frame object malformed (must be \n \"frame\": {\"can_id\": int, \"can_dlc\": int, \"can_data\": [ int, int , int, int ,int , int ,int ,int]}");
			rc = -1;
		}
	}
	// Search signal then encode value.
	else if(args != NULL &&
		(json_object_object_get_ex(args, "signal_name", &json_name) && json_object_is_type(json_name, json_type_string)) &&
		(json_object_object_get_ex(args, "signal_value", &json_value) && (json_object_is_type(json_value, json_type_double) || json_object_is_type(json_value, json_type_int))))
	{
		rc = write_signal(json_object_get_string(json_name),
			(uint64_t)json_object_get_double(json_value));
	}
	else
	{
		AFB_ERROR("Request argument malformed. Please use the following syntax:");
		rc = -1;
	}

	if (rc >= 0)
		afb_req_success(request, NULL, NULL);
	else
		afb_req_fail(request, "error", NULL);
}

static struct json_object *get_signals_value(const std::string& name)
{
	struct utils::signals_found sf;
	struct json_object *ans = nullptr;

	openxc_DynamicField search_key = build_DynamicField(name);
	sf = utils::signals_manager_t::instance().find_signals(search_key);

	if (sf.can_signals.empty())
	{
		AFB_WARNING("No signal(s) found for %s.", name.c_str());
		return NULL;
	}
	ans = json_object_new_array();
	for(const auto& sig: sf.can_signals)
	{
		struct json_object *jobj = json_object_new_object();
		json_object_object_add(jobj, "event", json_object_new_string(sig->get_name().c_str()));
		json_object_object_add(jobj, "value", json_object_new_double(sig->get_last_value()));
		json_object_array_add(ans, jobj);
	}

	return ans;
}
void get(struct afb_req request)
{
	int rc = 0;
	struct json_object* args = nullptr,
		*json_name = nullptr;
	json_object *ans = nullptr;

	args = afb_req_json(request);

	// Process about Raw CAN message on CAN bus directly
	if (args != nullptr &&
		(json_object_object_get_ex(args, "event", &json_name) && json_object_is_type(json_name, json_type_string) ))
	{
		ans = get_signals_value(json_object_get_string(json_name));
		if (!ans)
			rc = -1;
	}
	else
	{
		AFB_ERROR("Request argument malformed. Please use the following syntax:");
		rc = -1;
	}

	if (rc >= 0)
		afb_req_success(request, ans, NULL);
	else
		afb_req_fail(request, "error", NULL);
}


static struct json_object *list_can_message(const std::string& name)
{
	struct utils::signals_found sf;
	struct json_object *ans = nullptr;

	openxc_DynamicField search_key = build_DynamicField(name);
	sf = utils::signals_manager_t::instance().find_signals(search_key);

	if (sf.can_signals.empty() && sf.diagnostic_messages.empty())
	{
		AFB_WARNING("No signal(s) found for %s.", name.c_str());
		return NULL;
	}
	ans = json_object_new_array();
	for(const auto& sig: sf.can_signals)
	{
		json_object_array_add(ans,
			json_object_new_string(sig->get_name().c_str()));
	}
	for(const auto& sig: sf.diagnostic_messages)
	{
		json_object_array_add(ans,
			json_object_new_string(sig->get_name().c_str()));
	}

	return ans;
}

void list(struct afb_req request)
{
	int rc = 0;
	json_object *ans = nullptr;
	struct json_object* args = nullptr,
		*json_name = nullptr;
	args = afb_req_json(request);
	const char *name;
	if ((args != nullptr) &&
		(json_object_object_get_ex(args, "event", &json_name) && json_object_is_type(json_name, json_type_string)))
	{
		name = json_object_get_string(json_name);
	}
	else
	{
		name = "*";
	}

	ans = list_can_message(name);
	if (!ans)
		rc = -1;

	if (rc >= 0)
		afb_req_success(request, ans, NULL);
	else
		afb_req_fail(request, "error", NULL);
}

/// @brief Initialize the binding.
///
/// @param[in] service Structure which represent the Application Framework Binder.
///
/// @return Exit code, zero if success.
int initv2()
{
	uint32_t ret = 1;
	can_bus_t& can_bus_manager = application_t::instance().get_can_bus_manager();

	can_bus_manager.set_can_devices();
	can_bus_manager.start_threads();

	/// Initialize Diagnostic manager that will handle obd2 requests.
	/// We pass by default the first CAN bus device to its Initialization.
	/// TODO: be able to choose the CAN bus device that will be use as Diagnostic bus.
	if(application_t::instance().get_diagnostic_manager().initialize())
		ret = 0;

	// Add a recurring dignostic message request to get engine speed at all times.
	openxc_DynamicField search_key = build_DynamicField("diagnostic_messages.engine.speed");
	struct utils::signals_found sf = utils::signals_manager_t::instance().find_signals(search_key);

	if(sf.can_signals.empty() && sf.diagnostic_messages.size() == 1)
	{
		struct afb_req request;
		request.itf = nullptr;
		request.closure = nullptr;

		struct event_filter_t event_filter;
		event_filter.frequency = sf.diagnostic_messages.front()->get_frequency();

		utils::signals_manager_t& sm = utils::signals_manager_t::instance();
		std::map<int, std::shared_ptr<low_can_subscription_t> >& s = sm.get_subscribed_signals();

		subscribe_unsubscribe_diagnostic_messages(request, true, sf.diagnostic_messages, event_filter, s, true);
	}

	if(ret)
		AFB_ERROR("There was something wrong with CAN device Initialization.");

	return ret;
}
