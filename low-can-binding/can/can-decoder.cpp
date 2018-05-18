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
#include "can-message-definition.hpp"
#include "../binding/low-can-hat.hpp"

/// @brief Parses the signal's bitfield from the given data and returns the raw
/// value.
///
/// @param[in] signal - The signal to be parsed from the data.
/// @param[in] message - can_message_t to parse
///
/// @return Returns the raw value of the signal parsed as a bitfield from the given byte
/// array.
///
float decoder_t::parse_signal_bitfield(can_signal_t& signal, const can_message_t& message)
{
	 return bitfield_parse_float(message.get_data(), CAN_MESSAGE_SIZE,
			signal.get_bit_position(), signal.get_bit_size(), signal.get_factor(),
			signal.get_offset());
}

/// @brief Wraps a raw CAN signal value in a DynamicField without modification.
///
/// This is an implementation of the Signal type signature, and can be
/// used directly in the can_signal_t.decoder field.
///
/// @param[in] signal - The details of the signal that contains the state mapping.
/// @param[in] value - The numerical value that will be wrapped in a DynamicField.
/// @param[out] send - An output argument that will be set to false if the value should
///     not be sent for any reason.
///
/// @return Returns a DynamicField with the original, unmodified raw CAN signal value as
/// its numeric value. The 'send' argument will not be modified as this decoder
/// always succeeds.
///
openxc_DynamicField decoder_t::decode_noop(can_signal_t& signal, float value, bool* send)
{
	openxc_DynamicField decoded_value = build_DynamicField(value);

	return decoded_value;
}
/// @brief Coerces a numerical value to a boolean.
///
/// This is an implementation of the Signal type signature, and can be
/// used directly in the can_signal_t.decoder field.
///
/// @param[in] signal  - The details of the signal that contains the state mapping.
/// @param[in] value - The numerical value that will be converted to a boolean.
/// @param[out] send - An output argument that will be set to false if the value should
///     not be sent for any reason.
///
/// @return Returns a DynamicField with a boolean value of false if the raw signal value
/// is 0.0, otherwise true. The 'send' argument will not be modified as this
/// decoder always succeeds.
///
openxc_DynamicField decoder_t::decode_boolean(can_signal_t& signal, float value, bool* send)
{
	openxc_DynamicField decoded_value = build_DynamicField(value == 0.0 ? false : true);

	return decoded_value;
}
/// @brief Update the metadata for a signal and the newly received value.
///
/// This is an implementation of the Signal type signature, and can be
/// used directly in the can_signal_t.decoder field.
///
/// This function always flips 'send' to false.
///
/// @param[in] signal  - The details of the signal that contains the state mapping.
/// @param[in] value - The numerical value that will be converted to a boolean.
/// @param[out] send - This output argument will always be set to false, so the caller will
///      know not to publish this value to the pipeline.
///
/// @return Return value is undefined.
///
openxc_DynamicField decoder_t::decode_ignore(can_signal_t& signal, float value, bool* send)
{
	if(send)
	  *send = false;

	openxc_DynamicField decoded_value;

	return decoded_value;
}

/// @brief Find and return the corresponding string state for a CAN signal's
/// raw integer value.
///
/// This is an implementation of the Signal type signature, and can be
/// used directly in the can_signal_t.decoder field.
///
/// @param[in] signal  - The details of the signal that contains the state mapping.
/// @param[in] value - The numerical value that should map to a state.
/// @param[out] send - An output argument that will be set to false if the value should
///     not be sent for any reason.
///
/// @return Returns a DynamicField with a string value if a matching state is found in
/// the signal. If an equivalent isn't found, send is sent to false and the
/// return value is undefined.
///
openxc_DynamicField decoder_t::decode_state(can_signal_t& signal, float value, bool* send)
{
	const std::string signal_state = signal.get_states((uint8_t)value);
	openxc_DynamicField decoded_value = build_DynamicField(signal_state);
	if(signal_state.size() <= 0)
	{
		*send = false;
		AFB_ERROR("No state found with index: %d", (int)value);
	}
	return decoded_value;
}


/// @brief Parse a signal from a CAN message, apply any required transforations
///      to get a human readable value and public the result to the pipeline.
///
/// If the can_signal_t has a non-NULL 'decoder' field, the raw CAN signal value
/// will be passed to the decoder before publishing.
///
/// @param[in] signal - The details of the signal to decode and forward.
/// @param[in] message - The received CAN message that should contain this signal.
/// @param[out] send - An output parameter that will be flipped to false if the value could
///      not be decoded.
///
/// The decoder returns an openxc_DynamicField, which may contain a number,
/// string or boolean.
///
openxc_DynamicField decoder_t::translate_signal(can_signal_t& signal, const can_message_t& message, bool* send)
{
	float value = decoder_t::parse_signal_bitfield(signal, message);
	AFB_DEBUG("Decoded message from parse_signal_bitfield: %f", value);

	// Must call the decoders every time, regardless of if we are going to
	// decide to send the signal or not.
	openxc_DynamicField decoded_value = decoder_t::decode_signal(signal,
			value, send);

	signal.set_received(true);

	// Don't send if they is no changes
	if ((signal.get_last_value() == value && !signal.get_send_same()) || !*send )
	{
		*send = false;
	}
	signal.set_last_value(value);
	signal.set_timestamp(message.get_timestamp());
	signal.get_message()->set_last_value(message);
	return decoded_value;
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
openxc_DynamicField decoder_t::decode_signal( can_signal_t& signal, float value, bool* send)
{
	signal_decoder decoder = signal.get_decoder() == nullptr ?
							decode_noop : signal.get_decoder();
	openxc_DynamicField decoded_value = decoder(signal,
			value, send);
	return decoded_value;
}

/// @brief Decode a transformed, human readable value from an raw CAN signal
/// already parsed from a CAN message.
///
/// This is the same as decode_signal but you must parse the bitfield value of the signal from the CAN
/// message yourself. This is useful if you need that raw value for something
/// else.
///
/// @param[in] signal - The details of the signal to decode and forward.
/// @param[in] message - Raw CAN message to decode
/// @param[out] send - An output parameter that will be flipped to false if the value could
///      not be decoded.
///
openxc_DynamicField decoder_t::decode_signal( can_signal_t& signal, const can_message_t& message, bool* send)
{
	float value = parse_signal_bitfield(signal, message);
	return decode_signal(signal, value, send);
}


///
/// @brief Decode the payload of an OBD-II PID.
///
/// This function matches the type signature for a DiagnosticResponse, so
/// it can be used as the decoder for a DiagnosticRequest. It returns the decoded
/// value of the PID, using the standard formulas (see
/// http://en.wikipedia.org/wiki/OBD-II_PIDs#Mode_01).
///
/// @param[in] response - the received DiagnosticResponse (the data is in response.payload,
///  a byte array). This is most often used when the byte order is
///  signiticant, i.e. with many OBD-II PID formulas.
/// @param[in] parsed_payload - the entire payload of the response parsed as an int.
///
/// @return Float decoded value.
///
float decoder_t::decode_obd2_response(const DiagnosticResponse* response, float parsed_payload)
{
	return diagnostic_decode_obd2_pid(response);
}
