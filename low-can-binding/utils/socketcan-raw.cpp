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

#include "socketcan-raw.hpp"

#include <net/if.h>
#include <sys/ioctl.h>

namespace utils
{
	/// @brief Bind the socket.
	/// @return 0 if success.
	int socketcan_raw_t::bind(const struct sockaddr* addr, socklen_t len)
	{
		return socket_ != INVALID_SOCKET ? ::bind(socket_, addr, len) : 0;
	}

 	/// @brief Open a raw socket CAN.
	/// @param[in] device_name is the kernel network device name of the CAN interface.
	///
	/// @return Upon successful completion, shall return a non-negative integer, the socket file descriptor. Otherwise, a value of -1 shall be returned and errno set to indicate the error.
	int socketcan_raw_t::open(std::string device_name)
	{
		close();

		struct ifreq ifr;
		socket_ = socketcan_t::open(PF_CAN, SOCK_RAW, CAN_RAW);

		// Attempts to open a socket to CAN bus
		::strcpy(ifr.ifr_name, device_name.c_str());
		AFB_DEBUG("ifr_name is : %s", ifr.ifr_name);
		if(::ioctl(socket_, SIOCGIFINDEX, &ifr) < 0)
		{
			AFB_ERROR("ioctl failed. Error was : %s", strerror(errno));
			close();
		}
		else
		{
			tx_address_.can_family = AF_CAN;
			tx_address_.can_ifindex = ifr.ifr_ifindex;

			if(bind((struct sockaddr *)&tx_address_, sizeof(tx_address_)) < 0)
			{
				AFB_ERROR("Bind failed. %s", strerror(errno));
				close();
			}
		}
		return socket_;
	}

	socketcan_raw_t& operator>>(socketcan_raw_t& s, can_message_t& cm)
	{
		struct canfd_frame frame;

		const struct sockaddr_can& addr = s.get_tx_address();
		socklen_t addrlen = sizeof(addr);
		struct ifreq ifr;

		ssize_t nbytes = ::recvfrom(s.socket(), &frame, sizeof(frame), 0, (struct sockaddr*)&addr, &addrlen);
		ifr.ifr_ifindex = addr.can_ifindex;
		ioctl(s.socket(), SIOCGIFNAME, &ifr);

		AFB_DEBUG("Data available: %i bytes read", (int)nbytes);
		AFB_DEBUG("read: Found on bus %s:\n id: %X, length: %X, data %02X%02X%02X%02X%02X%02X%02X%02X", ifr.ifr_name, frame.can_id, frame.len,
			frame.data[0], frame.data[1], frame.data[2], frame.data[3], frame.data[4], frame.data[5], frame.data[6], frame.data[7]);

		struct timeval tv;
		ioctl(s.socket(), SIOCGSTAMP, &tv);
		uint64_t timestamp = 1000000 * tv.tv_sec + tv.tv_usec;
		cm = ::can_message_t::convert_from_frame(frame , nbytes, timestamp);

		return s;
	}
}
