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

const std::vector<CanSignal> getSignals()
{
	return SIGNALS[MESSAGE_SET_ID];
}

size_t getSignalCount()
{
	return SIGNALS[MESSAGE_SET_ID].size();
}

std::vector<CanSignal> find_can_signals(openxc_DynamicField& key)
{
	std::vector<CanSignal> signals;

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
			ERROR(interface, "find_can_signals: wrong openxc_DynamicField specified. Use openxc_DynamicField_Type_NUM or openxc_DynamicField_Type_STRING type only.");
			CanSignal cs;
			::memset(&cs, 0, sizeof(CanSignal));
			signals.push_back(cs);
			return signals;
			break;
	}
	return signals;
}