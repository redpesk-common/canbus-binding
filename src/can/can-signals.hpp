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

#include <map>
#include <mutex>
#include <queue>
#include <vector>
#include <string>

#include "openxc.pb.h"
#include "utils/timer.hpp"
#include "can/can-bus.hpp"
#include "can/can-message.hpp"
#include "obd2/obd2-signals.hpp"

extern "C"
{
	#include <afb/afb-binding.h>
	#include <afb/afb-event-itf.h>
}

#define MESSAGE_SET_ID 0

/**
 * @brief The type signature for a CAN signal decoder.
 *
 * @desc A SignalDecoder transforms a raw floating point CAN signal into a number,
 * string or boolean.
 *
 * @param[in] signal - The CAN signal that we are decoding.
 * @param[in] signals - The list of all signals.
 * @param[in] signalCount - The length of the signals array.
 * @param[in] value - The CAN signal parsed from the message as a raw floating point
 *	value.
 * @param[out] send - An output parameter. If the decoding failed or the CAN signal should
 *	not send for some other reason, this should be flipped to false.
 *
 * @return a decoded value in an openxc_DynamicField struct.
 */
typedef openxc_DynamicField (*SignalDecoder)(can_signal_t& signal,
		const std::vector<CanSignal>& signals, float value, bool* send);

/**
 * @brief: The type signature for a CAN signal encoder.
 *
 * @desc A SignalEncoder transforms a number, string or boolean into a raw floating
 * point value that fits in the CAN signal.
 *
 * @params[in] signal - The CAN signal to encode. 
 * @params[in] value - The dynamic field to encode.
 * @params send - An output parameter. If the encoding failed or the CAN signal should
 * not be encoded for some other reason, this will be flipped to false.
 */
typedef uint64_t (*SignalEncoder)(can_signal_t* signal,
		openxc_DynamicField* value, bool* send);


class can_signal_t
{
private:
	can_message_definition_t message_; /*!< message_ - The message this signal is a part of. */
	std::string generic_name_; /*!< generic_name_ - The name of the signal to be output over USB.*/
	uint8_t bitPosition_; /*!< bitPosition_ - The starting bit of the signal in its CAN message (assuming
										*	non-inverted bit numbering, i.e. the most significant bit of
										*	each byte is 0) */
	uint8_t bitSize_; /*!< bitSize_ - The width of the bit field in the CAN message. */
	float factor_; /*!< factor_ - The final value will be multiplied by this factor. Use 1 if you
							*	don't need a factor. */
	float offset_; /*!< offset_ - The final value will be added to this offset. Use 0 if you
							*	don't need an offset. */
	float min_value_; /*!< min_value_ - The minimum value for the processed signal.*/
	float max_value_; /*!< max_value_ - The maximum value for the processed signal. */
	FrequencyClock frequency_; /*!< frequency_ - A FrequencyClock struct to control the maximum frequency to
								*	process and send this signal. To process every value, set the
								*	clock's frequency to 0. */
	bool send_same_; /*!< send_same_ - If true, will re-send even if the value hasn't changed.*/
	bool force_send_changed_; /*!< force_send_changed_ - If true, regardless of the frequency, it will send the
								*	value if it has changed. */
	std::map<int, std::string> states_; /*!< states_ - A map of CAN signal state describing the mapping
																	* between numerical and string values for valid states. */
	uint8_t state_count_; /*!< state_count_ - The length of the states array. */
	bool writable_; /*!< writable - True if the signal is allowed to be written from the USB host
					*	back to CAN. Defaults to false.*/
	SignalDecoder decoder_; /*!< decoder_ - An optional function to decode a signal from the bus to a human
								* readable value. If NULL, the default numerical decoder is used. */
	SignalEncoder encoder_; /*!< encoder_ - An optional function to encode a signal value to be written to
							 * CAN into a byte array. If NULL, the default numerical encoder
							 * is used. */
	bool received_; /*!< received_ - True if this signal has ever been received.*/
	float lastValue_; /*!< lastValue_ - The last received value of the signal. If 'received' is false,
					   *	this value is undefined. */

public:
	can_message_definition_t& get_message() const;
	std::string& get_generic_name() const;
};

void find_can_signals(const openxc_DynamicField &key, std::vector<CanSignal*>& found_signals);
