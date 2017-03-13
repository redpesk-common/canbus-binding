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

#include <string>
#include <json-c/json.h>
#include <sys/timeb.h>

#include "openxc.pb.h"
#include "../obd2/active-diagnostic-request.hpp"

/**
 * @brief Build a specific VehicleMessage containing a SimpleMessage.
 *
 * @param[in] message - simple message to include into openxc_VehicleMessage
 *
 * @return a vehicle message including simple message that will be convert into 
 * a JSON object before being pushed to the subscribers
 */
openxc_VehicleMessage build_VehicleMessage(active_diagnostic_request_t* request, const DiagnosticResponse& response, float parsed_value);
openxc_VehicleMessage build_VehicleMessage(const openxc_SimpleMessage& message);

/**
 * @fn openxc_SimpleMessage build_SimpleMessage(const std::string& name, const openxc_DynamicField& value);
 *
 * @brief Build an openxc_SimpleMessage associating a name to an openxc_DynamicField
 *
 * @param[in] name - const string reference name to assign to the created SimpleMessage
 *  this will set has_name member to true and assign name to the name member. Maximum size for name is 
 *  set to 100 char.
 * @param[in] value - const reference with DynamicField to assign to SimpleMessage
 *  value.
 *
 * @return an openxc_SimpleMessage struct initialized with name and value provided.
 */
openxc_SimpleMessage build_SimpleMessage(const std::string& name, const openxc_DynamicField& value);

/**
 * @fn openxc_DynamicField build_DynamicField(const std::string& value);
 *
 * @brief Build an openxc_DynamicField with a string value
 *
 * @param[in] value - const string reference value to assign to builded
 *  openxc_DynamicField.
 *
 * @return openxc_DynamicField initialized with a string value.
 */
openxc_DynamicField build_DynamicField(const std::string& value);

/**
 * @fn openxc_DynamicField build_DynamicField(double value);
 *
 * @brief Build an openxc_DynamicField with a double value
 *
 * @param[in] value - double value to assign to builded openxc_DynamicField.
 *
 * @return openxc_DynamicField initialized with a double value.
 */
openxc_DynamicField build_DynamicField(double value);

/**
 * @fn openxc_DynamicField build_DynamicField(bool value);
 *
 * @brief Build an openxc_DynamicField with a boolean value
 *
 * @param[in] value - boolean value to assign to builded openxc_DynamicField.
 *
 * @return openxc_DynamicField initialized with a boolean value.
 */
openxc_DynamicField build_DynamicField(bool value);

/**
 * @fn void jsonify_DynamicField(const openxc_DynamicField& field, json_object* value);
 *
 * @brief Make a JSON object from a DynamicField
 *
 * @param[in] field - openxc_DynamicField struct to convert into
 *  a json object.
 * @param[out] value - pointer to the object to set up.
 */
void jsonify_DynamicField(const openxc_DynamicField& field, json_object* value);

/**
 * @fn openxc_SimpleMessage get_simple_message(const openxc_VehicleMessage& v_msg);
 *
 * @brief Extract the simple message value from an openxc_VehicleMessage
 *  and return it. If there isn't SimpleMessage in the VehicleMessage then
 *  returned value will be a SimpleMessage with all field set at false.
 *  DynamicField from SimpleMessage will be boolean DynamicField set to false too.
 *
 * @param[in] v_msg - const reference to openxc_VehicleMessage
 *
 * @return A simpleMessage from the provided VehicleMessage.
 */
openxc_SimpleMessage get_simple_message(const openxc_VehicleMessage& v_msg);

/**
 * @fn json_object* jsonify_simple(const openxc_SimpleMessage& s_msg);
 *
 * @brief Make a JSON object from a SimpleMessage
 *
 * @param[in] s_msg - const reference to an openxc_SimpleMessage 
 * struct to convert into a json object.
 * @param[out] json - pointer with the DynamicField converted into json object
 * 
 * @return True if SimpleMessage has been transformed into json object
 *  and false if not. In such case, a json object is returned { "error": "error msg"}
 */
bool jsonify_simple(const openxc_SimpleMessage& s_msg, json_object* json);