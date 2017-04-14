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

#include "can-message-set.hpp"

can_message_set_t::can_message_set_t(
		uint8_t index,
		const std::string& name,
		uint8_t can_bus_count,
		unsigned short can_message_count,
		unsigned short can_signal_count,
		unsigned short can_command_count,
		unsigned short obd2_signal_count)
	: index_{index}
	, name_{name}
	, can_bus_count_{can_bus_count}
	, can_message_count_{can_message_count}
	, can_signal_count_{can_signal_count}
	, can_command_count_{can_command_count}
	, obd2_signal_count_{obd2_signal_count}
{
}
