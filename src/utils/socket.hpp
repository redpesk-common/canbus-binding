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

#define INVALID_SOCKET -1

namespace utils
{
	class socket_t
	{
	public:
		socket_t();
		socket_t(const socket_t&) = delete;
		socket_t(socket_t&&);
		~socket_t();
		
		explicit operator bool() const;
		
		int open(int domain, int type, int protocol);
		int close();
		int setopt(int level, int optname, const void* optval, socklen_t optlen);
		int socket() const;
		int bind(const struct sockaddr* addr, socklen_t len);
		
	private:
		int socket_;
	};
}

