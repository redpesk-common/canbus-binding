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

#pragma once

#include <vector>

#include "uds/uds.h"
#include "can/can-bus-dev.hpp"
#include "can/can-message.hpp"
#include "obd2/active-diagnostic-request.hpp"

#include "low-can-binding.hpp"

/* Private: Each CAN bus needs its own set of shim functions, so this should
 * match the maximum CAN controller count.
 */
#define MAX_SHIM_COUNT can_bus_t.get_can_devices().size()

/**
 * @brief The core structure for running the diagnostics module on the VI.
 *
 * @desc This stores details about the active requests and shims required to connect
 * the diagnostics library to the VI's CAN peripheral.
 */
class diagnostic_manager_t {
protected:
	static bool shims_send(const uint32_t arbitration_id, const uint8_t* data, const uint8_t size);
	static void shims_logger(const char* m, ...);
	static void shims_timer();

private:
	DiagnosticShims shims_; /*!< shims_ - An array of shim functions for each CAN bus that plug the diagnostics
								* library (uds-c) into the VI's CAN peripheral.*/
	can_bus_dev_t* bus_; /*!< bus_ - A pointer to the CAN bus that should be used for all standard OBD-II requests, if the bus is not
							* explicitly spcified in the request. If NULL, all requests require an explicit bus.*/
	std::queue<active_diagnostic_request_t> recurringRequests_; /*!< recurringRequests - A queue of active, recurring diagnostic requests. When
																* a response is received for a recurring request or it times out, it is
																* popped from the queue and pushed onto the back. */
	std::vector<active_diagnostic_request_t> nonrecurringRequests_; /*!< nonrecurringRequests - A list of active one-time diagnostic requests. When a
																	* response is received for a non-recurring request or it times out, it is
																	* removed from this list and placed back in the free list.*/
	std::vector<active_diagnostic_request_t> freeRequestEntries_; /*!< freeRequestEntries - A list of all available slots for active diagnostic
																* requests. This free list is backed by statically allocated entries in
																* the requestListEntries attribute.*/
	std::vector<active_diagnostic_request_t> requestListEntries_[50]; /*!< requestListEntries - Static allocation for all active diagnostic requests.*/

	bool initialized_; /*!< * initialized - True if the DiagnosticsManager has been initialized with shims. It will interface with the uds-c lib*/

public:
	diagnostic_manager_t();
	diagnostic_manager_t(can_bus_dev_t& bus);

	void init_diagnostic_shims();

	can_bus_dev_t* get_can_bus_dev();

	void checkSupportedPids(const active_diagnostic_request_t& request,
	const DiagnosticResponse& response, float parsedPayload);

	bool addRecurringRequest(DiagnosticRequest* request, const char* name,
		bool waitForMultipleResponses, const DiagnosticResponseDecoder decoder,
		const DiagnosticResponseCallback callback, float frequencyHz);

	void reset();

	void add_request(int pid);
};
