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

#include "signals.hpp"

/** 
 * @brief Can signal event map making access to afb_event
 * externaly to an openxc existing structure.
 *
 * @desc Event map is making relation between can_signal_t generic name
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
	return subscribed_signals;
}

/**
 * @fn std::vector<std::string> find_signals(const openxc_DynamicField &key)
 * @brief return signals name found searching through CAN_signals and OBD2 pid
 *
 * @param[in] const openxc_DynamicField : can contain numeric or string value in order to search against 
 *   can signals or obd2 signals name.
 *
 * @return std::vector<std::string> Vector of signals name found. 
 */
std::vector<std::string> find_signals(const openxc_DynamicField &key)
{
	std::vector<std::string> found_signals_name;

	switch(key.type)
	{
		case openxc_DynamicField_Type::openxc_DynamicField_Type_STRING:
				lookup_signals_by_name(key.string_value, config->get_can_signals(), found_signals_name);
				lookup_signals_by_name(key.string_value, get_obd2_signals(), found_signals_name);
			break;
		case openxc_DynamicField_Type::openxc_DynamicField_Type_NUM:
				lookup_signals_by_id(key.numeric_value, config->get_can_signals(), found_signals_name);
				lookup_signals_by_id(key.numeric_value, get_obd2_signals(), found_signals_name);
			break;
		default:
			ERROR(binder_interface, "find_signals: wrong openxc_DynamicField specified. Use openxc_DynamicField_Type_NUM or openxc_DynamicField_Type_STRING type only.");
			break;
	}
	DEBUG(binder_interface, "Found %d signal(s)", (int)found_signals_name.size());
	return found_signals_name;
}
