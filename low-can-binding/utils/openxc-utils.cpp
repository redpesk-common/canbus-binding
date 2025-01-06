/*
 * Copyright (C) 2015-2025 IoT.bzh Company
 * Author: "Romain Forlot" <romain.forlot@iot.bzh>
 * Author: "Loic Collignon" <loic.collignon@iot.bzh>
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

#include <canbus-binding/utils/openxc-utils.hpp>

#include <canbus-binding/can/signals.hpp>
#include <canbus-binding/diagnostic/active-diagnostic-request.hpp>

#include <canbus-binding/binding/application.hpp>
#include <canbus-binding/can/can-decoder.hpp>


#include "converter.hpp"


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
	::memset(&message, 0, sizeof(message));
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
	::memset(&v, 0, sizeof(v));

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
	::memset(&v, 0, sizeof(v));

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

	::memset(&v, 0, sizeof(v));
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
	::strncpy(s.name, name.c_str(), 99);
	s.has_value = true;
	s.value = value;

	return s;
}

/// @brief Build an openxc_DynamicField with a json object
///
/// @param[in] value - const json_object pointer to assign to convert in an
///  openxc_DynamicField.
///
/// @return openxc_DynamicField initialized with a json object.
///
const openxc_DynamicField build_DynamicField(json_object* value)
{
	switch(json_object_get_type(value))
	{
		case json_type_string:
			return build_DynamicField(json_object_get_string(value));
		case json_type_double:
			return build_DynamicField(json_object_get_double(value));
		case json_type_int:
			return build_DynamicField(json_object_get_double(value));
		case json_type_boolean:
			return build_DynamicField((bool)json_object_get_boolean(value));
		default:
			openxc_DynamicField d;
			::memset(&d, 0, sizeof(openxc_DynamicField));
			return d;
	}
}

const openxc_DynamicField build_DynamicField(std::vector<uint8_t> &array)
{
	openxc_DynamicField d;
	d.has_type = true;
	d.type = openxc_DynamicField_Type_BYTES;

	d.has_string_value = false;
	d.has_numeric_value = false;
	d.has_boolean_value = false;
	d.has_bytes_value = true;
	d.error = false;

	size_t size = array.size();

	if(size > 2040)
	{
		AFB_ERROR("Error to generate array dynamic field, too large data");
		return d;
	}
	else
	{
		 d.length_array = (uint32_t) size;
	}

	for(int i=0;i<size;i++)
		d.bytes_value[i] = array[i];

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
const openxc_DynamicField build_DynamicField(const char* value)
{
	openxc_DynamicField d;
	d.has_type = true;
	d.type = openxc_DynamicField_Type_STRING;

	d.has_string_value = true;
	d.has_numeric_value = false;
	d.has_boolean_value = false;
	d.has_bytes_value = false;
	d.has_json_value = false;
	::strncpy(d.string_value, value, 99);
	d.error = false;

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
	return build_DynamicField(value.c_str());
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
	d.has_bytes_value = false;
	d.has_json_value = false;
	d.numeric_value = value;
	d.error = false;

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
	d.has_bytes_value = false;
	d.has_json_value = false;
	d.boolean_value = value;
	d.error = false;

	return d;
}

///
/// @brief Build an openxc_DynamicField with a json value
///
/// @param[in] value - json value to assign to builded openxc_DynamicField.
///
/// @return openxc_DynamicField initialized with a json value.
///
const openxc_DynamicField build_DynamicField_json(json_object *value)
{
	openxc_DynamicField d;
	d.has_type = true;
	d.type = openxc_DynamicField_Type_JSON;

	d.has_string_value = false;
	d.has_numeric_value = false;
	d.has_boolean_value = false;
	d.has_bytes_value = false;
	d.has_json_value = true;
	d.json_value = value;
	d.error = false;

	return d;
}

const openxc_DynamicField build_DynamicField_error()
{
	openxc_DynamicField d;
	d.has_type = false;
	d.has_string_value = false;
	d.has_numeric_value = false;
	d.has_boolean_value = false;
	d.has_bytes_value = false;
	d.has_json_value = false;
	d.error = true;
	return d;
}

const openxc_DynamicField generate_openxc_DynamicField_from_message(std::shared_ptr<message_definition_t> messages_definition, std::shared_ptr<message_t> message, bool &send)
{
	openxc_DynamicField ret = build_DynamicField_json(json_object_new_array());
	openxc_DynamicField dynamicField_tmp;
	json_object *signal_json_tmp;

	for (std::shared_ptr<signal_t> sig : messages_definition->get_signals())
	{
		signal_json_tmp = json_object_new_object();
		dynamicField_tmp = decoder_t::translate_signal(*sig, message, &send);
		if (!dynamicField_tmp.error) // Not Error Match
		{
			if (dynamicField_tmp.has_type)
			{
				json_object_object_add(signal_json_tmp, sig->get_name().c_str(),
							jsonify_DynamicField(dynamicField_tmp));
				if (sig != nullptr && sig->get_unit() != "")
					json_object_object_add(signal_json_tmp, "unit", json_object_new_string(sig->get_unit().c_str()));
				json_object_array_add(ret.json_value, signal_json_tmp);
			}
			send = true;
		}
		else // Error Match
		{
			send = false;
			break;
		}
	}
	return ret;
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
json_object* jsonify_DynamicField(const openxc_DynamicField& field)
{
	if(field.has_numeric_value)
		return json_object_new_double(field.numeric_value);
	else if(field.has_boolean_value)
		return json_object_new_boolean(field.boolean_value);
	else if(field.has_string_value)
		return json_object_new_string(field.string_value);
	else if(field.has_bytes_value)
		return json_object_new_string(converter_t::to_hex(field.bytes_value, field.length_array).c_str());
	else if(field.has_json_value)
		return json_object_get(field.json_value);
	return nullptr;
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
		json_object_object_add(json, s_msg.name,
					jsonify_DynamicField(s_msg.value));
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
/// @param[in] sig - signal reference to the subscription of openxc_VehicleMessage,
/// to get more informations about it
/// @param[out] json - pointer with the DynamicField converted into json object
///
/// @return True if VehicleMessage has been transformed into json object
///  and false if not. In such case, a json object is returned { "error": "error msg"}
///
bool jsonify_vehicle(const openxc_VehicleMessage& v_msg, std::shared_ptr<signal_t> sig, json_object* json)
{
	if(sig)
		json_object_object_add(json,"id", json_object_new_int(sig->get_message()->get_id()));
	if(jsonify_simple(get_simple_message(v_msg), json))
	{
		if(sig && sig->get_unit() != "")
			json_object_object_add(json, "unit", json_object_new_string(sig->get_unit().c_str()));

		if(v_msg.has_timestamp)
			json_object_object_add(json, "timestamp", json_object_new_int64(v_msg.timestamp));

		return true;
	}
	json_object_object_add(json, "error", json_object_new_string("openxc_SimpleMessage doesn't have name'"));
	return false;
}
