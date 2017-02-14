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
#include "can-decoder.h"
#include "openxc.pb.h"

void can_decode_message(CanBus_c *can_bus)
{
	CanMessage_c *can_message;
	int i;
	std:vector <CanSignal> *signals;
	CanSignal sig;
	openxc_VehicleMessage vehicle_message;
	openxc_SimpleMessage s_message;
	openxc_DynamicField key, ret;
	Decoder_c decoder();

	vehicle_message = {.has_type = true,
					  .type = openxc_VehicleMessage_Type::openxc_VehicleMessage_Type_SIMPLE,
					  .has_simple_message = true };

	while(true)
	{
		if(can_message = can_bus->next_can_message())
		{
			/* First we have to found which CanSignal is */
			key = { .has_type = true,
					.type = openxc_DynamicField_Type::openxc_DynamicField_Type_NUM,
					.has_numeric_value = true,
					.numeric_value = (double)can_message.get_id() };
			signals = GetSignals(key);

			/* Decoding the message ! Don't kill the messenger ! */
			if(signals.size() > 0)
			{
				for(i=0; i< signals.size(); i++)
				{
					sig = signals.back();
					ret = decoder.decodeSignal(&sig, can_message, SIGNALS, SIGNALS.size(), true);

					s_message = {.has_name = true,
										.name = sig->genericName,
										.has_value = true,
										.value = ret
									};
					vehicle_message.simple_message = s_message;
					vehicle_message_q.push(vehicle_message);
					
					signals.pop_back();
				}
			}
		}
	}
}