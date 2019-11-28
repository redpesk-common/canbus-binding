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

#include <vector>

#include <sys/socket.h>
#include <linux/can/bcm.h>
#include <string.h>

#include "../binding/low-can-hat.hpp"
#include "../can/message/can-message.hpp"

#define INVALID_SOCKET -1
#define NO_CAN_ID 0xFFFFFFFFU

namespace utils
{

	class socketcan_t
	{
	public:
		socketcan_t();
		socketcan_t(const socketcan_t& s) = delete;
		socketcan_t(socketcan_t&&);
		socketcan_t& operator=(const socketcan_t& s);
		virtual ~socketcan_t();

		const struct sockaddr_can& get_tx_address() const;
		explicit operator bool() const;

		int socket() const;
		virtual int open(std::string device_name) = 0;
		int setopt(int level, int optname, const void* optval, socklen_t optlen);
		virtual int close();
		virtual std::shared_ptr<message_t> read_message() = 0;
		virtual int write_message(message_t& obj) = 0;
		virtual int write_message(std::vector<message_t>& vobj);

	protected:
		int socket_;
		struct sockaddr_can tx_address_;
		int open(int domain, int type, int protocol);
		int bind(const struct sockaddr* addr, socklen_t len);
		int connect(const struct sockaddr* addr, socklen_t len);
	};
}
