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

openxc_VehicleMessage build_VehicleMessage_with_SimpleMessage(openxc_DynamicField_Type type, const openxc_SimpleMessage& message)
{
	struct timeb t_msec;
	long long int timestamp_msec;
	
	openxc_VehicleMessage v;
	
	if(!::ftime(&t_msec))
	{
		timestamp_msec = ((long long int) t_msec.time) * 1000ll + 
							(long long int) t_msec.millitm;

		v.has_type = true;
		v.type = openxc_VehicleMessage_Type::openxc_VehicleMessage_Type_SIMPLE;
		v.has_simple_message = true;
		v.simple_message =  message;
		v.has_timestamp = true;
		v.timestamp = timestamp_msec;

		return v;
	}

	v.has_type = true,
	v.type = openxc_VehicleMessage_Type::openxc_VehicleMessage_Type_SIMPLE;
	v.has_simple_message = true;
	v.simple_message =  message;

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
		json = json_object_new_object();
		json_object_object_add(json, "name", json_object_new_string(s_msg.name));
		jsonify_DynamicField(s_msg.value, json);
		return true;
	}
	json_object_object_add(json, "error", json_object_new_string("openxc_SimpleMessage doesn't have name'"));
	return false;
}