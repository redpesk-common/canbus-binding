/*
 * Copyright (C) 2015, 2016 "IoT.bzh"
 * Author "Romain Forlot" <romain.forlot@iot.bzh>
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#pragma once

#include "can-utils.h"

/** Public: Return the currently active CAN configuration. */
CanMessageSet* getActiveMessageSet();

/** Public: Retrive a list of all possible CAN configurations.
 *  *
 *   * Returns a pointer to an array of all configurations.
 *    */
CanMessageSet* getMessageSets();

/** Public: Return the length of the array returned by getMessageSets() */
int getMessageSetCount();

/* Public: Return the number of CAN buses configured in the active
 *  * configuration. This is limited to 2, as the hardware controller only has 2
 *   * CAN channels.
 *    */
int getCanBusCount();

/* Public: Return an array of all CAN messages to be processed in the active
 *  * configuration.
 *   */
CanMessageDefinition* getMessages();

/* Public: Return an array of all CAN signals to be processed in the active
 *  * configuration.
 *   */
CanSignal* getSignals();

/* Public: Return an array of all OpenXC CAN commands enabled in the active
 *  * configuration that can write back to CAN with a custom handler.
 *   *
 *    * Commands not defined here are handled using a 1-1 mapping from the signals
 *     * list.
 *      */
CanCommand* getCommands();

/* Public: Return the length of the array returned by getCommandCount(). */
int getCommandCount();

/* Public: Return the length of the array returned by getSignals(). */
int getSignalCount();

/* Public: Return the length of the array returned by getMessages(). */
int getMessageCount();

/* Public: Return an array of the metadata for the 2 CAN buses you want to
 *  * monitor. The size of this array is fixed at 2.
 *   */
CanBus* getCanBuses();

/* Public: Decode CAN signals from raw CAN messages, translate from engineering
 *  * units to something more human readable, and send the resulting value over USB
 *   * as an OpenXC-style JSON message.
 *    *
 *     * This is the main workhorse function of the VI. Every time a new
 *      * CAN message is received that matches one of the signals in the list returend
 *       * by getSignals(), this function is called with the message ID and 64-bit data
 *        * field.
 *         *
 *          * bus - The CAN bus this message was received on.
 *           * message - The received CAN message.
 *            */
void decodeCanMessage(openxc::pipeline::Pipeline* pipeline, CanBus* bus, CanMessage* message);

