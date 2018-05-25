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

#include "can-encoder.hpp"

#include "canutil/write.h"
#include "../utils/openxc-utils.hpp"
#include "can-message-definition.hpp"

/// @brief Write a value in a CAN signal in the destination buffer.
///
/// @param[in] signal - The CAN signal to write, including the bit position and bit size.
/// @param[in] value - The encoded integer value to write in the CAN signal.
/// @param[out] data - The destination buffer.
/// @param[in] length - The length of the destination buffer.
///
/// @return Returns a can_frame struct initialized and ready to be send.
const can_frame encoder_t::build_frame(const std::shared_ptr<can_signal_t>& signal, uint64_t value)
{
	struct can_frame cf;
	::memset(&cf, 0, sizeof(cf));

	cf.can_id = signal->get_message()->get_id();
	cf.can_dlc = CAN_MAX_DLEN;

	signal->set_last_value((float)value);

	for(const auto& sig: signal->get_message()->get_can_signals())
	{
		float last_value = sig->get_last_value();
		bitfield_encode_float(last_value,
							sig->get_bit_position(),
							sig->get_bit_size(),
							sig->get_factor(),
							sig->get_offset(),
							cf.data,
							CAN_MAX_DLEN);
	}

	return cf;
}

/// @brief Encode a boolean into an integer, fit for a CAN signal bitfield.
///
/// This is a shortcut for encodeDynamicField(CanSignal*, openxc_DynamicField*,
/// bool*) that takes care of creating the DynamicField object for you with the
/// boolean value.
///
/// @param[in] signal  - The CAN signal to encode this value for..
/// @param[in] value - The boolean value to encode
/// @param[out] send - An output argument that will be set to false if the value should
///     not be sent for any reason.
///
/// @return Returns the encoded integer. If 'send' is changed to false, the field could
/// not be encoded and the return value is undefined.
///
uint64_t encoder_t::encode_boolean(const can_signal_t& signal, bool value, bool* send)
{
	return encode_number(signal, float(value), send);
}
/// @brief Encode a float into an integer, fit for a CAN signal's bitfield.
///
/// This is a shortcut for encodeDynamicField(CanSignal*, openxc_DynamicField*,
/// bool*) that takes care of creating the DynamicField object for you with the
/// float value.
///
/// @param[in] signal  - The CAN signal to encode this value for.
/// @param[in] value - The float value to encode.
/// @param[out] send - This output argument will always be set to false, so the caller will
///      know not to publish this value to the pipeline.
///
/// @return Returns the encoded integer. If 'send' is changed to false, the field could
/// not be encoded and the return value is undefined.
///
uint64_t encoder_t::encode_number(const can_signal_t& signal, float value, bool* send)
{
	return float_to_fixed_point(value, signal.get_factor(), signal.get_offset());
}

/// @brief Encode a string into an integer, fit for a CAN signal's bitfield.
///
/// Be aware that the behavior is undefined if there are multiple values assigned
/// to a single state. See https://github.com/openxc/vi-firmware/issues/185.
///
/// This is a shortcut for encodeDynamicField(CanSignal*, openxc_DynamicField*,
/// bool*) that takes care of creating the DynamicField object for you with the
/// string state value.
///
/// @param[in] signal  - The details of the signal that contains the state mapping.
/// @param[in] value - The string state value to encode.
/// @param[out] send - An output argument that will be set to false if the value should
///     not be sent for any reason.
///
/// @return Returns the encoded integer. If 'send' is changed to false, the field could
/// not be encoded and the return value is undefined.
///
uint64_t encoder_t::encode_state(const can_signal_t& signal, const std::string& state, bool* send)
{
	uint64_t value = 0;
	if(state == "")
	{
		AFB_DEBUG("Can't write state of "" -- not sending");
		*send = false;
	}
	else
	{
		uint64_t signal_state = signal.get_states(state);
		if(signal_state != -1) {
			value = signal_state;
		} else {
			AFB_DEBUG("Couldn't find a valid signal state for %s", state.c_str());
			*send = false;
		}
	}
	return value;
}

/// @brief Parse a signal from a CAN message and apply any required
/// transforations to get a human readable value.
///
/// If the can_signal_t has a non-NULL 'decoder' field, the raw CAN signal value
/// will be passed to the decoder before returning.
///
/// @param[in] signal - The details of the signal to decode and forward.
/// @param[in] value - The numerical value that will be converted to a boolean.
/// @param[out] send - An output parameter that will be flipped to false if the value could
///      not be decoded.
///
/// @return The decoder returns an openxc_DynamicField, which may contain a number,
/// string or boolean. If 'send' is false, the return value is undefined.
///
uint64_t encoder_t::encode_DynamicField( can_signal_t& signal, const openxc_DynamicField& field, bool* send)
{
	uint64_t value = 0;
	switch(field.type) {
		case openxc_DynamicField_Type_STRING:
			value = encode_state(signal, field.string_value, send);
			break;
		case openxc_DynamicField_Type_NUM:
			value = encode_number(signal, (float)field.numeric_value, send);
			break;
		case openxc_DynamicField_Type_BOOL:
			value = encode_boolean(signal, field.boolean_value, send);
			break;
		default:
			AFB_DEBUG("Dynamic field didn't have a value, can't encode");
			*send = false;
			break;
	}
	return value;
}
