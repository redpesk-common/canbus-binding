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

#include <thread>
#include <mutex>
#include <condition_variable>
#include <linux/can/j1939.h>
#include <net/if.h>
#include "../socketcan.hpp"
#include "../../can/message/j1939-message.hpp"

#ifndef J1939_NAME_ECU
#define J1939_NAME_ECU 0x1234
#endif

namespace utils
{

	/**
	 * @enum state
	 * @brief The state of the address claiming
	 */
	enum class claiming_state {
		INITIAL, ///< INITIAL - INITIAL state
		CLAIMING, ///< CLAIMING - CLAIMING state
		OPERATIONAL, ///< OPERATIONAL - OPERATIONAL state
		INVALID
	};

	class socketcan_j1939_addressclaiming_t;
	class socketcan_j1939_t : public socketcan_t
	{
		public:
			using socketcan_t::socketcan_t;
			virtual int open(std::string device_name);
			virtual int open(std::string device_name, name_t name, pgn_t pgn, uint8_t addr);
			virtual std::shared_ptr<message_t> read_message();
			virtual std::shared_ptr<message_t> read_message(int flag);
			virtual int write_message(message_t& obj);
			virtual int write_j1939_message(pgn_t pgn, std::vector<uint8_t> &data, uint32_t len_data);

		protected:
			struct ifreq ifr_;
			static std::mutex mutex_claiming_;
			static std::condition_variable signal_address_claiming_;
			void define_tx_address(std::string device_name, name_t name, pgn_t pgn, uint8_t addr);
			int add_filter(name_t name, pgn_t pgn, uint8_t addr, name_t name_mask, pgn_t pgn_mask, uint8_t addr_mask);
			void define_opt(bool promisc = true, bool recv_own_msgs = true, bool broadcast = true);

	};
}
