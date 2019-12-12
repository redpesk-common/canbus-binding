/*
 * Copyright (C) 2015, 2018 "IoT.bzh"
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
#include <algorithm>
#include <wrap-json.h>
#include <systemd/sd-event.h>
#include <ctl-config.h>
#include "openxc.pb.h"
#include "application.hpp"
#include "../can/can-encoder.hpp"
#include "../can/can-bus.hpp"
#include "../can/signals.hpp"
#include "../can/message/message.hpp"
#include "../utils/signals.hpp"
#include "../diagnostic/diagnostic-message.hpp"
#include "../utils/openxc-utils.hpp"
#include "../utils/signals.hpp"

#ifdef USE_FEATURE_J1939
	#include "../can/message/j1939-message.hpp"
	#include <linux/can/j1939.h>
#endif

///*****************************************************************************
///
///		Controller Definitions and Callbacks
///
///****************************************************************************/

int config_low_can(afb_api_t apiHandle, CtlSectionT *section, json_object *json_obj)
{
	AFB_DEBUG("Config %s", json_object_to_json_string(json_obj));
	CtlConfigT *ctrlConfig = (CtlConfigT *) afb_api_get_userdata(apiHandle);
	int active_message_set = 0;
	json_object *dev_mapping = nullptr;
	const char *diagnotic_bus = nullptr;

	if(! ctrlConfig || ! ctrlConfig->external)
		return -1;

	application_t *application = (application_t*) ctrlConfig->external;

	if(wrap_json_unpack(json_obj, "{si, s?s}",
			      "active_message_set", &active_message_set,
			      "diagnostic_bus", &diagnotic_bus))
		return -1;

	application->set_active_message_set((uint8_t)active_message_set);

	if(wrap_json_unpack(json_obj, "{so}",
			    "dev-mapping", &dev_mapping))
		return -1;

	application->get_can_bus_manager().set_can_devices(dev_mapping);

	/// Initialize Diagnostic manager that will handle obd2 requests.
	/// We pass by default the first CAN bus device to its Initialization.
	if(! diagnotic_bus || application_t::instance().get_diagnostic_manager().initialize(diagnotic_bus))
		AFB_WARNING("Diagnostic Manager: not initialized. No diagnostic messages will be processed.");

	return 0;
}

CtlSectionT ctlSections_[] = {
	[0]={.key="config" , .uid="config", .info=nullptr,
		 .loadCB=config_low_can,
		 .handle=nullptr,
		 .actions=nullptr},
	[1]={.key="plugins" , .uid="plugins", .info=nullptr,
		.loadCB=PluginConfig,
		.handle=nullptr,
		.actions=nullptr},
	[2]={.key=nullptr , .uid=nullptr, .info=nullptr,
		.loadCB=nullptr,
		.handle=nullptr,
		.actions=nullptr},
};

///*****************************************************************************
///
///		Subscription and unsubscription
///
///****************************************************************************/

/// @brief This will determine if an event handle needs to be created and checks if
/// we got a valid afb_event to get subscribe or unsubscribe. After that launch the subscription or unsubscription
/// against the application framework using that event handle.
static int subscribe_unsubscribe_signal(afb_req_t request,
					bool subscribe,
					std::shared_ptr<low_can_subscription_t>& can_subscription,
					map_subscription& s)
{
	int ret = 0;
	int sub_index = can_subscription->get_index();
	bool subscription_exists = s.count(sub_index);

	// Susbcription part
	if(subscribe)
	{
		/* There is no valid request to subscribe so this must be an
		 * internal permanent diagnostic request. Skip the subscription
		 * part and don't register it into the current "low-can"
		 * subsciptions.
		 */
		if(! request)
		{
			return 0;
		}

		// Event doesn't exist , so let's create it
		if ((ret = can_subscription->subscribe(request)) < 0)
			return ret;

		if(! subscription_exists)
				s[sub_index] = can_subscription;

		return ret;
	}

	// Unsubscrition part
	if(! subscription_exists)
	{
		AFB_NOTICE("There isn't any valid subscriptions for that request.");
		return ret;
	}
	else if (subscription_exists &&
		 ! afb_event_is_valid(s[sub_index]->get_event()) )
	{
		AFB_NOTICE("Event isn't valid, no need to unsubscribed.");
		return ret;
	}

	if( (ret = s[sub_index]->unsubscribe(request)) < 0)
		return ret;
	s.find(sub_index)->second->set_index(-1);
	s.erase(sub_index);
	return ret;
}

static int add_to_event_loop(std::shared_ptr<low_can_subscription_t>& can_subscription)
{
		struct sd_event_source* event_source = nullptr;
		return ( sd_event_add_io(afb_daemon_get_event_loop(),
			&event_source,
			can_subscription->get_socket()->socket(),
			EPOLLIN,
			read_message,
			can_subscription.get()));
}

static int subscribe_unsubscribe_diagnostic_messages(afb_req_t request,
						     bool subscribe,
						     list_ptr_diag_msg_t diagnostic_messages,
						     struct event_filter_t& event_filter,
						     map_subscription& s,
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

		auto it = std::find_if(s.begin(), s.end(), [&sig](std::pair<int, std::shared_ptr<low_can_subscription_t> > sub)
		{
			return (! sub.second->get_diagnostic_message().empty());
		});
		can_subscription = it != s.end() ?
			it->second :
			std::make_shared<low_can_subscription_t>(low_can_subscription_t(event_filter));
		// If the requested diagnostic message is not supported by the car then unsubcribe it
		// no matter what we want, worst case will be a failed unsubscription but at least we won't
		// poll a PID for nothing.
		if(sig->get_supported() && subscribe)
		{
			if (!app.is_engine_on())
				AFB_WARNING("signal: Engine is off, %s won't received responses until it's on",  sig->get_name().c_str());

			diag_m.add_recurring_request(diag_req, sig->get_name().c_str(), false, sig->get_decoder(), sig->get_callback(), event_filter.frequency, perm_rec_diag_req);
			if(can_subscription->create_rx_filter(sig) < 0)
				return -1;
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
			{
				AFB_DEBUG("%s cancelled due to unsubscribe", sig->get_name().c_str());
			}
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

static int subscribe_unsubscribe_signals(afb_req_t request,
					 bool subscribe,
					 list_ptr_signal_t signals,
					 struct event_filter_t& event_filter,
					 map_subscription& s)
{
	int rets = 0;
	for(const auto& sig: signals)
	{
		auto it =  std::find_if(s.begin(), s.end(), [&sig, &event_filter](std::pair<int, std::shared_ptr<low_can_subscription_t> > sub)
		{
			return sub.second->is_signal_subscription_corresponding(sig, event_filter) ;
		});
		std::shared_ptr<low_can_subscription_t> can_subscription;
		if(it != s.end())
			can_subscription = it->second;
		else
		{
			can_subscription = std::make_shared<low_can_subscription_t>(low_can_subscription_t(event_filter));
			if(can_subscription->create_rx_filter(sig) < 0)
				return -1;
			if(add_to_event_loop(can_subscription) < 0)
				return -1;
		}

		if(subscribe_unsubscribe_signal(request, subscribe, can_subscription, s) < 0)
			return -1;

		rets++;
		AFB_DEBUG("%s Signal: %s %ssubscribed", sig->get_message()->is_fd() ? "FD": "", sig->get_name().c_str(), subscribe ? "":"un");
	}
	return rets;
}

///
/// @brief subscribe to all signals in the vector signals
///
/// @param[in] afb_req request : contains original request use to subscribe or unsubscribe
/// @param[in] subscribe boolean value, which chooses between a subscription operation or an unsubscription
/// @param[in] signals -  struct containing vectors with signal_t and diagnostic_messages to subscribe
/// @param[in] event_filter - stuct containing filter on the signal
///
/// @return Number of correctly subscribed signal
///
static int subscribe_unsubscribe_signals(afb_req_t request,
					 bool subscribe,
					 const struct utils::signals_found& signals,
					 struct event_filter_t& event_filter)
{
	int rets = 0;
	utils::signals_manager_t& sm = utils::signals_manager_t::instance();

	std::lock_guard<std::mutex> subscribed_signals_lock(sm.get_subscribed_signals_mutex());
	map_subscription& s = sm.get_subscribed_signals();

	rets += subscribe_unsubscribe_diagnostic_messages(request, subscribe, signals.diagnostic_messages, event_filter, s, false);
	rets += subscribe_unsubscribe_signals(request, subscribe, signals.signals, event_filter, s);

	return rets;
}

static event_filter_t generate_filter(json_object* args)
{
	event_filter_t event_filter;
	struct json_object  *filter, *obj;

		// computes the filter
	if (json_object_object_get_ex(args, "filter", &filter))
	{
		if (json_object_object_get_ex(filter, "frequency", &obj)
		&& (json_object_is_type(obj, json_type_double) || json_object_is_type(obj, json_type_int)))
			event_filter.frequency = (float)json_object_get_double(obj);
		if (json_object_object_get_ex(filter, "min", &obj)
		&& (json_object_is_type(obj, json_type_double) || json_object_is_type(obj, json_type_int)))
			event_filter.min = (float)json_object_get_double(obj);
		if (json_object_object_get_ex(filter, "max", &obj)
		&& (json_object_is_type(obj, json_type_double) || json_object_is_type(obj, json_type_int)))
			event_filter.max = (float)json_object_get_double(obj);
		if (json_object_object_get_ex(filter, "rx_id", &obj)
		&& (json_object_is_type(obj, json_type_int)))
			event_filter.rx_id = (canid_t) json_object_get_int(obj);
		if (json_object_object_get_ex(filter, "tx_id", &obj)
		&& (json_object_is_type(obj, json_type_int)))
			event_filter.tx_id = (canid_t) json_object_get_int(obj);
	}
	return event_filter;
}


static int one_subscribe_unsubscribe_events(afb_req_t request, bool subscribe, const std::string& tag, json_object* args)
{
	int ret = 0;
	struct utils::signals_found sf;

	// subscribe or unsubscribe
	openxc_DynamicField search_key = build_DynamicField(tag);
	sf = utils::signals_manager_t::instance().find_signals(search_key);


#ifdef USE_FEATURE_ISOTP
	if(sf.signals.size() > 1)
	{
		sf.signals.remove_if([](std::shared_ptr<signal_t> x){
			bool isotp = x->get_message()->is_isotp();
			if(isotp)
				AFB_NOTICE("ISO TP messages need to be subscribed one by one (rx, tx)");

			return isotp;
		});
	}
#endif

	if (sf.signals.empty() && sf.diagnostic_messages.empty())
	{
		AFB_NOTICE("No signal(s) found for %s.", tag.c_str());
		ret = -1;
	}
	else
	{
		event_filter_t event_filter = generate_filter(args);
		ret = subscribe_unsubscribe_signals(request, subscribe, sf, event_filter);
	}
	return ret;
}

static int one_subscribe_unsubscribe_id(afb_req_t request, bool subscribe, const uint32_t& id, json_object *args)
{
	int ret = 0;
	std::shared_ptr<message_definition_t> message_definition = application_t::instance().get_message_definition(id);
	struct utils::signals_found sf;

	if(message_definition)
		sf.signals = list_ptr_signal_t(message_definition->get_signals().begin(), message_definition->get_signals().end());

	if(sf.signals.empty())
	{
		AFB_NOTICE("No signal(s) found for %d.", id);
		ret = -1;
	}
	else
	{
		event_filter_t event_filter = generate_filter(args);
		ret = subscribe_unsubscribe_signals(request, subscribe, sf, event_filter);
	}

	return ret;
}


static int process_one_subscribe_args(afb_req_t request, bool subscribe, json_object *args)
{
	int rc = 0, rc2=0;
	json_object *x = nullptr, *event = nullptr, *id = nullptr;


	// 2 cases : ID(PGN) and event

	json_object_object_get_ex(args,"event",&event);
	json_object_object_get_ex(args,"id",&id) || json_object_object_get_ex(args,"pgn",&id);

	if( args == NULL || (id && ((std::string)json_object_get_string(id)).compare("*") == 0))
	{
		rc = one_subscribe_unsubscribe_events(request, subscribe, "*", args);
	}
	else
	{
		if(event)
		{
			if (json_object_get_type(event) != json_type_array) // event is set before and check if it's an array
			{
				rc = one_subscribe_unsubscribe_events(request, subscribe, json_object_get_string(event), args);
			}
			else // event is set and it's not an array
			{
				for (int i = 0 ; i < json_object_array_length(event); i++)
				{
					x = json_object_array_get_idx(event, i);
					rc2 = one_subscribe_unsubscribe_events(request, subscribe, json_object_get_string(x), args);
					if (rc >= 0)
						rc = rc2 >= 0 ? rc + rc2 : rc2;
				}
			}
		}

		if(id)
		{
			if (json_object_get_type(id) != json_type_array) // id is set before and check if it's an array
			{
				rc = one_subscribe_unsubscribe_id(request, subscribe, json_object_get_int(id), args);
			}
			else // event is set and it's not an array
			{
				for (int i = 0 ; i < json_object_array_length(id); i++)
				{
					x = json_object_array_get_idx(id, i);
					rc2 = one_subscribe_unsubscribe_id(request, subscribe, json_object_get_int(x), args);
					if (rc >= 0)
						rc = rc2 >= 0 ? rc + rc2 : rc2;
				}
			}
		}
	}
	return rc;
}

static void do_subscribe_unsubscribe(afb_req_t request, bool subscribe)
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

void auth(afb_req_t request)
{
	afb_req_session_set_LOA(request, 1);
	afb_req_success(request, NULL, NULL);
}

void subscribe(afb_req_t request)
{
	do_subscribe_unsubscribe(request, true);
}

void unsubscribe(afb_req_t request)
{
	do_subscribe_unsubscribe(request, false);
}

/*
static int send_frame(struct canfd_frame& cfd, const std::string& bus_name, socket_type type)
{
	if(bus_name.empty())
	{
		return -1;
	}

	std::map<std::string, std::shared_ptr<low_can_subscription_t> >& cd = application_t::instance().get_can_devices();

	if( cd.count(bus_name) == 0)
	{
		cd[bus_name] = std::make_shared<low_can_subscription_t>(low_can_subscription_t());
	}


	if(type == socket_type::BCM)
	{
		return low_can_subscription_t::tx_send(*cd[bus_name], cfd, bus_name);
	}
	else if(type == socket_type::J1939)
	{
		return low_can_subscription_t::j1939_send(*cd[bus_name], cfd, bus_name);
	}
	else{
		return -1;
	}
}
*/
static int send_message(message_t *message, const std::string& bus_name, uint32_t flags, event_filter_t &event_filter, std::shared_ptr<signal_t> signal)
{
	if(bus_name.empty())
		return -1;

	std::map<std::string, std::shared_ptr<low_can_subscription_t> >& cd = application_t::instance().get_can_devices();

	if( cd.count(bus_name) == 0)
		cd[bus_name] = std::make_shared<low_can_subscription_t>(low_can_subscription_t(event_filter));

	cd[bus_name]->set_signal(signal);


	if(flags&CAN_PROTOCOL)
		return low_can_subscription_t::tx_send(*cd[bus_name], message, bus_name);
#ifdef USE_FEATURE_ISOTP
	else if(flags&ISOTP_PROTOCOL)
		return low_can_subscription_t::isotp_send(*cd[bus_name], message, bus_name);
#endif
#ifdef USE_FEATURE_J1939
	else if(flags&J1939_PROTOCOL)
		return low_can_subscription_t::j1939_send(*cd[bus_name], message, bus_name);
#endif
	else
		return -1;
}


static void write_raw_frame(afb_req_t request, const std::string& bus_name, message_t *message,
			    struct json_object *can_data, uint32_t flags, event_filter_t &event_filter)
{

	struct utils::signals_found sf;

	utils::signals_manager_t::instance().lookup_signals_by_id(message->get_id(), application_t::instance().get_all_signals(), sf.signals);

	if( !sf.signals.empty() )
	{
		AFB_DEBUG("ID WRITE RAW : %d", sf.signals.front()->get_message()->get_id());
		if(flags & CAN_PROTOCOL)
		{
			if(sf.signals.front()->get_message()->is_fd())
			{
				AFB_DEBUG("CANFD_MAX_DLEN");
				message->set_flags(CAN_PROTOCOL_WITH_FD_FRAME);
				message->set_maxdlen(CANFD_MAX_DLEN);
			}
			else
			{
				AFB_DEBUG("CAN_MAX_DLEN");
				message->set_maxdlen(CAN_MAX_DLEN);
			}

			if(sf.signals.front()->get_message()->is_isotp())
			{
				flags = ISOTP_PROTOCOL;
				message->set_maxdlen(MAX_ISOTP_FRAMES * message->get_maxdlen());
			}
		}

#ifdef USE_FEATURE_J1939
		if(flags&J1939_PROTOCOL)
			message->set_maxdlen(J1939_MAX_DLEN);
#endif

		if(message->get_length() > 0 && message->get_length() <= message->get_maxdlen())
		{
			std::vector<uint8_t> data;
			for (int i = 0 ; i < message->get_length() ; i++)
			{
				struct json_object *one_can_data = json_object_array_get_idx(can_data, i);
				data.push_back((json_object_is_type(one_can_data, json_type_int)) ?
						(uint8_t)json_object_get_int(one_can_data) : 0);
			}
			message->set_data(data);
		}
		else
		{
			if(flags&CAN_PROTOCOL)
				afb_req_fail(request, "Invalid", "Frame BCM");
			else if(flags&J1939_PROTOCOL)
				afb_req_fail(request, "Invalid", "Frame J1939");
			else if(flags&ISOTP_PROTOCOL)
				afb_req_fail(request, "Invalid", "Frame ISOTP");
			else
				afb_req_fail(request, "Invalid", "Frame");

			return;
		}

		if(! send_message(message, application_t::instance().get_can_bus_manager().get_can_device_name(bus_name), flags, event_filter, sf.signals.front()))
			afb_req_success(request, nullptr, "Message correctly sent");
		else
			afb_req_fail(request, "Error", "sending the message. See the log for more details.");
	}
	else
	{
		afb_req_fail(request, "Error", "no find id in signals. See the log for more details.");
	}

}

static void write_frame(afb_req_t request, const std::string& bus_name, json_object *json_value, event_filter_t &event_filter)
{
	message_t *message;
	uint32_t id;
	uint32_t length;
	struct json_object *can_data = nullptr;
	std::vector<uint8_t> data;

	AFB_DEBUG("JSON content %s", json_object_get_string(json_value));

	if(!wrap_json_unpack(json_value, "{si, si, so !}",
				  "can_id", &id,
				  "can_dlc", &length,
				  "can_data", &can_data))
	{
		message = new can_message_t(0, id, length, false, 0, data, 0);
		write_raw_frame(request, bus_name, message, can_data, CAN_PROTOCOL, event_filter);
	}
#ifdef USE_FEATURE_J1939
	else if(!wrap_json_unpack(json_value, "{si, si, so !}",
				  "pgn", &id,
				  "length", &length,
				  "data", &can_data))
	{
		message = new j1939_message_t(length, data, 0, J1939_NO_NAME, (pgn_t)id, J1939_NO_ADDR);
		write_raw_frame(request, bus_name, message, can_data, J1939_PROTOCOL, event_filter);
	}
#endif
	else
	{
		afb_req_fail(request, "Invalid", "Frame object malformed");
		return;
	}
	delete message;
}

static void write_signal(afb_req_t request, const std::string& name, json_object *json_value, event_filter_t &event_filter)
{
	struct canfd_frame cfd;
	struct utils::signals_found sf;
	signal_encoder encoder = nullptr;
	bool send = true;

	::memset(&cfd, 0, sizeof(cfd));

	openxc_DynamicField search_key = build_DynamicField(name);
	sf = utils::signals_manager_t::instance().find_signals(search_key);
	openxc_DynamicField dynafield_value = build_DynamicField(json_value);

	if (sf.signals.empty())
	{
		afb_req_fail_f(request, "No signal(s) found for %s. Message not sent.", name.c_str());
		return;
	}

	std::shared_ptr<signal_t> sig = sf.signals.front();
	if(! sig->get_writable())
	{
		afb_req_fail_f(request, "%s isn't writable. Message not sent.", sig->get_name().c_str());
		return;
	}

	uint64_t value = (encoder = sig->get_encoder()) ?
			encoder(*sig, dynafield_value, &send) :
			encoder_t::encode_DynamicField(*sig, dynafield_value, &send);

	uint32_t flags = INVALID_FLAG;

	if(sig->get_message()->is_j1939())
		flags = J1939_PROTOCOL;
	else if(sig->get_message()->is_isotp())
		flags = ISOTP_PROTOCOL;
	else
		flags = CAN_PROTOCOL;

//	cfd = encoder_t::build_frame(sig, value);
	message_t *message = encoder_t::build_message(sig, value, false, false);

	if(! send_message(message, sig->get_message()->get_bus_device_name(), flags, event_filter, sig) && send)
		afb_req_success(request, nullptr, "Message correctly sent");
	else
		afb_req_fail(request, "Error", "Sending the message. See the log for more details.");

	if(sig->get_message()->is_j1939())
#ifdef USE_FEATURE_J1939
		delete (j1939_message_t*) message;
#else
		afb_req_fail(request, "Warning", "J1939 not implemented in your kernel.");
#endif
	else
		delete (can_message_t*) message;
}

void write(afb_req_t request)
{
	struct json_object* args = nullptr, *json_value = nullptr, *name = nullptr;
	args = afb_req_json(request);

	if(args != NULL)
	{
		event_filter_t event_filter = generate_filter(args);

		if(json_object_object_get_ex(args,"bus_name",&name))
		{
			if(json_object_object_get_ex(args,"frame",&json_value))
				write_frame(request, (std::string)json_object_get_string(name), json_value, event_filter);
			else
				afb_req_fail(request, "Error", "Request argument malformed");
		}
		else if(json_object_object_get_ex(args,"signal_name",&name))
		{
			if(json_object_object_get_ex(args,"signal_value",&json_value))
				write_signal(request, (std::string)json_object_get_string(name), json_value, event_filter);
			else
				afb_req_fail(request, "Error", "Request argument malformed");
		}
		else
		{
			afb_req_fail(request, "Error", "Request argument malformed");
		}
	}
	else
	{
		afb_req_fail(request, "Error", "Request argument null");
	}
}

static struct json_object *get_signals_value(const std::string& name)
{
	struct utils::signals_found sf;
	struct json_object *ans = nullptr;

	openxc_DynamicField search_key = build_DynamicField(name);
	sf = utils::signals_manager_t::instance().find_signals(search_key);

	if (sf.signals.empty())
	{
		AFB_WARNING("No signal(s) found for %s.", name.c_str());
		return NULL;
	}
	ans = json_object_new_array();
	for(const auto& sig: sf.signals)
	{
		struct json_object *jobj = json_object_new_object();
		json_object_object_add(jobj, "event", json_object_new_string(sig->get_name().c_str()));
		json_object_object_add(jobj, "value", json_object_new_double(sig->get_last_value()));
		json_object_array_add(ans, jobj);
	}

	return ans;
}
void get(afb_req_t request)
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

	if (sf.signals.empty() && sf.diagnostic_messages.empty())
	{
		AFB_WARNING("No signal(s) found for %s.", name.c_str());
		return NULL;
	}
	ans = json_object_new_array();
	for(const auto& sig: sf.signals)
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

void list(afb_req_t request)
{
	int rc = 0;
	json_object *ans = nullptr;
	struct json_object* args = nullptr,
		*json_name = nullptr;
	args = afb_req_json(request);
	const char *name;
	if ((args != nullptr) &&
		(json_object_object_get_ex(args, "event", &json_name) && json_object_is_type(json_name, json_type_string)))
		name = json_object_get_string(json_name);
	else
		name = "*";

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
int init_binding(afb_api_t api)
{
	int ret = 0;
	application_t& application = application_t::instance();
	can_bus_t& can_bus_manager = application.get_can_bus_manager();

	if(application.get_message_set().empty())
	{
		AFB_ERROR("No message_set defined");
		return -1;
	}

	can_bus_manager.start_threads();
	utils::signals_manager_t& sm = utils::signals_manager_t::instance();

	if (application.get_diagnostic_manager().is_initialized())
	{
		// Add a recurring dignostic message request to get engine speed at all times.
		openxc_DynamicField search_key = build_DynamicField("diagnostic_messages.engine.speed");
		struct utils::signals_found sf = sm.find_signals(search_key);

		if(sf.signals.empty() && sf.diagnostic_messages.size() == 1)
		{
			afb_req_t request = nullptr;

			struct event_filter_t event_filter;
			event_filter.frequency = sf.diagnostic_messages.front()->get_frequency();

			map_subscription& s = sm.get_subscribed_signals();

			subscribe_unsubscribe_diagnostic_messages(request, true, sf.diagnostic_messages, event_filter, s, true);
		}
	}

#ifdef USE_FEATURE_J1939
	std::string j1939_bus;
	vect_ptr_msg_def_t current_messages_definition = application.get_messages_definition();
	for(std::shared_ptr<message_definition_t> message_definition: current_messages_definition)
	{
		if(message_definition->is_j1939())
		{
			if (j1939_bus == message_definition->get_bus_device_name() )
				continue;
			j1939_bus = message_definition->get_bus_device_name();

			std::shared_ptr<low_can_subscription_t> low_can_j1939 = std::make_shared<low_can_subscription_t>();
			application.set_subscription_address_claiming(low_can_j1939);

			ret = low_can_subscription_t::open_socket(*low_can_j1939,
								  j1939_bus,
								  J1939_ADDR_CLAIM_PROTOCOL);

			if(ret < 0)
			{
				AFB_ERROR("Error open socket address claiming for j1939 protocol");
				return -1;
			}
			add_to_event_loop(low_can_j1939);
			break;
		}
	}
#endif

	if(ret)
		AFB_ERROR("There was something wrong with CAN device Initialization.");

	return ret;
}

int load_config(afb_api_t api)
{
	int ret = 0;
	CtlConfigT *ctlConfig;
	const char *dirList = getenv("CONTROL_CONFIG_PATH");
	std::string bindingDirPath = GetBindingDirPath(api);
	std::string filepath = bindingDirPath + "/etc";

	if (!dirList)
		dirList=CONTROL_CONFIG_PATH;

	filepath.append(":");
	filepath.append(dirList);
	const char *configPath = CtlConfigSearch(api, filepath.c_str(), "control");

	if (!configPath)
	{
		AFB_ERROR_V3("CtlPreInit: No control-%s* config found invalid JSON %s ", GetBinderName(), filepath.c_str());
		return -1;
	}

	// create one API per file
	ctlConfig = CtlLoadMetaData(api, configPath);
	if (!ctlConfig)
	{
		AFB_ERROR_V3("CtrlPreInit No valid control config file in:\n-- %s", configPath);
		return -1;
	}

	// Save the config in the api userdata field
	afb_api_set_userdata(api, ctlConfig);

	setExternalData(ctlConfig, (void*) &application_t::instance());
	ret= CtlLoadSections(api, ctlConfig, ctlSections_);

	return ret;
}
