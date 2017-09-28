/*
 * Copyright (C) 2015, 2016 "IoT.bzh"
 * Author "Romain Forlot" <romain.forlot@iot.bzh>
 * Author "Loic Collignon" <loic.collignon@iot.bzh>
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

#include "openxc-utils.hpp"

#include "../binding/application.hpp"

///
/// @brief Build a specific VehicleMessage containing a DiagnosticResponse.
///
/// @param[in] request - Original request use to retrieve decoder and callback
/// @param[in] response - Response to the request that will be decoded if decoder set
///  and put into the DiagnosticResponse of the VehicleMessage.
/// @param[in] parsed_value - raw parsed value of the payload from CAN message
///
/// @return a vehicle message including simple message that will be convert into
/// a JSON object before being pushed to the subscribers
///
const openxc_VehicleMessage build_VehicleMessage(active_diagnostic_request_t* request, const DiagnosticResponse& response, float parsed_value)
{
	openxc_VehicleMessage message;
	application_t& app = application_t::instance();

	message.has_type = true;
	message.type = openxc_VehicleMessage_Type::openxc_VehicleMessage_Type_DIAGNOSTIC;
	message.has_diagnostic_response = true;
	message.diagnostic_response.has_bus = true;
	message.diagnostic_response.bus = app.get_can_bus_manager().get_can_device_index(
																app.get_diagnostic_manager().get_bus_name());
	message.diagnostic_response.has_message_id = true;

	if(request->get_id() != OBD2_FUNCTIONAL_BROADCAST_ID)
	{
		message.diagnostic_response.message_id = response.arbitration_id
			- DIAGNOSTIC_RESPONSE_ARBITRATION_ID_OFFSET;
	}
	else
	{
		// must preserve responding arb ID for responses to functional broadcast
		// requests, as they are the actual module address and not just arb ID +
		// 8.
		message.diagnostic_response.message_id = response.arbitration_id;
	}

	message.diagnostic_response.has_mode = true;
	message.diagnostic_response.mode = response.mode;
	message.diagnostic_response.has_pid = response.has_pid;
	if(message.diagnostic_response.has_pid)
		message.diagnostic_response.pid = response.pid;
	message.diagnostic_response.has_success = true;
	message.diagnostic_response.success = response.success;
	message.diagnostic_response.has_negative_response_code = !response.success;
	message.diagnostic_response.negative_response_code =
			response.negative_response_code;

	if(response.payload_length > 0)
	{
		if(request->get_decoder() != nullptr)
		{
			message.diagnostic_response.has_value = true;
			message.diagnostic_response.value = parsed_value;
		}
		else
		{
			message.diagnostic_response.has_payload = true;
			::memcpy(message.diagnostic_response.payload.bytes, response.payload,
					response.payload_length);
			message.diagnostic_response.payload.size = response.payload_length;
		}
	}

	return message;
}
///
/// @brief Build a specific VehicleMessage containing a SimpleMessage with associated timestamp
///
/// @param[in] message - simple message to include into openxc_VehicleMessage
/// @param[in] timestamp - timestamp from ioctl when reading the socket
///
/// @return a vehicle message including simple message that will be convert into
/// a JSON object before being pushed to the subscribers
///
const openxc_VehicleMessage build_VehicleMessage(const openxc_SimpleMessage& message, uint64_t timestamp)
{
	openxc_VehicleMessage v;

	v.has_type = true,
	v.type = openxc_VehicleMessage_Type::openxc_VehicleMessage_Type_SIMPLE;
	v.has_simple_message = true;
	v.simple_message =  message;
	v.has_timestamp = true;
	v.timestamp = timestamp;

	return v;
}

///
/// @brief Build a specific VehicleMessage containing a SimpleMessage.
///
/// @param[in] message - simple message to include into openxc_VehicleMessage
///
/// @return a vehicle message including simple message that will be convert into
/// a JSON object before being pushed to the subscribers
///
const openxc_VehicleMessage build_VehicleMessage(const openxc_SimpleMessage& message)
{
	openxc_VehicleMessage v;

	v.has_type = true,
	v.type = openxc_VehicleMessage_Type::openxc_VehicleMessage_Type_SIMPLE;
	v.has_simple_message = true;
	v.simple_message =  message;
	v.has_timestamp = true;
	v.timestamp = system_time_us();

	return v;
}

///
/// @brief Build an empty VehicleMessage that isn't usable by at least the struct
///  is initialized for the most part and can be use to check a false return value.
///
/// @return A VehicleMessage with all boolean value to false.
///
openxc_VehicleMessage build_VehicleMessage()
{
	openxc_VehicleMessage v;

	::memset(&v, 0, sizeof(openxc_VehicleMessage));
	return v;
}

bool is_valid(const openxc_VehicleMessage& v)
{
	if (v.has_type == false &&
		v.has_can_message == false &&
		v.has_simple_message == false &&
		v.has_diagnostic_response == false &&
		v.has_control_command == false &&
		v.has_command_response == false &&
		v.has_timestamp == false)
			return false;
	return true;
}

///
/// @brief Build an openxc_SimpleMessage associating a name to an openxc_DynamicField
///
/// @param[in] name - const string reference name to assign to the created SimpleMessage
///  this will set has_name member to true and assign name to the name member. Maximum size for name is
///  set to 100 char.
/// @param[in] value - const reference with DynamicField to assign to SimpleMessage
///  value.
///
/// @return an openxc_SimpleMessage struct initialized with name and value provided.
///
const openxc_SimpleMessage build_SimpleMessage(const std::string& name, const openxc_DynamicField& value)
{
	openxc_SimpleMessage s;

	s.has_name = true;
	::strncpy(s.name, name.c_str(), 100);
	s.has_value = true;
	s.value = value;

	return s;
}

///
/// @brief Build an openxc_DynamicField with a string value
///
/// @param[in] value - const string reference value to assign to builded
///  openxc_DynamicField.
///
/// @return openxc_DynamicField initialized with a string value.
///
const openxc_DynamicField build_DynamicField(const char* value)
{
	openxc_DynamicField d;
	d.has_type = true;
	d.type = openxc_DynamicField_Type_STRING;

	d.has_string_value = true;
	d.has_numeric_value = false;
	d.has_boolean_value = false;
	::strncpy(d.string_value, value, 100);

	return d;
}

///
/// @brief Build an openxc_DynamicField with a string value
///
/// @param[in] value - const string reference value to assign to builded
///  openxc_DynamicField.
///
/// @return openxc_DynamicField initialized with a string value.
///
const openxc_DynamicField build_DynamicField(const std::string& value)
{
	openxc_DynamicField d;
	d.has_type = true;
	d.type = openxc_DynamicField_Type_STRING;

	d.has_string_value = true;
	d.has_numeric_value = false;
	d.has_boolean_value = false;
	::strncpy(d.string_value, value.c_str(), 100);

	return d;
}

///
/// @fn openxc_DynamicField build_DynamicField(double value);
///
/// @brief Build an openxc_DynamicField with a double value
///
/// @param[in] value - double value to assign to builded openxc_DynamicField.
///
/// @return openxc_DynamicField initialized with a double value.
///
const openxc_DynamicField build_DynamicField(double value)
{
	openxc_DynamicField d;
	d.has_type = true;
	d.type = openxc_DynamicField_Type_NUM;

	d.has_string_value = false;
	d.has_numeric_value = true;
	d.has_boolean_value = false;
	d.numeric_value = value;

	return d;
}

///
/// @brief Build an openxc_DynamicField with a boolean value
///
/// @param[in] value - boolean value to assign to builded openxc_DynamicField.
///
/// @return openxc_DynamicField initialized with a boolean value.
///
const openxc_DynamicField build_DynamicField(bool value)
{
	openxc_DynamicField d;
	d.has_type = true;
	d.type = openxc_DynamicField_Type_BOOL;

	d.has_string_value = false;
	d.has_numeric_value = false;
	d.has_boolean_value = true;
	d.boolean_value = value;

	return d;
}

int get_bool_from_DynamicField(const openxc_VehicleMessage& v_msg, bool* ret)
{
	if(v_msg.has_simple_message && v_msg.simple_message.has_value && v_msg.simple_message.value.has_boolean_value)
	{
		*ret = v_msg.simple_message.value.boolean_value;
		return 0;
	}

	return -1;
}

double get_numerical_from_DynamicField(const openxc_VehicleMessage& v_msg)
{
	return (v_msg.has_simple_message && v_msg.simple_message.has_value && v_msg.simple_message.value.has_numeric_value) ?
		v_msg.simple_message.value.numeric_value : -1.0;
}

const std::string get_string_from_DynamicField(const openxc_VehicleMessage& v_msg)
{
return (v_msg.has_simple_message && v_msg.simple_message.has_value && v_msg.simple_message.value.has_string_value) ?
		v_msg.simple_message.value.string_value : "";
}

///
/// @brief Extract the simple message value from an openxc_VehicleMessage
///  and return it. If there isn't SimpleMessage in the VehicleMessage then
///  returned value will be a SimpleMessage with all field set at false.
///  DynamicField from SimpleMessage will be boolean DynamicField set to false too.
///
/// @param[in] v_msg - const reference to openxc_VehicleMessage
///
/// @return A simpleMessage from the provided VehicleMessage.
///
const openxc_SimpleMessage get_simple_message(const openxc_VehicleMessage& v_msg)
{
	if (v_msg.has_simple_message)
		return v_msg.simple_message;

	openxc_SimpleMessage s_msg = { false, "", false, build_DynamicField(false), false, build_DynamicField(false)};
	return s_msg;
}

///
/// @brief Make a JSON object from a DynamicField
///
/// @param[in] field - openxc_DynamicField struct to convert into
///  a json object.
/// @param[out] value - pointer to the object to set up.
///
void jsonify_DynamicField(const openxc_DynamicField& field, json_object* value)
{
	if(field.has_numeric_value)
		json_object_object_add(value, "value", json_object_new_double(field.numeric_value));
	else if(field.has_boolean_value)
		json_object_object_add(value, "value", json_object_new_boolean(field.boolean_value));
	else if(field.has_string_value)
		json_object_object_add(value, "value", json_object_new_string(field.string_value));
}

///
/// @brief Make a JSON object from a SimpleMessage
///
/// @param[in] s_msg - const reference to an openxc_SimpleMessage
/// struct to convert into a json object.
/// @param[out] json - pointer with the DynamicField converted into json object
///
/// @return True if SimpleMessage has been transformed into json object
///  and false if not. In such case, a json object is returned { "error": "error msg"}
///
bool jsonify_simple(const openxc_SimpleMessage& s_msg, json_object* json)
{
	if(s_msg.has_name)
	{
		json_object_object_add(json, "name", json_object_new_string(s_msg.name));
		jsonify_DynamicField(s_msg.value, json);
		return true;
	}
	json_object_object_add(json, "error", json_object_new_string("openxc_SimpleMessage doesn't have name'"));
	return false;
}

///
/// @brief Make a JSON object from a VehicleMessage
///
/// @param[in] v_msg - const reference to an openxc_VehicleMessage
/// struct to convert into a json object.
/// @param[out] json - pointer with the DynamicField converted into json object
///
/// @return True if VehicleMessage has been transformed into json object
///  and false if not. In such case, a json object is returned { "error": "error msg"}
///
bool jsonify_vehicle(const openxc_VehicleMessage& v_msg, json_object* json)
{
	if(jsonify_simple(get_simple_message(v_msg), json))
	{
		if(v_msg.has_timestamp)
		{
			json_object_object_add(json, "timestamp", json_object_new_int64(v_msg.timestamp));
			return true;
		}
		return true;
	}
	json_object_object_add(json, "error", json_object_new_string("openxc_SimpleMessage doesn't have name'"));
	return false;
}
