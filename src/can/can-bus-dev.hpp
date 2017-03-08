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

#pragma once

#include <string>
#include <thread>
#include <linux/can.h>

#include "../utils/socket.hpp"

class can_bus_t;
class can_message_t;

/// @brief Object representing a can device. Handle opening, closing and reading on the
/// socket. This is the low level object to be use by can_bus_t.
class can_bus_dev_t 
{
private:
	std::string device_name_;
	utils::socket_t can_socket_;

	int32_t id_; /// < an identifier used through binding that refer to that device
	
	bool is_fdmode_on_; /// < boolean telling if whether or not the can socket use fdmode.
	struct sockaddr_can txAddress_; /// < internal member using to bind to the socket

	std::thread th_reading_; /// < Thread handling read the socket can device filling can_message_q_ queue of can_bus_t
	bool is_running_; /// < boolean telling whether or not reading is running or not
	void can_reader(can_bus_t& can_bus);

public:
	can_bus_dev_t(const std::string& dev_name);

	int open();
	int close();

	void start_reading(can_bus_t& can_bus);

	void stop_reading();

	std::pair<struct canfd_frame&, size_t> read();
		
	int send_can_message(can_message_t& can_msg);
};
