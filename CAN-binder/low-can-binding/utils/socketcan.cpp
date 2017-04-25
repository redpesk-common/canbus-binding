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
#include <string.h>
#include <linux/can/raw.h>
#include <net/if.h>
#include <sys/ioctl.h>

#include "socketcan.hpp"
#include "low-can-binding.hpp"
#include "can-message.hpp"

namespace utils
{

	/// @brief Construct a default, invalid, socket.
	socketcan_t::socketcan_t()
		: socket_{INVALID_SOCKET}
	{}

	/// @brief Construct a socket by moving an existing one.
	socketcan_t::socketcan_t(socketcan_t&& s)
		: socket_{s.socket_}
	{
		s.socket_ = INVALID_SOCKET;
	}

	/// @brief Destruct the socket.
	socketcan_t::~socketcan_t()
	{
		if(socket_ != INVALID_SOCKET)
			::close(socket_);
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

	/// @brief Bind the socket.
	/// @return 0 if success.
	int socketcan_t::bind(const struct sockaddr* addr, socklen_t len)
	{
		return socket_ != INVALID_SOCKET ? ::bind(socket_, addr, len) : 0;
	}

	/// @brief Connect the socket.
	/// @return 0 if success.
	int socketcan_t::connect(const struct sockaddr* addr, socklen_t len)
	{
		return socket_ != INVALID_SOCKET ? ::connect(socket_, addr, len) : 0;
	}

	/// @brief Get the file descriptor.
	/// @return The socket's file descriptor
	int socketcan_t::socket() const
	{
		return socket_;
	}

	/// @brief Open a raw socket CAN.
	/// @param[in] device_name is the kernel network device name of the CAN interface.
	///
	/// @return Upon successful completion, shall return a non-negative integer, the socket file descriptor. Otherwise, a value of -1 shall be returned and errno set to indicate the error.
	int socketcan_t::open(std::string device_name)
	{
		close();
		
		struct ifreq ifr;
		socket_ = ::socket(PF_CAN, SOCK_DGRAM, CAN_BCM);

		// Attempts to open a socket to CAN bus
		::strcpy(ifr.ifr_name, device_name.c_str());
		DEBUG(binder_interface, "%s: ifr_name is : %s", __FUNCTION__, ifr.ifr_name);
		if(::ioctl(socket_, SIOCGIFINDEX, &ifr) < 0)
		{
			ERROR(binder_interface, "%s: ioctl failed. Error was : %s", __FUNCTION__, strerror(errno));
			close();
		}
		else
		{
			tx_address_.can_family = AF_CAN;
			tx_address_.can_ifindex = ifr.ifr_ifindex;

			if(connect((struct sockaddr *)&tx_address_, sizeof(tx_address_)) < 0)
			{
				ERROR(binder_interface, "%s: Connect failed. %s", __FUNCTION__, strerror(errno));
				close();
			}
		}
		return socket_;
	}

	socketcan_t& operator>>(socketcan_t& s, const can_message_t& cm)
	{
		struct {
			struct bcm_msg_head msg_head;
			struct canfd_frame frames;
		} msg;

		struct sockaddr_can addr = s.get_tx_address();
		socklen_t addrlen = sizeof(addr);
		struct ifreq ifr;

		ssize_t nbytes = ::recvfrom(s.socket(), &msg, sizeof(msg), 0, (struct sockaddr*)&addr, &addrlen);
		ifr.ifr_ifindex = addr.can_ifindex;
		ioctl(s.socket(), SIOCGIFNAME, &ifr);

		printf("Data available: %i bytes read\n", (int)nbytes);
		printf("read: Found on bus %s:\n id: %X, length: %X, data %02X%02X%02X%02X%02X%02X%02X%02X\n", ifr.ifr_name, msg.msg_head.can_id, msg.frames.len,
			msg.frames.data[0], msg.frames.data[1], msg.frames.data[2], msg.frames.data[3], msg.frames.data[4], msg.frames.data[5], msg.frames.data[6], msg.frames.data[7]);
		return s;
	}

	socketcan_t& operator<<(socketcan_t& s, const struct bcm_msg_head& obj)
	{
		struct sockaddr_can addr = s.get_tx_address();
		::sendto(s.socket(), &obj, sizeof(bcm_msg_head), 0, (struct sockaddr*)&addr, sizeof(addr));
		return s;
	}

	socketcan_t& operator<<(socketcan_t& s, const struct canfd_frame& obj)
	{
		struct sockaddr_can addr = s.get_tx_address();
		::sendto(s.socket(), &obj, sizeof(canfd_frame), 0, (struct sockaddr*)&addr, sizeof(addr));
		return s;
	}

	socketcan_t& operator<<(socketcan_t& s, const struct can_frame& obj)
	{
		struct sockaddr_can addr = s.get_tx_address();
		::sendto(s.socket(), &obj, sizeof(can_frame), 0, (struct sockaddr*)&addr, sizeof(addr));
		return s;
	}

	socketcan_t& operator<<(socketcan_t& s, const struct basic_bcm_msg<struct can_frame>& obj)
	{
		s << obj.msg_head;
		s << obj.frames;
		return s;
	}

	socketcan_t& operator<<(socketcan_t& s, const struct canfd_bcm_msg& obj)
	{
		s << obj.msg_head;
		s << obj.frames;
		return s;
	}
}