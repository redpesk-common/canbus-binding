/*
* Copyright (C) 2015, 2016, 2017 "IoT.bzh"
* Author "Romain Forlot" <romain.forlot@iot.bzh>
* Author "Loïc Collignon" <loic.collignon@iot.bzh>
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
*       http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*/


#include <unistd.h>
#include <string.h>
#include <net/if.h>
#include <sys/ioctl.h>
#include <linux/can/raw.h>

#include <mutex>

#include "can-bus-dev.hpp"

#include "can-bus.hpp"
#include "can-message.hpp"
#include "../low-can-binding.hpp"

/// @brief Class constructor
/// @param dev_name String representing the device name into the linux /dev tree
can_bus_dev_t::can_bus_dev_t(const std::string& dev_name)
	: device_name_{dev_name}
{
}

std::string can_bus_dev_t::get_device_name() const
{
	return device_name_;
}

/// @brief Open the can socket and returning it
/// @return -1 if something wrong.
int can_bus_dev_t::open()
{
	const int canfd_on = 1;
	const int timestamp_on = 1;
	struct ifreq ifr;
	struct timeval timeout;

	DEBUG(binder_interface, "CAN Handler socket : %d", can_socket_.socket());
	if (can_socket_)
		return 0;

	can_socket_.open(PF_CAN, SOCK_RAW, CAN_RAW);
	if (can_socket_)
	{
		DEBUG(binder_interface, "CAN Handler socket correctly initialized : %d", can_socket_.socket());

		// Set timeout for read
		can_socket_.setopt(SOL_SOCKET, SO_RCVTIMEO, (char *)&timeout, sizeof(timeout));

		// Set timestamp for receveid frame
		if (can_socket_.setopt(SOL_SOCKET, SO_TIMESTAMP, &timestamp_on, sizeof(timestamp_on)) < 0)
			WARNING(binder_interface, "setsockopt SO_TIMESTAMP error: %s", ::strerror(errno));
		DEBUG(binder_interface, "Switch CAN Handler socket to use fd mode");

		// try to switch the socket into CAN_FD mode
		if (can_socket_.setopt(SOL_CAN_RAW, CAN_RAW_FD_FRAMES, &canfd_on, sizeof(canfd_on)) < 0)
		{
			NOTICE(binder_interface, "Can not switch into CAN Extended frame format.");
			is_fdmode_on_ = false;
		}
		else
		{
			DEBUG(binder_interface, "Correctly set up CAN socket to use FD frames.");
			is_fdmode_on_ = true;
		}

		// Attempts to open a socket to CAN bus
		::strcpy(ifr.ifr_name, device_name_.c_str());
		DEBUG(binder_interface, "ifr_name is : %s", ifr.ifr_name);
		if(::ioctl(can_socket_.socket(), SIOCGIFINDEX, &ifr) < 0)
		{
			ERROR(binder_interface, "ioctl failed. Error was : %s", strerror(errno));
		}
		else
		{
			txAddress_.can_family = AF_CAN;
			txAddress_.can_ifindex = ifr.ifr_ifindex;

			// And bind it to txAddress
			DEBUG(binder_interface, "Bind the socket");
			if (can_socket_.bind((struct sockaddr *)&txAddress_, sizeof(txAddress_)) < 0)
				ERROR(binder_interface, "Bind failed. %s", strerror(errno));
			else return 0;
		}
		close();
	}
	else ERROR(binder_interface, "socket could not be created. Error was : %s", ::strerror(errno));
	return -1;
}

/// @brief Close the bus.
int can_bus_dev_t::close()
{
	return can_socket_.close();
}

/// @brief Read the can socket and retrieve canfd_frame
can_message_t can_bus_dev_t::read()
{
	ssize_t nbytes;
	struct canfd_frame cfd;

	// Test that socket is really opened
	if (can_socket_)
	{
		ERROR(binder_interface, "read_can: Socket unavailable. Closing thread.");
		is_running_ = false;
	}

	nbytes = ::read(can_socket_.socket(), &cfd, CANFD_MTU);

	// if we did not fit into CAN sized messages then stop_reading.
	if (nbytes != CANFD_MTU && nbytes != CAN_MTU)
	{
		if (errno == ENETDOWN)
			ERROR(binder_interface, "read: %s CAN device down", device_name_.c_str());
		ERROR(binder_interface, "read: Incomplete CAN(FD) frame");
		::memset(&cfd, 0, sizeof(cfd));
	}

	DEBUG(binder_interface, "read: Found id: %X, length: %X, data %02X%02X%02X%02X%02X%02X%02X%02X", cfd.can_id, cfd.len,
							cfd.data[0], cfd.data[1], cfd.data[2], cfd.data[3], cfd.data[4], cfd.data[5], cfd.data[6], cfd.data[7]);
	return can_message_t::convert_from_canfd_frame(cfd, nbytes);
}

/// @brief start reading threads and set flag is_running_
/// @param[in] can_bus reference can_bus_t. it will be passed to the thread to allow using can_bus_t queue.
void can_bus_dev_t::start_reading(can_bus_t& can_bus)
{
	DEBUG(binder_interface, "Launching reading thread");
	is_running_ = true;
	th_reading_ = std::thread(&can_bus_dev_t::can_reader, this, std::ref(can_bus));
	if(!th_reading_.joinable())
		is_running_ = false;
}

/// @brief stop the reading thread setting flag is_running_ to false and and wait that the thread finish its job.
void can_bus_dev_t::stop_reading()
{
	is_running_ = false;
}

/// @brief Thread function used to read the can socket.
/// @param[in] can_bus object to be used to read the can socket
void can_bus_dev_t::can_reader(can_bus_t& can_bus)
{
	while(is_running_)
	{
		can_message_t msg = read();
		{
			std::lock_guard<std::mutex> can_message_lock(can_bus.get_can_message_mutex());
			can_bus.push_new_can_message(msg);
		}
		can_bus.get_new_can_message_cv().notify_one();
	}
}

/// @brief Send a can message from a can_message_t object.
/// @param[in] can_msg the can message object to send
int can_bus_dev_t::send(can_message_t& can_msg)
{
	ssize_t nbytes;
	canfd_frame f;

	f = can_msg.convert_to_canfd_frame();

	if(can_socket_.socket())
	{
		nbytes = ::sendto(can_socket_.socket(), &f, sizeof(struct canfd_frame), 0,
			(struct sockaddr*)&txAddress_, sizeof(txAddress_));
		if (nbytes == -1)
		{
			ERROR(binder_interface, "send_can_message: Sending CAN frame failed.");
			return -1;
		}
		return (int)nbytes;
	}
	else
	{
		ERROR(binder_interface, "send_can_message: socket not initialized. Attempt to reopen can device socket.");
		open();
	}
	return 0;
}

/// @brief Send a can message from a can_message_t object.
/// @param[in] can bus used to send the message
/// @param[in] can_msg the can message object to send
bool can_bus_dev_t::shims_send(const uint32_t arbitration_id, const uint8_t* data, const uint8_t size)
{
	ssize_t nbytes;
	canfd_frame f;

	f.can_id = arbitration_id;
	f.len = size;
	::memcpy(f.data, data, size);

	if(can_socket_.socket())
	{
		nbytes = ::sendto(can_socket_.socket(), &f, sizeof(struct canfd_frame), 0,
			(struct sockaddr*)&txAddress_, sizeof(txAddress_));
		if (nbytes == -1)
		{
			ERROR(binder_interface, "send_can_message: Sending CAN frame failed.");
			return -1;
		}
		return (int)nbytes;
	}
	else
	{
		ERROR(binder_interface, "send_can_message: socket not initialized. Attempt to reopen can device socket.");
		open();
	}
	return 0;
}
