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

active_diagnostic_request_t::active_diagnostic_request_t()
	: can_bus_dev_{nullptr}, uint32_t id_{0}, DiagnosticRequestHandle{nullptr}, name_{""},
	  decoder_{nullptr}, callback_{nullptr}, reccuring_{false}, wait_for_multiple_responses_{false},
	  in_flight_{false}, frequency_clock_{frequency_clock_t()}, timeout_clock_{frequency_clock_t()}
{}

void updateDiagnosticRequestEntry(CanBus* bus, DiagnosticRequest* request,
		const char* name, bool waitForMultipleResponses,
		const DiagnosticResponseDecoder decoder,
		const DiagnosticResponseCallback callback, float frequencyHz)
{
	entry->bus = bus;
	entry->arbitration_id = request->arbitration_id;
	entry->handle = generate_diagnostic_request(
			&manager->shims[bus->address - 1], request, NULL);
	if(name != NULL) {
		strncpy(entry->name, name, MAX_GENERIC_NAME_LENGTH);
	} else {
		entry->name[0] = '\0';
	}
	entry->waitForMultipleResponses = waitForMultipleResponses;

	entry->decoder = decoder;
	entry->callback = callback;
	entry->recurring = frequencyHz != 0;
	entry->frequencyClock = {0};
	entry->frequencyClock.frequency = entry->recurring ? frequencyHz : 0;
	// time out after 100ms
	entry->timeoutClock = {0};
	entry->timeoutClock.frequency = 10;
	entry->inFlight = false;
	}