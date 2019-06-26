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
#include "./message.hpp"



/// @class can_message_t
///
/// @brief A compact representation of a single CAN message, meant to be used in in/out
/// buffers. It is a wrapper of a can_frame struct with some sugar around it for binding purposes.
class can_message_t : public message_t {
	private:
		uint8_t maxdlen_; ///< maxdlen_ - Max data length deduce from number of bytes read from the socket.*/
		uint32_t id_; ///< id_ - The ID of the message. */
		bool rtr_flag_; ///< rtr_flag_ - Telling if the frame has RTR flag positionned. Then frame hasn't data field*/
		uint8_t flags_; ///< flags_ - flags of a CAN FD frame. Needed if we catch FD frames.*/
		struct bcm_msg bcm_msg_;

	public:
		can_message_t();
		can_message_t(uint8_t maxdlen, uint32_t id, uint8_t length, message_format_t format, bool rtr_flag_, uint8_t flags, std::vector<uint8_t>& data, uint64_t timestamp);

		uint32_t get_id() const;

		static std::shared_ptr<can_message_t> convert_from_frame(const canfd_frame& frame, size_t nbytes, uint64_t timestamp);
		bool is_correct_to_send();
		bool is_set();
		struct bcm_msg get_bcm_msg();
		void set_bcm_msg(struct bcm_msg bcm_msg);

		std::string get_debug_message();

};
