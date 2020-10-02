/*
 * Copyright (C) 2019, 2020 "IoT.bzh"
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

#include <low-can/utils/socketcan-isotp.hpp>

#include <net/if.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <unistd.h>
#include <low-can/binding/application.hpp>

#include "../utils/converter.hpp"

namespace utils
{
	/**
	 * @brief Open ISOTP socket this default function will not work, because
	 * isotp need rx_id and tx_id
	 *
	 * @param device_name The device name where to open socket
	 * @return int -1 fail
	 */
	int socketcan_isotp_t::open(std::string device_name)
	{
		AFB_WARNING("NOT USE THIS FUNCTION !");
		return open(device_name, NO_CAN_ID, NO_CAN_ID);
	}

	/**
	 * @brief Open ISOTP socket, the socket will be open and bind
	 * with rx_id and tx_id
	 *
	 * @param device_name The device name where to open socket
	 * @param rx_id The source can_id
	 * @param tx_id The destination can_id
	 * @return int 0 if ok else -1
	 */
	int socketcan_isotp_t::open(std::string device_name, canid_t rx_id, canid_t tx_id)
	{
		close();
		socket_ = socketcan_t::open(PF_CAN, SOCK_DGRAM, CAN_ISOTP);

		if(socket_ < 0)
		{
			AFB_ERROR("Error open ISO TP socket");
			return -1;
		}

		if(define_tx_address(device_name, rx_id, tx_id) < 0)
			return -1;

		struct can_isotp_options opts;
		memset(&opts, 0, sizeof(opts));
		setopt(SOL_CAN_ISOTP, CAN_ISOTP_OPTS, &opts, sizeof(opts));

		if(bind((struct sockaddr *)&tx_address_, sizeof(tx_address_)) < 0)
		{
			AFB_ERROR("Bind failed. %s", strerror(errno));
			close();
			return -1;
		}

		return socket_;
	}

	/**
	 * @brief Allows to read message
	 *
	 * @return std::shared_ptr<message_t> The message that was read
	 */
	std::shared_ptr<message_t> socketcan_isotp_t::read_message()
	{

		std::shared_ptr<can_message_t> cm = std::make_shared<can_message_t>();
		uint8_t msg[MAX_ISOTP_FRAMES];
		ssize_t nbytes = read(socket(), msg, MAX_ISOTP_FRAMES);

		cm->set_id(tx_address_.can_addr.tp.rx_id);

		if(nbytes < 0)
		{
			AFB_ERROR("Can't read the next message from socket '%d'. '%s'", socket(), strerror(errno));
			return cm;
		}

		std::vector<uint8_t> data;
		for (int i=0; i < nbytes; i++)
			data.push_back(msg[i]);

		std::string data_string;
		data_string = converter_t::to_hex(msg, nbytes);
		AFB_DEBUG("DATA ISO TP for id : %x = %s", cm->get_id(), data_string.c_str());


		cm->set_data(data);
		cm->erase_flags();
		cm->set_flags(ISOTP_PROTOCOL);
		cm->set_length((uint32_t)nbytes);
		cm->set_sub_id((int)socket());
		// Need to define behaviour

		return cm;
	}

	/**
	 * @brief Allows to write can message
	 *
	 * @param m The message to send
	 * @return int 0 if ok else -1
	 */
	int socketcan_isotp_t::write_message(message_t& m)
	{
		size_t size = m.get_length();
		if(size < MAX_ISOTP_FRAMES)
		{
			ssize_t ret = write(socket(), m.get_data(), size);
			if(ret < 0)
			{
				AFB_ERROR("Error sending : %i %s", errno, ::strerror(errno));
				return -1;
			}

			if(ret != size)
				AFB_WARNING("ISOTP wrote only %zd byte", ret);
		}
		else
		{
			AFB_ERROR("Error sending too much data");
			return -1;
		}

		return 0;
	}

	/**
	 * @brief Define the tx address for the bind function
	 *
	 * @param device_name The device can that you want to bind
	 * @param rx_id The source can_id
	 * @param tx_id The destination can_id
	 * @return int 0 if ok else -1
	 */
	int socketcan_isotp_t::define_tx_address(std::string device_name, canid_t rx_id, canid_t tx_id)
	{
		struct ifreq ifr;
		::strcpy(ifr.ifr_name, device_name.c_str());
		AFB_DEBUG("ifr_name is : %s", ifr.ifr_name);

		if(::ioctl(socket_, SIOCGIFINDEX, &ifr) < 0)
		{
			AFB_ERROR("ioctl failed. Error was : %s", strerror(errno));
			close();
		}
		else
			tx_address_.can_ifindex = ifr.ifr_ifindex;

		tx_address_.can_family = AF_CAN;

		if(tx_id == NO_CAN_ID || rx_id == NO_CAN_ID)
		{
			AFB_ERROR("Error tx_id or rx_id");
			return -1;
		}

		tx_address_.can_addr.tp.rx_id = rx_id;
		tx_address_.can_addr.tp.tx_id = tx_id;

		return 0;
	}
}
