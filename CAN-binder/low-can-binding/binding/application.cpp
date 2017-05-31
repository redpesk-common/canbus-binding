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

#include "application.hpp"

#include "../utils/signals.hpp"
#include "../utils/openxc-utils.hpp"

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


std::shared_ptr<diagnostic_message_t> application_t::get_diagnostic_message(const std::string& message_name) const
{
	struct utils::signals_found found;
	 found = utils::signals_manager_t::instance().find_signals(build_DynamicField(message_name));
	if(! found.diagnostic_messages.empty())
		return found.diagnostic_messages.front();
	return nullptr;
}

DiagnosticRequest* application_t::get_request_from_diagnostic_message(const std::string& message_name) const
{
	std::shared_ptr<diagnostic_message_t> diag_msg = get_diagnostic_message(message_name);
	if( diag_msg != nullptr && diag_msg->get_supported())
		return new DiagnosticRequest(diag_msg->build_diagnostic_request());
	return nullptr;
}