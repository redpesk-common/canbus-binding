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
#include <ctime>

#include "application.hpp"

#include "../utils/signals.hpp"
#include "../utils/openxc-utils.hpp"
#include "low-can-subscription.hpp"

#define MICROSECONDS_IN_SECOND	1000000
#define ENGINE_VALUE_TIMEOUT	5

application_t::application_t()
	: can_bus_manager_{}
	, message_set_{}
{}

/// @brief Return singleton instance of configuration object.
application_t& application_t::instance()
{
	static application_t config;
	return config;
}

can_bus_t& application_t::get_can_bus_manager()
{
	return can_bus_manager_;
}

std::map<std::string, std::shared_ptr<low_can_subscription_t> >& application_t::get_can_devices()
{
	return can_devices_;
}

diagnostic_manager_t& application_t::get_diagnostic_manager()
{
	return diagnostic_manager_;
}

uint8_t application_t::get_active_message_set() const
{
	return active_message_set_;
}

int application_t::add_message_set(std::shared_ptr<message_set_t> new_message_set)
{
	set_parents(new_message_set);

	for(auto old_msg_set : message_set_)
	{
		if(old_msg_set->get_index() == new_message_set->get_index())
		{
			for(auto new_msg_def : new_message_set->get_messages_definition())
			{
				if(old_msg_set->add_message_definition(new_msg_def) < 0)
					return -1;
			}

			if(diagnostic_manager_.is_initialized())
			{
				for(auto new_diag_msg : new_message_set->get_diagnostic_messages())
				{
					if(old_msg_set->add_diagnostic_message(new_diag_msg) < 0)
						return -1;
				}
			}
			return 0;
		}
	}

	message_set_.push_back(new_message_set);
	return 0;
}

std::vector<std::shared_ptr<message_set_t> > application_t::get_message_set()
{
	return message_set_;
}

vect_ptr_signal_t application_t::get_all_signals()
{
	return message_set_[active_message_set_]->get_all_signals();
}

vect_ptr_diag_msg_t application_t::get_diagnostic_messages()
{
	return message_set_[active_message_set_]->get_diagnostic_messages();
}

vect_ptr_msg_def_t application_t::get_messages_definition()
{
	return message_set_[active_message_set_]->get_messages_definition();
}


std::vector<std::shared_ptr<message_definition_t>> application_t::get_messages_definition(uint32_t id)
{
	std::vector<std::shared_ptr<message_definition_t>> ret = std::vector<std::shared_ptr<message_definition_t>>();
	vect_ptr_msg_def_t messages_definition = get_messages_definition();
	for(std::shared_ptr<message_definition_t> &msg_def : messages_definition)
	{
		if(msg_def->get_id() == id)
		{
			ret.push_back(msg_def);
		}
	}
	return ret;
}


uint32_t application_t::get_signal_id(diagnostic_message_t& sig) const
{
	return sig.get_pid();
}

uint32_t application_t::get_signal_id(signal_t& sig) const
{
	return sig.get_message()->get_id();
}

void application_t::set_active_message_set(uint8_t id)
{
	active_message_set_ = id;
}

bool application_t::is_engine_on()
{
	struct utils::signals_found sf;
	openxc_DynamicField search_key = build_DynamicField("engine.speed");
	sf = utils::signals_manager_t::instance().find_signals(search_key);
	bool engine_on = false;
	uint64_t last_timestamp_in_s;

	if(sf.signals.size() == 1)
	{
		last_timestamp_in_s = sf.signals.front()->get_last_value_with_timestamp().second
						/ MICROSECONDS_IN_SECOND;

		if(sf.signals.front()->get_last_value_with_timestamp().first > 0 &&
		   std::difftime(std::time(nullptr), last_timestamp_in_s) < ENGINE_VALUE_TIMEOUT)
			engine_on = true;
		else
			AFB_NOTICE("is_engine_on: engine.speed CAN signal found, but engine seems off");
	}
	else
	{
		AFB_NOTICE("is_engine_on: Can't identify a useable engine.speed CAN signal");
	}

	if(sf.diagnostic_messages.size() == 1)
	{
		last_timestamp_in_s = sf.diagnostic_messages.front()->get_last_value_with_timestamp().second
						/ MICROSECONDS_IN_SECOND;

		if(sf.diagnostic_messages.front()->get_last_value_with_timestamp().first > 0 &&
		   std::difftime(std::time(nullptr), last_timestamp_in_s) < ENGINE_VALUE_TIMEOUT)
			engine_on = true;
		else
			AFB_NOTICE("is_engine_on: engine.speed diagnostic message found, but engine seems off");
	}
	else
	{
		AFB_NOTICE("is_engine_on: Can't identify a useable engine.speed diagnostic message");
	}

	return engine_on;
}

void application_t::set_parents(std::shared_ptr<message_set_t> new_message_set)
{
	vect_ptr_msg_def_t messages_definition = new_message_set->get_messages_definition();
	for(std::shared_ptr<message_definition_t> cmd : messages_definition)
	{
		cmd->set_parent(new_message_set);
		std::vector<std::shared_ptr<signal_t>> signals = cmd->get_signals();
		for(std::shared_ptr<signal_t> sig: signals)
			sig->set_parent(cmd);
	}

	std::vector<std::shared_ptr<diagnostic_message_t>> diagnostic_messages = new_message_set->get_diagnostic_messages();
	for(std::shared_ptr<diagnostic_message_t> dm : diagnostic_messages)
		dm->set_parent(new_message_set);
}

#ifdef USE_FEATURE_J1939
std::shared_ptr<utils::socketcan_t> application_t::get_socket_address_claiming()
{
	return subscription_address_claiming_->get_socket();
}

std::shared_ptr<low_can_subscription_t> application_t::get_subscription_address_claiming()
{
	return subscription_address_claiming_;
}


void application_t::set_subscription_address_claiming(std::shared_ptr<low_can_subscription_t> new_subscription)
{
	subscription_address_claiming_ = new_subscription;
}

#endif
