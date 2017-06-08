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

/**
 * @class can_message_definition_t
 *
 * @brief The definition of a CAN message. This includes a lot of metadata, so
 * to save memory this struct should not be used for storing incoming and
 * outgoing CAN messages.
 */

#pragma once

#include <vector>
#include <memory>

#include "can-signals.hpp"
#include "can-message.hpp"
#include "can-message-set.hpp"
#include "../utils/timer.hpp"

class can_message_set_t;

/// @brief The definition of a CAN message. This includes a lot of metadata, so
///  to save memory this class gets the can_signal_t object related to a CAN message.
class can_message_definition_t
{
private:
	can_message_set_t* parent_; ///< parent_ - Pointer to the CAN message set holding this CAN message definition */
	std::string bus_; ///< bus_ - Address of CAN bus device. */
	uint32_t id_; ///< id_ - The ID of the message.*/
	can_message_format_t format_; ///< format_ - the format of the message's ID.*/
	frequency_clock_t frequency_clock_; ///<  clock_ - an optional frequency clock to control the output of this
							///      message, if sent raw, or simply to mark the max frequency for custom
							///      handlers to retrieve.*/
	bool force_send_changed_; ///< force_send_changed_ - If true, regardless of the frequency, it will send CAN
							///	message if it has changed when using raw passthrough.*/
	std::vector<uint8_t> last_value_; ///< last_value_ - The last received value of the message. Defaults to undefined.
										///	This is required for the forceSendChanged functionality, as the stack
										///	needs to compare an incoming CAN message with the previous frame.*/
	std::vector<std::shared_ptr<can_signal_t> > can_signals_; ///< can_signals_ - Vector holding can_signal_t object which share the same arbitration ID */

public:
	//can_message_definition_t(const can_message_definition_t& b);
	can_message_definition_t(const std::string bus);
	can_message_definition_t(const std::string bus, uint32_t id, frequency_clock_t frequency_clock, bool force_send_changed);
	can_message_definition_t(const std::string bus, uint32_t id, can_message_format_t format, frequency_clock_t frequency_clock, bool force_send_changed);
	can_message_definition_t(const std::string bus, uint32_t id, can_message_format_t format, frequency_clock_t frequency_clock, bool force_send_changed, const std::vector<std::shared_ptr<can_signal_t> >& can_signals);

	const std::string get_bus_name() const;
	const std::string get_bus_device_name() const;
	uint32_t get_id() const;
	std::vector<std::shared_ptr<can_signal_t> >& get_can_signals();

	void set_parent(can_message_set_t* parent);
	void set_last_value(const can_message_t& cm);
};
