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

#include <vector>
#include <fcntl.h>

#include "can/can-bus.hpp"
#include "can/can-signals.hpp"
#include "can/can-message.hpp"
#include "obd2/diagnostic-manager.hpp"

#include "low-can-binding.hpp"

/**
 * @brief Class representing a configuration attached to the binding.
 *
 * @desc It regroups all needed objects instance from other class
 *  that will be used along the binding life. It gets a global vision 
 *  on which signals are implemented for that binding. 
 *  Here, it is only the definition of the class with predefined accessors
 *  methods used in the binding.
 *
 *  It will be the reference point to needed objects.
 */
class configuration_t
{
	private:
		can_bus_t can_bus_manager_ = can_bus_t(afb_daemon_rootdir_open_locale(binder_interface->daemon, "can_buses.json", O_RDONLY, NULL));
		diagnostic_manager_t diagnostic_manager_;
		uint8_t active_message_set_ = 0;

	public:
		configuration_t();

		const std::vector<obd2_signal_t> obd2_signals_;
		const std::vector<can_message_set_t> can_message_set_;
		const std::vector<std::vector<can_signal_t>> can_signals_;
		const std::vector<std::vector<can_message_definition_t>> can_message_definition_;

		configuration_t& get_configuration() const;

		can_bus_t& get_can_bus_manager();

		diagnostic_manager_t& get_diagnostic_manager() const;

		uint8_t get_active_message_set() const;

		const std::vector<can_message_set_t>& get_can_message_set() const;

		const std::vector<can_signal_t>& get_can_signals() const;

		const std::vector<can_message_definition_t>& get_can_message_definition() const;

		const std::vector<obd2_signal_t>& get_obd2_signals() const;

		uint32_t get_signal_id(obd2_signal_t& sig) const;

		uint32_t get_signal_id(can_signal_t& sig) const;

		void set_active_message_set(uint8_t id);

/*
		/// TODO: implement this function as method into can_bus class
		/// @brief Pre initialize actions made before CAN bus initialization
		/// @param[in] bus A CanBus struct defining the bus's metadata
		/// @param[in] writable Configure the controller in a writable mode. If false, it will be configured as "listen only" and will not allow writes or even CAN ACKs.
		/// @param[in] buses An array of all CAN buses.
		/// @param[in] busCount The length of the buses array.
		void pre_initialize(can_bus_dev_t* bus, bool writable, can_bus_dev_t* buses, const int busCount);
		/// TODO: implement this function as method into can_bus class
		/// @brief Post-initialize actions made after CAN bus initialization
		/// @param[in] bus A CanBus struct defining the bus's metadata
		/// @param[in] writable Configure the controller in a writable mode. If false, it will be configured as "listen only" and will not allow writes or even CAN ACKs.
		/// @param[in] buses An array of all CAN buses.
		/// @param[in] busCount The length of the buses array.
		void post_initialize(can_bus_dev_t* bus, bool writable, can_bus_dev_t* buses, const int busCount);
		/// TODO: implement this function as method into can_bus class
		/// @brief Check if the device is connected to an active CAN bus, i.e. it's received a message in the recent past.
		/// @return true if a message was received on the CAN bus within CAN_ACTIVE_TIMEOUT_S seconds.
		void logBusStatistics(can_bus_dev_t* buses, const int busCount);
		/// TODO: implement this function as method into can_bus class
		/// @brief Log transfer statistics about all active CAN buses to the debug log.
		/// @param[in] buses An array of active CAN buses.
		/// @param[in] busCount The length of the buses array.
		bool isBusActive(can_bus_dev_t* bus);
		*/
};

// Make a global variable pointer
extern configuration_t *config;

