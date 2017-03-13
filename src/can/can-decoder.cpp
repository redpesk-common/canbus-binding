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

#include "can-decoder.hpp"
#include "canutil/read.h"
#include "../utils/openxc-utils.hpp"

float decoder_t::parseSignalBitfield(can_signal_t& signal, const can_message_t& message)
{
	 return bitfield_parse_float(message.get_data(), CAN_MESSAGE_SIZE,
			signal.get_bit_position(), signal.get_bit_size(), signal.get_factor(),
			signal.get_offset());
}

openxc_DynamicField decoder_t::noopDecoder(can_signal_t& signal,
		const std::vector<can_signal_t>& signals, float value, bool* send)
{
	openxc_DynamicField decoded_value = build_DynamicField(value);

	return decoded_value;
}

openxc_DynamicField decoder_t::booleanDecoder(can_signal_t& signal,
		const std::vector<can_signal_t>& signals, float value, bool* send)
{
	openxc_DynamicField decoded_value = build_DynamicField(value == 0.0 ? false : true);

	return decoded_value;
}

openxc_DynamicField decoder_t::ignoreDecoder(can_signal_t& signal,
		const std::vector<can_signal_t>& signals, float value, bool* send)
{
	if(send)
	  *send = false;
	
	openxc_DynamicField decoded_value = {0, openxc_DynamicField_Type_BOOL, 0, "", 0, 0, 0, 0};
	
	return decoded_value;
}

openxc_DynamicField decoder_t::stateDecoder(can_signal_t& signal,
		const std::vector<can_signal_t>& signals, float value, bool* send)
{
	const std::string signal_state = signal.get_states(value);
	openxc_DynamicField decoded_value = build_DynamicField(signal_state);
	if(signal_state.size() <= 0)
	{
		*send = false;
		ERROR(binder_interface, "stateDecoder: No state found with index: %d", (int)value);
	}
	return decoded_value;
}

openxc_DynamicField decoder_t::translateSignal(can_signal_t& signal, can_message_t& message,
	const std::vector<can_signal_t>& signals)
{
	if(&signal == nullptr || &message == nullptr)
	{
		openxc_DynamicField ret = {0, openxc_DynamicField_Type_BOOL, 0, "", 0, 0, 0, 0};
		return ret;
	}

	float value = decoder_t::parseSignalBitfield(signal, message);
	DEBUG(binder_interface, "translateSignal: Decoded message from parseSignalBitfield: %f", value);

	bool send = true;
	// Must call the decoders every time, regardless of if we are going to
	// decide to send the signal or not.
	openxc_DynamicField decoded_value = decoder_t::decodeSignal(signal,
			value, signals, &send);

	signal.set_received(true);
	signal.set_last_value(value);
	return decoded_value;
}

openxc_DynamicField decoder_t::decodeSignal( can_signal_t& signal,
		float value, const std::vector<can_signal_t>& signals, bool* send)
{
	SignalDecoder decoder = signal.get_decoder() == nullptr ?
							noopDecoder : signal.get_decoder();
	openxc_DynamicField decoded_value = decoder(signal, signals,
			value, send);
	return decoded_value;
}

openxc_DynamicField decoder_t::decodeSignal( can_signal_t& signal,
		const can_message_t& message, const std::vector<can_signal_t>& signals, bool* send)
{
	float value = parseSignalBitfield(signal, message);
	return decodeSignal(signal, value, signals, send);
}
