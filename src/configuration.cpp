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

#include "configuration.hpp"

#include "utils/signals.hpp"

/// @brief Return singleton instance of configuration object.
configuration_t& configuration_t::instance()
{
	static configuration_t config;
	return config;
}

can_bus_t& configuration_t::get_can_bus_manager()
{
	return can_bus_manager_;
}

const std::map<std::string, std::shared_ptr<can_bus_dev_t>>& configuration_t::get_can_bus_devices()
{
	return can_bus_manager_.get_can_devices();
}

diagnostic_manager_t& configuration_t::get_diagnostic_manager()
{
	return diagnostic_manager_;
}

uint8_t configuration_t::get_active_message_set() const
{
	return active_message_set_;
}

const std::vector<can_message_set_t>& configuration_t::get_can_message_set()
{
	return can_message_set_;
}

std::vector<can_signal_t>& configuration_t::get_can_signals()
{
	return can_signals_[active_message_set_];
}

std::vector<diagnostic_message_t>& configuration_t::get_diagnostic_messages()
{
	return diagnostic_messages_[active_message_set_];
}

const std::vector<can_message_definition_t>& configuration_t::get_can_message_definition()
{
	return can_message_definition_[active_message_set_];
}

const can_message_definition_t& configuration_t::get_can_message_definition(std::uint8_t message_set_id, std::uint8_t message_id)
{
	return can_message_definition_[message_set_id][message_id];
}

uint32_t configuration_t::get_signal_id(diagnostic_message_t& sig) const
{
	return sig.get_pid();
}

uint32_t configuration_t::get_signal_id(can_signal_t& sig) const
{
	return sig.get_message().get_id();
}

void configuration_t::set_active_message_set(uint8_t id)
{
	active_message_set_ = id;
}

/// @brief return diagnostic messages name found searching through diagnostic messages list.
///
/// @param[in] key - can contain numeric or string value in order to search against
///   can signals or obd2 signals name.
/// @param[out] found_signals - Vector of signals name found.
///
void configuration_t::find_diagnostic_messages(const openxc_DynamicField &key, std::vector<diagnostic_message_t*>& found_signals)
{
	switch(key.type)
	{
		case openxc_DynamicField_Type::openxc_DynamicField_Type_STRING:
				lookup_signals_by_name(key.string_value, diagnostic_messages_[active_message_set_], found_signals);
			break;
		case openxc_DynamicField_Type::openxc_DynamicField_Type_NUM:
				lookup_signals_by_id(key.numeric_value, diagnostic_messages_[active_message_set_], found_signals);
			break;
		default:
			ERROR(binder_interface, "find_diagnostic_messages: wrong openxc_DynamicField specified. Use openxc_DynamicField_Type_NUM or openxc_DynamicField_Type_STRING type only.");
			break;
	}
	DEBUG(binder_interface, "find_diagnostic_messages: Found %d signal(s)", (int)found_signals.size());
}

/// @brief return signals name found searching through CAN signals list.
///
/// @param[in] key - can contain numeric or string value in order to search against
///   can signals or obd2 signals name.
/// @param[out] found_signals - provided vector to fill with pointer to matched signals.
///
void configuration_t::find_can_signals(const openxc_DynamicField& key, std::vector<can_signal_t*>& found_signals)
{
	switch(key.type)
	{
		case openxc_DynamicField_Type::openxc_DynamicField_Type_STRING:
			lookup_signals_by_name(std::string(key.string_value), can_signals_[active_message_set_], found_signals);
			break;
		case openxc_DynamicField_Type::openxc_DynamicField_Type_NUM:
			lookup_signals_by_id(key.numeric_value, can_signals_[active_message_set_], found_signals);
			break;
		default:
			ERROR(binder_interface, "find_can_signals: wrong openxc_DynamicField specified. Use openxc_DynamicField_Type_NUM or openxc_DynamicField_Type_STRING type only.");
			break;
	}
	DEBUG(binder_interface, "find_can_signals: Found %d signal(s)", (int)found_signals.size());
}
