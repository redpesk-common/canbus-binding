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

#pragma once

#include <queue>
#include <string>
#include <vector>
#include <fnmatch.h>
#include <linux/can.h>

#include "can-utils.hpp"
#include "low-can-binding.hpp"

extern "C"
{
	#include <afb/afb-event-itf.h>
}

/**
 * @brief Dumb SIGNALS array. It is composed by CanMessageSet
 * SIGNALS[MESSAGE_SET_ID][CanSignal]
 */
std::vector<std::vector<CanSignal>> SIGNALS {
	{}// message set: example
};

/** Public: Return the currently active CAN configuration. */
CanMessageSet* getActiveMessageSet();

/** Public: Retrive a list of all possible CAN configurations.
 *
 * Returns a pointer to an array of all configurations.
 */
CanMessageSet* getMessageSets();

/** Public: Return the length of the array returned by getMessageSets() */
int getMessageSetCount();

/* Public: Return the number of CAN buses configured in the active
 * configuration. This is limited to 2, as the hardware controller only has 2
 * CAN channels.
 */
int getCanBusCount();

/* Public: Return an array of all CAN messages to be processed in the active
 * configuration.
 */
CanMessageDefinition* getMessages();

/* Public: Return signals from an signals array filtered on name.
 */
const std::vector<CanSignal> getSignals();

/* Public: Return an array of all OpenXC CAN commands enabled in the active
 * configuration that can write back to CAN with a custom handler.
 *
 * * Commands not defined here are handled using a 1-1 mapping from the signals
 * list.
 *		*/
CanCommand* getCommands();

/* Public: Return the length of the array returned by getCommandCount(). */
int getCommandCount();

/* Public: Return the length of the array returned by getSignals(). */
size_t getSignalCount();

/* Public: Return the length of the array returned by getMessages(). */
int getMessageCount();

/**
 * @brief Return an array of the metadata for the 2 CAN buses you want to
 * monitor. The size of this array is fixed at 2.
 */
CanBus* getCanBuses();

/**
 * @brief Find one or many signals based on its name or id
 * passed through openxc_DynamicField.
 *
 * params[openxc_DynamicField&] - a const reference with the key to search into signal.
 * Key is either a signal name or its CAN arbitration id.
 *
 * return[std::vector<std::string>] return found CanSignal generic name vector.
 */
std::vector<CanSignal> find_can_signals(const openxc_DynamicField &key);

uint32_t get_CanSignal_id(const CanSignal& sig)
{
	return sig.message->id;
}