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

#include <unistd.h>
#include <string>
#include <linux/can/raw.h>
#include <net/if.h>
#include <sys/ioctl.h>

#include "socketcan.hpp"

namespace utils
{

	/// @brief Construct a default, invalid, socket.
	socketcan_t::socketcan_t()
		: socket_{INVALID_SOCKET}
	{
		::memset(&tx_address_, 0, sizeof(tx_address_));
	}

	/// @brief Construct a socket by moving an existing one.
	socketcan_t::socketcan_t(socketcan_t&& s)
		: socket_{s.socket_}
	{
		::memset(&tx_address_, 0, sizeof(tx_address_));
	}

	socketcan_t& socketcan_t::operator=(const socketcan_t& s)
	{
		socket_ = std::move(s.socket_);
		return *this;
	}

	socketcan_t::~socketcan_t()
	{
		close();
		socket_ = INVALID_SOCKET;
	}

	const struct sockaddr_can& socketcan_t::get_tx_address() const
	{
		return tx_address_;
	}

	/// @brief Test if socket is valid.
	/// @return true if valid, false otherwise.
	socketcan_t::operator bool() const
	{
		return socket_ != INVALID_SOCKET;
	}

	/// @brief Open the socket.
	/// @param[in] domain Specifies the communications domain in which a socket is to be created.
	/// @param[in] type Specifies the type of socket to be created.
	/// @param[in] protocol Specifies a particular protocol to be used with the socket. Specifying a protocol of 0 causes socket() to use an unspecified default protocol appropriate for the requested socket type.
	/// @return Upon successful completion, shall return a non-negative integer, the socket file descriptor. Otherwise, a value of -1 shall be returned and errno set to indicate the error.
	int socketcan_t::open(int domain, int type, int protocol)
	{
		close();
		socket_ = ::socket(domain, type, protocol);
		return socket_;
	}

	/// @brief Close the socket.
	/// @return 0 if success.
	int socketcan_t::close()
	{
		return socket_ != INVALID_SOCKET ? ::close(socket_) : 0;
	}

	/// @brief Set socket option.
	/// @return 0 if success.
	int socketcan_t::setopt(int level, int optname, const void* optval, socklen_t optlen)
	{
		return socket_ != INVALID_SOCKET ? ::setsockopt(socket_, level, optname, optval, optlen) : 0;
	}

	/// @brief Get the file descriptor.
	/// @return The socket's file descriptor
	int socketcan_t::socket() const
	{
		return socket_;
	}
}
