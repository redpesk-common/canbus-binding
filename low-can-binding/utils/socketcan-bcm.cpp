/*
 * Copyright (C) 2015, 2016 , 2017, 2018, 2019 "IoT\.bzh"
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

#include <low-can/utils/socketcan-bcm.hpp>

#include <net/if.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <low-can/binding/application.hpp>


namespace utils
{

 	/// @brief Open a raw socket CAN.
	/// @param[in] device_name is the kernel network device name of the CAN interface.
	///
	/// @return Upon successful completion, shall return a non-negative integer, the socket file descriptor. Otherwise, a value of -1 shall be returned and errno set to indicate the error.
	int socketcan_bcm_t::open(std::string device_name)
	{
		struct ifreq ifr;
		if(socketcan_t::open(PF_CAN, SOCK_DGRAM, CAN_BCM) < 0)
		{
			AFB_ERROR("No socket opened %d", socket_);
			close();
			return socket_;
		}

		// Attempts to open a socket to CAN bus
		tx_address_.can_family = AF_CAN;
		::strcpy(ifr.ifr_name, device_name.c_str());
		AFB_DEBUG("BCM socket ifr_name is : %s", ifr.ifr_name);

		socket_mutex_.lock();
		int ret = ::ioctl(socket_, SIOCGIFINDEX, &ifr);
		socket_mutex_.unlock();

		if(ret < 0)
		{
			AFB_ERROR("ioctl failed. Error was : %s", strerror(errno));
			close();
			return -1;
		}

		tx_address_.can_ifindex = ifr.ifr_ifindex;
		if(connect((struct sockaddr *)&tx_address_, sizeof(tx_address_)) < 0)
		{
			AFB_ERROR("Connect failed. %s", strerror(errno));
			close();
			return -1;
		}
		// Needed because of using systemD event loop. See sd_event_add_io manual.
		fcntl(socketcan_t::socket_, F_SETFL, O_NONBLOCK);

		return socket_;
	}

	/// Read the socket to retrieve the associated CAN message. All the hard work is do into
	/// convert_from_frame method and if there isn't CAN message retrieve, only BCM head struct,
	/// then CAN message will be zeroed and must be handled later.
	std::shared_ptr<message_t> socketcan_bcm_t::read_message()
	{
		union bcm_msg msg;
		std::shared_ptr<can_message_t> cm = std::make_shared<can_message_t>();

		const struct sockaddr_can& addr = get_tx_address();
		socklen_t addrlen = sizeof(addr);
		struct ifreq ifr;

		struct timeval tv = {0,0};
		uint64_t timestamp = 0;

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

		if(ioctl(socket(), SIOCGSTAMP, &tv))
			AFB_ERROR("Error retrieving the timestamp of the message");
		else
			timestamp = 1000000L * tv.tv_sec + tv.tv_usec;

		if (msg.msg_head.opcode == RX_CHANGED)
		{
			cm = can_message_t::convert_from_frame(msg.fd_frames[0] , frame_size, timestamp);
		}
		else if (msg.msg_head.opcode == RX_TIMEOUT)
			cm = std::make_shared<can_message_t>(can_message_t(msg.msg_head.can_id, timestamp, true));
		cm->set_sub_id((int)socket());
		return cm;
	}

	int socketcan_bcm_t::write_message(message_t& m)
	{
		can_message_t&  cm = reinterpret_cast<can_message_t&>(m);
		union bcm_msg obj = cm.get_bcm_msg();
		size_t size = (obj.msg_head.flags & CAN_FD_FRAME) ?
			(size_t)((char*)&obj.fd_frames[obj.msg_head.nframes] - (char*)&obj):
			(size_t)((char*)&obj.frames[obj.msg_head.nframes] - (char*)&obj);
		if (::sendto(socket(), &obj, size, 0, (const struct sockaddr*)&get_tx_address(), sizeof(get_tx_address())) < 0)
		{
			AFB_ERROR("Error sending : %i %s", errno, ::strerror(errno));
			return -1;
		}
		return 0;
	}
}
