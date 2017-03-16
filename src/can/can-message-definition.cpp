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

can_message_definition_t::can_message_definition_t(std::uint8_t message_set_id, const std::string bus)
	: message_set_id_{message_set_id}, bus_{bus}, last_value_{CAN_MESSAGE_SIZE}
{}

can_message_definition_t::can_message_definition_t(std::uint8_t message_set_id, const std::string bus, uint32_t id, frequency_clock_t frequency_clock, bool force_send_changed)
	: message_set_id_{message_set_id}, bus_{bus}, id_{id}, frequency_clock_{frequency_clock}, force_send_changed_{force_send_changed}, last_value_{CAN_MESSAGE_SIZE}
{}

can_message_definition_t::can_message_definition_t(std::uint8_t message_set_id, const std::string bus, uint32_t id, can_message_format_t format, frequency_clock_t frequency_clock, bool force_send_changed)
	: message_set_id_{message_set_id}, bus_{bus}, id_{id}, format_{format}, frequency_clock_{frequency_clock}, force_send_changed_{force_send_changed}, last_value_{CAN_MESSAGE_SIZE}
{}

uint32_t can_message_definition_t::get_id() const
{
	return id_;
}