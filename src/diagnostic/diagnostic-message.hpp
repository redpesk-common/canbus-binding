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
#include "can/can-bus.hpp"
#include "can/can-message.hpp"

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

/**
 * @brief - Object to handle obd2 session with pre-scan of supported pid
 * then request them regularly
 */
class obd2_signals_t {
	private:
		uint8_t pid_;
		const char* generic_name_;
		const int min_;
		const int max_;
		enum UNIT unit_;
		int frequency_;
		bool supported_;

	public:
		const char* generic_name = generic_name_;
		obd2_signals_t(uint8_t pid, const char* generic_name, const int min_, const int max_, enum UNIT unit, int frequency, bool supported);

		void add_request(int pid);

		void find_obd2_signals(const openxc_DynamicField &key, std::vector<obd2_signals_t*>& found_signals);

		bool is_obd2_response(can_message_t can_message);
		bool is_obd2_request(DiagnosticRequest *request);
		bool is_obd2_signal(const char *name);

		float decode_obd2_response(const DiagnosticResponse* response, float parsedPayload);
};

uint32_t get_signal_id(const obd2_signals_t& sig);
std::vector<obd2_signals_t>& get_obd2_signals();