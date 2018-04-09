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

#include <string>
#include <vector>

#include "../utils/socketcan-bcm.hpp"
#include "uds/uds.h"
#include "uds/uds_types.h"
#include "../utils/timer.hpp"

class active_diagnostic_request_t;
class diagnostic_manager_t;

/// @brief The signature for an optional function that can apply the neccessary
/// formula to translate the binary payload into meaningful data.
///
/// @param[in] response - the received DiagnosticResponse (the data is in response.payload,
///  a byte array). This is most often used when the byte order is signiticant, i.e. with many OBD-II PID formulas.
/// @param[in] parsed_payload - the entire payload of the response parsed as an int.
///
/// @return float value after decoding.
///
typedef float (*DiagnosticResponseDecoder)(const DiagnosticResponse* response,
		float parsed_payload);

/// @brief: The signature for an optional function to handle a new diagnostic
/// response.
///
/// @param[in] request - The original diagnostic request.
/// @param[in] response - The response object that was just received.
/// @param[in] parsed_payload - The payload of the response, parsed as a float.
///
typedef void (*DiagnosticResponseCallback)(const active_diagnostic_request_t* request,
		const DiagnosticResponse* response, float parsed_payload);

///
/// @brief An active diagnostic request, either recurring or one-time.
///
/// Will host a diagnostic_message_t class members to describe an on going
///  diagnostic request on the CAN bus. Diagnostic message will be converted to
///  a DiagnosticRequest using ad-hoc method build_diagnostic_request from diagnostic message.
///  Then missing member, that can not be hosted into a DiagnosticRequest struct, will be passed
///  as argument when adding the request to (non)-recurrent vector. Argument will be used to instanciate
///  an active_diagnostic_request_t object before sending it.
///
class active_diagnostic_request_t {
private:
	std::string bus_; ///< bus_ - The CAN bus this request should be made on, or is currently in flight-on
	uint32_t id_; ///< id_ - The arbitration ID (aka message ID) for the request.
	DiagnosticRequestHandle* handle_; ///< handle_ - A handle for the request to keep track of it between
									  ///< sending the frames of the request and receiving all frames of the response.
	std::string name_; ///< name_ - Human readable name, to be used when publishing received responses.
	static std::string prefix_; ///< prefix_ - It has to reflect the JSON object which it comes from. It makes easier sorting
								///< incoming CAN messages.
	DiagnosticResponseDecoder decoder_; ///< decoder_ - An optional DiagnosticResponseDecoder to parse the payload of responses
										///< to this request. If the decoder is NULL, the output will include the raw payload
										///< instead of a parsed value.
	DiagnosticResponseCallback callback_; ///< callback_ - An optional DiagnosticResponseCallback to be notified whenever a
										  ///< response is received for this request.
	bool recurring_; ///< bool recurring_ - If true, this is a recurring request and it will remain as active until explicitly cancelled.
					 ///< The frequencyClock attribute controls how often a recurring request is made.
	bool permanent_; ///< bool permanent_ - If true, this a	permanent recurring request and will remain as active indefinitely (can't be cancelled).
	bool wait_for_multiple_responses_; ///< wait_for_multiple_responses_ - False by default, when any response is received for a request
									   ///< it will be removed from the active list. If true, the request will remain active until the timeout
									   ///< clock expires, to allow it to receive multiple response (e.g. to a functional broadcast request).
	frequency_clock_t frequency_clock_; ///< frequency_clock_ - A frequency_clock_t object to control the send rate for a
										///< recurring request. If the request is not reecurring, this attribute is not used.
	frequency_clock_t timeout_clock_; ///< timeout_clock_ - A frequency_clock_t object to monitor how long it's been since
									  ///< this request was sent.
	utils::socketcan_bcm_t socket_; ///< socket_ - A BCM socket setup to send cyclic message to CAN ID 7DF.
public:
	bool operator==(const active_diagnostic_request_t& b);
	active_diagnostic_request_t& operator=(const active_diagnostic_request_t& adr);

	active_diagnostic_request_t();
	active_diagnostic_request_t(active_diagnostic_request_t&&) = default;
	active_diagnostic_request_t(const std::string& bus, uint32_t id,
		const std::string& name, bool wait_for_multiple_responses,
		const DiagnosticResponseDecoder decoder,
		const DiagnosticResponseCallback callback, float frequencyHz, bool permanent);
	~active_diagnostic_request_t();

	uint32_t get_id() const;
	DiagnosticRequestHandle* get_handle();
	uint16_t get_pid() const;
	const std::string get_name() const;
	static std::string& get_prefix();
	DiagnosticResponseDecoder& get_decoder();
	DiagnosticResponseCallback& get_callback();
	bool get_recurring() const;
	bool get_permanent() const;
	frequency_clock_t& get_frequency_clock();
	frequency_clock_t& get_timeout_clock();
	utils::socketcan_bcm_t& get_socket();

	void set_handle(DiagnosticShims& shims, DiagnosticRequest* request);

	static bool is_diagnostic_signal(const std::string& name);

	bool response_received() const;
};
