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

#include <map>
#include <vector>
#include <string>
#include <memory>

#include "../can/can-bus.hpp"
#include "../can/can-message-set.hpp"
#include "../can/can-signals.hpp"
#include "../diagnostic/diagnostic-manager.hpp"

#include "low-can-hat.hpp"

///
/// @brief Class representing a configuration attached to the binding.
///
/// It regroups all needed objects instance from other class
///  that will be used along the binding life. It gets a global vision 
///  on which signals are implemented for that binding. 
///  Here, it is only the definition of the class with predefined accessors
///  methods used in the binding.
///
///  It will be the reference point to needed objects.
///
class application_t
{
	private:
		can_bus_t can_bus_manager_; ///< instanciate the CAN bus manager. It's the one in charge of initialize the CAN bus devices.
		diagnostic_manager_t diagnostic_manager_; ///< Diagnostic manager use to manage diagnostic message communication.
		uint8_t active_message_set_ = 0; ///< Which is the active message set ? Default to 0.

		std::vector<std::shared_ptr<can_message_set_t> > can_message_set_; ///< Vector holding all message set from JSON signals description file

		application_t(); ///< Private constructor with implementation generated by the AGL generator.

	public:
		static application_t& instance();

		can_bus_t& get_can_bus_manager();

		const std::string get_diagnostic_bus() const;

		diagnostic_manager_t& get_diagnostic_manager() ;

		uint8_t get_active_message_set() const;

		std::vector<std::shared_ptr<can_message_set_t> > get_can_message_set();

		std::vector<std::shared_ptr<can_signal_t> > get_all_can_signals();

		std::vector<std::shared_ptr<diagnostic_message_t> >& get_diagnostic_messages();

		const std::vector<std::string>& get_signals_prefix() const;

		std::vector<std::shared_ptr<can_message_definition_t> >& get_can_message_definition();

		uint32_t get_signal_id(diagnostic_message_t& sig) const;

		uint32_t get_signal_id(can_signal_t& sig) const;

		void set_active_message_set(uint8_t id);

		std::shared_ptr<diagnostic_message_t> get_diagnostic_message(const std::string& message_name) const;
		DiagnosticRequest* get_request_from_diagnostic_message(const std::string& message_name) const;
/*
		/// TODO: implement this function as method into can_bus class
		/// @brief Pre initialize actions made before CAN bus initialization
		/// @param[in] bus A CanBus struct defining the bus's metadata
		/// @param[in] writable Configure the controller in a writable mode. If false, it will be configured as "listen only" and will not allow writes or even CAN ACKs.
		/// @param[in] buses An array of all CAN buses.
		void pre_initialize(can_bus_dev_t* bus, bool writable, can_bus_dev_t* buses, const int busCount);
		/// TODO: implement this function as method into can_bus class
		/// @brief Post-initialize actions made after CAN bus initialization
		/// @param[in] bus A CanBus struct defining the bus's metadata
		/// @param[in] writable Configure the controller in a writable mode. If false, it will be configured as "listen only" and will not allow writes or even CAN ACKs.
		/// @param[in] buses An array of all CAN buses.
		void post_initialize(can_bus_dev_t* bus, bool writable, can_bus_dev_t* buses, const int busCount);
		/// TODO: implement this function as method into can_bus class
		/// @brief Check if the device is connected to an active CAN bus, i.e. it's received a message in the recent past.
		/// @return true if a message was received on the CAN bus within CAN_ACTIVE_TIMEOUT_S seconds.
		void logBusStatistics(can_bus_dev_t* buses, const int busCount);
		/// TODO: implement this function as method into can_bus class
		/// @brief Log transfer statistics about all active CAN buses to the debug log.
		/// @param[in] buses An array of active CAN buses.
		bool isBusActive(can_bus_dev_t* bus);
		*/
};
