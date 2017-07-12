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

#include "can-signals.hpp"
#include "can-message.hpp"
#include "openxc.pb.h"

class decoder_t
{
public:
	static float parse_signal_bitfield(can_signal_t& signal, const can_message_t& message);

	static openxc_DynamicField decode_state(can_signal_t& signal, float value, bool* send);
	static openxc_DynamicField decode_boolean(can_signal_t& signal, float value, bool* send);
	static openxc_DynamicField decode_ignore(can_signal_t& signal, float value, bool* send);
	static openxc_DynamicField decode_noop(can_signal_t& signal, float value, bool* send);

	static openxc_DynamicField translate_signal(can_signal_t& signal, const can_message_t& messag, bool* send);

	static openxc_DynamicField decode_signal(can_signal_t& signal, const can_message_t& message, bool* send);

	static openxc_DynamicField decode_signal(can_signal_t& signal, float value, bool* send);

	static float decode_obd2_response(const DiagnosticResponse* response, float parsed_payload);
};
