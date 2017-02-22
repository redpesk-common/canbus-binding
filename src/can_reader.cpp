/*
 * Copyright (C) 2015, 2016 "IoT.bzh"
 * Author "Romain Forlot" <romain.forlot@iot.bzh>
 * Author "Loic Collignon" <loic.collignon@iot.bzh>
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

#include "can_reader.hpp"

void can_reader(can_bus_dev_t &can_bus_dev, can_bus_t& can_bus)
{
	can_message_t can_message(can_bus.interface_);

	while(can_bus_dev.is_running())
	{
		can_message.convert_from_canfd_frame(can_bus_dev.read(can_bus.interface_));
		can_bus.push_new_can_message(can_message);
	}
}