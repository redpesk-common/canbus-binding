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
	std::shared_ptr<can_message_t> socketcan_bcm_t::read_message()
	{
		struct bcm_msg msg;
		std::shared_ptr<can_message_t> cm = std::make_shared<can_message_t>();

		const struct sockaddr_can& addr = get_tx_address();
		socklen_t addrlen = sizeof(addr);
		struct ifreq ifr;

		ssize_t nbytes = ::recvfrom(socket(), &msg, sizeof(msg), 0, (struct sockaddr*)&addr, &addrlen);
		if(nbytes < 0)
		{
			AFB_ERROR("Can't read the next message from socket '%d'. '%s'", socket(), strerror(errno));
			return cm;
		}
		ifr.ifr_ifindex = addr.can_ifindex;
		if(ioctl(socket(), SIOCGIFNAME, &ifr) < 0)
		{
			AFB_ERROR("Can't read the interface name. '%s'", strerror(errno));
			return cm;
		}
		long unsigned int frame_size = nbytes-sizeof(struct bcm_msg_head);

		AFB_DEBUG("Data available: %li bytes read. BCM head, opcode: %i, can_id: %i, nframes: %i", frame_size, msg.msg_head.opcode, msg.msg_head.can_id, msg.msg_head.nframes);

		struct timeval tv;
		ioctl(socket(), SIOCGSTAMP, &tv);
		uint64_t timestamp = 1000000 * tv.tv_sec + tv.tv_usec;
		*cm = can_message_t::convert_from_frame(msg.fd_frames[0] , frame_size, timestamp);
		cm->set_sub_id((int)socket());

		return cm;
	}

	void socketcan_bcm_t::write_message(std::vector<std::shared_ptr<can_message_t>>& vobj)
	{
		for(const auto& obj : vobj)
			write_message(obj);
	}

	void socketcan_bcm_t::write_message(std::shared_ptr<can_message_t> m)
	{
		struct can_frame obj;
		obj.can_id  = m->get_id();
		obj.can_dlc = m->get_length();
		::memcpy(obj.data, m->get_data(), CAN_MAX_DLEN);
		if (::sendto(socket(), &obj, sizeof(&obj), 0, (const struct sockaddr*)&get_tx_address(), sizeof(get_tx_address())) < 0)
			AFB_API_ERROR(afbBindingV3root, "Error sending : %i %s", errno, ::strerror(errno));
	}

	void socketcan_bcm_t::write_message(struct bcm_msg& obj)
	{
		size_t size = (obj.msg_head.flags & CAN_FD_FRAME) ?
		(size_t)((char*)&obj.fd_frames[obj.msg_head.nframes] - (char*)&obj):
		(size_t)((char*)&obj.frames[obj.msg_head.nframes] - (char*)&obj);
		if (::sendto(socket(), &obj, size, 0, (const struct sockaddr*)&get_tx_address(), sizeof(get_tx_address())) < 0)
			AFB_API_ERROR(afbBindingV3root, "Error sending : %i %s", errno, ::strerror(errno));
	}
}
