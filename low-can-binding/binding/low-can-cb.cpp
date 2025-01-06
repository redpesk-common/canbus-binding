/*
 * Copyright (C) 2015-2025 IoT.bzh Company
 * Author: "Romain Forlot" <romain.forlot@iot.bzh>
 * Author: "Loic Collignon" <loic.collignon@iot.bzh>
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

#include <mutex>
#include <regex>
#include <vector>
#include <algorithm>

#include <sys/epoll.h>
#include <afb/afb-binding>

extern "C" {
#include <rp-utils/rp-jsonc.h>
#include <rp-utils/rp-path-search.h>
#include <afb-helpers4/plugin-store.h>
#include <afb-helpers4/afb-data-utils.h>
}

#include <canbus-binding/binding/low-can-hat.hpp>

#include "openxc.pb.h"

#include <canbus-binding/binding/plugin.hpp>
#include <canbus-binding/binding/application.hpp>
#include <canbus-binding/can/can-encoder.hpp>
#include <canbus-binding/can/can-bus.hpp>
#include <canbus-binding/can/signals.hpp>
#include <canbus-binding/can/message/message.hpp>
#include <canbus-binding/diagnostic/diagnostic-message.hpp>
#include <canbus-binding/utils/signals.hpp>
#include <canbus-binding/utils/openxc-utils.hpp>

#ifdef USE_FEATURE_J1939
	#include <canbus-binding/can/message/j1939-message.hpp>
	#include <linux/can/j1939.h>
#endif

/*******************************************************************************
*
*		Static Api Verbs implementation
*
*******************************************************************************/

/// @brief This will determine if an event handle needs to be created and checks if
/// we got a valid afb_event to get subscribe or unsubscribe. After that launch the subscription or unsubscription
/// against the application framework using that event handle.
static int subscribe_unsubscribe_signal(afb::req request,
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
			return 0;

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

static void on_can_event(afb_evfd_t evfd, int fd, uint32_t revents, void *userdata)
{
	low_can_subscription_t &can_subscription = *(low_can_subscription_t*) userdata;

	if (can_subscription.get_socket()->socket() != fd)
	{
		AFB_ERROR("%s: subscription socket and callback fd do not match. Should not happens, you got a valid subscription with a wrong socket. Abort.", __FUNCTION__);
		return;
	}
	if ((revents & EPOLLIN) != 0)
	{
		read_message(can_subscription);
	}

	// check if error or hangup
	if ((revents & (EPOLLERR|EPOLLRDHUP|EPOLLHUP)) != 0)
	{
		afb_evfd_unref(evfd);
		can_subscription.get_socket()->close();
	}
}

static int add_to_event_loop(std::shared_ptr<low_can_subscription_t>& can_subscription)
{
	// JOBOL: missing C++ handler class, must use C one. Don't know
	// if it is wanted or not.
	afb_evfd_t efd = nullptr;
	return afb_evfd_create(&efd,
				can_subscription->get_socket()->socket(),
				EPOLLIN,
				on_can_event,
				can_subscription.get(),
				true,
				true);
}

static int subscribe_unsubscribe_diagnostic_messages(afb::req request,
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
		event_filter.frequency = sig->get_frequency();
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
		if (sig->get_supported()) {
			if (subscribe) {
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
			} else {
				diag_m.cleanup_request(diag_m.find_recurring_request(*diag_req), true);
				AFB_DEBUG("%s cancelled due to unsubscribe", sig->get_name().c_str());
			}
		}
		else
		{
			AFB_WARNING("signal: %s isn't supported. Canceling operation.", sig->get_name().c_str());
			return -1;
		}
		int ret = subscribe_unsubscribe_signal(request, subscribe, can_subscription, s);
		if(ret < 0)
			return ret;

		rets++;
	}
	return rets;
}

static int subscribe_unsubscribe_signals(afb::req request,
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

static event_filter_t generate_filter(json_object* args)
{
	event_filter_t event_filter = {};
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
		if (json_object_object_get_ex(filter, "promisc", &obj)
			&& (json_object_is_type(obj, json_type_boolean)))
			event_filter.promisc = (bool)json_object_get_boolean(obj);
		if (json_object_object_get_ex(filter, "rx_id", &obj)
		&& (json_object_is_type(obj, json_type_int)))
			event_filter.rx_id = (canid_t) json_object_get_int(obj);
		if (json_object_object_get_ex(filter, "tx_id", &obj)
		&& (json_object_is_type(obj, json_type_int)))
			event_filter.tx_id = (canid_t) json_object_get_int(obj);
	}
	return event_filter;
}


static int one_subscribe_unsubscribe_events(afb::req request, bool subscribe, const std::string& tag, json_object *args, map_subscription& s)
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
		if(application_t::instance().get_diagnostic_manager().is_initialized())
			ret += subscribe_unsubscribe_diagnostic_messages(request,
									 subscribe,
									 sf.diagnostic_messages,
									 event_filter,
									 s,
									 false);
		ret += subscribe_unsubscribe_signals(request, subscribe, sf.signals, event_filter, s);
	}
	return ret;
}

static int one_subscribe_unsubscribe_id(afb::req request, bool subscribe, const uint32_t& id, json_object *args, map_subscription& s)
{
	std::vector<std::shared_ptr<message_definition_t>> messages_definition = application_t::instance().get_messages_definition(id);
	for(auto message_definition : messages_definition)
	{
	struct utils::signals_found sf;

	if(message_definition)
		sf.signals = list_ptr_signal_t(message_definition->get_signals().begin(), message_definition->get_signals().end());

	if(sf.signals.empty())
	{
		AFB_NOTICE("No signal(s) found for %d.", id);
		return -1;
	}

	event_filter_t event_filter = generate_filter(args);
	std::shared_ptr<low_can_subscription_t> can_subscription = std::make_shared<low_can_subscription_t>(low_can_subscription_t(event_filter));
	can_subscription->set_message_definition(message_definition);

	if(can_subscription->create_rx_filter(message_definition) < 0)
		return -1;
	if(add_to_event_loop(can_subscription) < 0)
		return -1;

	if(subscribe_unsubscribe_signal(request, subscribe, can_subscription, s) < 0)
		return -1;
	}

	return 0;
}

static int process_one_subscribe_args(afb::req request, bool subscribe, json_object *args, map_subscription& s)
{
	int rc = -1, rc2 = 0;
	json_object *x = nullptr, *event = nullptr, *id = nullptr;


	// 2 cases : ID(PGN) and event

	json_object_object_get_ex(args,"event",&event);
	json_object_object_get_ex(args,"id",&id) || json_object_object_get_ex(args,"pgn",&id);

	if( args == NULL || (id && ((std::string)json_object_get_string(id)).compare("*") == 0))
	{
		rc = one_subscribe_unsubscribe_events(request, subscribe, "*", args, s);
	}
	else
	{
		if(event)
		{
			if (json_object_get_type(event) != json_type_array) // event is set before and check if it's an array
			{
				rc = one_subscribe_unsubscribe_events(request, subscribe, json_object_get_string(event), args, s);
			}
			else // event is set and it's not an array
			{
				for (int i = 0 ; i < json_object_array_length(event); i++)
				{
					x = json_object_array_get_idx(event, i);
					rc2 = one_subscribe_unsubscribe_events(request, subscribe, json_object_get_string(x), args, s);
					if (rc >= 0)
						rc = rc2 >= 0 ? rc + rc2 : rc2;
				}
			}
		}

		if(id)
		{
			if (json_object_get_type(id) != json_type_array) // id is set before and check if it's an array
			{
				rc = one_subscribe_unsubscribe_id(request, subscribe, json_object_get_int(id), args, s);
			}
			else // event is set and it's not an array
			{
				for (int i = 0 ; i < json_object_array_length(id); i++)
				{
					x = json_object_array_get_idx(id, i);
					rc2 = one_subscribe_unsubscribe_id(request, subscribe, json_object_get_int(x), args, s);
					if (rc >= 0)
						rc = rc2 >= 0 ? rc + rc2 : rc2;
				}
			}
		}
	}
	return rc;
}

static bool get_json_param(afb::req request, json_object *&json) noexcept
{
	afb::data data;
	if (!request.try_convert(0, afb::JSON_C(), data)) {
		request.reply(AFB_ERRNO_INVALID_REQUEST);
		return false;
	}
	json = reinterpret_cast<json_object*>(*data);
	return true;
}

static void do_subscribe_unsubscribe(afb::req request, afb::received_data params, bool subscribe)
{
	utils::signals_manager_t& sm = utils::signals_manager_t::instance();
	std::lock_guard<std::mutex> subscribed_signals_lock(sm.get_subscribed_signals_mutex());
	map_subscription& s = sm.get_subscribed_signals();
	int rc = 0;
	struct json_object *args, *x;

	if (!get_json_param(request, args))
		return;

	if (json_object_get_type(args) == json_type_array)
	{
		for(int i = 0; i < json_object_array_length(args); i++)
		{
			x = json_object_array_get_idx(args, i);
			rc += process_one_subscribe_args(request, subscribe, x, s);
		}
	}
	else
	{
		rc += process_one_subscribe_args(request, subscribe, args, s);
	}

	if (rc >= 0)
		request.reply(0);
	else
		request.reply(-1);
}

static void auth(afb::req request, afb::received_data params) noexcept
{
	request.session_set_LOA(1);
	request.reply(0);
}

static void subscribe(afb::req request, afb::received_data params)
{
	do_subscribe_unsubscribe(request, params, true);
}

static void unsubscribe(afb::req request, afb::received_data params)
{
	do_subscribe_unsubscribe(request, params, false);
}

static int send_message(message_t *message, const std::string& bus_name, uint32_t flags, event_filter_t &event_filter, std::shared_ptr<signal_t> signal)
{
	if(bus_name.empty())
		return -1;

	std::shared_ptr<low_can_subscription_t> sub = std::make_shared<low_can_subscription_t>(low_can_subscription_t(event_filter));

	sub->set_signal(signal);


	if(flags&CAN_PROTOCOL)
		return sub->tx_send(message, bus_name);
#ifdef USE_FEATURE_ISOTP
	else if(flags&ISOTP_PROTOCOL)
		return sub->isotp_send(message, bus_name);
#endif
#ifdef USE_FEATURE_J1939
	else if(flags&J1939_PROTOCOL)
		return sub->j1939_send(message, bus_name);
#endif
	else
		return -1;
}

static void write_raw_frame(afb::req request, const std::string& bus_name, message_t *message,
			    json_object *can_data, uint32_t flags, event_filter_t &event_filter)
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

		if(message->get_length() > 0 &&
		   message->get_length() <= message->get_maxdlen() &&
		   json_object_get_type(can_data) == json_type_array)
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
			{
				afb_data_t reply = afb_data_string("Invalid: Frame BCM");
				request.reply(-1, 1, &reply);
			}
			else if(flags&J1939_PROTOCOL)
			{
				afb_data_t reply = afb_data_string("Invalid: Frame J1939");
				request.reply(-1, 1, &reply);
			}
			else if(flags&ISOTP_PROTOCOL)
			{
				afb_data_t reply = afb_data_string("Invalid: Frame ISOTP");
				request.reply(-1, 1, &reply);
			}
			else
			{
				afb_data_t reply = afb_data_string("Invalid: Frame");
				request.reply(1, 1, &reply);
			}

			return;
		}

		if(! send_message(message, application_t::instance().get_can_bus_manager().get_can_device_name(bus_name), flags, event_filter, sf.signals.front()))
			request.reply(0);
		else
			request.reply(-2);
	}
	else
	{
		afb_data_t reply;
		const char msg[] = "Error: no find id in signals. See the log for more details.";
		afb_create_data_raw(&reply, AFB_PREDEFINED_TYPE_STRINGZ, msg, sizeof msg, NULL, NULL);

		request.reply(-1, 1, &reply);
	}

}

static void write_frame(afb::req request, const std::string& bus_name, json_object *json_value, event_filter_t &event_filter)
{
	message_t *message;
	uint32_t id;
	uint32_t length;
	struct json_object *can_data = nullptr;
	std::vector<uint8_t> data;

	AFB_DEBUG("JSON content %s", json_object_get_string(json_value));

	if(!rp_jsonc_unpack(json_value, "{si, si, so !}",
				  "can_id", &id,
				  "can_dlc", &length,
				  "can_data", &can_data))
	{
		message = new can_message_t(0, id, length, false, 0, data, 0);
		write_raw_frame(request, bus_name, message, can_data, CAN_PROTOCOL, event_filter);
	}
#ifdef USE_FEATURE_J1939
	else if(!rp_jsonc_unpack(json_value, "{si, si, so !}",
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
		afb_data_t reply = afb_data_string("Invalid: Frame object malformed");
		request.reply(-1, 1, &reply);
		return;
	}
	delete message;
}

static void write_signal(afb::req request, const std::string& name, json_object *json_value, event_filter_t &event_filter)
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
		afb_data_t data = afb_data_string_copy(std::string("No signal(s) found for "+name+". Message not sent.").c_str(), 0);
		request.reply(-1, 1, &data);
		return;
	}

	std::shared_ptr<signal_t> sig = sf.signals.front();
	if(! sig->get_writable())
	{
		afb_data_t data = afb_data_string_copy(std::string(sig->get_name()+" isn't writable. Message not sent.").c_str(), 0);
		request.reply(-1, 1, &data);
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

	message_t *message = encoder_t::build_message(sig, value, false, false);

	if(! send_message(message, sig->get_message()->get_bus_device_name(), flags, event_filter, sig) && send)
		request.reply(0);
	else
		request.reply(-1);

	delete message;
}

static void write(afb::req request, afb::received_data params)
{
	struct json_object* args, *json_value = nullptr, *name = nullptr;

	if (!get_json_param(request, args))
		return;

	if(args != NULL)
	{
		event_filter_t event_filter = generate_filter(args);

		if(json_object_object_get_ex(args,"bus_name",&name))
		{
			if(json_object_object_get_ex(args,"frame",&json_value))
			{
				write_frame(request, (std::string)json_object_get_string(name), json_value, event_filter);
				return;
			}
		}
		else if(json_object_object_get_ex(args,"signal_name",&name))
		{
			if(json_object_object_get_ex(args,"signal_value",&json_value))
			{
				write_signal(request, (std::string)json_object_get_string(name), json_value, event_filter);
				return;
			}
		}
	}
	request.reply(AFB_ERRNO_INVALID_REQUEST);
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
		struct json_object *jobj = sig->afb_verb_get_last_value();
		json_object_array_add(ans, jobj);
	}

	return ans;
}

static struct json_object *get_id_value(const uint32_t& id)
{
	std::vector<std::shared_ptr<message_definition_t>> messages_definition = application_t::instance().get_messages_definition(id);
	struct json_object *ret = json_object_new_array();
	for(auto message_definition : messages_definition)
	{
		struct utils::signals_found sf;
		struct json_object *ans = nullptr;

		if(message_definition)
			sf.signals = list_ptr_signal_t(message_definition->get_signals().begin(), message_definition->get_signals().end());

		if(sf.signals.empty())
		{
			AFB_WARNING("no signal(s) found for %d.", id);
			return NULL;
		}

		ans = json_object_new_object();
		struct json_object *jsignals = json_object_new_array();
		json_object_object_add(ans, "signals", jsignals);
		for(const auto& sig: sf.signals)
		{
			struct json_object *jobj = sig->afb_verb_get_last_value();
			json_object_array_add(jsignals, jobj);
		}
		json_object_array_add(ret, ans);
	}

	return ret;
}

// TODO: convert "ans" json_object to a normal C/C++ struct avoiding json-c
// serialization process.
static void get(afb::req request, afb::received_data params)
{
	int rc = 0;
	struct json_object* args = nullptr, *json_name = nullptr, *ans = nullptr;

	if (!get_json_param(request, args))
		return;

	// Process about Raw CAN message on CAN bus directly
	if (args != nullptr &&
		(json_object_object_get_ex(args, "event", &json_name) &&json_object_is_type(json_name, json_type_string) ))
	{
		ans = get_signals_value(json_object_get_string(json_name));
		if (!ans)
			rc = -1;
	}
	else if (args != nullptr &&
		(json_object_object_get_ex(args, "id", &json_name)))
	{
		if (json_object_get_type(json_name) == json_type_string) // id is set before and check if it's an array
		{
			ans = get_id_value(json_object_get_int(json_name));
		}
		else if(json_object_get_type(json_name) == json_type_array)
		{
			ans = json_object_new_array();
			for (int i = 0 ; i < json_object_array_length(json_name); i++)
			{
				json_object *sub_ans = nullptr;
				json_object *x = json_object_array_get_idx(json_name, i);
				sub_ans = get_id_value(json_object_get_int(x));
				if(!sub_ans)
					rc = -1;
				else {
					struct json_object *jobj = json_object_new_object();
					struct json_object *jid = json_object_new_string(json_object_get_string(x));
					json_object_object_add(jobj, "id", jid);
					json_object_object_add(jobj, "data", sub_ans);
					json_object_array_add(ans, jobj);
				}
			}
		}
		else
			rc = -1;
		if (!ans)
			rc = -1;
	}
	else
	{
		AFB_ERROR("Request argument malformed. Please use the following syntax:");
		rc = -1;
	}

	if (rc >= 0) {
		afb::data data = afb_data_json_c_hold(ans);
		request.reply(0, data);
	}
	else {
		json_object_put(ans);
		request.reply(-1);
	}
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

// TODO: convert "ans" json_object to a normal C/C++ struct avoiding json-c
// serialization process.
static void list(afb::req request, afb::received_data params)
{
	int rc = 0;
	struct json_object* args = nullptr, *json_name = nullptr, *ans = nullptr;

	if (!get_json_param(request, args))
		return;

	const char *name;
	if ((args != nullptr) &&
		(json_object_object_get_ex(args, "event", &json_name) && json_object_is_type(json_name, json_type_string)))
		name = json_object_get_string(json_name);
	else
		name = "*";

	ans = list_can_message(name);
	if (!ans)
		rc = -1;

	if (rc >= 0) {
		afb::data data = afb_data_json_c_hold(ans);
		request.reply(0, data);
	}
	else {
		json_object_put(ans);
		request.reply(-1);
	}
}

/*******************************************************************************
*
*		Dynamic API's verbs implementations
*
*******************************************************************************/

static void simple_subscribe_unsubscribe_signal(afb::req request, signal_t* signal, json_object *args, bool subscribe)
{
	utils::signals_manager_t& sm = utils::signals_manager_t::instance();
	std::lock_guard<std::mutex> subscribed_signals_lock(sm.get_subscribed_signals_mutex());
	map_subscription& s = sm.get_subscribed_signals();

	if(!signal)
		request.reply(AFB_ERRNO_INVALID_REQUEST);
	else
	{
		list_ptr_signal_t list_sig {signal->get_shared_ptr()};
		event_filter_t evt_filter = generate_filter(args);
		if(subscribe_unsubscribe_signals(request, subscribe, list_sig, evt_filter, s))
			request.reply();
		else
			request.reply(AFB_ERRNO_INTERNAL_ERROR);
	}
}

static void simple_unsubscribe_signal(afb::req request, signal_t* signal, json_object *args)
{
	simple_subscribe_unsubscribe_signal(request, signal, args, false);
}

static void simple_subscribe_signal(afb::req request, signal_t* signal, json_object *args)
{
	simple_subscribe_unsubscribe_signal(request, signal, args, true);
}

static void signal_verb(afb::req request, afb::received_data params)
{
	struct json_object* actionJ = nullptr, *optionsJ = nullptr;

	if (!get_json_param(request, actionJ))
		return;

	std::string action;

	signal_t* signal = (signal_t*) afb_req_get_vcbdata(request);

	if(json_object_object_get_ex(actionJ, "write", &optionsJ)) {
		if (afb_req_session_get_LOA(request) < AFB_SESSION_LOA_1)
			request.reply(AFB_ERRNO_BAD_API_STATE);
		else if(signal->get_writable() && signal->afb_verb_write_on_bus(optionsJ))
			request.reply(-1);
		else
			request.reply();
	}
	else if(json_object_object_get_ex(actionJ, "subscribe", &optionsJ))
	{
		simple_subscribe_signal(request, signal, optionsJ);
	}
	else if(json_object_object_get_ex(actionJ, "unsubscribe", &optionsJ))
	{
		simple_unsubscribe_signal(request, signal, optionsJ);
	}
	else if(json_object_is_type(actionJ, json_type_string))
	{
		action = json_object_get_string(actionJ);
		if(action == "get")
		{
			json_object *ret = signal->afb_verb_get_last_value();
			if (ret) {
				afb::data data = afb_data_json_c_hold(ret);
				request.reply(0, data);
			}
			else
			{
				request.reply(-1);
			}
		}
		else if(action == "subscribe")
		{
			simple_subscribe_signal(request, signal, nullptr);
		}
		else if(action == "unsubscribe")
		{
			simple_unsubscribe_signal(request, signal, nullptr);
		}
	}
	else
	{
		AFB_REQ_INFO(request, "invalid request %s", json_object_get_string(actionJ));
		request.reply(AFB_ERRNO_INVALID_REQUEST);
	}
}

static int add_verb(afb::api &api, std::shared_ptr<signal_t> sig, std::shared_ptr<diagnostic_message_t> diag_sig)
{
	struct afb_auth *auth = nullptr;
	std::string info = "Verb acting on 1 signal. Behavior depending on the json arguments provided.";
	std::string signame;
	void *s = nullptr;

	if (sig)
	{
		signame = sig->get_message()->get_parent()->get_name() + '/' + sig->get_name();
		if(! sig->get_permission().empty())
		{
			auth = reinterpret_cast<struct afb_auth*>(::malloc(sizeof *auth));
			if (auth == NULL)
				return -1;
			auth->type = afb_auth_Permission;
			auth->text = sig->get_permission().c_str();
		}
		s = (void*) sig.get();
	}
	else if(diag_sig)
	{
		signame = diag_sig->get_name();
		s = (void*) diag_sig.get();
	}
	else
	{
		return -1;
	}

	// JOBOL: Missing c++ verb cb handling to correctly add verbs
	if(api.add_verb(signame, info, afb::verbcb<signal_verb>, s, auth, 0, 0))
		AFB_WARNING("You got problem adding verb for the signal %s. Ignoring!", signame.c_str());

	return 0;
}

/*******************************************************************************
*
*			Binding initialization parts
*
*******************************************************************************/
/// @brief Initialize the binding.
///
/// @param[in] service Structure which represent the Application Framework Binder.
///
/// @return Exit code, zero if success.
static int init_binding(afb::api api)
{
	int ret = 0;
	application_t& application = application_t::instance();
	can_bus_t& can_bus_manager = application.get_can_bus_manager();

	can_bus_manager.start_threads();
	utils::signals_manager_t& sm = utils::signals_manager_t::instance();
	std::lock_guard<std::mutex> subscribed_signals_lock(sm.get_subscribed_signals_mutex());

	if (application.get_diagnostic_manager().is_initialized())
	{
		// Add a recurring dignostic message request to get engine speed at all times.
		openxc_DynamicField search_key = build_DynamicField("diagnostic_messages.engine.speed");
		struct utils::signals_found sf = sm.find_signals(search_key);

		if(sf.signals.empty() && sf.diagnostic_messages.size() == 1)
		{
			afb::req request = nullptr;

			struct event_filter_t event_filter;
			event_filter.frequency = sf.diagnostic_messages.front()->get_frequency();
			map_subscription& s = sm.get_subscribed_signals();

			subscribe_unsubscribe_diagnostic_messages(request, true, sf.diagnostic_messages, event_filter, s, true);
		}
	}

	if (!application.valid_active_message_set()) {
		AFB_API_ERROR(api, "No valid message set");
		return -1;
	}

#ifdef USE_FEATURE_J1939
	vect_ptr_msg_def_t current_messages_definition = application.get_messages_definition();
	for(std::shared_ptr<message_definition_t> message_definition: current_messages_definition)
	{
		if(message_definition->is_j1939())
		{
			std::shared_ptr<low_can_subscription_t> low_can_j1939 = std::make_shared<low_can_subscription_t>();
			application.set_subscription_address_claiming(low_can_j1939);

			ret = low_can_j1939->open_socket(message_definition->get_bus_device_name(),
								  J1939_ADDR_CLAIM_PROTOCOL);

			if(ret < 0)
			{
				AFB_API_ERROR(api, "Error open socket address claiming for j1939 protocol");
				return -1;
			}
			add_to_event_loop(low_can_j1939);
			break;
		}
	}
#endif
	if(ret)
		AFB_API_ERROR(api, "There was something wrong with the binding initialization.");

	return ret;
}

static int process_config(afb::api api, json_object *json_obj, application_t& application)
{
	int active_message_set = 0;
	json_object *dev_mapping = nullptr;
	json_object *config = nullptr;
	json_object *preinit = nullptr;
	json_object *postinit = nullptr;
	const char *ecu = nullptr;
	const char *diagnotic_bus = nullptr;

	AFB_API_DEBUG(api, "Config %s", json_object_to_json_string(json_obj));

	if(rp_jsonc_unpack(json_obj, "{sO}", "config", &config)) {
		AFB_API_ERROR(api, "No entry 'config' found in %s", json_object_to_json_string(json_obj));
		return -1;
	}
	if (rp_jsonc_unpack(config,   "{si s?s s?s s?o s?o}",
			      "active_message_set", &active_message_set,
			      "diagnostic_bus", &diagnotic_bus,
			      "default_j1939_name", &ecu,
			      "preinit", &preinit,
			      "postinit", &postinit)) {
		AFB_API_ERROR(api, "Invalid 'config' entry %s", json_object_to_json_string(config));
		return -1;
	}

	if(ecu)
	{
#ifdef USE_FEATURE_J1939
		application.set_default_j1939_ecu(ecu);
		AFB_API_INFO(api, "Default J1939 ECU name set to %s", ecu);
#else
		AFB_API_INFO(api, "J1939 feature disable, doing nothing");
#endif
	}

	application.set_active_message_set((uint8_t)active_message_set);

	if(rp_jsonc_unpack(config, "{so}",
			    "dev-mapping", &dev_mapping))
		return -1;

	if(application.get_can_bus_manager().set_can_devices(dev_mapping) < 0)
		return -1;

	/// Initialize Diagnostic manager that will handle obd2 requests.
	/// We pass by default the first CAN bus device to its Initialization.
	if(! diagnotic_bus)
		AFB_API_NOTICE(api, "Diagnostic Manager: no diagnostic bus specified. Service will run without the diagnostic manager.");
	else if(! application_t::instance().get_diagnostic_manager().initialize(diagnotic_bus))
	{
		AFB_API_ERROR(api, "Diagnostic Manager: not initialized. Problem initializing the diagnostic manager with the bus: %s", diagnotic_bus);
		return -1;
	}

	return 0;
}

struct search_json_object_t
{
	afb::api api;
	json_object *global;
};

static int found_json(void *closure, const rp_path_search_entry_t *entry)
{
	search_json_object_t *sj = reinterpret_cast<search_json_object_t*>(closure);
	json_object *obj = json_object_from_file(entry->path);
	if (obj == NULL)
		AFB_API_WARNING(sj->api, "Can't read JSON file %s", entry->path);
	else {
		AFB_API_DEBUG(sj->api, "Processing JSON file %s", entry->path);
		rp_jsonc_object_merge(sj->global, obj, rp_jsonc_merge_option_join_or_keep);
		json_object_put(obj);
	}
	return 0; /* continue */
}

static int found_so(void *closure, const rp_path_search_entry_t *entry)
{
	afb::api api = reinterpret_cast<afb_api_t>(closure);
	application_t& application = application_t::instance();
	plugin_store_t pstore = application.get_plugins();
	plugin_t *plugin = plugin_store_get_load(&pstore, entry->path, entry->path, nullptr);
	if (!plugin)
		AFB_WARNING("Can't load SO file %s", entry->path);
	else {
		const void *ptr = plugin_get_object(plugin, CANBUS_PLUGIN_EXPORT);
		const canbus_plugin_t *desc = to_canbus_plugin(ptr);
		if (!desc)
			AFB_WARNING("Not a plugin SO file %s", entry->path);
		else {
			AFB_INFO("Initialisation of plugin %s (%s)", desc->id, entry->path);
			int rc = desc->init(application);
			if (rc >= 0)
				AFB_INFO("Using plugin %s (%s)", desc->id, entry->path);
			else {
				AFB_ERROR("Failed to initialize plugin %s (%s)", desc->id, entry->path);
				desc = nullptr;
				return -1;
			}
		}
		if (!desc)
			plugin_store_drop(&pstore, plugin);
	}
	application.set_plugins(pstore);
	return 0; /* continue */
}

static int do_preinit(afb::api api, json_object *config)
{
	int rc = 0;
	rp_path_search_t *paths = nullptr;
	const char *str;
	json_object *obj;
	struct utils::signals_found all_signals;
	openxc_DynamicField search_key;
	search_json_object_t search_json_object;

	// set api's data to be application instance (very unuseful but legacy...)
	application_t& application = application_t::instance();
	api.set_userdata(&application);

	// default path search
	rc = rp_path_search_make_dirs(&paths, "${AFB_ROOTDIR}/etc:${AFB_ROOTDIR}/plugins:${CANBUS_PLUGINS_PATH}");
	if (rc >= 0 && 0 == rp_jsonc_unpack(config, "{ss}", "plugins-path", &str))
		// supplementary paths
		rc = rp_path_search_extend_dirs(&paths, str, 1);
	if (rc < 0) {
		AFB_API_ERROR(api, "Got error %s", strerror(-rc));
		goto end;
	}

	// collect json configs
	if (json_object_object_get_ex(config, "config", &obj))
		json_object_get(obj);
	else
		obj = json_object_new_object();

	search_json_object.api = api;
	search_json_object.global = obj;
	rc = rp_path_search_match(paths, RP_PATH_SEARCH_FILE|RP_PATH_SEARCH_FLEXIBLE|RP_PATH_SEARCH_RECURSIVE,
			NULL, "json", found_json, &search_json_object);
	application.set_config(obj);

	if(process_config(api, obj, application))
	{
		AFB_API_ERROR(api, "No config found or invalid JSON: %s", json_object_to_json_string(config));
		goto end;
	}

	// collect shared object plugins
	rc = rp_path_search_match(paths, RP_PATH_SEARCH_FILE|RP_PATH_SEARCH_FLEXIBLE|RP_PATH_SEARCH_RECURSIVE,
			NULL, "so", found_so, (afb_api_t)api);
	if (rc < 0)
		goto end;

	// check if something exists
	if(application.get_message_set().empty()) {
		AFB_API_ERROR(api, "No message_set defined");
		rc = -1;
		goto end;
	}

	// add verbs
	search_key = build_DynamicField("*");
	all_signals = utils::signals_manager_t::instance().find_signals(search_key);
	for(const auto& sig: all_signals.signals)
	{
		rc = add_verb(api, sig, nullptr);
		if (rc < 0)
			break;
	}
	if (rc < 0)
		goto end;

	// add verbs (diagnostic)
	if(application.get_diagnostic_manager().is_initialized())
	{
		for(const auto& sig: all_signals.diagnostic_messages)
		{
			rc = add_verb(api, nullptr, sig);
			if (rc < 0)
				break;
		}
	}
end:
	rp_path_search_unref(paths);
	return rc;
}

static int mainctl(afb::api api, afb::ctlid ctlid, afb::ctlarg ctlarg, void *userdata)
{
	int ret = 0;

	switch (ctlid)
	{
	case afb_ctlid_Pre_Init:
		ret = do_preinit(api, ctlarg->pre_init.config);
		break;
	case afb_ctlid_Init:
		ret = init_binding(api);
		break;
	default:
		break;
	}

	return ret;
}

#include "low-can-apidef.h"
