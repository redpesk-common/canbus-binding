/*
 * Copyright (C) 2015, 2016, 2017 "IoT.bzh"
 * Author "Romain Forlot" <romain.forlot@iot.bzh>
 * Author "Loïc Collignon" <loic.collignon@iot.bzh>
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

#pragma once

#include <utility>
#include <mutex>
#include <queue>
#include <thread>
#include <linux/can.h>
#include <condition_variable>

#include "openxc.pb.h"
#include "can-message.hpp"
#include "../utils/config-parser.hpp"
#include "../binding/low-can-subscription.hpp"

#define CAN_ACTIVE_TIMEOUT_S 30

class diagnostic_manager_t;

/// @brief Object used to handle decoding and manage event queue to be pushed.
///
/// This object is also used to initialize can_bus_dev_t object after reading
/// json conf file describing the CAN devices to use. Thus, those object will read
/// on the device the CAN frame and push them into the can_bus_t can_message_q_ queue.
///
/// That queue will later be decoded and pushed to subscribers.
class can_bus_t
{
private:
	utils::config_parser_t conf_file_; ///< configuration file handle used to initialize can_bus_dev_t objects.

	bool apply_filter(const openxc_VehicleMessage& vehicle_message, std::shared_ptr<low_can_subscription_t> can_subscription);
	void process_can_signals(const can_message_t& can_message, std::map<int, std::shared_ptr<low_can_subscription_t> >& s);
	void process_diagnostic_signals(diagnostic_manager_t& manager, const can_message_t& can_message, std::map<int, std::shared_ptr<low_can_subscription_t> >& s);

	void can_decode_message();
	std::thread th_decoding_; ///< thread that will handle decoding a can frame
	bool is_decoding_ = false; ///< boolean member controling thread while loop

	void can_event_push();
	std::thread th_pushing_; ///< thread that will handle pushing decoded can frame to subscribers
	bool is_pushing_ = false; ///< boolean member controling thread while loop

	std::condition_variable new_can_message_cv_; ///< condition_variable use to wait until there is a new CAN message to read
	std::mutex can_message_mutex_; ///< mutex protecting the can_message_q_ queue.
	std::queue <can_message_t> can_message_q_; ///< queue that will store can_message_t to be decoded

	std::condition_variable new_decoded_can_message_; ///< condition_variable use to wait until there is a new vehicle message to read from the queue vehicle_message_q_
	std::mutex decoded_can_message_mutex_;  ///< mutex protecting the vehicle_message_q_ queue.
	std::queue <std::pair<int, openxc_VehicleMessage> > vehicle_message_q_; ///< queue that will store openxc_VehicleMessage to be pushed

	std::vector<std::pair<std::string, std::string> > can_devices_mapping_; ///< can_devices_mapping_ - holds a mapping between logical CAN devices names and linux CAN devices names.
public:
	explicit can_bus_t(utils::config_parser_t conf_file);
	can_bus_t(can_bus_t&&);
	~can_bus_t();

	void set_can_devices();
	int get_can_device_index(const std::string& bus_name) const;
	const std::string get_can_device_name(const std::string& id_name) const;

	void start_threads();
	void stop_threads();

	const can_message_t next_can_message();
	void push_new_can_message(const can_message_t& can_msg);
	std::mutex& get_can_message_mutex();
	std::condition_variable& get_new_can_message_cv();

	std::pair<int, openxc_VehicleMessage> next_vehicle_message();
	void push_new_vehicle_message(int subscription_id, const openxc_VehicleMessage& v_msg);
};
