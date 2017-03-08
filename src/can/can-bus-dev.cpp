/*
 * Copyright (C) 2015, 2016, 2017 "IoT.bzh"
 * Author "Romain Forlot" <romain.forlot@iot.bzh>
 * Author "Loïc Collignon" <loic.collignon@iot.bzh>
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

#include <string.h>
#include <linux/can/raw.h>
#include <net/if.h>
#include <sys/ioctl.h>

#include "can-bus-dev.hpp"
#include "low-can-binding.hpp"

/// @brief Class constructor
/// @param dev_name String representing the device name into the linux /dev tree
can_bus_dev_t::can_bus_dev_t(const std::string& dev_name)
	: device_name_{dev_name}
{
}

/// @brief Open the can socket and returning it
/// @return -1
int can_bus_dev_t::open()
{
	const int canfd_on = 1;
	const int timestamp_on = 1;
	struct ifreq ifr;
	struct timeval timeout;

	DEBUG(binder_interface, "CAN Handler socket : %d", can_socket_.socket());
	if (can_socket_) return 0;

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
			ERROR(binder_interface, "ioctl failed. Error was : %s", strerror(errno));
		else
		{
			txAddress_.can_family = AF_CAN;
			txAddress_.can_ifindex = ifr.ifr_ifindex;

			// And bind it to txAddress
			DEBUG(binder_interface, "Bind the socket");
			if (::bind(can_socket_.socket(), (struct sockaddr *)&txAddress_, sizeof(txAddress_)) < 0)
				ERROR(binder_interface, "Bind failed. %s", strerror(errno));
			else
				return 0;
		}
		close();
	}
	else ERROR(binder_interface, "socket could not be created. Error was : %s", ::strerror(errno));
	return -1;
}
