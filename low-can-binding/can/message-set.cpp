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
		const vect_ptr_msg_def_t& messages_definition,
		const vect_ptr_diag_msg_t& diagnostic_messages)
	: index_{index}
	, name_{name}
	, messages_definition_{messages_definition}
	, diagnostic_messages_{diagnostic_messages}
{}


int message_set_t::add_message_definition(std::shared_ptr<message_definition_t> msg_def)
{
	for (auto old_msg_def : messages_definition_)
	{
		if(old_msg_def->get_id() == msg_def->get_id())
		{
			AFB_ERROR("Same id between : %s and %s", old_msg_def->get_name().c_str(), msg_def->get_name().c_str());
			return -1;
		}
	}
	messages_definition_.push_back(msg_def);
	return 0;
}


int message_set_t::add_diagnostic_message(std::shared_ptr<diagnostic_message_t> diag_msg)
{
	for (auto old_diag_msg : diagnostic_messages_)
	{
		if(old_diag_msg->get_pid() == diag_msg->get_pid())
		{
			AFB_ERROR("Same pid between : %s and %s", old_diag_msg->get_generic_name().c_str(), diag_msg->get_generic_name().c_str());
			return -1;
		}
	}
	diagnostic_messages_.push_back(diag_msg);
	return 0;
}


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

uint8_t message_set_t::get_index() const
{
	return index_;
}
