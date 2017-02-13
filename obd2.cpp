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


void shims_logger(afb_binding_interface *itf)
{
	//DEBUG(itf, "dd");
}

void shims_timer()
{
}

/*
 * Will scan for supported Obd2 pids
 */
Obd2Handler::Obd2Handler(afb_binding_interface *itf, CanBus_c cb)
{
	CanBus_c can_bus = cb;
	DiagnosticShims shims = diagnostic_init_shims(shims_logger, can_bus.send_can_message, NULL);

	int n_pids, i;

	n_pids = size(Obd2Pid);
	for(i=0; i<=n_pids; i++)
	{
	}
}

Obd2Handler::add_request(int pid)
{
	DiagnosticRequest request = {
	arbitration_id: OBD2_FUNCTIONAL_BROADCAST_ID,
	mode: 0x1, has_pid: true, pid: pid};
}

Obd2Handler::is_obd2_request(DiagnosticRequest* request)
{
	return request->mode == 0x1 && request->has_pid && request->pid < 0xff;
}

Obd2Handler::decode_obd2_response(DiagnosticResponse* responce)
{
	return diagnostic_decode_obd2_pid(response);
}
