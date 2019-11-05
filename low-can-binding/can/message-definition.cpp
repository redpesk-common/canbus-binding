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

#include "message-definition.hpp"

#include "../binding/application.hpp"

message_definition_t::message_definition_t(
	const std::string bus,
	uint32_t id,
	uint32_t flags,
	frequency_clock_t frequency_clock,
	bool force_send_changed,
	const vect_ptr_signal_t& signals)
	:  parent_{nullptr},
	bus_{bus},
	id_{id},
	flags_{flags},
	frequency_clock_{frequency_clock},
	force_send_changed_{force_send_changed},
	last_value_{CAN_MESSAGE_SIZE},
	signals_{signals}
{}

message_definition_t::message_definition_t(const std::string bus,
	uint32_t id,
	const std::string name,
	uint32_t length,
	uint32_t flags,
	frequency_clock_t frequency_clock,
	bool force_send_changed,
	const vect_ptr_signal_t& signals)
	: parent_{nullptr},
	bus_{bus},
	id_{id},
	name_{name},
	length_{length},
	flags_{flags},
	frequency_clock_{frequency_clock},
	force_send_changed_{force_send_changed},
	last_value_{CAN_MESSAGE_SIZE},
	signals_{signals}
{}

const std::string message_definition_t::get_bus_device_name() const
{
	return application_t::instance().get_can_bus_manager()
		.get_can_device_name(bus_);
}

uint32_t message_definition_t::get_id() const
{
	return id_ & CAN_EFF_MASK ?
		id_ | CAN_EFF_FLAG :
		id_;
}

bool message_definition_t::is_fd() const
{
	return (flags_&FD_FRAME);
}

bool message_definition_t::is_j1939() const
{
	return (flags_&J1939_PROTOCOL);
}

bool message_definition_t::is_isotp() const
{
	return (flags_&ISOTP_PROTOCOL);
}

vect_ptr_signal_t& message_definition_t::get_signals()
{
	return signals_;
}

void message_definition_t::set_parent(std::shared_ptr<message_set_t> parent)
{
	parent_= parent;
}

void message_definition_t::set_last_value(std::shared_ptr<message_t> m)
{
	last_value_= m->get_data_vector();
}

uint32_t message_definition_t::get_length() const
{
	return length_;
}

uint32_t message_definition_t::get_flags() const
{
	return flags_;
}
