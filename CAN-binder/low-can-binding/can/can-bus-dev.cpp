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

#include <map>
#include <mutex>
#include <unistd.h>
#include <linux/can/raw.h>

#include "can-bus-dev.hpp"

#include "can-bus.hpp"
#include "can-message.hpp"
#include "../low-can-binding.hpp"

/// @brief Class constructor
///
/// @param[in] dev_name - String representing the device name into the linux /dev tree
/// @param[in] address - integer identifier of the bus, set using init_can_dev from can_bus_t.
can_bus_dev_t::can_bus_dev_t(const std::string& dev_name, int32_t address)
	: device_name_{dev_name}, address_{address}
{}

std::string can_bus_dev_t::get_device_name() const
{
	return device_name_;
}

uint32_t can_bus_dev_t::get_address() const
{
	return address_;
}

/// @brief Open the can socket and returning it
///
///  We try to open CAN socket and apply the following options
///  timestamp received messages and pass the socket to FD mode.
///
/// @return socket value or -1 if something wrong.
int can_bus_dev_t::open()
{
	return can_socket_.open(device_name_);
}

/// @brief Set some option on the socket, timestamp and canfd frame usage.
void can_bus_dev_t::configure()
{
	if (can_socket_)
	{
		const int timestamp_on = 1;

		DEBUG(binder_interface, "%s: CAN Handler socket correctly initialized : %d", __FUNCTION__, can_socket_.socket());
		if (can_socket_.setopt(SOL_SOCKET, SO_TIMESTAMP, &timestamp_on, sizeof(timestamp_on)) < 0)
			WARNING(binder_interface, "%s: setsockopt SO_TIMESTAMP error: %s", __FUNCTION__, ::strerror(errno));
	}
	else
	{
		ERROR(binder_interface, "open_raw: socket could not be created. Error was : %s", ::strerror(errno));
	}
}

/// @brief Close the bus.
///
/// @return interger return value of socket.close() function
int can_bus_dev_t::close()
{
	return can_socket_.close();
}

/// @brief Read the can socket and retrieve canfd_frame.
///
///  Read operation are blocking and we try to read CANFD frame
///  rather than classic CAN frame. CANFD frame are retro compatible.
can_message_t can_bus_dev_t::read()
{
	ssize_t nbytes;
	struct canfd_frame cfd;

	// Test that socket is really opened
	if (!can_socket_)
	{
		ERROR(binder_interface, "read: Socket unavailable. Closing thread.");
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

	DEBUG(binder_interface, "%s: Found id: %X, length: %X, data %02X%02X%02X%02X%02X%02X%02X%02X", __FUNCTION__, cfd.can_id, cfd.len,
							cfd.data[0], cfd.data[1], cfd.data[2], cfd.data[3], cfd.data[4], cfd.data[5], cfd.data[6], cfd.data[7]);
	return can_message_t::convert_from_canfd_frame(cfd, nbytes);
}

/// @brief start reading threads and set flag is_running_
/// @param[in] can_bus reference can_bus_t. it will be passed to the thread to allow using can_bus_t queue.
void can_bus_dev_t::start_reading(can_bus_t& can_bus)
{
	DEBUG(binder_interface, "%s: Launching reading thread", __FUNCTION__);
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
/// @param[in] can_bus - object to be used to read the can socket
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
/// @param[in] can_msg - the can message object to send
///
/// @return 0 if message snet, -1 if something wrong.
int can_bus_dev_t::send(can_message_t& can_msg)
{
	ssize_t nbytes;
	canfd_frame f;

	f = can_msg.convert_to_canfd_frame();

	if(can_socket_)
	{
		nbytes = can_socket_.send(f);
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
		return -1;
	}
	return 0;
}

/// @brief Static method used to send diagnostic CAN message
/// that follow isotp SendShimsMessage signature. This method is launched
/// from diagnostic manager's' same name method. It will use the diagnostic
/// manager configured CAN bus device to send the CAN message.
///
/// @param[in] arbitration_id - CAN arbitration id.
/// @param[in] data - CAN message payload to send
/// @param[in] size - size of the data to send
///
/// @return True if message sent, false if not.
bool can_bus_dev_t::shims_send(const uint32_t arbitration_id, const uint8_t* data, const uint8_t size)
{
	ssize_t nbytes;
	canfd_frame f;

	f.can_id = arbitration_id;
	f.len = size;
	::memcpy(f.data, data, size);

	if(can_socket_)
	{
		nbytes = can_socket_.send(f);
		if (nbytes == -1)
		{
			ERROR(binder_interface, "send_can_message: Sending CAN frame failed.");
			return false;
		}
		return true;
	}
	else
	{
		ERROR(binder_interface, "send_can_message: socket not initialized. Attempt to reopen can device socket.");
		open();
	}
	return false;
}
