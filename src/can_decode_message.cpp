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

#include "can_decode_message.hpp"

void can_decode_message(can_bus_t &can_bus)
{
	can_message_t can_message(interface) ;
	std::vector <CanSignal> signals;
	std::vector <CanSignal>::iterator signals_i;
	openxc_VehicleMessage vehicle_message;
	openxc_DynamicField search_key, ret;
	bool send = true;

	decoder_t decoder();

	while(true)
	{
		if(can_message = can_bus.next_can_message(interface))
		{
			/* First we have to found which CanSignal is */
			search_key = build_DynamicField((double)can_message.get_id())
			signals = find_can_signals(search_key);
			
			/* Decoding the message ! Don't kill the messenger ! */
			for(const auto& sig : signals)
			{	
				subscribed_signals_i = subscribed_signals.find(sig.genericName);
				
				if(subscribed_signals_i != subscribed_signals.end() &&
					afb_event_is_valid(subscribed_signals_i->second))
				{
					ret = decoder.decodeSignal(sig, can_message, getSignals(), &send);

					s_message = build_SimpleMessage(sig.genericName, ret);
						
					vehicle_message = build_VehicleMessage_with_SimpleMessage(openxc_DynamicField_Type::openxc_DynamicField_Type_NUM, s_message);
					vehicle_message_q.push(vehicle_message);
				}
			}
		}
	}
}
