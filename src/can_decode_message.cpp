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
#include <sys/timeb.h>

#include <afb/afb-binding.h>

#include "can-utils.h"
#include "can-decoder.h"
#include "openxc.pb.h"

union DynamicField
{
	char string[100];
	double numeric_value;
	bool boolean_value;
};

void can_decode_message(can_bus_t &can_bus)
{
	can_message_t *can_message;
	std:vector <CanSignal> signals;
	std:vector <CanSignal>::iterator signals_i;
	openxc_VehicleMessage vehicle_message;
	openxc_DynamicField search_key, ret;
	
	decoder_t decoder();


	while(true)
	{
		if(can_message = can_bus.next_can_message())
		{
			/* First we have to found which CanSignal is */
			DynamicField signal_id = (double)can_message.get_id();
			search_key = build_DynamicField(openxc_DynamicField_Type_NUM, signal_id)

			signals = find_signals(search_key);
			
			/* Decoding the message ! Don't kill the messenger ! */
			for(signals_i=signals.begin(); signal_i != signals.end(); signals_i++)
			{
				subscribed_signals_i = subscribed_signals.find(signals_i);
				
				if(subscribed_signals_i != subscribed_signals.end() &&
					afb_event_is_valid(subscribed_signals_i->second))
				{
					ret = decoder.decodeSignal(&sig, can_message, SIGNALS, SIGNALS.size(), true);

					s_message = build_SimpleMessage(subscribed_signals_i->first->genericName, ret);
						
					vehicle_message = build_VehicleMessage_with_SimpleMessage(openxc_DynamicField_Type::openxc_DynamicField_Type_NUM, s_message);
					vehicle_message_q.push(vehicle_message);
				}
			}
		}
	}
}

/*
 * Build a specific VehicleMessage containing a SimpleMessage.
 */
openxc_VehicleMessage build_VehicleMessage_with_SimpleMessage(openxc_DynamicField_Type type,
															  openxc_SimpleMessage message)
{
	struct timeb t_msec;
	long long int timestamp_msec;
	if(!ftime(&t_msec))
	{
		timestamp_msec = ((long long int) t_msec.time) * 1000ll + 
                        (long long int) t_msec.millitm;

	return openxc_VehicleMessage v = {.has_type = true,
					  .type = openxc_VehicleMessage_Type::openxc_VehicleMessage_Type_SIMPLE,
					  .has_simple_message = true,
					  .simple_message =  message,
					  .has_timestamp = true,
					  .timestamp = timestamp_msec};
	}

	return openxc_VehicleMessage v = {.has_type = true,
					  .type = openxc_VehicleMessage_Type::openxc_VehicleMessage_Type_SIMPLE,
					  .has_simple_message = true,
					  .simple_message =  message};
}

/*
 * Build an openxc_SimpleMessage associating a name to an openxc_DynamicField
 */
openxc_SimpleMessage build_SimpleMessage(std:string name, openxc_DynamicField value)
{
	return openxc_SimpleMessage s = {.has_name = true,
									 .name = name,
									 .has_value = true,
									 .value = value};
}


}

/* 
 * Build an openxc_DynamicField using its type and an union.
 * Why do not use of union in first place anyway...
 */
openxc_DynamicField build_DynamicField(openxc_DynamicField_Type type, DynamicField field)
{
	openxc_DynamicField d = {.has_type = true,
									.type = type};
	
	switch(type)
	{
		case openxc_DynamicField_Type_BOOL:
			d.has_boolean_value = true;
			d.boolean_value = field;
			break;
		case openxc_DynamicField_Type_NUM:
			d.has_numeric_value = true;
			d.numeric_value = field;
			break;
		case openxc_DynamicField_Type_STRING:
			d.has_string_value = true;
			strcpy(d.string_value, field);
			break;
		default:
			return nullptr;
	}

	return d;
}