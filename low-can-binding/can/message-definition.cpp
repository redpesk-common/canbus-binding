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
	bool is_fd,
	message_format_t format,
	frequency_clock_t frequency_clock,
	bool force_send_changed,
	const std::vector<std::shared_ptr<signal_t> >& signals)
	:  parent_{nullptr},
	bus_{bus},
	id_{id},
	is_fd_(is_fd),
	format_{format},
	frequency_clock_{frequency_clock},
	force_send_changed_{force_send_changed},
	last_value_{CAN_MESSAGE_SIZE},
	signals_{signals}
{}

message_definition_t::message_definition_t(const std::string bus,
	uint32_t id,
	const std::string name,
	uint32_t length,
	bool is_fd,
	message_format_t format,
	frequency_clock_t frequency_clock,
	bool force_send_changed,
	const std::vector<std::shared_ptr<signal_t> >& signals)
	: parent_{nullptr},
	bus_{bus},
	id_{id},
	name_{name},
	length_{length},
	is_fd_(is_fd),
	format_{format},
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
	return id_;
}

bool message_definition_t::is_fd() const
{
	return is_fd_;
}

bool message_definition_t::is_j1939() const
{
	if(format_ == message_format_t::J1939)
	{
		return true;
	}
	else
	{
		return false;
	}
}

std::vector<std::shared_ptr<signal_t>>& message_definition_t::get_signals()
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

message_format_t message_definition_t::get_format() const
{
	return format_;
}