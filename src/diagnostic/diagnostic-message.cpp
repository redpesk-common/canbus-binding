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

#include "obd2-signals.hpp"

#include "../configuration.hpp"
#include "../utils/signals.hpp"

const char *UNIT_NAMES[10] = {
	"POURCENT",
	"DEGREES_CELSIUS",
	"KPA",
	"RPM",
	"GRAMS_SEC",
	"SECONDS",
	"KM",
	"KM_H",
	"PA",
	"NM"
};

obd2_signal_t::obd2_signal_t(uint8_t pid, const char* generic_name, const int min, const int max, enum UNIT unit, int frequency, bool supported)
	:	pid_{pid}, generic_name_{generic_name}, min_{min}, max_{max}, unit_{unit}, frequency_{frequency}, supported_{supported}
{
}

uint32_t obd2_signal_t::get_pid()
{
	return (uint32_t)pid_;
}

const std::string& obd2_signal_t::get_generic_name() const
{
	return generic_name_;
}

const std::string obd2_signal_t::get_name() const
{
	return active_diagnostic_request_t::get_prefix() + "." + generic_name_;
}

int obd2_signal_t::get_frequency() const
{
	return frequency_;
}

/**
 * @brief Build a DiagnosticRequest struct to be passed
 *  to diagnostic manager instance.
 */
const DiagnosticRequest obd2_signal_t::build_diagnostic_request()
{
	return {/*arbitration_id: */OBD2_FUNCTIONAL_BROADCAST_ID,
			/*mode: */0x1,
			/*has_pid: */true,
			/*pid: */pid_,
			/*pid_length: */0,
			/*payload[]: */{0},
			/*payload_length: */0,
			/*no_frame_padding: */false,
			/*DiagnosticRequestType: */DiagnosticRequestType::DIAGNOSTIC_REQUEST_TYPE_PID };
}

/**
* @brief Check if a request is an OBD-II PID request.
*
* @return true if the request is a mode 1 request and it has a 1 byte PID.
*/
bool obd2_signal_t::is_obd2_request(const DiagnosticRequest* request)
{
	return request->mode == 0x1 && request->has_pid && request->pid < 0xff;
}
