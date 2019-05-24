/*
 * Copyright (C) 2015, 2016 ,2017 "IoT.bzh"
 * Author "Romain Forlot" <romain.forlot@iot.bzh>
 * Author "Loïc Collignon" <loic.collignon@iot.bzh>
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

#include "socketcan.hpp"
#include "../can/can-message.hpp"

#define MAX_BCM_CAN_FRAMES 257

namespace utils
{
	struct bcm_msg
	{
		struct bcm_msg_head msg_head;
		union {
			struct canfd_frame fd_frames[MAX_BCM_CAN_FRAMES];
			struct can_frame frames[MAX_BCM_CAN_FRAMES];
		};
	};

	/// @brief derivated socketcan class specialized for BCM CAN socket.make_bcm_head
	class socketcan_bcm_t : public socketcan_t
	{
	public:
		using socketcan_t::socketcan_t;

		virtual int open(std::string device_name);
		virtual std::shared_ptr<can_message_t> read_message();
		virtual void write_message(std::vector<std::shared_ptr<can_message_t>>& vobj);
		virtual void write_message(std::shared_ptr<can_message_t> obj);
		void write_message(struct bcm_msg& obj);

	private:
		int connect(const struct sockaddr* addr, socklen_t len);
	};


}
