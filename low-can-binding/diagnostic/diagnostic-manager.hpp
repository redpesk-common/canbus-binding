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

#include <systemd/sd-event.h>
#include <map>
#include <vector>

#include "../utils/socketcan-bcm.hpp"
#include "uds/uds.h"
#include "openxc.pb.h"
#include "../can/can-bus.hpp"
#include "active-diagnostic-request.hpp"

///  Each CAN bus requires its own set of shim functions, so this should
/// match the maximum CAN controller count.
///
#define DIAGNOSTIC_RESPONSE_ARBITRATION_ID_OFFSET 0x8

class active_diagnostic_request_t;

///
/// @brief The core structure for running the diagnostics module of the binding.
///
/// This stores details about the active requests and shims required to connect
/// the diagnostics library to the CAN device.
///
class diagnostic_manager_t {
private:
	DiagnosticShims shims_; /*!< shims_ - Shim functions for each CAN bus that plug the diagnostics
							 * library (uds-c) into the VI's CAN peripheral.*/
	std::string bus_; /*!< bus_ - A pointer to the CAN bus that should be used for all standard OBD-II requests, if the bus is not
						* explicitly spcified in the request. Default to the first bus CAN at initialization.*/
	std::vector<active_diagnostic_request_t*> recurring_requests_; /*!< recurringRequests - A list of active recurring diagnostic requests.*/
	std::vector<active_diagnostic_request_t*> non_recurring_requests_; /*!< nonrecurringRequests - A list of active one-time diagnostic requests. When a
																	   * response is received for a non-recurring request or it times out, it is removed*/
	bool initialized_; /*!< * initialized - True if the DiagnosticsManager has been initialized with shims. It will interface with the uds-c lib*/

	void init_diagnostic_shims();
	void reset();

	static bool shims_send(const uint32_t arbitration_id, const uint8_t* data, const uint8_t size);
	static void shims_logger(const char* m, ...);
public:
	diagnostic_manager_t();
	~diagnostic_manager_t();

	bool initialize();

	const std::string get_bus_name() const;
	const std::string get_bus_device_name() const;
	active_diagnostic_request_t* get_last_recurring_requests() const;
	DiagnosticShims& get_shims();

	void find_and_erase(active_diagnostic_request_t* entry, std::vector<active_diagnostic_request_t*>& requests_list);
	void cancel_request(active_diagnostic_request_t* entry);
	void cleanup_request(active_diagnostic_request_t* entry, bool force);
	void cleanup_active_requests(bool force);
	active_diagnostic_request_t* find_recurring_request(DiagnosticRequest& request);

	// Subscription parts
	active_diagnostic_request_t* add_request(DiagnosticRequest* request, const std::string& name,
		bool waitForMultipleResponses, const DiagnosticResponseDecoder decoder,
		const DiagnosticResponseCallback callback);
	bool validate_optional_request_attributes(float frequencyHz);
	active_diagnostic_request_t* add_recurring_request(DiagnosticRequest* request, const char* name,
		bool waitForMultipleResponses, const DiagnosticResponseDecoder decoder,
		const DiagnosticResponseCallback callback, float frequencyHz, bool permanent);

	// Decoding part
	openxc_VehicleMessage relay_diagnostic_response(active_diagnostic_request_t* adr, const DiagnosticResponse& response, const uint64_t timestamp);
	openxc_VehicleMessage relay_diagnostic_handle(active_diagnostic_request_t* entry, const can_message_t& cm);
	openxc_VehicleMessage find_and_decode_adr(const can_message_t& cm);
	bool is_diagnostic_response(const can_message_t& cm);
};
