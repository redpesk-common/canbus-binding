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
#include <iostream>
#include <memory>
#include <linux/can.h>
#include <linux/can/bcm.h>
#include "../../utils/timer.hpp"

#define CAN_MESSAGE_SIZE 8

#define MAX_BCM_CAN_FRAMES 257

struct bcm_msg
{
	struct bcm_msg_head msg_head;
	union {
		struct canfd_frame fd_frames[MAX_BCM_CAN_FRAMES];
		struct can_frame frames[MAX_BCM_CAN_FRAMES];
	};
};

/**
 * @enum message_format_t
 * @brief The ID format for a CAN message.
 */
enum class message_format_t {
	STANDARD, ///< STANDARD - standard 11-bit CAN arbitration ID. */
	EXTENDED, ///< EXTENDED - an extended frame, with a 29-bit arbitration ID. */
	J1939,	  ///< J1939 	- Format for j1939 messages
	INVALID,  ///< INVALID  - INVALID code used at initialization to signify that it isn't usable'*/
};


/// @class message_t
///
/// @brief A compact representation of a single CAN message, meant to be used in in/out
/// buffers. It is a wrapper of a can_frame struct with some sugar around it for binding purposes.
class message_t {
protected:
	uint32_t maxdlen_; ///< maxdlen_ - Max data length deduce from number of bytes read from the socket.*/
	uint32_t length_; ///< length_ - the length of the data array. */
	message_format_t format_; ///< format_ - the format of the message's ID.*/
	std::vector<uint8_t> data_; ///< data_ - The message's data field with a size of 8 which is the standard about CAN bus messages.*/
	uint64_t timestamp_; ///< timestamp_ - timestamp of the received message*/
	int sub_id_; ///< sub_id_ - Subscription index. */



public:
	message_t();
	message_t(uint32_t maxdlen, uint32_t length, message_format_t format, std::vector<uint8_t>& data, uint64_t timestamp);
	virtual ~message_t() = default;

	int get_sub_id() const;
	const uint8_t* get_data() const;
	const std::vector<uint8_t> get_data_vector() const;
	uint32_t get_length() const;
	uint64_t get_timestamp() const;

	void set_data(std::vector<uint8_t> &data);
	void set_sub_id(int sub_id);
	void set_timestamp(uint64_t timestamp);
	message_format_t get_msg_format();
	virtual bool is_set() = 0;
	virtual std::string get_debug_message() = 0;
	virtual uint32_t get_id() const = 0;

};
