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
	class socketcan_j1939_data_t : public socketcan_j1939_t
	{
		public:
			using socketcan_j1939_t::socketcan_j1939_t;
			virtual int open(std::string device_name, name_t ecu_name, pgn_t pgn);
			virtual int write_message(message_t& obj);
	};
}
