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

#include <queue>
#include <vector>

#include "uds/uds.h"
#include "../can/can-bus-dev.hpp"
#include "../can/can-message.hpp"
#include "active-diagnostic-request.hpp"

#include "../low-can-binding.hpp"

/* Private: Each CAN bus needs its own set of shim functions, so this should
 * match the maximum CAN controller count.
 */
#define MAX_SHIM_COUNT can_bus_t.get_can_devices().size()

class active_diagnostic_request_t;

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
	std::shared_ptr<can_bus_dev_t> bus_; /*!< bus_ - A pointer to the CAN bus that should be used for all standard OBD-II requests, if the bus is not
						  * explicitly spcified in the request. If NULL, all requests require an explicit bus.*/
	std::vector<active_diagnostic_request_t*> recurring_requests_; /*!< recurringRequests - A queue of active, recurring diagnostic requests. When
																  * a response is received for a recurring request or it times out, it is
																  * popped from the queue and pushed onto the back. */
	std::vector<active_diagnostic_request_t*> non_recurring_requests_; /*!< nonrecurringRequests - A list of active one-time diagnostic requests. When a
																	   * response is received for a non-recurring request or it times out, it is
																	   * removed from this list and placed back in the free list.*/
	std::vector<active_diagnostic_request_t*> free_request_entries_; /*!< freeRequestEntries - A list of all available slots for active diagnostic
																	 * requests. This free list is backed by statically allocated entries in
																	 * the requestListEntries attribute.*/
	std::vector<active_diagnostic_request_t*> request_list_entries_; /*!< requestListEntries - Static allocation for all active diagnostic requests.*/

	bool initialized_; /*!< * initialized - True if the DiagnosticsManager has been initialized with shims. It will interface with the uds-c lib*/

	void init_diagnostic_shims();
	void reset();
public:
	diagnostic_manager_t();

	bool initialize(std::shared_ptr<can_bus_dev_t> cbd);

	std::shared_ptr<can_bus_dev_t> get_can_bus_dev();
	active_diagnostic_request_t* get_free_entry();
	DiagnosticShims& get_shims();

	void find_and_erase(active_diagnostic_request_t* entry, std::vector<active_diagnostic_request_t*>& requests_list);
	void cancel_request(active_diagnostic_request_t* entry);
	void cleanup_request(active_diagnostic_request_t* entry, bool force);
	void cleanup_active_requests(bool force);
	bool lookup_recurring_request(const DiagnosticRequest* request);


	bool validate_optional_request_attributes(float frequencyHz);

	void checkSupportedPids(const active_diagnostic_request_t& request,
		const DiagnosticResponse& response, float parsedPayload);

	bool add_request(DiagnosticRequest* request, const std::string name,
		bool waitForMultipleResponses, const DiagnosticResponseDecoder decoder,
		const DiagnosticResponseCallback callback);

	bool add_recurring_request(DiagnosticRequest* request, const char* name,
		bool waitForMultipleResponses, const DiagnosticResponseDecoder decoder,
		const DiagnosticResponseCallback callback, float frequencyHz);
};
