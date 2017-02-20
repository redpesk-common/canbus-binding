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

#include <linux/can.h>
#include <linux/can/raw.h>
#include <queue>

#include <afb/afb-binding.h>

#include "can-utils.h"
#include "openxc-utils.hpp"
#include "openxc.pb.h"
#include "json-c/json.h"

void can_event_push(can_bus_t& can_bus)
{
	openxc_VehicleMessage v_message;
	openxc_SimpleMessage s_message;
	iterator it_event;

	while(true)
	{
		if(v_message = can_bus->next_vehicle_message())
		{
			s_message = get_simple_message(v_msg);
			const auto& it_event = subscribed_signals.find(s_message.name);
			if(! it_event->end() && afb_event_is_valid(it_event->second))
				afb_event_push(it_event->second, jsonify_simple(s_message));
		}
	}
}
