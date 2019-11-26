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

#include "message-set.hpp"

#include "../can/message-definition.hpp"

message_set_t::message_set_t(
		uint8_t index,
		const std::string& name,
		const std::vector<std::shared_ptr<message_definition_t> >& messages_definition,
		const vect_ptr_diag_msg_t& diagnostic_messages)
	: index_{index}
	, name_{name}
	, messages_definition_{messages_definition}
	, diagnostic_messages_{diagnostic_messages}
{}

/// @brief Returns a vector holding all message definitions which are handled by this message set.
vect_ptr_msg_def_t& message_set_t::get_messages_definition()
{
	return messages_definition_;
}

std::vector<std::shared_ptr<signal_t>> message_set_t::get_all_signals() const
{
	vect_ptr_signal_t signals;
	for(const auto& cmd: messages_definition_)
	{
		std::vector<std::shared_ptr<signal_t>> cmd_signals = cmd->get_signals();
		signals.insert( signals.end(),
							cmd_signals.begin(),
							cmd_signals.end()
		);
	}

	return signals;
}

/// @brief Returns a vector holding all diagnostic message definitions which are handled by this message set.
vect_ptr_diag_msg_t& message_set_t::get_diagnostic_messages()
{
	return diagnostic_messages_;
}
