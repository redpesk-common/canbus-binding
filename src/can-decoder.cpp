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

float decoder_t::parseSignalBitfield(CanSignal& signal, const can_message_t& message)
{
	 return bitfield_parse_float(message.get_data(), CAN_MESSAGE_SIZE,
			signal.bitPosition, signal.bitSize, signal.factor,
			signal.offset);
}

openxc_DynamicField decoder_t::noopDecoder(CanSignal& signal,
		const std::vector<CanSignal>& signals, float value, bool* send)
{
	openxc_DynamicField decoded_value;
	decoded_value.has_type = true;
	decoded_value.type = openxc_DynamicField_Type_NUM;
	decoded_value.has_numeric_value = true;
	decoded_value.numeric_value = value;

	return decoded_value;
}

openxc_DynamicField decoder_t::booleanDecoder(CanSignal& signal,
		const std::vector<CanSignal>& signals, float value, bool* send)
{
	openxc_DynamicField decoded_value;
	decoded_value.has_type = true;
	decoded_value.type = openxc_DynamicField_Type_BOOL;
	decoded_value.has_boolean_value = true;
	decoded_value.boolean_value = value == 0.0 ? false : true;

	return decoded_value;
}

openxc_DynamicField decoder_t::ignoreDecoder(CanSignal& signal,
		const std::vector<CanSignal>& signals, float value, bool* send)
{
	if(send)
	  *send = false;
	
	openxc_DynamicField decoded_value = {0, openxc_DynamicField_Type_BOOL, 0, "", 0, 0, 0, 0};
	
	return decoded_value;
}

openxc_DynamicField decoder_t::stateDecoder(CanSignal& signal,
		const std::vector<CanSignal>& signals, float value, bool* send)
{
	openxc_DynamicField decoded_value = {0, openxc_DynamicField_Type_BOOL, 0, "", 0, 0, 0, 0};
	decoded_value.has_type = true;
	decoded_value.type = openxc_DynamicField_Type_STRING;
	decoded_value.has_string_value = true;

	/* TODO: Handle SignalState 
	const CanSignalState* signalState = lookupSignalState(value, signal);
	if(signalState != NULL) {
		::strcpy(decoded_value.string_value, signalState->name);
	} else {
		*send = false;
	}*/
	return decoded_value;
}

openxc_DynamicField decoder_t::translateSignal(CanSignal& signal, can_message_t& message,
	const std::vector<CanSignal>& signals)
{
	if(&signal == nullptr || &message == nullptr)
	{
		openxc_DynamicField ret = {0, openxc_DynamicField_Type_BOOL, 0, "", 0, 0, 0, 0};
		return ret;
	}

	float value = parseSignalBitfield(signal, message);
	DEBUG(binder_interface, "translateSignal: Decoded message: %f", value);

	bool send = true;
	// Must call the decoders every time, regardless of if we are going to
	// decide to send the signal or not.
	openxc_DynamicField decoded_value = decodeSignal(signal,
			value, signals, &send);

	signal.received = true;
	signal.lastValue = value;
	return decoded_value;
}

openxc_DynamicField decoder_t::decodeSignal( CanSignal& signal,
		float value, const std::vector<CanSignal>& signals, bool* send)
{
	SignalDecoder decoder = signal.decoder == NULL ?
							noopDecoder : signal.decoder;
	openxc_DynamicField decoded_value = decoder(signal, signals,
			value, send);
	return decoded_value;
}

openxc_DynamicField decoder_t::decodeSignal( CanSignal& signal,
		const can_message_t& message, const std::vector<CanSignal>& signals, bool* send)
{
	float value = parseSignalBitfield(signal, message);
	return decodeSignal(signal, value, signals, send);
}
