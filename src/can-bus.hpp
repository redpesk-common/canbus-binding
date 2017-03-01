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

		/**
		 * @brief thread to decoding raw CAN messages. 
		 *
		 * @desc It will take from the can_message_q_ queue the next can message to process then it will search 
		 *  about signal subscribed if there is a valid afb_event for it. We only decode signal for which a 
		 *  subscription has been made. Can message will be decoded using translateSignal that will pass it to the
		 *  corresponding decoding function if there is one assigned for that signal. If not, it will be the default
		 *  noopDecoder function that will operate on it.
		 */
		void can_decode_message();
		std::thread th_decoding_; /*!< thread that'll handle decoding a can frame */
		bool is_decoding_; /*!< boolean member controling thread while loop*/

		/**
		 * @brief thread to push events to suscribers. It will read subscribed_signals map to look 
		 * which are events that has to be pushed.
		 */
		void can_event_push();
		std::thread th_pushing_; /*!<  thread that'll handle pushing decoded can frame to subscribers */
		bool is_pushing_; /*!< boolean member controling thread while loop*/

		std::condition_variable new_can_message_; /*!< condition_variable use to wait until there is a new CAN message to read*/
		std::mutex can_message_mutex_; /*!< mutex protecting the can_message_q_ queue.*/
		bool has_can_message_; /*!< boolean members that control whether or not there is can_message into the queue */
		std::queue <can_message_t> can_message_q_; /*!< queue that'll store can_message_t to decoded */

		std::condition_variable new_decoded_can_message_; /*!< condition_variable use to wait until there is a new vehicle message to read from the queue vehicle_message_q_*/
		std::mutex decoded_can_message_mutex_;  /*!< mutex protecting the vehicle_message_q_ queue.*/
		bool has_vehicle_message_; /*!< boolean members that control whether or not there is openxc_VehicleMessage into the queue */
		std::queue <openxc_VehicleMessage> vehicle_message_q_; /*!< queue that'll store openxc_VehicleMessage to pushed */

		std::map<std::string, std::shared_ptr<can_bus_dev_t>> can_devices_m_; /*!< Can device map containing all can_bus_dev_t objects initialized during init_can_dev function*/

	public:
		/**
		 * @brief Class constructor
		 *
		 * @param struct afb_binding_interface *interface between daemon and binding
		 * @param int file handle to the json configuration file.
		 */
		can_bus_t(int conf_file);
		
		/**
		 * @brief Will initialize can_bus_dev_t objects after reading 
		 * the configuration file passed in the constructor.
		 */
		int init_can_dev();

		/**
		 * @brief read the conf_file_ and will parse json objects
		 * in it searching for canbus objects devices name.
		 *
		 * @return Vector of can bus device name string.
		 */
		std::vector<std::string> read_conf();
		
		/**
		 * @brief Will initialize threads that will decode
		 *  and push subscribed events.
		 */
		void start_threads();

		/**
		 * @brief Will stop all threads holded by can_bus_t object
		 *  which are decoding and pushing then will wait that's 
		 * they'll finish their job.
		 */
		void stop_threads();

		/**
		 * @brief Telling if the decoding thread is running.
		 *  This is the boolean value on which the while loop
		 *  take its condition. Set it to false will stop the 
		 *  according thread.
		 *
		 * @return true if decoding thread is running, false if not.
		 */
		bool is_decoding();

		/**
		 * @brief Telling if the pushing thread is running
		 *  This is the boolean value on which the while loop
		 *  take its condition. Set it to false will stop the 
		 *  according thread.
		 *
		 * @return true if pushing thread is running, false if not.
		 */
		bool is_pushing();

		/**
		 * @brief Return first can_message_t on the queue 
		 *
		 * @return a can_message_t 
		 */
		can_message_t next_can_message();
		
		/**
		 * @brief Push a can_message_t into the queue
		 *
		 * @param the const reference can_message_t object to push into the queue
		 */
		void push_new_can_message(const can_message_t& can_msg);		
		
		/**
		 * @brief return can_message_mutex_ member
		 *
		 * @return  return can_message_mutex_ member
		 */
		std::mutex& get_can_message_mutex();
		
		/**
		 * @brief return new_can_message_ member
		 *
		 * @return  return new_can_message_ member
		 */
		std::condition_variable& get_new_can_message();


		/**
		 * @brief Return first openxc_VehicleMessage on the queue 
		 *
		 * @return a openxc_VehicleMessage containing a decoded can message
		 */
		openxc_VehicleMessage next_vehicle_message();
		
		/**
		 * @brief Push a openxc_VehicleMessage into the queue
		 *
		 * @param the const reference openxc_VehicleMessage object to push into the queue
		 */
		void push_new_vehicle_message(const openxc_VehicleMessage& v_msg);

		/**
		 * @brief Return a map with the can_bus_dev_t initialized
		 *
		 * @return map can_bus_dev_m_ map
		 */
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
		std::string device_name_; /*!< std::string device_name_ - name of the linux device handling the can bus. Generally vcan0, can0, etc. */
		int can_socket_; /*!< socket handler for the can device */
		bool is_fdmode_on_; /*!< boolean telling if whether or not the can socket use fdmode. */
		struct sockaddr_can txAddress_; /*!< internal member using to bind to the socket */
		
		std::thread th_reading_; /*!< Thread handling read the socket can device filling can_message_q_ queue of can_bus_t */
		bool is_running_; /*!< boolean telling whether or not reading is running or not */
		
		/**
		*
		* @brief Thread function used to read the can socket.
		*
		* @param[in] can_bus_dev_t object to be used to read the can socket
		* @param[in] can_bus_t object used to fill can_message_q_ queue
		*/
		void can_reader(can_bus_t& can_bus);

	public:
		/**
		 * @brief Class constructor 
		 * 
		 * @param const string representing the device name into the linux /dev tree
		 */
		can_bus_dev_t(const std::string& dev_name);

		/**
		 * @brief Open the can socket and returning it 
		 *
		 * @return 
		 */
		int open();
		
		/**
		 * @brief Open the can socket and returning it 
		 *
		 * @return 
		 */
		int close();
		
		/**
		 * @brief Telling if the reading thread is running
		 *  This is the boolean value on which the while loop
		 *  take its condition. Set it to false will stop the 
		 *  according thread.
		 *
		 * @return true if reading thread is running, false if not.
		 */
		bool is_running();
		
		/**
 		 * @brief start reading threads and set flag is_running_
		 *
		 * @param can_bus_t reference can_bus_t. it will be passed to the thread 
		 *  to allow using can_bus_t queue.
 		 */
		void start_reading(can_bus_t& can_bus);

		/**
 		 * @brief stop the reading thread setting flag is_running_ to false and
		 * and wait that the thread finish its job.
		 */
		void stop_reading();

		/**
 		* @brief Read the can socket and retrieve canfd_frame
		*
		* @param const struct afb_binding_interface* interface pointer. Used to be able to log 
		*  using application framework logger.
 		*/
		std::pair<struct canfd_frame&, size_t> read();
		
		/**
		* @brief Send a can message from a can_message_t object.
		* 
		* @param const can_message_t& can_msg: the can message object to send 
		* @param const struct afb_binding_interface* interface pointer. Used to be able to log 
		*  using application framework logger.
		*/
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