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

#include <climits>
#include <cmath>
#include <low-can/can/can-decoder.hpp>
#include <low-can/utils/openxc-utils.hpp>
#include <low-can/utils/frame-codec.hpp>
#include <low-can/can/message-definition.hpp>
#include <low-can/binding/low-can-hat.hpp>

#include "canutil/read.h"
#include "../utils/converter.hpp"


/// @brief Handle sign of the signal according to several decoding methods
///
/// @param[in] signal - The signal
/// @param[in] data_signal - The data of the signal
/// @param[in] new_end_bit - The last bit of in the last byte of the data (data_signal[0])
/// @param[in] can_data - The whole can data (needed for SIGN BIT EXTERN)
///
/// @return Returns the sign of the data
///
int decoder_t::handle_sign(const signal_t& signal, std::vector<uint8_t>& data_signal, uint8_t new_end_bit, const std::vector<uint8_t>& can_data)
{
	uint8_t data_byte = 0;
	uint8_t mask = 0;
	int end_bit = 0;

	if(signal.get_sign() == sign_t::UNSIGNED)
		return 1;
	else if(signal.get_sign() == sign_t::SIGN_BIT_EXTERN) {
		end_bit = signal.get_bit_sign_position() % CHAR_BIT;
		mask = static_cast<uint8_t>((1 << (end_bit + 1)) - 1);
		data_byte = can_data[signal.get_bit_sign_position() / CHAR_BIT] & mask;
	}
	else {
		end_bit = new_end_bit;
		mask = static_cast<uint8_t>((1 << (end_bit + 1)) - 1);
		data_byte = data_signal[0] & mask;
	}

	//if negative: decode with right method
	if(data_byte  >> end_bit) {
		switch(signal.get_sign())
		{
			//remove the sign bit to get the absolute value
			case sign_t::SIGN_BIT:
				data_signal[0] = static_cast<uint8_t>(data_signal[0] & (mask >> 1));
				break;
			//same method twos complement = ones complement + 1
			case sign_t::ONES_COMPLEMENT:
			case sign_t::TWOS_COMPLEMENT:
				//complement only until end_bit
				data_signal[0] = ((data_signal[0] ^ mask) & mask);
				if(data_signal.size() > 1)
					for(int i=1; i < data_signal.size(); i++)
						data_signal[i] = data_signal[i] ^ 0xFF;
				if(signal.get_sign() == sign_t::TWOS_COMPLEMENT)
					data_signal[data_signal.size() - 1] = static_cast<uint8_t>(data_signal[data_signal.size() - 1] + 1);
				break;
			case sign_t::SIGN_BIT_EXTERN:
				break;
			default:
				AFB_ERROR("Not a valid sign entry %d, considering the value as unsigned", signal.get_sign());
				break;
		}
		return -1;
	}
	return 1;
}

/// @brief Parses the signal's bitfield from the given data and returns the raw
/// value.
///
/// @param[in] signal - The signal to be parsed from the data.
/// @param[in] message - message_t to parse
///
/// @return Returns the raw value of the signal parsed as a bitfield from the given byte
/// array.
///
uint64_t decoder_t::parse_signal_raw_value(signal_t& signal, std::shared_ptr<message_t> message)
{
	return frame_codec::decode(
				message->get_data_vector().data(),
				signal.get_bit_position(),
				signal.get_bit_size(),
				signal.get_endian() != LittleEndian,
				signal.get_sign() != UNSIGNED);
}

/// @brief Parses the signal's bitfield from the given data and returns the value.
///
/// @param[in] signal - The signal to be parsed from the data.
/// @param[in] message - message_t to parse
///
/// @return Returns the value of the signal parsed as a bitfield from the given byte
/// array.
///
float decoder_t::parse_signal_bitfield(signal_t& signal, std::shared_ptr<message_t> message)
{
	int sign;
	std::vector<uint8_t> data;
	std::vector<uint8_t> data_signal;
	uint8_t bit_size = (uint8_t) signal.get_bit_size();
	uint32_t bit_position = signal.get_bit_position();

	int new_start_byte = 0;
	int new_end_byte = 0;
	uint8_t new_start_bit = 0;
	uint8_t new_end_bit = 0;

	data = message->get_data_vector();
	converter_t::signal_to_bits_bytes(bit_position, bit_size, new_start_byte, new_end_byte, new_start_bit, new_end_bit);

	for(int i=new_start_byte;i<=new_end_byte;i++)
		data_signal.push_back(data[i]);

	sign = handle_sign(signal, data_signal, new_end_bit, data);

	if(data_signal.size() > 65535)
		AFB_ERROR("Too long data signal %s", signal.get_name().c_str());

	return static_cast<float>(sign) * bitfield_parse_float(data_signal.data(), (uint16_t) data_signal.size(),
			new_start_bit, bit_size, signal.get_factor(),
			signal.get_offset());
}


/// @brief Decode and return string bytes (hex) for a CAN signal's.
///
/// This is an implementation of the Signal type signature, and can be
/// used directly in the signal_t.decoder field.
///
/// @param[in] signal  - The details of the signal.
/// @param[in] message - The message with data to decode.
/// @param[out] send - An output argument that will be set to false if the value should
///     not be sent for any reason.
///
/// @return Returns a DynamicField with a string value of bytes (hex)
///
openxc_DynamicField decoder_t::decode_bytes(signal_t& signal, std::shared_ptr<message_t> message, bool* send)
{
	int i=0;
	openxc_DynamicField decoded_value;
	std::vector<uint8_t> data = message->get_data_vector();
	uint32_t length = message->get_length();
	uint32_t bit_position = signal.get_bit_position();
	uint32_t bit_size = signal.get_bit_size();

	std::vector<uint8_t> new_data = std::vector<uint8_t>();
	new_data.reserve((bit_size / CHAR_BIT) + 1);

	int new_start_byte = 0;
	int new_end_byte = 0;
	uint8_t new_start_bit = 0;
	uint8_t new_end_bit = 0;

	converter_t::signal_to_bits_bytes(bit_position, bit_size, new_start_byte, new_end_byte, new_start_bit, new_end_bit);

	if(new_end_byte >= length)
		new_end_byte = length-1;

	if(new_start_byte >= length)
	{
		AFB_ERROR("Error in signal's description");
		return decoded_value;
	}

	uint8_t mask_first_v = static_cast<uint8_t>(0xFF << new_start_bit);
	uint8_t mask_last_v = static_cast<uint8_t>(0xFF >> (7 - new_end_bit));

	if(new_start_byte == new_end_byte)
	{
		data[new_start_byte] = data[new_start_byte] & (mask_first_v & mask_last_v);
	}
	else
	{
		data[new_start_byte] = data[new_start_byte] & mask_first_v;
		data[new_end_byte] = data[new_end_byte] & mask_last_v;
	}

	for(i=new_start_byte ; i <= new_end_byte ; i++)
		new_data.push_back(data[i]);

	decoded_value = build_DynamicField(new_data);

	return decoded_value;
}


/// @brief Decode and return string bytes (hex) for a CAN signal's.
///
/// This is an implementation of the Signal type signature, and can be
/// used directly in the signal_t.decoder field.
///
/// @param[in] signal  - The details of the signal.
/// @param[in] message - The message with data to decode.
/// @param[out] send - An output argument that will be set to false if the value should
///     not be sent for any reason.
///
/// @return Returns a DynamicField with a string value of bytes (hex)
///
openxc_DynamicField decoder_t::decode_ascii(signal_t& signal, std::shared_ptr<message_t> message, bool* send)
{
	std::string ret_s = "";
	openxc_DynamicField openxc_bytes = decode_bytes(signal,message,send);
	if(!openxc_bytes.has_bytes_value)
		AFB_ERROR("Error no bytes value to translate to ascii");
	ret_s = converter_t::to_ascii(openxc_bytes.bytes_value,openxc_bytes.length_array);
	openxc_DynamicField ret = build_DynamicField(ret_s);
	return ret;
}

/// @brief Wraps a raw CAN signal value in a DynamicField without modification.
///
/// This is an implementation of the Signal type signature, and can be
/// used directly in the signal_t.decoder field.
///
/// @param[in] signal - The details of the signal that contains the state mapping.
/// @param[in] message - The message with data to decode.
/// @param[out] send - An output argument that will be set to false if the value should
///     not be sent for any reason.
///
/// @return Returns a DynamicField with the original, unmodified raw CAN signal value as
/// its numeric value. The 'send' argument will not be modified as this decoder
/// always succeeds.
///
openxc_DynamicField decoder_t::decode_noop(signal_t& signal, std::shared_ptr<message_t> message, bool* send)
{
	uint64_t value = parse_signal_raw_value(signal, message);
	AFB_DEBUG("Decoded message from parse_signal_raw_value: %llu", (long long unsigned)value);

	uint64_t prval = signal.get_last_raw_value();
	signal.set_last_raw_value(value);
	float val = signal.get_last_value();

	/* If we have an exact value to match then min = max
	 * here we compare equality of float values naively because they come
	 * from the signal object attributes and hasn't been computed at all so
	 * values are comparable using simple operator (it's a bit lazy ok well)
	 */
	float min_value = signal.get_min_value();
	float max_value = signal.get_max_value();
	if( (! isnanf(min_value) && val < min_value) ||
	    (! isnanf(max_value) && val > max_value)
	  )
	{
		AFB_DEBUG("Value doesn't match for signal %s", signal.get_name().c_str());
		signal.set_last_raw_value(prval); // restore older value
		return build_DynamicField_error();
	}

	// Don't send if they is no changes
	if ((prval == value && !signal.get_send_same()) || !*send)
		*send = false;

	signal.set_received(true);
	openxc_DynamicField decoded_value = build_DynamicField(val);
	return decoded_value;
}
/// @brief Coerces a numerical value to a boolean.
///
/// This is an implementation of the Signal type signature, and can be
/// used directly in the signal_t.decoder field.
///
/// @param[in] signal  - The details of the signal that contains the state mapping.
/// @param[in] message - The message with data to decode.
/// @param[out] send - An output argument that will be set to false if the value should
///     not be sent for any reason.
///
/// @return Returns a DynamicField with a boolean value of false if the raw signal value
/// is 0.0, otherwise true. The 'send' argument will not be modified as this
/// decoder always succeeds.
///
openxc_DynamicField decoder_t::decode_boolean(signal_t& signal, std::shared_ptr<message_t> message, bool* send)
{
	uint64_t value = parse_signal_raw_value(signal, message);
	AFB_DEBUG("Decoded message from parse_signal_raw_value: %llu", (long long unsigned)value);

	// Don't send if they is no changes
	if ((signal.get_last_raw_value() == value && !signal.get_send_same()) || !*send)
		*send = false;

	signal.set_last_raw_value(value);
	signal.set_received(true);

	openxc_DynamicField decoded_value = build_DynamicField(value != 0);
	return decoded_value;
}
/// @brief Update the metadata for a signal and the newly received value.
///
/// This is an implementation of the Signal type signature, and can be
/// used directly in the signal_t.decoder field.
///
/// This function always flips 'send' to false.
///
/// @param[in] signal  - The details of the signal that contains the state mapping.
/// @param[in] message - The message with data to decode.
/// @param[out] send - This output argument will always be set to false, so the caller will
///      know not to publish this value to the pipeline.
///
/// @return Return value is undefined.
///
openxc_DynamicField decoder_t::decode_ignore(signal_t& signal, std::shared_ptr<message_t> message, bool* send)
{
	uint64_t value = parse_signal_raw_value(signal, message);

	if(send)
	  *send = false;

	signal.set_last_raw_value(value);
	openxc_DynamicField decoded_value;

	return decoded_value;
}

/// @brief Find and return the corresponding string state for a CAN signal's
/// raw integer value.
///
/// This is an implementation of the Signal type signature, and can be
/// used directly in the signal_t.decoder field.
///
/// @param[in] signal  - The details of the signal that contains the state mapping.
/// @param[in] message - The message with data to decode.
/// @param[out] send - An output argument that will be set to false if the value should
///     not be sent for any reason.
///
/// @return Returns a DynamicField with a string value if a matching state is found in
/// the signal. If an equivalent isn't found, send is sent to false and the
/// return value is undefined.
///
openxc_DynamicField decoder_t::decode_state(signal_t& signal, std::shared_ptr<message_t> message, bool* send)
{
	uint64_t value = parse_signal_raw_value(signal, message);
	AFB_DEBUG("Decoded message from parse_signal_raw_value: %llu", (long long unsigned)value);

	// check validity of received data
	const std::string signal_state = signal.get_states((uint8_t)value);
	if(signal_state.size() <= 0)
	{
		AFB_ERROR("No state found with index: %d", (int)value);
		return build_DynamicField_error();
	}

	if ((signal.get_last_raw_value() == value && !signal.get_send_same()) || !*send )
		*send = false;

	signal.set_last_raw_value(value);
	signal.set_received(true);

	openxc_DynamicField decoded_value = build_DynamicField(signal_state);
	return decoded_value;
}


/// @brief Parse a signal from a CAN message, apply any required transforations
///      to get a human readable value and public the result to the pipeline.
///
/// If the signal_t has a non-NULL 'decoder' field, the raw CAN signal value
/// will be passed to the decoder before publishing.
///
/// @param[in] signal - The details of the signal to decode and forward.
/// @param[in] message - The message with data to decode.
/// @param[out] send - An output parameter that will be flipped to false if the value could
///      not be decoded.
///
/// The decoder returns an openxc_DynamicField, which may contain a number,
/// string or boolean.
///
openxc_DynamicField decoder_t::translate_signal(signal_t& signal, std::shared_ptr<message_t> message, bool* send)
{
	// Must call the decoders every time, regardless of if we are going to
	// decide to send the signal or not.
	openxc_DynamicField decoded_value
		// if the message is special TIMEOUT send last value
		 = message->is_timeout() ? build_DynamicField(signal.get_last_value())
					// otherwise decode the frame
					: decoder_t::decode_signal(signal, message, send);

	signal.set_received(true);
	signal.set_timestamp(message->get_timestamp());
	signal.get_message()->set_last_value(message);
	return decoded_value;
}

/// @brief Parse a signal from a CAN message and apply any required
/// transforations to get a human readable value.
///
/// If the signal_t has a non-NULL 'decoder' field, the raw CAN signal value
/// will be passed to the decoder before returning.
///
/// @param[in] signal - The details of the signal to decode and forward.
/// @param[in] message - The message with data to decode.
/// @param[out] send - An output parameter that will be flipped to false if the value could
///      not be decoded.
///
/// @return The decoder returns an openxc_DynamicField, which may contain a number,
/// string or boolean. If 'send' is false, the return value is undefined.
///
openxc_DynamicField decoder_t::decode_signal( signal_t& signal, std::shared_ptr<message_t> message, bool* send)
{
	signal_decoder decoder = signal.get_decoder() == nullptr ?
							decode_noop : signal.get_decoder();

	openxc_DynamicField decoded_value = decoder(signal,
			message, send);
	return decoded_value;
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
