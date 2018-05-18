/*
 * Copyright (C) 2015, 2016, 2017 "IoT.bzh"
 * Author "Romain Forlot" <romain.forlot@iot.bzh>
 * Author "Loïc Collignon" <loic.collignon@iot.bzh>
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

#include "can-message-set.hpp"

#include "../can/can-message-definition.hpp"

can_message_set_t::can_message_set_t(
		uint8_t index,
		const std::string& name,
		const std::vector<std::shared_ptr<can_message_definition_t> >& can_messages_definition,
		const std::vector<std::shared_ptr<diagnostic_message_t> >& diagnostic_messages)
	: index_{index}
	, name_{name}
	, can_messages_definition_{can_messages_definition}
	, diagnostic_messages_{diagnostic_messages}
{}

/// @brief Returns a vector holding all message definitions which are handled by this message set.
std::vector<std::shared_ptr<can_message_definition_t> >& can_message_set_t::get_can_message_definition()
{
	return can_messages_definition_;
}

std::vector<std::shared_ptr<can_signal_t> > can_message_set_t::get_all_can_signals() const
{
	std::vector<std::shared_ptr<can_signal_t> > can_signals;
	for(const auto& cmd: can_messages_definition_)
	{
		std::vector<std::shared_ptr<can_signal_t> >& cmd_signals = cmd->get_can_signals();
		can_signals.insert( can_signals.end(),
							cmd_signals.begin(),
							cmd_signals.end()
		);
	}

	return can_signals;
}

/// @brief Returns a vector holding all diagnostic message definitions which are handled by this message set.
std::vector<std::shared_ptr<diagnostic_message_t> >& can_message_set_t::get_diagnostic_messages()
{
	return diagnostic_messages_;
}