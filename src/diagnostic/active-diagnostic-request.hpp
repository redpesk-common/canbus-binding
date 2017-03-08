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
#include "uds/uds_types.h"
#include "can/can-bus.hpp"
#include "can/can-message.hpp"

#include "low-can-binding.hpp"

class active_diagnostic_request_t;
class can_bus_dev_t;

/* Public: The signature for an optional function that can apply the neccessary
 * formula to translate the binary payload into meaningful data.
 *
 * response - the received DiagnosticResponse (the data is in response.payload,
 *      a byte array). This is most often used when the byte order is
 *      signiticant, i.e. with many OBD-II PID formulas.
 * parsed_payload - the entire payload of the response parsed as an int.
 */
typedef float (*DiagnosticResponseDecoder)(const DiagnosticResponse* response,
		float parsed_payload);

/* Public: The signature for an optional function to handle a new diagnostic
 * response.
 *
 * manager - The DiagnosticsManager providing this response.
 * request - The original diagnostic request.
 * response - The response object that was just received.
 * parsed_payload - The payload of the response, parsed as a float.
 */
typedef void (*DiagnosticResponseCallback)(const active_diagnostic_request_t* request,
		const DiagnosticResponse* response, float parsed_payload);

/**
 * @brief An active diagnostic request, either recurring or one-time.
 */
class active_diagnostic_request_t {
	private:
		can_bus_dev_t* bus_; /*!< can_bus_dev_t* bus_ - The CAN bus this request should be made on, or is currently in flight-on*/
		uint32_t id_; /*!< The arbitration ID (aka message ID) for the request.*/
		DiagnosticRequestHandle* handle_; /*!< DiagnosticRequestHandle* handle - A handle for the request to keep track of it between
										   * sending the frames of the request and receiving all frames of the response.*/
		std::string name_; /*!< std::string name_ - An optional human readable name this response, to be used when publishing received 
							* responses. If the name is NULL, the published output will use the raw OBD-II response format.*/
		DiagnosticResponseDecoder decoder_; /*!< decoder - An optional DiagnosticResponseDecoder to parse the payload of responses
											 * to this request. If the decoder is NULL, the output will include the raw payload
											 * instead of a parsed value.*/
		DiagnosticResponseCallback callback_; /*!< callback - An optional DiagnosticResponseCallback to be notified whenever a
											   * response is received for this request.*/
		bool recurring_; /*!< bool recurring_ - If true, this is a recurring request and it will remain as active until explicitly cancelled.
						  * The frequencyClock attribute controls how often a recurrin request is made.*/
		bool waitForMultipleResponses_; /*!< bool waitForMultipleResponses_ - False by default, when any response is received for a request
										 * it will be removed from the active list. If true, the request will remain active until the timeout
										 * clock expires, to allow it to receive multiple response (e.g. to a functional broadcast request).*/
		bool inFlight_; /*!< inFlight - True if the request has been sent and we are waiting for a response.*/
		FrequencyClock frequencyClock_; /*!< FrequencyClock frequencyClock_ - A FrequencyClock struct to control the send rate for a
										 * recurring request. If the request is not reecurring, this attribute is not used.*/
		FrequencyClock timeoutClock_; /*!< FrequencyClock timeoutClock_ - A FrequencyClock struct to monitor how long it's been since
									   * this request was sent.*/
	public:
};
