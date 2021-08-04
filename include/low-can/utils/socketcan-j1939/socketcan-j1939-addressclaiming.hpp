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

#include "./socketcan-j1939.hpp"

namespace utils
{
	class socketcan_j1939_addressclaiming_t : public socketcan_j1939_t
	{
		public:
			socketcan_j1939_addressclaiming_t();
			virtual std::shared_ptr<message_t> read_message();
			virtual int open(std::string device_name, name_t ecu_name, pgn_t pgn);
			virtual claiming_state get_state();
			afb_timer_t timer_handle_;
			std::pair<uint64_t, bool> table_j1939_address_[J1939_IDLE_ADDR];

		private:
			int claim_address(bool first_claim, bool new_address);
			int pgn_request();
			void initialize_table_j1939_address();
			int save_addr_name(uint8_t addr, name_t name);
			uint8_t choose_new_address();
			uint8_t get_addr_table(name_t name);
			void change_state(claiming_state new_state);
			void launch_timer();
			static void no_response_claiming(afb_timer_t timerhandle, void *closure, unsigned decount);
			static int free_timer_handle(void *timer_context);

			uint8_t current_address_;
			bool signal_stop_;
			claiming_state claiming_state_;
			std::string device_name_;
	};
}
