/*
 * Copyright (C) 2015, 2016 "IoT.bzh"
 * Author "Romain Forlot" <romain.forlot@iot.bzh>
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

#include <net/if.h>
#include <sys/socket.h>
#include <json-c/json.h>
#include <linux/can/raw.h>
#include <map>
#include <cerrno>
#include <vector>
#include <string>
#include <algorithm>

#include "can-bus.hpp"

#include "can-signals.hpp"
#include "can-decoder.hpp"
#include "../binding/application.hpp"
#include "../utils/signals.hpp"
#include "../utils/openxc-utils.hpp"

/// @brief Class destructor
///
/// @param[in] conf_file - Stop threads and unlock them to correctly finish them
/// even without any activity on the CAN bus.
can_bus_t::~can_bus_t()
{
	stop_threads();
	new_can_message_cv_.notify_one();
}

/// @brief Class constructor
///
/// @param[in] conf_file - handle to the json configuration file.
can_bus_t::can_bus_t(utils::config_parser_t conf_file)
	: conf_file_{conf_file}
{}

/// @brief Take a decoded message to determine if its value complies with the desired
/// filters.
///
/// @param[in] vehicle_message - The decoded message to be analyzed.
/// @param[in] can_subscription - the subscription which will be notified depending
///  on its filtering values. Filtering values are stored in the event_filtermember.
///
/// @return True if the value is compliant with event filter values, false if not...
bool can_bus_t::apply_filter(const openxc_VehicleMessage& vehicle_message, std::shared_ptr<low_can_subscription_t> can_subscription)
{
	bool send = false;
	if(is_valid(vehicle_message))
	{
		float min = can_subscription->get_min();
		float max = can_subscription->get_max();
		double value = get_numerical_from_DynamicField(vehicle_message);
		send = (value < min || value > max) ? false : true;
	}
	return send;
}

/// @brief Will make the decoding operation on a classic CAN message. It will not
/// handle CAN commands nor diagnostic messages that have their own method to get
/// this happens.
///
/// It will add to the vehicle_message queue the decoded message and tell the event push
/// thread to process it.
///
/// @param[in] can_message - a single CAN message from the CAN socket read, to be decode.
///
/// @return How many signals has been decoded.
void can_bus_t::process_can_signals(const can_message_t& can_message, std::map<int, std::shared_ptr<low_can_subscription_t> >& s)
{
	int subscription_id = can_message.get_sub_id();
	openxc_DynamicField decoded_message;
	openxc_VehicleMessage vehicle_message;

	// First we have to found which can_signal_t it is
	std::shared_ptr<low_can_subscription_t> sig = s[subscription_id];

	if( s.find(subscription_id) != s.end() && afb_event_is_valid(s[subscription_id]->get_event()))
	{
		bool send = true;
		decoded_message = decoder_t::translate_signal(*sig->get_can_signal(), can_message, &send);
		openxc_SimpleMessage s_message = build_SimpleMessage(sig->get_name(), decoded_message);
		vehicle_message = build_VehicleMessage(s_message, can_message.get_timestamp());

		if(send && apply_filter(vehicle_message, sig))
		{
			std::lock_guard<std::mutex> decoded_can_message_lock(decoded_can_message_mutex_);
			push_new_vehicle_message(subscription_id, vehicle_message);
			AFB_DEBUG("%s CAN signals processed.",  sig->get_name().c_str());
		}
	}
}

/// @brief Will make the decoding operation on a diagnostic CAN message.Then it find the subscribed signal
/// corresponding and will add the vehicle_message to the queue of event to pushed before notifying
/// the event push thread to process it.
///
/// @param[in] manager - the diagnostic manager object that handle diagnostic communication
/// @param[in] can_message - a single CAN message from the CAN socket read, to be decode.
///
/// @return How many signals has been decoded.
void can_bus_t::process_diagnostic_signals(diagnostic_manager_t& manager, const can_message_t& can_message, std::map<int, std::shared_ptr<low_can_subscription_t> >& s)
{
	int subscription_id = can_message.get_sub_id();

	openxc_VehicleMessage vehicle_message = manager.find_and_decode_adr(can_message);
	if (can_message.get_timestamp())
		{vehicle_message.timestamp = can_message.get_timestamp();}
	if( (vehicle_message.has_simple_message && vehicle_message.simple_message.has_name) &&
		s.find(subscription_id) != s.end() && afb_event_is_valid(s[subscription_id]->get_event()))
	{
		if (apply_filter(vehicle_message, s[subscription_id]))
		{
			std::lock_guard<std::mutex> decoded_can_message_lock(decoded_can_message_mutex_);
			push_new_vehicle_message(subscription_id, vehicle_message);
			AFB_DEBUG("%s CAN signals processed.",  s[subscription_id]->get_name().c_str());
		}
	}
}

/// @brief thread to decoding raw CAN messages.
///
///  Depending on the nature of message, if arbitration ID matches ID for a diagnostic response
///  then decoding a diagnostic message else use classic CAN signals decoding functions.
///
/// It will take from the can_message_q_ queue the next can message to process then it search
///  about signal subscribed if there is a valid afb_event for it. We only decode signal for which a
///  subscription has been made. Can message will be decoded using translate_signal that will pass it to the
///  corresponding decoding function if there is one assigned for that signal. If not, it will be the default
///  noopDecoder function that will operate on it.
///
///  TODO: make diagnostic messages parsing optionnal.
void can_bus_t::can_decode_message()
{
	utils::signals_manager_t& sm = utils::signals_manager_t::instance();

	while(is_decoding_)
	{
		std::unique_lock<std::mutex> can_message_lock(can_message_mutex_);
		new_can_message_cv_.wait(can_message_lock);
		while(!can_message_q_.empty())
		{
			const can_message_t can_message = next_can_message();
			can_message_lock.unlock();

			{
				std::lock_guard<std::mutex> subscribed_signals_lock(sm.get_subscribed_signals_mutex());
				std::map<int, std::shared_ptr<low_can_subscription_t> >& s = sm.get_subscribed_signals();
				if(application_t::instance().get_diagnostic_manager().is_diagnostic_response(can_message))
					{process_diagnostic_signals(application_t::instance().get_diagnostic_manager(), can_message, s);}
				else
					{process_can_signals(can_message, s);}
			}
			can_message_lock.lock();
		}
		new_decoded_can_message_.notify_one();
		can_message_lock.unlock();
	}
}

/// @brief thread to push events to suscribers. It will read subscribed_signals map to look
/// which are events that has to be pushed.
void can_bus_t::can_event_push()
{
	json_object* jo;
	utils::signals_manager_t& sm = utils::signals_manager_t::instance();

	while(is_pushing_)
	{
		std::unique_lock<std::mutex> decoded_can_message_lock(decoded_can_message_mutex_);
		new_decoded_can_message_.wait(decoded_can_message_lock);
		while(!vehicle_message_q_.empty())
		{
			std::pair<int, openxc_VehicleMessage> v_message = next_vehicle_message();
			decoded_can_message_lock.unlock();
			{
				std::lock_guard<std::mutex> subscribed_signals_lock(sm.get_subscribed_signals_mutex());
				std::map<int, std::shared_ptr<low_can_subscription_t> >& s = sm.get_subscribed_signals();
				if(s.find(v_message.first) != s.end() && afb_event_is_valid(s[v_message.first]->get_event()))
				{
					jo = json_object_new_object();
					jsonify_vehicle(v_message.second, jo);
					if(afb_event_push(s[v_message.first]->get_event(), jo) == 0)
					{
						if(v_message.second.has_diagnostic_response)
							{on_no_clients(s[v_message.first], v_message.second.diagnostic_response.pid, s);}
						else
							{on_no_clients(s[v_message.first], s);}
					}
				}
			}
			decoded_can_message_lock.lock();
		}
		decoded_can_message_lock.unlock();
	}
}

/// @brief Will initialize threads that will decode
///  and push subscribed events.
void can_bus_t::start_threads()
{
	is_decoding_ = true;
	th_decoding_ = std::thread(&can_bus_t::can_decode_message, this);
	th_decoding_.detach();

	is_pushing_ = true;
	th_pushing_ = std::thread(&can_bus_t::can_event_push, this);
	th_pushing_.detach();
}

/// @brief Will stop all threads holded by can_bus_t object
///  which are decoding and pushing then will wait that's
/// they'll finish their job.
void can_bus_t::stop_threads()
{
	is_decoding_ = false;
	is_pushing_ = false;
}

/// @brief return new_can_message_cv_ member
///
/// @return  return new_can_message_cv_ member
std::condition_variable& can_bus_t::get_new_can_message_cv()
{
	return new_can_message_cv_;
}

/// @brief return can_message_mutex_ member
///
/// @return  return can_message_mutex_ member
std::mutex& can_bus_t::get_can_message_mutex()
{
	return can_message_mutex_;
}

/// @brief Return first can_message_t on the queue
///
/// @return a can_message_t
const can_message_t can_bus_t::next_can_message()
{
	can_message_t can_msg;

	if(!can_message_q_.empty())
	{
		can_msg = can_message_q_.front();
		can_message_q_.pop();
		AFB_DEBUG("Here is the next can message : id %X, length %X, data %02X%02X%02X%02X%02X%02X%02X%02X", can_msg.get_id(), can_msg.get_length(),
			can_msg.get_data()[0], can_msg.get_data()[1], can_msg.get_data()[2], can_msg.get_data()[3], can_msg.get_data()[4], can_msg.get_data()[5], can_msg.get_data()[6], can_msg.get_data()[7]);
		return can_msg;
	}

	return can_msg;
}

/// @brief Push a can_message_t into the queue
///
/// @param[in] can_msg - the const reference can_message_t object to push into the queue
void can_bus_t::push_new_can_message(const can_message_t& can_msg)
{
	can_message_q_.push(can_msg);
}

/// @brief Return first openxc_VehicleMessage on the queue
///
/// @return a openxc_VehicleMessage containing a decoded can message
std::pair<int, openxc_VehicleMessage> can_bus_t::next_vehicle_message()
{
	std::pair<int, openxc_VehicleMessage> v_msg;

	if(! vehicle_message_q_.empty())
	{
		v_msg = vehicle_message_q_.front();
		vehicle_message_q_.pop();
		AFB_DEBUG("next vehicle message poped");
		return v_msg;
	}

	return v_msg;
}

/// @brief Push a openxc_VehicleMessage into the queue
///
/// @param[in] v_msg - const reference openxc_VehicleMessage object to push into the queue
void can_bus_t::push_new_vehicle_message(int subscription_id, const openxc_VehicleMessage& v_msg)
{
	vehicle_message_q_.push(std::make_pair(subscription_id, v_msg));
}

/// @brief Fills the CAN device map member with value from device
/// mapping configuration file read at initialization.
void can_bus_t::set_can_devices()
{
	if(conf_file_.check_conf())
	{
		can_devices_mapping_ = conf_file_.get_devices_name();

		if(can_devices_mapping_.empty())
		{
			AFB_ERROR("No mapping found in config file: '%s'. Check it that it have a CANbus-mapping section.",
				conf_file_.filepath().c_str());
		}
	}
}


/// @brief Return the CAN device index from the map
/// map are sorted so index depend upon alphabetical sorting.
int can_bus_t::get_can_device_index(const std::string& bus_name) const
{
	int i = 0;
	for(const auto& d: can_devices_mapping_)
	{
		if(d.first == bus_name)
			break;
		i++;
	}
	return i;
}

/// @brief Return CAN device name from a logical CAN device name gotten from
/// the signals.json description file which comes from a CAN databases file in
/// general.
const std::string can_bus_t::get_can_device_name(const std::string& id_name) const
{
	std::string ret = "";
	for(const auto& d: can_devices_mapping_)
	{
		if(d.first == id_name)
		{
			ret = d.second;
			break;
		}
	}
	return ret;
}
