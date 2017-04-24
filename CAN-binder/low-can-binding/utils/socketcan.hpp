#pragma once

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

#include <sys/socket.h>
#include <linux/can/bcm.h>

#include <vector>

#define INVALID_SOCKET -1

namespace utils
{

	template <typename T>
	struct basic_bcm_msg
	{
		bcm_msg_head msg_head;
		std::vector<T> frames;
	};

	struct canfd_bcm_msg : public basic_bcm_msg<canfd_frame>
	{
	canfd_bcm_msg() { msg_head.flags |= CAN_FD_FRAME; }
	};

	class socketcan_t
	{
	public:
		socketcan_t();
		socketcan_t(const socketcan_t&) = delete;
		socketcan_t(socketcan_t&&);
		~socketcan_t();

		const struct sockaddr_can& get_tx_address() const;
		explicit operator bool() const;

		int socket() const;
		int open(std::string device_name);
		int setopt(int level, int optname, const void* optval, socklen_t optlen);
		int close();

	private:
		int socket_;
		struct sockaddr_can tx_address_;

		int open(int domain, int type, int protocol);
		int bind(const struct sockaddr* addr, socklen_t len);
		int connect(const struct sockaddr* addr, socklen_t len);
	};

	template <typename T>
	socketcan_t& operator<<(socketcan_t& s, const std::vector<T>& vobj)
	{
		for(const auto& obj : vobj)
			s << obj;
		return s;
	}

	socketcan_t& operator<<(socketcan_t& s, const canfd_frame& frame);
	socketcan_t& operator<<(socketcan_t& s, const can_frame& frame);
	socketcan_t& operator<<(socketcan_t& s, const struct basic_bcm_msg<can_frame>& obj);
	socketcan_t& operator<<(socketcan_t& s, const struct canfd_bcm_msg& obj);
	socketcan_t& operator<<(socketcan_t& s, const struct bcm_msg_head& obj);
}
