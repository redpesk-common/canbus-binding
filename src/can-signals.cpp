/*
 * Copyright (C) 2015, 2016 "IoT.bzh"
 * Author "Romain Forlot" <romain.forlot@iot.bzh>
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

#include "can-signals.hpp"

#include <fnmatch.h>

#include "can-decoder.hpp"
#include "low-can-binding.hpp"

std::vector<std::vector<CanMessageDefinition>> CAN_MESSAGES = {
	{
		{0x620, CanMessageFormat::STANDARD, {10, 0, nullptr}, false, (uint8_t)NULL},
		{0x442, CanMessageFormat::STANDARD, {10, 0, nullptr}, false, (uint8_t)NULL},
	},
};

/**
 * @brief Dumb SIGNALS array. It is composed by CanMessageSet
 * SIGNALS[MESSAGE_SET_ID][CanSignal]
 */
std::vector<std::vector<CanSignal>> SIGNALS = {
	{
		{&(CAN_MESSAGES[MESSAGE_SET_ID][0]), "can.driver_door.open", 2, 4, 1.000000, 0.000000, 0.000000, 0.000000, {10, 0, nullptr}, false, true, nullptr, 0, false, decoder_t::booleanDecoder, nullptr, false, (float)NULL},
		{&(CAN_MESSAGES[MESSAGE_SET_ID][1]), "can.driver_door.close", 2, 4, 1.000000, 0.000000, 0.000000, 0.000000, {10, 0, nullptr}, false, true, nullptr, 0, false, decoder_t::booleanDecoder, nullptr, false, (float)NULL}
	},
};

/** 
 * @brief Can signal event map making access to afb_event
 * externaly to an openxc existing structure.
 *
 * @desc Event map is making relation between CanSignal generic name
 * and the afb_event struct used by application framework to pushed
 * to the subscriber.
 */
std::map<std::string, struct afb_event> subscribed_signals;

/**
* @brief Mutex allowing safe manipulation on subscribed_signals map.
* @desc To ensure that the map object isn't modified when we read it, you
*  have to set this mutex before use subscribed_signals map object.
*/
std::mutex subscribed_signals_mutex;

std::mutex& get_subscribed_signals_mutex()
{
	return subscribed_signals_mutex;
}

std::map<std::string, struct afb_event>& get_subscribed_signals()
{
	DEBUG(binder_interface, "Here are the first subscribed_signals: %s", subscribed_signals.begin()->first.c_str() );
	return subscribed_signals;
}

const std::vector<CanSignal>& getSignals()
{
	return SIGNALS[MESSAGE_SET_ID];
}

size_t getSignalCount()
{
	return SIGNALS[MESSAGE_SET_ID].size();
}

std::vector<CanSignal> find_can_signals(const openxc_DynamicField &key)
{
	std::vector<CanSignal> signals = {};

	switch(key.type)
	{
		case openxc_DynamicField_Type::openxc_DynamicField_Type_STRING:
			for(const CanSignal& s : getSignals())
			{
				if(fnmatch(key.string_value, s.genericName, FNM_CASEFOLD) == 0)
					signals.push_back(s);
			}
			break;
		case openxc_DynamicField_Type::openxc_DynamicField_Type_NUM:
			for(const CanSignal& s : getSignals())
			{
				CanMessageDefinition *msg_def = s.message;
				if(msg_def->id == key.numeric_value)
					signals.push_back(s);
			}
			break;
		default:
			ERROR(binder_interface, "find_can_signals: wrong openxc_DynamicField specified. Use openxc_DynamicField_Type_NUM or openxc_DynamicField_Type_STRING type only.");
			return signals;
			break;
	}
	return signals;
}

inline uint32_t get_CanSignal_id(const CanSignal& sig)
{
	return sig.message->id;
}