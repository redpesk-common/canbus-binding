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

#pragma once

#include "signals.hpp"
#include "message/can-message.hpp"
#include "openxc.pb.h"
#include "message/can-message.hpp"
#ifdef USE_FEATURE_J1939
#include "message/j1939-message.hpp"
#endif

class encoder_t
{
public:
	static message_t* build_message(const std::shared_ptr<signal_t>& signal, uint64_t value, bool factor=true, bool offset=true);
	static message_t* build_frame(const std::shared_ptr<signal_t>& signal, uint64_t value, message_t *message, bool factor=true, bool offset=true);
	static uint64_t encode_state(const signal_t& signal, const std::string& value, bool* send);
	static uint64_t encode_boolean(const signal_t& signal, bool value, bool* send);
	static uint64_t encode_number(const signal_t& signal, float value, bool* send);
	static void encode_data(std::shared_ptr<signal_t> sig, std::vector<uint8_t> &data, bool filter=false, bool factor=true, bool offset=true);

	static uint64_t encode_DynamicField(signal_t& signal, const openxc_DynamicField& field, bool* send);
};