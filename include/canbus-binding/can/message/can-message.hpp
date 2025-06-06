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

#include <linux/can/bcm.h>

#include <canbus-binding/can/message/message.hpp>

union bcm_msg
{
	struct bcm_msg_head msg_head;
	struct {
		/// Avoids the error:
		///   "flexible array member ‘bcm_msg_head::frames’ not at end of ‘struct bcm_msg’"
		char _head_[sizeof(struct bcm_msg_head)];
		union {
			struct can_frame frames[MAX_BCM_CAN_FRAMES];
			struct canfd_frame fd_frames[MAX_BCM_CAN_FRAMES];
		};
	};
};

/// @class can_message_t
///
/// @brief A compact representation of a single CAN message, meant to be used in in/out
/// buffers. It is a wrapper of a can_frame struct with some sugar around it for binding purposes.
class can_message_t : public message_t {
	private:
		uint32_t id_; ///< id_ - The ID of the message. */
		bool rtr_flag_; ///< rtr_flag_ - Telling if the frame has RTR flag positionned. Then frame hasn't data field*/
		union bcm_msg bcm_msg_;

	public:
		virtual ~can_message_t() = default;
		can_message_t();
		can_message_t(  uint32_t maxdlen,
				uint32_t id,
				uint32_t length,
				bool rtr_flag_,
				uint32_t flags,
				std::vector<uint8_t>& data,
				uint64_t timestamp);
		can_message_t(  uint32_t id,
				uint64_t timestamp,
				bool is_timeout = true);

		uint32_t get_id() const;
		void set_id(const canid_t id);
		bool is_correct_to_send();

		static std::unique_ptr<can_message_t> convert_from_frame(const canfd_frame& frame, size_t nbytes, uint64_t timestamp);
		struct canfd_frame convert_to_canfd_frame();
		struct std::vector<canfd_frame> convert_to_canfd_frame_vector();
		union bcm_msg& get_bcm_msg();
		void set_bcm_msg(const union bcm_msg &bcm_msg);

		struct can_frame convert_to_can_frame();
		bool is_set();
		std::string get_debug_message();
};
