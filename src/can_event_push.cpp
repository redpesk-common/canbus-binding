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

#include <linux/can.h>
#include <linux/can/raw.h>
#include <queue>

#include <afb/afb-binding.h>

#include "can-utils.h"
#include "openxc.pb.h"
#include "json-c/json.h"

void can_event_push(can_bus_t *can_bus)
{
	openxc_VehicleMessage *v_message;
	openxc_SimpleMessage *s_message;
	iterator it_event;

	while(true)
	{
		if(v_message = can_bus->next_vehicle_message())
		{
			s_message = get_simple_message(v_msg);
			it_event = event_map.find(s_msg->name);
			afb_event_push(it_event->second, jsonify_simple(s_msg));
		}
	}
}

void jsonify_DynamicField(openxc_DynamicField *field, json_object *value)
{
	if(field->has_numeric_value)
		json_object_object_add(value, "value", json_object_new_double(field->numeric_value));
	else if(field->has_boolean_value)
		json_object_object_add(value, "value", json_object_new_boolean(field->boolean_value));
	else if(field->has_string_value)
		json_object_object_add(value, "value", json_object_new_string(field->string_value));

	return value;
}

/* Extract the simple message value from an openxc_VehicleMessage
 * and return it, or null if there isn't.
 */
openxc_SimpleMessage* get_simple_message(openxc_VehicleMessage *v_msg)
{
	if(v_msg->has_simple_message)
		return v_msg->simple_message;
	
	return nullptr;
}

json_object* jsonify_simple(openxc_SimpleMessage *s_msg)
{
	json_object *json;
	json = json_object_new_object();

	if(!s_msg->has_name)
		return nullptr;

	json_object_object_add(json, "name", json_object_new_string(s_msg->name));
	jsonify_DynamicField(&s_msg->value, json);

	return json;
}