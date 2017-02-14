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
#include "can-signals.h"

bool match(const std::string &first, const std::string &second)
{

}

CanSignal* getSignals(openxc_DynamicField *key)
{
	int n_signals, i;
	std::vector <CanSignal> ret_signals;

	n_signals = SIGNALS.size();

	switch(key->type):
	{
		case openxc_DynamicField_Type::openxc_DynamicField_Type_STRING:
			for(i=0; i<=n_signals; i++)
			{
				if(match((std::string*)key->string_value, (std::string*)SIGNALS[i]->genericName))
					ret_signals.insert(&SIGNALS[i]);
			}
			break;
		case openxc_DynamicField_Type::openxc_DynamicField_Type_NUM:
			for(i=0; i<=n_signals; i++)
			{
				CanMessageDefinition *msg_def = SIGNALS[i]->message;
				if(msg_def->id == key->numeric_value)
					ret_signals.insert(&SIGNALS[i])
			}
			break;
		default:
			return NULL;
			break;
	}

	return &ret_signals;
}