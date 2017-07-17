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

#include "socketcan-bcm.hpp"

#include <net/if.h>
#include <sys/ioctl.h>
#include <fcntl.h>

#include "../binding/application.hpp"

namespace utils
{
	/// @brief Connect the socket.
	/// @return 0 if success.
	int socketcan_bcm_t::connect(const struct sockaddr* addr, socklen_t len)
	{
		return socket_ != INVALID_SOCKET ? ::connect(socket_, addr, len) : 0;
	}

 	/// @brief Open a raw socket CAN.
	/// @param[in] device_name is the kernel network device name of the CAN interface.
	///
	/// @return Upon successful completion, shall return a non-negative integer, the socket file descriptor. Otherwise, a value of -1 shall be returned and errno set to indicate the error.
	int socketcan_bcm_t::open(std::string device_name)
	{
		close();

		struct ifreq ifr;
		socket_ = socketcan_t::open(PF_CAN, SOCK_DGRAM, CAN_BCM);

		// Attempts to open a socket to CAN bus
		::strcpy(ifr.ifr_name, device_name.c_str());
		AFB_DEBUG("BCM socket ifr_name is : %s", ifr.ifr_name);
		if(::ioctl(socket_, SIOCGIFINDEX, &ifr) < 0)
		{
			AFB_ERROR("ioctl failed. Error was : %s", strerror(errno));
			close();
		}
		else
		{
			tx_address_.can_family = AF_CAN;
			tx_address_.can_ifindex = ifr.ifr_ifindex;

			if(connect((struct sockaddr *)&tx_address_, sizeof(tx_address_)) < 0)
			{
				AFB_ERROR("Connect failed. %s", strerror(errno));
				close();
			}
			// Needed because of using systemD event loop. See sd_event_add_io manual.
			fcntl(socketcan_t::socket_, F_SETFL, O_NONBLOCK);
		}
		return socket_;
	}

	/// Read the socket to retrieve the associated CAN message. All the hard work is do into
	/// convert_from_frame method and if there isn't CAN message retrieve, only BCM head struct,
	/// then CAN message will be zeroed and must be handled later.
	socketcan_bcm_t& operator>>(socketcan_bcm_t& s, can_message_t& cm)
	{
		struct utils::simple_bcm_msg msg;

		::memset(&msg, 0, sizeof(msg));
		const struct sockaddr_can& addr = s.get_tx_address();
		socklen_t addrlen = sizeof(addr);
		struct ifreq ifr;

		ssize_t nbytes = ::recvfrom(s.socket(), &msg, sizeof(msg), 0, (struct sockaddr*)&addr, &addrlen);
		ifr.ifr_ifindex = addr.can_ifindex;
		ioctl(s.socket(), SIOCGIFNAME, &ifr);
		long unsigned int frame_size = nbytes-sizeof(struct bcm_msg_head);

		AFB_DEBUG("Data available: %li bytes read. BCM head, opcode: %i, can_id: %i, nframes: %i", frame_size, msg.msg_head.opcode, msg.msg_head.can_id, msg.msg_head.nframes);
		AFB_DEBUG("read: Found on bus %s:\n id: %X, length: %X, data %02X%02X%02X%02X%02X%02X%02X%02X", ifr.ifr_name, msg.msg_head.can_id, msg.frames.can_dlc,
			msg.frames.data[0], msg.frames.data[1], msg.frames.data[2], msg.frames.data[3], msg.frames.data[4], msg.frames.data[5], msg.frames.data[6], msg.frames.data[7]);

		struct timeval tv;
		ioctl(s.socket(), SIOCGSTAMP, &tv);
		uint64_t timestamp = 1000000 * tv.tv_sec + tv.tv_usec;
		cm = ::can_message_t::convert_from_frame(msg.frames ,
				frame_size,
				timestamp);
		cm.set_sub_id((int)s.socket());

		return s;
	}
}
