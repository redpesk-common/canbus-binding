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

#include "obd2.hpp"

void shims_logger(const char* m, const struct afb_binding_interface *interface)
{
	DEBUG(interface, "%s", m);
}

void shims_timer()
{
}

/*
 * Will scan for supported Obd2 pids
 */
obd2_handler_t::obd2_handler_t(const struct afb_binding_interface *interface, can_bus_t can_bus)
	: can_bus_{can_bus}
{
	can_bus_t can_bus_ = can_bus;
	DiagnosticShims shims_ = diagnostic_init_shims(shims_logger, can_bus.send_can_message, NULL);

	int n_pids_, i_;

	n_pids_ = size(Obd2Pid);
	for(i_=0; i_<=n_pids_; i_++)
	{
	}
}

void obd2_handler_t::add_request(int pid)
{
	DiagnosticRequest request = {
	arbitration_id: OBD2_FUNCTIONAL_BROADCAST_ID,
	mode: 0x1, has_pid: true, pid: pid};
}

bool obd2_handler_t::is_obd2_request(DiagnosticRequest* request)
{
	return request->mode == 0x1 && request->has_pid && request->pid < 0xff;
}

bool obd2_handler_t::is_obd2_signal(const char *name)
{
	if(fnmatch("obd2.*", name, FNM_CASEFOLD) == 0)
		return true;
	return false;
}

bool obd2_handler_t::decode_obd2_response(DiagnosticResponse* responce)
{
	return diagnostic_decode_obd2_pid(response);
}
