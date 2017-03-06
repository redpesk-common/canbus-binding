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
#include "can-bus.hpp"

#include "low-can-binding.hpp"

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

/**
 *	@brief A representation of an OBD-II PID.
 *
 * pid - The 1 byte PID.
 * name - A human readable name to use for this PID when published.
 * min - minimum value for this pid
 * max - maximum value for this pid
 * unit - unit used
 * frequency - The frequency to request this PID if supported by the vehicle
 *		when automatic, recurring OBD-II requests are enabled.
 *	supported - is it supported by the vehicle. Initialized after scan
 */
typedef struct _Obd2Pid {
	uint8_t pid;
	const char* generic_name;
	const int min;
	const int max;
	enum UNIT unit;
	int frequency;
	bool supported;
} Obd2Pid;

std::vector<Obd2Pid>& get_obd2_signals();
uint32_t get_signal_id(const Obd2Pid& sig);
void find_obd2_signals(const openxc_DynamicField &key, std::vector<Obd2Pid*>& found_signals);

/**
 * @brief - Object to handle obd2 session with pre-scan of supported pid
 * then request them regularly
 */
class obd2_handler_t {
	private:

	public:
		obd2_handler_t();
		/**
		 * @brief:
		 *
		 * Returns
		 */
		void find_obd2_pid(const char *name, std::vector<Obd2Pid> *pids);

		/**
		 * @brief Check if a request is an OBD-II PID request.
		 *
		 * @return true if the request is a mode 1  request and it has a 1 byte PID.
		 */
		bool is_obd2_request(DiagnosticRequest *request);

		/**
		* @brief Check if requested signal name is an obd2 pid
		* 
		* @return true if name began with ob2.* else false.
		*/
		bool is_obd2_signal(const char *name);

		/**
		* @brief Decode the payload of an OBD-II PID.
		*
		* This function matches the type signature for a DiagnosticResponseDecoder, so
		* it can be used as the decoder for a DiagnosticRequest. It returns the decoded
		* value of the PID, using the standard formulas (see
		* http://en.wikipedia.org/wiki/OBD-II_PIDs#Mode_01).
		*
		* @param[in] DiagnosticResponse response - the received DiagnosticResponse (the data is in response.payload,
		*      a byte array). This is most often used when the byte order is
		*      signiticant, i.e. with many OBD-II PID formulas.
		* @param[in] float parsed_payload - the entire payload of the response parsed as an int.
		*/
		float handle_obd2_pid(const DiagnosticResponse* response, float parsedPayload);
};