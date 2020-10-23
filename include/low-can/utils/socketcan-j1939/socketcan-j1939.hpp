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
#include <low-can/utils/socketcan.hpp>
#include <low-can/can/message/j1939-message.hpp>

#define J1939_CAN_ID CAN_EFF_FLAG
#define J1939_CAN_MASK (CAN_EFF_FLAG | CAN_RTR_FLAG)


// PDU 1 = NO BROADCAST
// PDU 2 = BRODCAST
static inline bool j1939_pgn_is_pdu1(pgn_t pgn)
{
	return (pgn & 0xff00) < 0xf000;
}

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
			void define_opt(bool broadcast = true, bool promisc = false);

			name_t get_j1939_name() const;
			void set_j1939_name(name_t ecu_name);
		protected:
			uint64_t j1939_name_ = 0x1234; ///< Default ECU j1939 name using noted using a little endianness ie: 0xC0509600227CC7AA
			struct ifreq ifr_;
			static std::mutex mutex_claiming_;
			static std::condition_variable signal_address_claiming_;
			void define_tx_address(std::string device_name, name_t name, pgn_t pgn, uint8_t addr);
			int add_filter(name_t name, pgn_t pgn, uint8_t addr, name_t name_mask, pgn_t pgn_mask, uint8_t addr_mask);
	};
}
