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

#pragma once

/*
 * Build a specific VehicleMessage containing a SimpleMessage.
 */
openxc_VehicleMessage build_VehicleMessage_with_SimpleMessage(openxc_DynamicField_Type type,
	const openxc_SimpleMessage& message);

/*
 * Build an openxc_SimpleMessage associating a name to an openxc_DynamicField
 */
openxc_SimpleMessage build_SimpleMessage(const std::string& name, const openxc_DynamicField& value);
/* 
 * Build an openxc_DynamicField depending what we pass as argument
 */
openxc_DynamicField build_DynamicField(const std::string& value);

/* 
 * Build an openxc_DynamicField depending what we pass as argument
 */
openxc_DynamicField build_DynamicField(double value);

/* 
 * Build an openxc_DynamicField depending what we pass as argument
 */
openxc_DynamicField build_DynamicField(bool value);

void jsonify_DynamicField(const openxc_DynamicField& field, const json_object& value);

/* Extract the simple message value from an openxc_VehicleMessage
 * and return it, or null if there isn't.
 */
openxc_SimpleMessage get_simple_message(const openxc_VehicleMessage& v_msg);

json_object jsonify_simple(const openxc_SimpleMessage& s_msg);