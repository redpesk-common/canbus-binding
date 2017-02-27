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

#include "can-utils.hpp"
#include "openxc-utils.hpp"
#include "can-signals.hpp"
#include "can-decoder.hpp"

#include "can_reader.hpp"

void can_decode_message(can_bus_t &can_bus)
{
	can_message_t can_message;
	std::vector <CanSignal> signals;
	std::vector <CanSignal>::iterator signals_i;
	openxc_VehicleMessage vehicle_message;
	openxc_DynamicField search_key, decoded_message;

	decoder_t decoder;

	while(can_bus.is_decoding())
	{
		{
			std::unique_lock<std::mutex> can_message_lock(can_bus.get_can_message_mutex());
			can_bus.get_new_can_message().wait(can_message_lock);
			can_message = can_bus.next_can_message();
		}

		/* First we have to found which CanSignal it is */
		search_key = build_DynamicField((double)can_message.get_id());
		signals = find_can_signals(search_key);

		/* Decoding the message ! Don't kill the messenger ! */
		for(auto& sig : signals)
		{
			{
				std::lock_guard<std::mutex> subscribed_signals_lock(get_subscribed_signals_mutex());
				std::map<std::string, struct afb_event> subscribed_signals = get_subscribed_signals();
				const auto& it_event = subscribed_signals.find(sig.genericName);
				
				if(it_event != subscribed_signals.end() &&
					afb_event_is_valid(it_event->second))
				{
					decoded_message = decoder.translateSignal(sig, can_message, getSignals());

					openxc_SimpleMessage s_message = build_SimpleMessage(sig.genericName, decoded_message);
					vehicle_message = build_VehicleMessage_with_SimpleMessage(openxc_DynamicField_Type::openxc_DynamicField_Type_NUM, s_message);

					std::lock_guard<std::mutex> decoded_can_message_lock(can_bus.get_decoded_can_message_mutex());
					can_bus.push_new_vehicle_message(vehicle_message);
				}
				can_bus.get_new_decoded_can_message().notify_one();
			}
		}
	}
}
