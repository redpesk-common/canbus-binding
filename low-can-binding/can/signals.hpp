///
/// Copyright (C) 2015, 2016 "IoT.bzh"
/// Author "Romain Forlot" <romain.forlot@iot.bzh>
///
/// Licensed under the Apache License, Version 2.0 (the "License");
/// you may not use this file except in compliance with the License.
/// You may obtain a copy of the License at
///
///	 http://www.apache.org/licenses/LICENSE-2.0
///
/// Unless required by applicable law or agreed to in writing, software
/// distributed under the License is distributed on an "AS IS" BASIS,
/// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
/// See the License for the specific language governing permissions and
/// limitations under the License.
///

#pragma once

#include <map>
#include <mutex>
#include <vector>
#include <string>
#include <memory>

#include "openxc.pb.h"
#include "message-definition.hpp"
#include "../utils/timer.hpp"
#include "../diagnostic/diagnostic-message.hpp"

#define MESSAGE_SET_ID 0

class signal_t;
///
/// @brief The type signature for a CAN signal decoder.
///
/// A signal_decoder transforms a raw floating point CAN signal into a number,
/// string or boolean.
///
/// @param[in] signal - The CAN signal that we are decoding.
/// @param[in] message - The message with data to decode.
/// @param[out] send - An output parameter. If decoding fails or CAN signal is
///	not sending, this should be flipped to false.
///
/// @return a decoded value in an openxc_DynamicField struct.
///
typedef openxc_DynamicField (*signal_decoder)(signal_t& signal, std::shared_ptr<message_t> message, bool* send);

///
/// @brief: The type signature for a CAN signal encoder.
///
/// A signal_encoder transforms a number, string or boolean into a raw floating
/// point value that fits in the CAN signal.
///
/// @param[in] signal - The CAN signal to encode.
/// @param[in] value - The dynamic field to encode.
/// @param[out] send - An output parameter. If the encoding failed or the CAN signal should
/// not be encoded for some other reason, this will be flipped to false.
///
typedef uint64_t (*signal_encoder)(signal_t& signal,
		 const openxc_DynamicField& field, bool* send);

enum sign_t
{
		UNSIGNED = 0,
		SIGN_BIT = 1,
		ONES_COMPLEMENT = 2,
		TWOS_COMPLEMENT = 3,
		SIGN_BIT_EXTERN = 4
};

class signal_t
{
private:
	std::shared_ptr<message_definition_t> parent_; /*!< parent_ - pointer to the parent message definition holding this signal*/
	std::string generic_name_; /*!< generic_name_ - The name of the signal to be output.*/
	static std::string prefix_; /*!< prefix_ - generic_name_ will be prefixed with it. It has to reflect the used protocol.
					 * which make easier to sort message when the come in.*/
	uint32_t bit_position_; /*!< bitPosition_ - The starting bit of the signal in its CAN message (assuming
				*	non-inverted bit numbering, i.e. the most significant bit of
				*	each byte is 0) */
	uint32_t bit_size_; /*!< bit_size_ - The width of the bit field in the CAN message. */
	float factor_; /*!< factor_ - The final value will be multiplied by this factor. Use 1 if you
			*	don't need a factor. */
	float offset_; /*!< offset_ - The final value will be added to this offset. Use 0 if you
			*	don't need an offset. */
	float min_value_; /*!< min_value_ - The minimum value for the processed signal.*/
	float max_value_; /*!< max_value_ - The maximum value for the processed signal. */
	frequency_clock_t frequency_; /*!< frequency_ - A frequency_clock_t struct to control the maximum frequency to
					   * process and send this signal. To process every value, set the
					   * clock's frequency to 0. */
	bool send_same_; /*!< send_same_ - If true, will re-send even if the value hasn't changed.*/
	bool force_send_changed_; /*!< force_send_changed_ - If true, regardless of the frequency, it will send the
				   * value if it has changed. */
	std::map<uint8_t, std::string> states_; /*!< states_ - A map of CAN signal state describing the mapping
						 * between numerical and string values for valid states. */
	bool writable_; /*!< writable - True if the signal is allowed to be written from the USB host
			 * back to CAN. Defaults to false.*/
	signal_decoder decoder_; /*!< decoder_ - An optional function to decode a signal from the bus to a human
				  * readable value. If NULL, the default numerical decoder is used. */
	signal_encoder encoder_; /*!< encoder_ - An optional function to encode a signal value to be written to
				  * CAN into a byte array. If NULL, the default numerical encoder
				  * is used. */
	bool received_; /*!< received_ - True if this signal has ever been received.*/
	float last_value_; /*!< lastValue_ - The last received value of the signal. If 'received' is false,
				* this value is undefined. */
	std::pair<bool, int> multiplex_; /*!< multiplex_ - If bool is false and int is 0 is not a multiplex signal
										If bool is true, that indicate that is a multiplexor
										If int is different of 0, that indicate the link with a multiplexor */
	sign_t sign_; /* !< sign_ - if the data is signed it indicates the encode */
	int32_t bit_sign_position_; /*!< bit_sign_position_ - The bit that indicates the sign of the signal in its CAN message*/
	std::string unit_; /* !< unit_ - The unit of the data */

public:

	signal_t(
		std::string generic_name,
		uint32_t bit_position,
		uint32_t bit_size,
		float factor,
		float offset,
		float min_value,
		float max_value,
		frequency_clock_t frequency,
		bool send_same,
		bool force_send_changed,
		std::map<uint8_t, std::string> states,
		bool writable,
		signal_decoder decoder,
		signal_encoder encoder,
		bool received,
		std::pair<bool, int> multiplex,
		sign_t sign,
		int32_t bit_sign_position,
		std::string unit);


	signal_t(
		std::string generic_name,
		uint32_t bit_position,
		uint32_t bit_size,
		float factor,
		float offset,
		float min_value,
		float max_value,
		frequency_clock_t frequency,
		bool send_same,
		bool force_send_changed,
		std::map<uint8_t, std::string> states,
		bool writable,
		signal_decoder decoder,
		signal_encoder encoder,
		bool received);


	std::shared_ptr<message_definition_t> get_message() const;
	const std::string get_generic_name() const;
	const std::string get_name() const;
	uint32_t get_bit_position() const;
	uint32_t get_bit_size() const;
	float get_factor() const;
	float get_offset() const;
	float get_min_value() const;
	float get_max_value() const;
	frequency_clock_t& get_frequency();
	bool get_send_same() const;
	const std::string get_states(uint8_t value);
	uint64_t get_states(const std::string& value) const;
	bool get_writable() const;
	signal_decoder& get_decoder();
	signal_encoder& get_encoder();
	bool get_received() const;
	float get_last_value() const;
	std::pair<float, uint64_t> get_last_value_with_timestamp() const;
	std::pair<bool, int> get_multiplex() const;
	sign_t get_sign() const;
	int32_t get_bit_sign_position() const;
	const std::string get_unit() const;

	void set_parent(std::shared_ptr<message_definition_t> parent);
	void set_received(bool r);
	void set_last_value(float val);
	void set_timestamp(uint64_t timestamp);
	void set_bit_position(uint32_t bit_position);
};
