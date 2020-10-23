/*
 * Copyright (C) 2018, 2019 "IoT.bzh"
 * Author "Arthur Guyader" <arthur.guyader@iot.bzh>
 *
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
#include <sys/ioctl.h>
#include <fcntl.h>
#include <iostream>
#include <algorithm>
#include <vector>
#include <low-can/utils/socketcan-j1939/socketcan-j1939.hpp>
#include <low-can/utils/socketcan-j1939/socketcan-j1939-addressclaiming.hpp>

namespace utils
{
	std::mutex socketcan_j1939_t::mutex_claiming_;
	std::condition_variable socketcan_j1939_t::signal_address_claiming_;

	/**
	 * @brief Apply a filter to the socket
	 *
	 * @param name - The name you want to receive
	 * @param pgn - The pgn you want to receive
	 * @param addr - The addr you want to receive
	 * @param name_mask - The mask to apply to the name (No mask : J1939_NO_NAME)
	 * @param pgn_mask - The mask to apply to the pgn (No mask : J1939_NO_PGN)
	 * @param addr_mask - The mask to apply to the addr (No mask : J1939_NO_ADDR)
	 */
	int socketcan_j1939_t::add_filter(name_t name, pgn_t pgn, uint8_t addr, name_t name_mask, pgn_t pgn_mask, uint8_t addr_mask)
	{
	//	AFB_DEBUG("[socketcan_j1939_t][add_filter] PGN : %" PRIu32 " ; NAME : %" PRIu64 " ; ADDR : %" PRIu8, pgn,(long unsigned int)name, addr);
	//	AFB_DEBUG("PGN_MASK : %" PRIu32 " ; NAME_MASK : %" PRIu64 "; ADDR_MASK : %" PRIu8, pgn_mask,(long unsigned int)name_mask, addr_mask);
		int filter_on = 0;
		struct j1939_filter filter;
		memset(&filter, 0, sizeof(filter));
		if (name)
		{
			filter.name = name;
			if(name_mask != J1939_NO_NAME)
				filter.name_mask = name_mask;
			else
				filter.name_mask = ~0ULL;
			++filter_on;
		}

		if (addr < 0xff)
		{
			filter.addr = addr;
			if(addr_mask != J1939_NO_ADDR)
				filter.addr_mask = addr_mask;
			else
				filter.addr_mask = ~0;

			++filter_on;
		}
		if (pgn <= J1939_PGN_MAX)
		{
			filter.pgn = pgn;
			if(pgn_mask != J1939_NO_PGN)
				filter.pgn_mask = pgn_mask;
			else
				filter.pgn_mask = ~0;

			++filter_on;
		}

		if(filter_on)
			return setopt(SOL_CAN_J1939, SO_J1939_FILTER, &filter, sizeof(filter));

		return 0;
	}

	/**
	 * @brief Define some parameters on the socket
	 *
	 * @param broadcast - Allows to write message with address brodcast (255)
	 * @param promisc - Allows to accept all packets that the socket receives even if they are not addressed directly to it
	 */
	void socketcan_j1939_t::define_opt(bool broadcast, bool promisc)
	{
		int broadcast_i = broadcast ? 1 : 0;
		int promisc_i = promisc ? 1 : 0;

		setopt(SOL_SOCKET, SO_BROADCAST, &broadcast_i, sizeof(broadcast_i));
		setopt(SOL_CAN_J1939, SO_J1939_PROMISC, &promisc_i, sizeof(promisc_i));
	}


	/**
	 * @brief Get the default ECU name in hexadecimal normally set in the
	 * main binding json configuration file, default to 0x1234
	 *
	 * @return int
	 */
	name_t socketcan_j1939_t::get_j1939_name() const
	{
		return j1939_name_;
	}

	/**
	 * @brief Set the default ECU name in hexadecimal normally set in the
	 * main binding json configuration file, default to 0x1234
	 *
	 * @param ecu_name string with a hexadecimal prefix '0x' representing
	 * the j1939 ecu name.
	 */
	void socketcan_j1939_t::set_j1939_name(name_t ecu_name)
	{
		j1939_name_ = ecu_name;
	}

	/**
	 * @brief Define the tx address for the bind function
	 *
	 * @param device_name - The device can that you want to bind
	 * @param name - The name that you want to bind
	 * @param pgn - The pgn that you want to bind
	 * @param addr - The addr that you want to bindthat you want to bind
	 */
	void socketcan_j1939_t::define_tx_address(std::string device_name, name_t name, pgn_t pgn, uint8_t addr)
	{

		::strcpy(ifr_.ifr_name, device_name.c_str());
		AFB_DEBUG("ifr_name is : %s", ifr_.ifr_name);


		if(::ioctl(socket_, SIOCGIFINDEX, &ifr_) < 0)
		{
			AFB_ERROR("ioctl failed. Error was : %s", strerror(errno));
			close();
		}
		else
		{
			tx_address_.can_ifindex = ifr_.ifr_ifindex;
		}

		tx_address_.can_family = AF_CAN;


		if(addr <= 0 || addr >= UINT8_MAX )
			tx_address_.can_addr.j1939.addr = J1939_NO_ADDR;
		else
			tx_address_.can_addr.j1939.addr = addr;
		if(name <= 0 || name >= UINT64_MAX )
			tx_address_.can_addr.j1939.name = J1939_NO_NAME;
		else
			tx_address_.can_addr.j1939.name = name;

		if(pgn <= 0 || pgn > J1939_PGN_MAX)
			tx_address_.can_addr.j1939.pgn = J1939_NO_PGN;
		else
			tx_address_.can_addr.j1939.pgn = pgn;
	}


	/**
	 * @brief Open default socket
	 *
	 * @param device_name The device name to open the socket
	 * @return int The number of the socket
	 */
	int socketcan_j1939_t::open(std::string device_name)
	{
		return open(device_name, 0, 0, 0);
	}

	/**
	 * @brief Open a socket with name pgn and address
	 *
	 * @param device_name The device name to open the socket
	 * @param name - The name that you want to bind
	 * @param pgn - The pgn that you want to bind
	 * @param addr - The addr that you want to bindthat you want to bind
	 * @return int The number of the socket
	 */
	int socketcan_j1939_t::open(std::string device_name, name_t name, pgn_t pgn, uint8_t addr)
	{
		close();

		socket_ = socketcan_t::open(PF_CAN, SOCK_DGRAM, CAN_J1939);
		if (socket_ < 0)
			return socket_;

		define_tx_address(device_name, name, pgn, addr);

		if(bind((struct sockaddr *)&tx_address_, sizeof(tx_address_)) < 0)
		{
			AFB_ERROR("Bind failed. %s", strerror(errno));
			close();
		}
		int enable = 1;
		setopt(SOL_SOCKET, SO_TIMESTAMP, &enable, sizeof(enable));

		return socket_;
	}

	/**
	 * @brief Launch recvfrom on the socket with blocking flag
	 *
	 * @return std::shared_ptr<message_t> The j1939 message that is read
	 */
	std::shared_ptr<message_t> socketcan_j1939_t::read_message()
	{
		return read_message(0);
	}

	/**
	 * @brief Launch recvfrom on the socket with the flag you want
	 *
	 * @param flag The flag param for the recvfrom
	 * @return std::shared_ptr<message_t> The j1939 message that is read
	 */
	std::shared_ptr<message_t> socketcan_j1939_t::read_message(int flag)
	{
		/* Initialize msghdr to retrieve the J1939 metadata and data
		 * more convenient to get the timestamp saving syscalls */
		uint8_t data[128] = {0};
		struct iovec iov;
		struct msghdr msg;
		char ctrlmsg[
			CMSG_SPACE(sizeof(struct timeval))
			+ CMSG_SPACE(sizeof(uint8_t)) /* dest addr */
			+ CMSG_SPACE(sizeof(uint64_t)) /* dest name */
			+ CMSG_SPACE(sizeof(uint8_t)) /* priority */
		];
		iov.iov_base = &data;
		msg.msg_name = &tx_address_;
		msg.msg_iov = &iov;
		msg.msg_iovlen = 1;
		msg.msg_control = &ctrlmsg;

		iov.iov_len = sizeof(data);
		msg.msg_namelen = sizeof(tx_address_);
		msg.msg_controllen = sizeof(ctrlmsg);
		msg.msg_flags = 0;

		/* Working variables to parse the recvmsg return */
		struct cmsghdr *cmsg;
		struct timeval tv = {0,0};
		uint64_t timestamp = 0;
		long recvflags = 0;

		/* Final container to hold the received message */
		std::shared_ptr<j1939_message_t> jm = std::make_shared<j1939_message_t>();

		ssize_t nbytes = recvmsg(socket_, &msg, flag);

		if(nbytes < 0)
			return nullptr;

		for (cmsg = CMSG_FIRSTHDR(&msg); cmsg; cmsg = CMSG_NXTHDR(&msg, cmsg))
		{
			switch (cmsg->cmsg_level) {
			case SOL_SOCKET:
				if (cmsg->cmsg_type == SO_TIMESTAMP) {
					memcpy(&tv, CMSG_DATA(cmsg), sizeof(tv));
					recvflags |= 1 << cmsg->cmsg_type;
				}
				break;
			}
		}
		timestamp = 1000000L * tv.tv_sec + tv.tv_usec;

		jm = j1939_message_t::convert_from_addr(tx_address_, data , nbytes, timestamp);
		jm->set_sub_id((int)socket());
		return jm;
	}

	/**
	 * @brief Write a j1939 message
	 *
	 * @param pgn The pgn that you want to emmit
	 * @param data The data that you want to emmit
	 * @param len_data The size of the data to emmit
	 * @return int 0 if the message is correctly send
	 */
	int socketcan_j1939_t::write_j1939_message(pgn_t pgn, std::vector<uint8_t> &data, uint32_t len_data)
	{
		j1939_message_t msg = j1939_message_t(len_data, data, 0, 0, pgn, 0);
		msg.set_sockname(pgn, J1939_NO_NAME, J1939_NO_ADDR);
		return write_message(msg);
	}

	/**
	 * @brief Write a j1939 message
	 *
	 * @param m A j1939 message
	 * @return int 0 if the message is correctly send
	 */
	int socketcan_j1939_t::write_message(message_t& m)
	{
		j1939_message_t& jm = reinterpret_cast<j1939_message_t&>(m);
		sockaddr_can sockname =  jm.get_sockname();
		uint8_t data[jm.get_data_vector().size()];

		for(int i=0; i<jm.get_data_vector().size(); i++)
			data[i] = jm.get_data_vector()[i];

		if(socket_ < 0) {
			AFB_ERROR("SOCKET SHOULD NOT BE CLOSED! %d", socket_);
			return 0;
		}
		if (sendto(socket_, &data, sizeof(data), 0, (const struct sockaddr *)&sockname, sizeof(sockname)) < 0)
		{
			AFB_ERROR("Error sending : %i %s", errno, ::strerror(errno));
			return -errno;
		}
		return 0;
	}
}
