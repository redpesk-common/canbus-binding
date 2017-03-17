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

#include "openxc.pb.h"
#include "can-signals.hpp"

///
/// @brief The type signature for a function to handle a custom OpenXC command.
///
/// @param[in] name - the name of the received command.
/// @param[in] value - the value of the received command, in a DynamicField. The actual type
///		may be a number, string or bool.
/// @param[in] event - an optional event from the received command, in a DynamicField. The
///		actual type may be a number, string or bool.
/// @param[in] signals - The list of all signals.
/// @param[in] signalCount - The length of the signals array.
///
typedef void (*CommandHandler)(const char* name, openxc_DynamicField* value,
		openxc_DynamicField* event, can_signal_t* signals, int signalCount);

/// @struct CanCommand
/// @brief The structure to represent a supported custom OpenXC command.
///
/// For completely customized CAN commands without a 1-1 mapping between an
/// OpenXC message from the host and a CAN signal, you can define the name of the
/// command and a custom function to handle it in the VI. An example is
/// the "turn_signal_status" command in OpenXC, which has a value of "left" or
/// "right". The vehicle may have separate CAN signals for the left and right
/// turn signals, so you will need to implement a custom command handler to send
/// the correct signals.
///
/// Command handlers are also useful if you want to trigger multiple CAN messages
/// or signals from a signal OpenXC message.
///
typedef struct {
	const char* generic_name; /*!< generic_name - The name of the command.*/
	CommandHandler handler; /*!< handler - An function to process the received command's data and perform some
 							 *	action.*/
} CanCommand;
