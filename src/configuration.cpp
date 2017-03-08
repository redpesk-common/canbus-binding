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

#include <configuration.hpp>

#define OBD2_FUNCTIONAL_BROADCAST_ID 0x7df

// Dumb signals and message implementation. To get compile.
 std::vector<obd2_signal_t> OBD2_PIDS;

std::vector<can_message_set_t> CAN_MESSAGE_SET;

std::vector<std::vector<can_message_definition_t>> CAN_MESSAGES_DEFINTION;

std::vector<std::vector<can_signal_t>> SIGNALS;

configuration_t::configuration_t()
	: obd2_signals_{OBD2_PIDS}, can_message_set_{CAN_MESSAGE_SET}, can_signals_{SIGNALS}, can_message_definition_{CAN_MESSAGES_DEFINTION}

configuration_t& configuration_t::get_configuration()
{ 
	return *this;
}

can_bus_t& configuration_t::get_can_bus_manager() const
{
	return can_bus_manager_;
}

diagnostic_manager_t& configuration_t::get_diagnostic_manager() const
{
	return diagnostic_manager_;
}

uint8_t configuration_t::get_active_message_set() const
{
	return active_message_set_;
}

const std::vector<can_message_set_t>& configuration_t::get_can_message_set() const
{
	return can_message_set_;
}

const std::vector<can_signal_t>& configuration_t::get_can_signals() const
{
	return can_signals_[active_message_set_];
}

const std::vector<can_message_definition_t>& configuration_t::get_can_message_definition() const
{
	return can_message_definition_[active_message_set_];
}

const std::vector<obd2_signal_t>& configuration_t::get_obd2_signals() const
{
	return obd2_signals_;
}

uint32_t configuration_t::get_signal_id(obd2_signal_t& sig) const
{
	return sig.get_pid();
}

uint32_t configuration_t::get_signal_id(can_signal_t& sig) const
{
	return sig.get_message()->get_id();
}

void configuration_t::set_active_message_set(uint8_t id)
{
	active_message_set_ = id;
}