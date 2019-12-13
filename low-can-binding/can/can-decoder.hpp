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

class decoder_t
{
public:
	static int handle_sign(const signal_t& signal, std::vector<uint8_t>& data_signal, uint8_t new_end_bit, const std::vector<uint8_t>& can_data);
	static float parse_signal_bitfield(signal_t& signal, std::shared_ptr<message_t> message);

	static openxc_DynamicField decode_state(signal_t& signal, std::shared_ptr<message_t> message, bool* send);
	static openxc_DynamicField decode_boolean(signal_t& signal, std::shared_ptr<message_t> message, bool* send);
	static openxc_DynamicField decode_ignore(signal_t& signal, std::shared_ptr<message_t> message, bool* send);
	static openxc_DynamicField decode_noop(signal_t& signal, std::shared_ptr<message_t> message, bool* send);
	static openxc_DynamicField decode_bytes(signal_t& signal, std::shared_ptr<message_t> message, bool* send);
	static openxc_DynamicField decode_ascii(signal_t& signal, std::shared_ptr<message_t> message, bool* send);
	static openxc_DynamicField decode_date(signal_t& signal, std::shared_ptr<message_t> message, bool* send);
	static openxc_DynamicField decode_time(signal_t& signal, std::shared_ptr<message_t> message, bool* send);

	static openxc_DynamicField translate_signal(signal_t& signal, std::shared_ptr<message_t> message, bool* send);

	static openxc_DynamicField decode_signal(signal_t& signal, std::shared_ptr<message_t> message, bool* send);

	static float decode_obd2_response(const DiagnosticResponse* response, float parsed_payload);
};
