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

#pragma once

#include <mutex>
#include <queue>
#include <thread>
#include <linux/can.h>
#include <condition_variable>

#include "timer.hpp"
#include "openxc.pb.h"
#include "can-signals.hpp"
#include "can-message.hpp"
#include "low-can-binding.hpp"

// TODO actual max is 32 but dropped to 24 for memory considerations
#define MAX_ACCEPTANCE_FILTERS 24
// TODO this takes up a ton of memory
#define MAX_DYNAMIC_MESSAGE_COUNT 12

#define CAN_ACTIVE_TIMEOUT_S 30

class can_bus_dev_t;

/** 
 * @class can_bus_t
 * @brief Object used to handle decoding and manage event queue to be pushed.
 *
 * This object is also used to initialize can_bus_dev_t object after reading 
 * json conf file describing the CAN devices to use. Thus, those object will read 
 * on the device the CAN frame and push them into the can_bus_t can_message_q_ queue.
 *
 * That queue will be later used to be decoded and pushed to subscribers.
 */
class can_bus_t {
	private:
		int conf_file_; /*!< conf_file_ - configuration file handle used to initialize can_bus_dev_t objects.*/

		void can_decode_message();
		std::thread th_decoding_; /*!< thread that'll handle decoding a can frame */
		bool is_decoding_; /*!< boolean member controling thread while loop*/

		void can_event_push();
		std::thread th_pushing_; /*!<  thread that'll handle pushing decoded can frame to subscribers */
		bool is_pushing_; /*!< boolean member controling thread while loop*/

		std::condition_variable new_can_message_cv_; /*!< condition_variable use to wait until there is a new CAN message to read*/
		std::mutex can_message_mutex_; /*!< mutex protecting the can_message_q_ queue.*/
		std::queue <can_message_t> can_message_q_; /*!< queue that'll store can_message_t to decoded */

		std::condition_variable new_decoded_can_message_; /*!< condition_variable use to wait until there is a new vehicle message to read from the queue vehicle_message_q_*/
		std::mutex decoded_can_message_mutex_;  /*!< mutex protecting the vehicle_message_q_ queue.*/
		std::queue <openxc_VehicleMessage> vehicle_message_q_; /*!< queue that'll store openxc_VehicleMessage to pushed */

		std::map<std::string, std::shared_ptr<can_bus_dev_t>> can_devices_m_; /*!< Can device map containing all can_bus_dev_t objects initialized during init_can_dev function*/

	public:
		can_bus_t(int conf_file);

		int init_can_dev();
		std::vector<std::string> read_conf();
		

		void start_threads();
		void stop_threads();

		can_message_t next_can_message();
		void push_new_can_message(const can_message_t& can_msg);		
		std::mutex& get_can_message_mutex();
		std::condition_variable& get_new_can_message_cv_();

		openxc_VehicleMessage next_vehicle_message();
		void push_new_vehicle_message(const openxc_VehicleMessage& v_msg);

		std::map<std::string, std::shared_ptr<can_bus_dev_t>> get_can_devices();
};


/**
 * @class can_bus_dev_t 
 *
 * @brief Object representing a can device. Handle opening, closing and reading on the
 *  socket. This is the low level object to be use by can_bus_t.
 */
class can_bus_dev_t {
	private:
		int32_t id_; /*!< int32_t id_ - an identifier used through binding that refer to that device*/
		std::string device_name_; /*!< std::string device_name_ - name of the linux device handling the can bus. Generally vcan0, can0, etc. */
		int can_socket_; /*!< socket handler for the can device */
		bool is_fdmode_on_; /*!< boolean telling if whether or not the can socket use fdmode. */
		struct sockaddr_can txAddress_; /*!< internal member using to bind to the socket */
		
		std::thread th_reading_; /*!< Thread handling read the socket can device filling can_message_q_ queue of can_bus_t */
		bool is_running_; /*!< boolean telling whether or not reading is running or not */
		
		void can_reader(can_bus_t& can_bus);

	public:
		can_bus_dev_t(const std::string& dev_name);

		int open();
		int close();
		

		void start_reading(can_bus_t& can_bus);

		void stop_reading();

		std::pair<struct canfd_frame&, size_t> read();
		
		int send_can_message(can_message_t& can_msg);
};

/** TODO: implement this function as method into can_bus class
 * @fn void pre_initialize(can_bus_dev_t* bus, bool writable, can_bus_dev_t* buses, const int busCount);
 * @brief Pre initialize actions made before CAN bus initialization
 *
 * @param[in] can_bus_dev_t bus - A CanBus struct defining the bus's metadata
 * @param[in] bool writable - configure the controller in a writable mode. If false, it will be
 *		configured as "listen only" and will not allow writes or even CAN ACKs.
 * @param[in] buses - An array of all CAN buses.
 * @param[in] int busCount - The length of the buses array.
 */
void pre_initialize(can_bus_dev_t* bus, bool writable, can_bus_dev_t* buses, const int busCount);

/** TODO: implement this function as method into can_bus class
 * @fn void post_initialize(can_bus_dev_t* bus, bool writable, can_bus_dev_t* buses, const int busCount);
 * @brief Post-initialize actions made after CAN bus initialization
 *
 * @param[in] bus - A CanBus struct defining the bus's metadata
 * @param[in] writable - configure the controller in a writable mode. If false, it will be
 *		configured as "listen only" and will not allow writes or even CAN ACKs.
 * @param[in] buses - An array of all CAN buses.
 * @param[in] busCount - The length of the buses array.
 */
void post_initialize(can_bus_dev_t* bus, bool writable, can_bus_dev_t* buses, const int busCount);

/** TODO: implement this function as method into can_bus class
 * @fn bool isBusActive(can_bus_dev_t* bus);
 * @brief Check if the device is connected to an active CAN bus, i.e. it's
 * received a message in the recent past.
 *
 * @return true if a message was received on the CAN bus within
 * CAN_ACTIVE_TIMEOUT_S seconds.
 */
bool isBusActive(can_bus_dev_t* bus);

/** TODO: implement this function as method into can_bus class
 *
 * @fn void logBusStatistics(can_bus_dev_t* buses, const int busCount);
 * @brief Log transfer statistics about all active CAN buses to the debug log.
 *
 * @param[in] buses - an array of active CAN buses.
 * @param[in] busCount - the length of the buses array.
 */
void logBusStatistics(can_bus_dev_t* buses, const int busCount);