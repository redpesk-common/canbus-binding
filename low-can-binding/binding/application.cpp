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
#include "low-can-socket.hpp"

#define MICROSECONDS_IN_SECOND	1000000
#define ENGINE_VALUE_TIMEOUT	5

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

std::map<std::string, std::shared_ptr<low_can_socket_t> >& application_t::get_can_devices()
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

std::vector<std::shared_ptr<can_message_set_t> > application_t::get_can_message_set()
{
	return can_message_set_;
}

std::vector<std::shared_ptr<can_signal_t> > application_t::get_all_can_signals()
{
	return can_message_set_[active_message_set_]->get_all_can_signals();
}

std::vector<std::shared_ptr<diagnostic_message_t> >& application_t::get_diagnostic_messages()
{
	return can_message_set_[active_message_set_]->get_diagnostic_messages();
}

std::vector<std::shared_ptr<can_message_definition_t> >& application_t::get_can_message_definition()
{
	return can_message_set_[active_message_set_]->get_can_message_definition();
}

uint32_t application_t::get_signal_id(diagnostic_message_t& sig) const
{
	return sig.get_pid();
}

uint32_t application_t::get_signal_id(can_signal_t& sig) const
{
	return sig.get_message()->get_id();
}

void application_t::set_active_message_set(uint8_t id)
{
	active_message_set_ = id;
}

bool application_t::isEngineOn()
{
	struct utils::signals_found sf;
	openxc_DynamicField search_key = build_DynamicField("engine.speed");
	sf = utils::signals_manager_t::instance().find_signals(search_key);
	bool engine_on = false;
	uint64_t last_timestamp_in_s;

	if(sf.can_signals.size() == 1)
	{
		last_timestamp_in_s = sf.can_signals.front()->get_last_value_with_timestamp().second
						/ MICROSECONDS_IN_SECOND;

		if(sf.can_signals.front()->get_last_value_with_timestamp().first > 0 &&
		   std::difftime(std::time(nullptr), last_timestamp_in_s) < ENGINE_VALUE_TIMEOUT)
		{
			engine_on = true;
		}
		else
		{
			AFB_NOTICE("is_engine_on: engine.speed CAN signal found, but engine seems off");
		}
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
		{
			engine_on = true;
		}
		else
		{
			AFB_NOTICE("is_engine_on: engine.speed diagnostic message found, but engine seems off");
		}
	}
	else
	{
		AFB_NOTICE("is_engine_on: Can't identify a useable engine.speed diagnostic message");
	}

	return engine_on;
}