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
		const std::vector<obd2_signal_t> obd2_signals_;
		const std::vector<can_message_set_t> can_message_set_;
		const std::vector<std::vector<can_signal_t>> can_signals_;
		const std::vector<std::vector<can_message_definition_t>> can_message_definition_;

		configuration_t& get_configuration()
		{ 
			return this;
		}

		can_bus_t& get_can_bus_manager()
		{
			return can_bus_manager_;
		}
		
		diagnostic_manager_t& get_diagnostic_manager()
		{
			return diagnostic_manager_;
		}

		uint8_t get_active_message_set()
		{
			return active_message_set_;
		}

		std::vector<can_message_set>& get_can_message_set()
		{
			return can_message_set_;
		}

		std::vector<can_signal>& get_can_signals()
		{
			return can_signal_[active_message_set_];
		}

		std::vector<CanSignal>& get_can_signals()
		{
			return SIGNALS[active_message_set_];
		}

		std::vector<can_message_definition>& get_can_message_definition()
		{
			return can_message_definition_[active_message_set_];
		}

		std::vector<obd2_signal_t>& get_obd2_signals()
		{
			return obd2_signals_;
		}

		std::vector<obd2_signal_t>& get_obd2_signals()
		{
			return OBD2_PIDS;
		}

		uint32_t get_signal_id(obd2_signal_t& sig)
		{
			return sig.get_pid();
		}

		uint32_t get_signal_id(const CanSignal& sig)
		{
			return sig.message->id;
		}

		void set_active_message_set(uint8_t id)
		{
			active_message_set_ = id;
		}
};