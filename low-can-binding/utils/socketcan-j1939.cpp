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

#include <net/if.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include "./socketcan-j1939.hpp"
#include "../binding/application.hpp"

namespace utils
{
	/// @brief Connect the socket.
	/// @return 0 if success.
	int socketcan_j1939_t::connect(const struct sockaddr* addr, socklen_t len)
	{
		return socket_ != INVALID_SOCKET ? ::connect(socket_, addr, len) : 0;
	}

	/// @brief Bind the socket.
	/// @return 0 if success.
	int socketcan_j1939_t::bind(const struct sockaddr* addr, socklen_t len)
	{
		return socket_ != INVALID_SOCKET ? ::bind(socket_, addr, len) : 0;
	}

	void socketcan_j1939_t::filter(name_t name, pgn_t pgn, uint8_t addr, uint64_t name_mask, uint32_t pgn_mask, uint8_t addr_mask)
	{
		int filter_on = 0;
		struct j1939_filter filter;
		memset(&filter, 0, sizeof(filter));
		if (name)
		{
			filter.name = name;
			if(name_mask)
			{
				filter.name_mask = name_mask;
			} else
			{
				filter.name_mask = ~0ULL;
			}
			++filter_on;
		}

		if (addr < 0xff)
		{
			filter.addr = addr;
			if(addr_mask)
			{
				filter.addr_mask = addr_mask;
			} else
			{
				filter.addr_mask = ~0;
			}
			++filter_on;
		}
		if (pgn <= J1939_PGN_MAX)
		{
			filter.pgn = pgn;
			if(pgn_mask)
			{
				filter.pgn_mask = pgn_mask;
			} else
			{
				filter.pgn_mask = ~0;
			}
			++filter_on;
		}
		if(filter_on){
			setopt(SOL_CAN_J1939, SO_J1939_FILTER, &filter, sizeof(filter));
		}
	}

	int socketcan_j1939_t::open(std::string device_name)
	{
		return open(device_name,0,0,0);
	}

	int socketcan_j1939_t::open(std::string device_name,  name_t name, pgn_t pgn, uint8_t addr)
	{

		struct ifreq ifr;
		socket_ = socketcan_t::open(PF_CAN, SOCK_DGRAM, CAN_J1939);

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

			if(addr <= 0 || addr >= UINT8_MAX )
			{
				tx_address_.can_addr.j1939.addr = J1939_NO_ADDR;
			}
			else
			{
				tx_address_.can_addr.j1939.addr = addr;
			}

			if(name <= 0 || name >= UINT64_MAX )
			{
				tx_address_.can_addr.j1939.name = J1939_NO_NAME;
			}
			else
			{
				tx_address_.can_addr.j1939.name = name;
			}

			if(pgn <= 0 || pgn > J1939_PGN_MAX)
			{
				tx_address_.can_addr.j1939.pgn = J1939_NO_PGN;
			}
			else
			{
				tx_address_.can_addr.j1939.pgn = pgn;
			}

			if(bind((struct sockaddr *)&tx_address_, sizeof(tx_address_)) < 0)
			{
				AFB_ERROR("Bind failed. %s", strerror(errno));
				close();
			}
			else
			{
				int promisc = 1; // 0 = disabled (default), 1 = enabled
				setopt(SOL_CAN_J1939, SO_J1939_PROMISC, &promisc, sizeof(promisc));
				int recv_own_msgs = 1; // 0 = disabled (default), 1 = enabled
				setopt(SOL_CAN_J1939, SO_J1939_RECV_OWN, &recv_own_msgs, sizeof(recv_own_msgs));
				if(tx_address_.can_addr.j1939.pgn != J1939_NO_PGN)
				{
					filter(J1939_NO_NAME,tx_address_.can_addr.j1939.pgn,J1939_NO_ADDR);
				}
			}
		}
		return socket_;
	}

	std::shared_ptr<message_t> socketcan_j1939_t::read_message()
	{
		std::shared_ptr<j1939_message_t> jm = std::make_shared<j1939_message_t>();
		uint8_t data[128] = {0};

		struct sockaddr_can addr;
		socklen_t addrlen = sizeof(addr);
		struct ifreq ifr;

		ssize_t nbytes = ::recvfrom(socket(), &data, sizeof(data), 0, (struct sockaddr*)&addr, &addrlen);
		ifr.ifr_ifindex = get_tx_address().can_ifindex;
		ioctl(socket(), SIOCGIFNAME, &ifr);

		AFB_DEBUG("Data available: %i bytes read", (int)nbytes);
		/*AFB_DEBUG("read: Found on bus %s:\n id: %X, length: %X, data %02X%02X%02X%02X%02X%02X%02X%02X", ifr.ifr_name, frame.can_id, frame.len,
			data[0], data[1], data[2], data[3], data[4], data[5], data[6], data[7]);*/

		struct timeval tv;
		ioctl(socket(), SIOCGSTAMP, &tv);
		uint64_t timestamp = 1000000 * tv.tv_sec + tv.tv_usec;
		jm = j1939_message_t::convert_from_addr(addr, data , nbytes, timestamp);
		jm->set_sub_id((int)socket());
		return jm;
	}


	void socketcan_j1939_t::write_message(std::vector<std::shared_ptr<can_message_t>>& vobj)
	{
		AFB_WARNING("Not implemented");
	}

	void socketcan_j1939_t::write_message(std::shared_ptr<can_message_t> cm)
	{
		AFB_WARNING("Not implemented");
	}

}
