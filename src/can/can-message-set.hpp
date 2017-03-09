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

#include <cstdint>
#include <string>

/// @brief A parent wrapper for a particular set of CAN messages and associated
/// CAN buses(e.g. a vehicle or program).
class can_message_set_t
{
private:
	uint8_t index_; /// < A numerical ID for the message set, ideally the index in an array for fast lookup
	const std::string name_; /// < The name of the message set.
	uint8_t can_bus_count_; /// < The number of CAN buses defined for this message set.
	uint16_t can_message_count_; /// < The number of CAN messages (across all buses) defined for this message set.
	uint16_t can_signal_count_; /// < The number of CAN signals (across all messages) defined for this message set.
	uint16_t can_command_count_; /// < The number of CanCommmands defined for this message set.
	uint16_t obd2_signal_count_; /// < The number of obd2 signals defined for this message set.
	
public:
	   can_message_set_t(
		   uint8_t index,
		   const std::string& name,
		   uint8_t can_bus_count,
		   short unsigned int can_message_count,
		   short unsigned int can_signal_count,
		   short unsigned int can_command_count,
		   short unsigned int obd2_signal_count);
	
};
