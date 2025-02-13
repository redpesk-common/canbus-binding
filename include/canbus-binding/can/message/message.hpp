/*
 * Copyright (C) 2015-2025 IoT.bzh Company
 * Author: "Romain Forlot" <romain.forlot@iot.bzh>
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

#include <linux/can.h>

#include <vector>
#include <string>
#include <cstdint>

#define CAN_MESSAGE_SIZE 8

#define MAX_BCM_CAN_FRAMES 257
#define MAX_ISOTP_FRAMES 4096

#define J1939_CAN_ID CAN_EFF_FLAG
#define J1939_CAN_MASK (CAN_EFF_FLAG | CAN_RTR_FLAG)

#define CAN_ISOTP_MAX_DLEN 4096

#define NO_CAN_ID 0xFFFFFFFFU

/**
 * FLAGS defining how to open socket and how you could decode the frame
 */
#define CAN_PROTOCOL		   0x0001
#define CAN_PROTOCOL_WITH_FD_FRAME 0x0002
#define J1939_ADDR_CLAIM_PROTOCOL  0x0004
#define J1939_PROTOCOL		   0x0008
#define ISOTP_PROTOCOL		   0x0010
#define ISOTP_SEND		   0x0020
#define ISOTP_RECEIVE		   0x0040
#define BYTE_FRAME_IS_BIG_ENDIAN   0x0080
#define BIT_POSITION_REVERSED	   0x0100
#define CONTINENTAL_BIT_POSITION   0x0200
#define INVALID_FLAG		   0x8000

/// @class message_t
///
/// @brief A compact representation of a single CAN message, meant to be used in in/out
/// buffers. It is a wrapper of a can_frame struct with some sugar around it for binding purposes.
class message_t {
protected:
	uint32_t maxdlen_; ///< maxdlen_ - Max data length deduce from number of bytes read from the socket.*/
	uint32_t length_; ///< length_ - the length of the data array. */
	uint32_t flags_; ///< format_ - the format mask of the message that control which socketcan will used*/
	std::vector<uint8_t> data_; ///< data_ - The message's data field with a size of 8 which is the standard about CAN bus messages.*/
	uint64_t timestamp_; ///< timestamp_ - timestamp of the received message*/
	int sub_id_; ///< sub_id_ - Subscription index. */
	bool is_timeout_; //< is_timeout_ - The message is a timeout message without data*/

public:
	message_t();
	message_t(uint32_t maxdlen, uint32_t length, uint32_t flags, std::vector<uint8_t>& data, uint64_t timestamp);
	message_t(uint64_t timestamp, bool is_timeout);
	virtual ~message_t() = default;

	int get_sub_id() const;
	const uint8_t* get_data() const;
	const std::vector<uint8_t> get_data_vector() const;
	uint32_t get_length() const;
	bool is_timeout() const;
	uint64_t get_timestamp() const;

	void set_data(std::vector<uint8_t> data);
	void set_sub_id(int sub_id);
	void set_timestamp(uint64_t timestamp);
	virtual bool is_set() = 0;
	virtual std::string get_debug_message() = 0;
	virtual uint32_t get_id() const = 0;
	virtual void set_id(canid_t id) = 0;
	uint32_t get_flags();
	void set_flags(uint32_t flags);
	void erase_flags();
	uint32_t get_maxdlen();
	void set_maxdlen(uint32_t maxdlen);
	void set_length(uint32_t length);
	void frame_swap();
};
