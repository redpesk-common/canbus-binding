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

#include "../configuration.hpp"

openxc_VehicleMessage build_VehicleMessage(active_diagnostic_request_t* request, const DiagnosticResponse& response, float parsed_value)
{
	openxc_VehicleMessage message;

	message.has_type = true;
	message.type = openxc_VehicleMessage_Type::openxc_VehicleMessage_Type_DIAGNOSTIC;
	message.has_diagnostic_response = true;
	message.diagnostic_response.has_bus = true;
	message.diagnostic_response.bus = configuration_t::instance().get_diagnostic_manager().get_can_bus_dev()->get_address();
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

openxc_VehicleMessage build_VehicleMessage(const openxc_SimpleMessage& message)
{
	openxc_VehicleMessage v;
	
	v.has_type = true,
	v.type = openxc_VehicleMessage_Type::openxc_VehicleMessage_Type_SIMPLE;
	v.has_simple_message = true;
	v.simple_message =  message;
	v.has_timestamp = true;
	v.timestamp = system_time_ms();

	return v;
}

openxc_SimpleMessage build_SimpleMessage(const std::string& name, const openxc_DynamicField& value)
{
	openxc_SimpleMessage s;

	s.has_name = true;
	::strncpy(s.name, name.c_str(), 100);
	s.has_value = true;
	s.value = value;

	return s;
}

openxc_DynamicField build_DynamicField(const std::string& value)
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

openxc_DynamicField build_DynamicField(double value)
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

openxc_DynamicField build_DynamicField(bool value)
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

openxc_SimpleMessage get_simple_message(const openxc_VehicleMessage& v_msg)
{
	if (v_msg.has_simple_message)
		return v_msg.simple_message;
	
	openxc_SimpleMessage s_msg = { false, "", false, build_DynamicField(false), false, build_DynamicField(false)};
	return s_msg;
}

void jsonify_DynamicField(const openxc_DynamicField& field, json_object* value)
{
	if(field.has_numeric_value)
		json_object_object_add(value, "value", json_object_new_double(field.numeric_value));
	else if(field.has_boolean_value)
		json_object_object_add(value, "value", json_object_new_boolean(field.boolean_value));
	else if(field.has_string_value)
		json_object_object_add(value, "value", json_object_new_string(field.string_value));
}

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