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

decoder_t::decoder_t()
 : decoded_value{false, openxc_DynamicField_Type_STRING, false, "", false, 0, false, false}
{
}

float decoder_t::parseSignalBitfield(const CanSignal& signal, const CanMessage& message)
{
	 return bitfield_parse_float(message->data, CAN_MESSAGE_SIZE,
			signal->bitPosition, signal->bitSize, signal->factor,
			signal->offset);
}

openxc_DynamicField decoder_t::noopDecoder(const CanSignal& signal,
        const CanSignal& signals, float value, bool* send)
{
	decoded_value = { .has_type = true,
					.type = openxc_DynamicField_Type_NUM,
					.has_numeric_value = true,
					.numeric_value = value };
    return decoded_value;
}

openxc_DynamicField decoder_t::booleanDecoder(const CanSignal& signal,
        const CanSignal& signals, float value, bool* send)
{
	decoded_value = { .has_type = true,
					.type = openxc_DynamicField_Type_BOOL,
					.has_boolean_value = true,
					.numeric_value = value == 0.0 ? false : true };
	return decoded_value;
}

openxc_DynamicField decoder_t::ignoreDecoder(const CanSignal& signal,
        const CanSignal& signals, float value, bool* send)
{
    if(send)
      *send = false;
    
    openxc_DynamicField decoded_value = {0};
    return decoded_value;
}

openxc_DynamicField decoder_t::stateDecoder(const CanSignal& signal,
        const CanSignal& signals, float value, bool* send)
{
    openxc_DynamicField decoded_value = {0};
    decoded_value.has_type = true;
    decoded_value.type = openxc_DynamicField_Type_STRING;
    decoded_value.has_string_value = true;

    const CanSignalState* signalState = lookupSignalState(value, signal);
    if(signalState != NULL) {
        ::strcpy(decoded_value.string_value, signalState->name);
    } else {
        *send = false;
    }
    return decoded_value;
}

openxc_DynamicField decoder_t::decodeSignal(const CanSignal& signal,
        float value, const CanSignal& signals, bool* send)
{
    SignalDecoder decoder = signal->decoder == NULL ?
				            noopDecoder : signal->decoder;
    openxc_DynamicField decoded_value = decoder(signal, signals,
            value, send);
    return decoded_value;
}

openxc_DynamicField decoder_t::decodeSignal(const CanSignal& signal,
        const CanMessage& message, const CanSignal& signals, bool* send) {
    float value = parseSignalBitfield(signal, message);
    return decodeSignal(signal, value, signals, send);
}