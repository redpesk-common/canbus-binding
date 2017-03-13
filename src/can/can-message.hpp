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

#include <vector>
#include <string>
#include <cstdint>
#include <linux/can.h>

#include "../utils/timer.hpp"
#include "can-message-set.hpp"

#define CAN_MESSAGE_SIZE 8

class can_bus_dev_t;

/**
 * @enum can_message_format_t
 * @brief The ID format for a CAN message.
 */
enum class can_message_format_t {
	STANDARD, /*!< STANDARD - standard 11-bit CAN arbitration ID. */
	EXTENDED, /*!< EXTENDED - an extended frame, with a 29-bit arbitration ID. */
	ERROR,    /*!< ERROR - ERROR code used at initialization to signify that it isn't usable'*/
};

/**
 * @class can_message_t
 *
 * @brief A compact representation of a single CAN message, meant to be used in in/out
 * buffers.
 */
class can_message_t {
private:
	uint8_t maxdlen_; /*!< maxdlen_ - Max data length deduce from number of bytes read from the socket.*/
	uint32_t id_; /*!< id_ - The ID of the message. */
	uint8_t length_; /*!< length_ - the length of the data array (max 8). */
	can_message_format_t format_; /*!< format_ - the format of the message's ID.*/
	bool rtr_flag_; /*!< rtr_flag_ - Telling if the frame has RTR flag positionned. Then frame hasn't data field*/
	uint8_t flags_; /*!< flags_ - flags of a CAN FD frame. Needed if we catch FD frames.*/
	std::vector<uint8_t> data_; /*!< data_ - The message's data field with a size of 8 which is the standard about CAN bus messages.*/

public:
	can_message_t();
	can_message_t(uint8_t maxdlen, uint32_t id, uint8_t length, can_message_format_t format, bool rtr_flag_, uint8_t flags, std::vector<uint8_t> data);

	uint32_t get_id() const;
	bool get_rtr_flag_() const;
	can_message_format_t get_format() const;
	uint8_t get_flags() const;
	const uint8_t* get_data() const;
	uint8_t get_length() const;

	void set_format(const can_message_format_t new_format);

	bool is_correct_to_send();

static can_message_t convert_from_canfd_frame(const struct canfd_frame& frame, size_t nbytes);
	canfd_frame convert_to_canfd_frame();
};
