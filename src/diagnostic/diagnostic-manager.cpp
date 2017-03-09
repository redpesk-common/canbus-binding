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

#include "obd2/diagnostic-manager.hpp"

#include "low-can-binding.hpp"

diagnostic_manager_t::diagnostic_manager_t(can_bus_dev_t& bus)
	: bus_(bus)
{}

void diagnostic_manager_t::shims_logger(const char* m, ...)
{
	DEBUG(binder_interface, "%s", m);
}

void diagnostic_manager_t::shims_timer()
{}

/**
 * @brief initialize shims used by UDS lib and set initialized_ to true.
 *  It is needed before used the diagnostic manager fully because shims are
 *  required by most member functions.
 */
void diagnostic_manager_t::init_diagnostic_shims()
{
	DiagnosticShims shims_ = diagnostic_init_shims(shims_logger, bus_.send_can_message, NULL);
	initialized_ = true;
}