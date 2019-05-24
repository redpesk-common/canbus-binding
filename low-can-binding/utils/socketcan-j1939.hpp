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

#pragma once

#include <linux/can/j1939.h>

#include "./socketcan.hpp"
#include "../can/message/j1939-message.hpp"

namespace utils
{

	class socketcan_j1939_t : public socketcan_t
	{
	public:
		using socketcan_t::socketcan_t;
		virtual int open(std::string device_name);
		int open(std::string device_name,  name_t name, pgn_t pgn, uint8_t addr);
		virtual std::shared_ptr<message_t> read_message();
		virtual void write_message(std::shared_ptr<message_t> obj);
		virtual void write_message(std::vector<std::shared_ptr<message_t>>& vobj);

	private:
		int connect(const struct sockaddr* addr, socklen_t len);
		int bind(const struct sockaddr* addr, socklen_t len);
		void filter(name_t name, pgn_t pgn, uint8_t addr, uint64_t name_mask=0, uint32_t pgn_mask=0, uint8_t addr_mask=0);
	};
}
