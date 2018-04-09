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
#include <map>

#include "diagnostic-message.hpp"

#include "../binding/application.hpp"
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

diagnostic_message_t::diagnostic_message_t(
		uint8_t pid,
		const std::string& generic_name,
		const int min,
		const int max,
		enum UNIT unit,
		float frequency,
		DiagnosticResponseDecoder decoder,
		DiagnosticResponseCallback callback,
		bool supported,
		bool received)
		: parent_{nullptr},
		pid_{pid},
		generic_name_{generic_name},
		min_{min},
		max_{max},
		unit_{unit},
		frequency_{frequency},
		decoder_{decoder},
		callback_{callback},
		supported_{supported},
		last_timestamp_{0},
		received_{received},
		last_value_{.0f}
{}

uint32_t diagnostic_message_t::get_pid()
{
	return (uint32_t)pid_;
}

const std::string diagnostic_message_t::get_generic_name() const
{
	return generic_name_;
}

const std::string diagnostic_message_t::get_name() const
{
	return active_diagnostic_request_t::get_prefix() + "." + generic_name_;
}

float diagnostic_message_t::get_frequency() const
{
	return frequency_;
}

DiagnosticResponseDecoder diagnostic_message_t::get_decoder() const
{
	return decoder_;
}
DiagnosticResponseCallback diagnostic_message_t::get_callback() const
{
	return callback_;
}

bool diagnostic_message_t::get_supported() const
{
	return supported_;
}

bool diagnostic_message_t::get_received() const
{
	return received_;
}

float diagnostic_message_t::get_last_value() const
{
	return last_value_;
}

std::pair<float, uint64_t> diagnostic_message_t::get_last_value_with_timestamp() const
{
	return std::make_pair(last_value_, last_timestamp_);
}

void diagnostic_message_t::set_supported(bool value)
{
	supported_ = value;
}

void diagnostic_message_t::set_parent(can_message_set_t* parent)
{
	parent_ = parent;
}

void diagnostic_message_t::set_received(bool r)
{
	received_ = r;
}

void diagnostic_message_t::set_last_value(float val)
{
	last_value_ = val;
}

void diagnostic_message_t::set_timestamp(uint64_t timestamp)
{
	last_timestamp_ = timestamp;
}

///
/// @brief Build a DiagnosticRequest struct to be passed
///  to diagnostic manager instance.
///
 const DiagnosticRequest diagnostic_message_t::build_diagnostic_request() const
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

///
/// @brief Check if a request is an OBD-II PID request.
///
/// @return true if the request is a mode 1 request and it has a 1 byte PID.
///
bool diagnostic_message_t::is_obd2_request(const DiagnosticRequest* request)
{
	return request->mode == 0x1 && request->has_pid && request->pid < 0xff;
}
