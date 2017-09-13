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
#include "../diagnostic/active-diagnostic-request.hpp"

const openxc_VehicleMessage build_VehicleMessage(active_diagnostic_request_t* request, const DiagnosticResponse& response, float parsed_value);
const openxc_VehicleMessage build_VehicleMessage(const openxc_SimpleMessage& message, uint64_t timestamp);
const openxc_VehicleMessage build_VehicleMessage(const openxc_SimpleMessage& message);
openxc_VehicleMessage build_VehicleMessage();
bool is_valid(const openxc_VehicleMessage& v);

const openxc_SimpleMessage build_SimpleMessage(const std::string& name, const openxc_DynamicField& value);
const openxc_DynamicField build_DynamicField(const char* value);
const openxc_DynamicField build_DynamicField(const std::string& value);
const openxc_DynamicField build_DynamicField(double value);
const openxc_DynamicField build_DynamicField(bool value);

int get_bool_from_DynamicField(const openxc_VehicleMessage& v_msg, bool& ret);
double get_numerical_from_DynamicField(const openxc_VehicleMessage& v_msg);
const std::string get_string_from_DynamicField(const openxc_VehicleMessage& v_msg);
const openxc_SimpleMessage get_simple_message(const openxc_VehicleMessage& v_msg);

void jsonify_DynamicField(const openxc_DynamicField& field, json_object* value);

bool jsonify_simple(const openxc_SimpleMessage& s_msg, json_object* json);

bool jsonify_vehicle(const openxc_VehicleMessage& v_msg, json_object* json);
