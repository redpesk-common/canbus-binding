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

#include <algorithm>

#include "diagnostic-manager.hpp"

#include "uds/uds.h"
#include "../configuration.hpp"

#define MAX_REQUEST_ENTRIES 50

diagnostic_manager_t::diagnostic_manager_t()
	: request_list_entries_(MAX_REQUEST_ENTRIES, active_diagnostic_request_t())
{}

diagnostic_manager_t::diagnostic_manager_t(can_bus_dev_t& bus)
	: bus_(&bus), request_list_entries_(MAX_REQUEST_ENTRIES, active_diagnostic_request_t())
{}

bool shims_send(const uint32_t arbitration_id, const uint8_t* data, const uint8_t size)
{
	can_bus_dev_t *can_bus_dev = config->get_diagnostic_manager().get_can_bus_dev();
	return can_bus_dev->shims_send(arbitration_id, data, size);
}

void diagnostic_manager_t::shims_logger(const char* m, ...)
{
	DEBUG(binder_interface, "%s", m);
}

void diagnostic_manager_t::shims_timer()
{}

can_bus_dev_t* diagnostic_manager_t::get_can_bus_dev()
{
	return bus_;
}
/**
 * @brief initialize shims used by UDS lib and set initialized_ to true.
 *  It is needed before used the diagnostic manager fully because shims are
 *  required by most member functions.
 */
void diagnostic_manager_t::init_diagnostic_shims()
{
	DiagnosticShims shims_ = diagnostic_init_shims(shims_logger, shims_send, NULL);
	initialized_ = true;
}