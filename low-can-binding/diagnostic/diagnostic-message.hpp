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
#include <string>

#include "uds/uds.h"
#include "../can/can-message-set.hpp"
#include "../can/can-message.hpp"
#include "active-diagnostic-request.hpp"

enum UNIT {
	POURCENT,
	DEGREES_CELSIUS,
	KPA,
	RPM,
	GRAMS_SEC,
	SECONDS,
	KM,
	KM_H,
	PA,
	NM,
	INVALID
};

class can_message_set_t;

/// @brief - A representation of an OBD-II PID Mode 01 (Note : An OBD-II PID mode 01 contains only one information).
class diagnostic_message_t
{
	private:
		can_message_set_t* parent_; /*!< parent_ - Pointer to the CAN message set holding this diagnostic message */
		uint8_t pid_; /*!< pid_ - The 1 byte PID.*/
		std::string generic_name_; /*!< generic_name_ - A human readable name to use for this PID when published.*/
		int min_; /*!< min_ - Minimum value that can take this pid */
		int max_; /*!< max_ - Maximum value that can take this pid */
		enum UNIT unit_; /*!< unit_ : Which unit system is used by that pid. See enum UNIT above.*/
		float frequency_; /*!< frequency_ - The frequency to request this PID if supported by the vehicle when automatic, recurring OBD-II requests are enabled.*/
		DiagnosticResponseDecoder decoder_; /*!< decoder_ - An optional DiagnosticResponseDecoder to parse the payload of responses
											 * to this request. If the decoder is NULL, the output will include the raw payload
											 * instead of a parsed value.*/
		DiagnosticResponseCallback callback_; /*!< callback_ - An optional DiagnosticResponseCallback to be notified whenever a
											   * response is received for this request.*/

		bool supported_; /*!< supported_ - boolean indicating whether this pid is supported by the vehicle or not.*/

		uint64_t last_timestamp_; /*!< last_timestamp_ - the last time (in microseconds since epoch)
								* that the message has been received. */

		bool received_; /*!< received_ - True if this signal has ever been received. */
		float last_value_; /*!< last_value_ - The last received value of the diagnostic message.
								* If 'received_' is false, this value is undefined. */

	public:
		const char* generic_name = generic_name_.c_str();
		diagnostic_message_t(uint8_t pid,
				     const std::string& generic_name,
				     const int min,
				     const int max,
				     enum UNIT unit,
				     float frequency,
				     DiagnosticResponseDecoder decoder,
				     DiagnosticResponseCallback callback,
				     bool supported,
				     bool received);

		uint32_t get_pid();
		const std::string get_generic_name() const;
		const std::string get_name() const;
		float get_frequency() const;
		DiagnosticResponseDecoder get_decoder() const;
		DiagnosticResponseCallback get_callback() const;
		bool get_supported() const;

		bool get_received() const;
		float get_last_value() const;
		std::pair<float, uint64_t> get_last_value_with_timestamp() const;

		void set_received(bool r);
		void set_last_value(float val);
		void set_timestamp(uint64_t timestamp);

		void set_supported(bool value);
		void set_parent(can_message_set_t* parent);
		const DiagnosticRequest build_diagnostic_request() const;

		bool is_obd2_response(const can_message_t& can_message);
		bool is_obd2_request(const DiagnosticRequest *request);
};
