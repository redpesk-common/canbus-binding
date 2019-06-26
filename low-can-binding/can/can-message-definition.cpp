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

#include "can-message-definition.hpp"

#include "../binding/application.hpp"

can_message_definition_t::can_message_definition_t(
	const std::string bus,
	uint32_t id,
	bool is_fd,
	can_message_format_t format,
	frequency_clock_t frequency_clock,
	bool force_send_changed,
	const std::vector<std::shared_ptr<can_signal_t> >& can_signals)
	:  parent_{nullptr},
	bus_{bus},
	id_{id},
	is_fd_(is_fd),
	format_{format},
	frequency_clock_{frequency_clock},
	force_send_changed_{force_send_changed},
	last_value_{CAN_MESSAGE_SIZE},
	can_signals_{can_signals}
{}

const std::string can_message_definition_t::get_bus_device_name() const
{
	return application_t::instance().get_can_bus_manager()
		.get_can_device_name(bus_);
}

uint32_t can_message_definition_t::get_id() const
{
	return id_;
}

bool can_message_definition_t::is_fd() const
{
	return is_fd_;
}

bool can_message_definition_t::is_j1939() const
{
	if(format_ == can_message_format_t::J1939)
	{
		return true;
	}
	else
	{
		return false;
	}
}

std::vector<std::shared_ptr<can_signal_t> >& can_message_definition_t::get_can_signals()
{
	return can_signals_;
}

void can_message_definition_t::set_parent(can_message_set_t* parent)
{
	parent_= parent;
}

void can_message_definition_t::set_last_value(const message_t& cm)
{
	last_value_= cm.get_data_vector();
}
