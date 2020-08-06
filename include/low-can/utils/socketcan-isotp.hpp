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

#pragma once

#include <linux/can/isotp.h>
#include <low-can/utils/socketcan.hpp>
#include <low-can/can/message/can-message.hpp>

#define CAN_ISOTP_MAX_DLEN 4096

namespace utils
{
	class socketcan_isotp_t : public socketcan_t
	{
	public:
		using socketcan_t::socketcan_t;

		virtual int open(std::string device_name);
		virtual int open(std::string device_name, canid_t rx_id, canid_t tx_id);
		virtual std::shared_ptr<message_t> read_message();
		virtual int write_message(message_t& obj);
		int define_tx_address(std::string device_name, canid_t rx_id, canid_t tx_id);
	};
}
