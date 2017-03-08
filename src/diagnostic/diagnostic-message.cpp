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

#include "obd2/obd2-signals.hpp"

#include "utils/signals.hpp"

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

/**
 * @fn std::vector<std::string> find_signals(const openxc_DynamicField &key)
 * @brief return signals name found searching through CAN_signals and OBD2 pid
 * 
 * @param[in] key - can contain numeric or string value in order to search against 
 *   can signals or obd2 signals name.
 *
 * @return Vector of signals name found. 
 */
void obd2_signal_t::find_obd2_signals(const openxc_DynamicField &key, std::vector<obd2_signal_t*>& found_signals)
{
	switch(key.type)
	{
		case openxc_DynamicField_Type::openxc_DynamicField_Type_STRING:
				lookup_signals_by_name(key.string_value, get_obd2_signals(), found_signals);
			break;
		case openxc_DynamicField_Type::openxc_DynamicField_Type_NUM:
				lookup_signals_by_id(key.numeric_value, get_obd2_signals(), found_signals);
			break;
		default:
			ERROR(binder_interface, "find_signals: wrong openxc_DynamicField specified. Use openxc_DynamicField_Type_NUM or openxc_DynamicField_Type_STRING type only.");
			break;
	}
	DEBUG(binder_interface, "Found %d signal(s)", (int)found_signals.size());
}

bool obd2_signal_t::is_obd2_response(can_message_t can_message)
{
	/*
	if(can_message.get_id() >= 0x7E8 && can_message.get_id() <= 0x7EF)
	{
		openxc_VehicleMessage message = {0};
		message.has_type = true;
		message.type = openxc_VehicleMessage_Type_DIAGNOSTIC;
		message.has_diagnostic_response = true;
		message.diagnostic_response = {0};
		message.diagnostic_response.has_bus = true;
		message.diagnostic_response.bus = bus->address;
		message.diagnostic_response.has_message_id = true;
		//7DF should respond with a random message id between 7e8 and 7ef
		//7E0 through 7E7 should respond with a id that is 8 higher (7E0->7E8)
		if(can_message.get_id() == 0x7DF)
		{
			message.diagnostic_response.message_id = rand()%(0x7EF-0x7E8 + 1) + 0x7E8;
		}
		else if(commandRequest->message_id >= 0x7E0 && commandRequest->message_id <= 0x7E7)
		{
			message.diagnostic_response.message_id = commandRequest->message_id + 8;
		}
		message.diagnostic_response.has_mode = true;
		message.diagnostic_response.mode = commandRequest->mode;
		if(commandRequest->has_pid)
		{
			message.diagnostic_response.has_pid = true;
			message.diagnostic_response.pid = commandRequest->pid;
		}
		message.diagnostic_response.has_value = true;
		message.diagnostic_response.value = rand() % 100;
		pipeline::publish(&message, &getConfiguration()->pipeline);
	}
	else //If it's outside the range, the command_request will return false
	{
		debug("Sent message ID is outside the valid range for emulator (7DF to 7E7)");
		status=false;
	}
	return false;
	*/
}

void obd2_signal_t::add_request(int pid)
{
	DiagnosticRequest request = {
	arbitration_id: OBD2_FUNCTIONAL_BROADCAST_ID,
	mode: 0x1, has_pid: true, pid_ };
}

/**
* @brief Check if a request is an OBD-II PID request.
*
* @return true if the request is a mode 1 request and it has a 1 byte PID.
*/
bool obd2_signal_t::is_obd2_request(DiagnosticRequest* request)
{
	return request->mode == 0x1 && request->has_pid && request->pid < 0xff;
}

/**
* @brief Check if requested signal name is an obd2 pid
* 
* @return true if name began with obd2 else false.
*/
bool obd2_signal_t::is_obd2_signal(const char *name)
{
	if(fnmatch("obd2.*", name, FNM_CASEFOLD) == 0)
		return true;
	return false;
}

/**
* @brief Decode the payload of an OBD-II PID.
*
* This function matches the type signature for a DiagnosticResponseDecoder, so
* it can be used as the decoder for a DiagnosticRequest. It returns the decoded
* value of the PID, using the standard formulas (see
* http://en.wikipedia.org/wiki/OBD-II_PIDs#Mode_01).
*
* @param[in] response - the received DiagnosticResponse (the data is in response.payload,
*  a byte array). This is most often used when the byte order is
*  signiticant, i.e. with many OBD-II PID formulas.
* @param[in] parsed_payload - the entire payload of the response parsed as an int.
*/
float obd2_signal_t::decode_obd2_response(const DiagnosticResponse* response, float parsedPayload)
{
	return diagnostic_decode_obd2_pid(response);
}