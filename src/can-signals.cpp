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

#include <string>
#include <vector>
#include <fnmatch.h>
#include "can-signals.h"

/* Can signal event map making access to afb_event
 * external to openxc existing structure.
 */
std::map <CanSignal, struct afb_event> subscribed_signals;
std::map <CanSignal, struct afb_event>::iterator subscribed_signals_i;

/* Find one or many signals based on its name or id
* passed through openxc_DynamicField.
*/
void find_can_signals(openxc_DynamicField *key, std:vector <CanSignal> *signals)
{
	int n_signals, i;

	n_signals = getSignalCount();

	switch(key->type):
	{
		case openxc_DynamicField_Type::openxc_DynamicField_Type_STRING:
			for(i=0; i<=n_signals; i++)
			{
				if(fnmatch(key->string_value, SIGNALS[i]->genericName) == 0)
					signals.insert(&SIGNALS[i]);
			}
			break;
		case openxc_DynamicField_Type::openxc_DynamicField_Type_NUM:
			for(i=0; i<=n_signals; i++)
			{
				CanMessageDefinition *msg_def = SIGNALS[i]->message;
				if(msg_def->id == key->numeric_value)
					signals.insert(&SIGNALS[i])
			}
			break;
		default:
			ERROR(interface, "find_signals: wrong openxc_DynamicField specified. Use openxc_DynamicField_Type_NUM or openxc_DynamicField_Type_STRING type only.");
			return NULL;
			break;
	}
}

CanSignal* getSignals()
{
	return &SIGNALS;
}

int getSignalCount()
{
	return SIGNALS.size();
}