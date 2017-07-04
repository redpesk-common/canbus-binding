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

#define INVALID_SOCKET -1

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
		int close();

	protected:
		int socket_;
		struct sockaddr_can tx_address_;

		int open(int domain, int type, int protocol);
	};

	template <typename T>
	socketcan_t& operator<<(socketcan_t& s, const std::vector<T>& vobj)
	{
		for(const auto& obj : vobj)
			s << obj;
		return s;
	}

	template <typename T>
	socketcan_t& operator<<(socketcan_t& s, const T& obj)
	{
		if (::sendto(s.socket(), &obj, sizeof(obj), 0, (const struct sockaddr*)&s.get_tx_address(), sizeof(s.get_tx_address())) < 0)
			AFB_ERROR("Error sending : %i %s", errno, ::strerror(errno));
		return s;
	}
}
