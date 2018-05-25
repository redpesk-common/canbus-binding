/*
 * Copyright (C) 2015, 2016, 2017 "IoT.bzh"
 * Author "Romain Forlot" <romain.forlot@iot.bzh>
 * Author "Loïc Collignon" <loic.collignon@iot.bzh>
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

#include <utility>
#include <mutex>
#include <queue>
#include <thread>
#include <linux/can.h>
#include <condition_variable>

#include "openxc.pb.h"
#include "can-message.hpp"
#include "../utils/config-parser.hpp"
#include "../binding/low-can-subscription.hpp"

/// @brief A container for a CAN module paried with a certain bus.
///
/// There are three things that control the operating mode of the CAN controller:
///
/// - Should arbitrary CAN message writes be allowed? See rawWritable.
/// - Should translated, simple vehicle message writes be allowed? See the
///   'writable' field in signals defined for this bus.
///
class can_bus_t
{
private:
	int index;
	std::string name;
	std::string device_name;
	float max_message_frequency;	//<! maxMessageFrequency - the default maximum frequency for all CAN messages when
									/// using the raw passthrough mode. To disable frequency limit, set
									/// this to 0.
	bool raw_writable;  //<! rawWritable - Set to True if the CAN bus connection should allow raw CAN messages
						/// writes. This is independent from the CanSignal 'writable' option, which
						/// can also be set to allow translated writes back to this bus.
	bool passthrough_can_messages; //<! passthroughCanMessages - Set to True if low-level CAN messages should be able to send to the
								   /// output interface, not just signals as simple vehicle messages.

public:
	int get_index() const;
	const std::string get_bus_name() const;
	const std::string get_bus_device_name() const;
	const float max_message_frequency() const;
	bool get_raw_writable() const;
	bool get_passthrough_can_messages() const;

};
