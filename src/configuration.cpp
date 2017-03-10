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

// Pre-defined OBD-II PIDs to query for if supported by the vehicle.
 std::vector<obd2_signal_t> OBD2_PIDS = {
	obd2_signal_t(0x04, "engine.load", 0, 100, POURCENT, 5, false),
	obd2_signal_t(0x05, "engine.coolant.temperature", -40, 215, DEGREES_CELSIUS, 1, false),
	obd2_signal_t(0x0a, "fuel.pressure", 0, 765, KPA, 1, false),
	obd2_signal_t(0x0b, "intake.manifold.pressure", 0, 255, KPA, 1, false),
	obd2_signal_t(0x0c, "engine.speed", 0, 16383, RPM, 5, false),
	obd2_signal_t(0x0d, "vehicle.speed", 0, 255, KM_H, 5, false),
	obd2_signal_t(0x0f, "intake.air.temperature", -40, 215, DEGREES_CELSIUS, 1, false),
	obd2_signal_t(0x10, "mass.airflow", 0, 655, GRAMS_SEC, 5, false),
	obd2_signal_t(0x11, "throttle.position", 0, 100, POURCENT, 5, false),
	obd2_signal_t(0x1f, "running.time", 0, 65535, SECONDS, 1, false),
	obd2_signal_t(0x2d, "EGR.error", -100, 99, POURCENT, 0, false),
	obd2_signal_t(0x2f, "fuel.level", 0, 100, POURCENT, 1, false),
	obd2_signal_t(0x33, "barometric.pressure", 0, 255, KPA, 1, false),
	obd2_signal_t(0x4c, "commanded.throttle.position", 0, 100, POURCENT, 1, false),
	obd2_signal_t(0x52, "ethanol.fuel.percentage", 0, 100, POURCENT, 1, false),
	obd2_signal_t(0x5a, "accelerator.pedal.position", 0, 100, POURCENT, 5, false),
	obd2_signal_t(0x5b, "hybrid.battery-pack.remaining.life", 0, 100, POURCENT, 5, false),
	obd2_signal_t(0x5c, "engine.oil.temperature",-40, 210, DEGREES_CELSIUS, 1, false),
	obd2_signal_t(0x63, "engine.torque", 0, 65535, NM, 1, false)
};

// Dumb signals and message implementation. To get compile.
std::vector<can_message_set_t> CAN_MESSAGE_SET;

std::vector<std::vector<can_message_definition_t>> CAN_MESSAGES_DEFINTION;

std::vector<std::vector<can_signal_t>> SIGNALS;

configuration_t& configuration_t::instance()
{
	static configuration_t config;
	return config;
}

configuration_t::configuration_t()
	: can_message_set_{CAN_MESSAGE_SET}, can_signals_{SIGNALS}, obd2_signals_{OBD2_PIDS}, can_message_definition_{CAN_MESSAGES_DEFINTION}
{}

configuration_t& configuration_t::get_configuration()
{
	return *this;
}

can_bus_t& configuration_t::get_can_bus_manager()
{
	return can_bus_manager_;
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

const std::vector<can_message_definition_t>& configuration_t::get_can_message_definition()
{
	return can_message_definition_[active_message_set_];
}

std::vector<obd2_signal_t>& configuration_t::get_obd2_signals()
{
	return obd2_signals_[active_message_set_];
}

uint32_t configuration_t::get_signal_id(obd2_signal_t& sig) const
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

/**
 * @fn std::vector<std::string> find_can_signals(const openxc_DynamicField &key)
 * @brief return signals name found searching through CAN_signals and OBD2 pid
 * 
 * @param[in] key - can contain numeric or string value in order to search against 
 *   can signals or obd2 signals name.
 *
 * @return Vector of signals name found. 
 */
void configuration_t::find_obd2_signals(const openxc_DynamicField &key, std::vector<obd2_signal_t*>& found_signals)
{
	switch(key.type)
	{
		case openxc_DynamicField_Type::openxc_DynamicField_Type_STRING:
				lookup_signals_by_name(key.string_value, obd2_signals_[active_message_set_], found_signals);
			break;
		case openxc_DynamicField_Type::openxc_DynamicField_Type_NUM:
				lookup_signals_by_id(key.numeric_value, obd2_signals_[active_message_set_], found_signals);
			break;
		default:
			ERROR(binder_interface, "find_obd2_signals: wrong openxc_DynamicField specified. Use openxc_DynamicField_Type_NUM or openxc_DynamicField_Type_STRING type only.");
			break;
	}
	DEBUG(binder_interface, "Found %d signal(s)", (int)found_signals.size());
}

/**
 * @fn void find_can_signals(const openxc_DynamicField& key, std::vector<can_signal_t*>& found_signals)
 * @brief return signals name found searching through CAN_signals and OBD2 pid
 * 
 * @param[in] key - can contain numeric or string value in order to search against 
 *   can signals or obd2 signals name.
 * @param[out] found_signals - provided vector to fill with ponter to signals matched.
 *
 */
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
	DEBUG(binder_interface, "Found %d signal(s)", (int)found_signals.size());
}