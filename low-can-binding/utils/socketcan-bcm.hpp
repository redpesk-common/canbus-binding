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

namespace utils
{
	struct simple_bcm_msg
	{
		struct bcm_msg_head msg_head;
		struct can_frame frames;
	};

#ifdef KERNEL_MINIMAL_VERSION_OK
	struct canfd_bcm_msg
	{
		struct bcm_msg_head msg_head;
		struct canfd_frame frames;
		canfd_bcm_msg() { msg_head.flags |= CAN_FD_FRAME; }
	};
#endif

	/// @brief derivated socketcan class specialized for BCM CAN socket.
	class socketcan_bcm_t : public socketcan_t
	{
	public:
		using socketcan_t::socketcan_t;

		virtual int open(std::string device_name);

	private:
		int connect(const struct sockaddr* addr, socklen_t len);
	};

	socketcan_bcm_t& operator>>(socketcan_bcm_t& s, can_message_t& cm);
//	socketcan_bcm_t& operator<<(socketcan_bcm_t& s, const struct simple_bcm_msg& obj);
//	socketcan_bcm_t& operator<<(socketcan_bcm_t& s, const struct canfd_bcm_msg& obj);
}
