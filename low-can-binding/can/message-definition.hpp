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
 * @class message_definition_t
 *
 * @brief The definition of a CAN message. This includes a lot of metadata, so
 * to save memory this struct should not be used for storing incoming and
 * outgoing CAN messages.
 */

#pragma once

#include <vector>
#include <memory>

#include "signals.hpp"
#include "message-set.hpp"
#include "../utils/timer.hpp"
#include "message/message.hpp"

class message_set_t;

/// @brief The definition of a CAN message. This includes a lot of metadata, so
///  to save memory this class gets the signal_t object related to a CAN message.
class message_definition_t
{
private:
	std::shared_ptr<message_set_t> parent_; ///< parent_ - Pointer to the CAN message set holding this CAN message definition */
	std::string bus_; ///< bus_ - Address of CAN bus device. */
	uint32_t id_; ///< id_ - The ID or the PGN (if j1939) of the message.*/
	std::string name_; ///< name_ - J1939 PGN name
	uint32_t length_; ///< length_ - Message data length in bytes. For J1939 message, this is the expected data size
	uint32_t flags_; ///< format_ - the format of the message's ID.*/
	frequency_clock_t frequency_clock_; ///<  clock_ - an optional frequency clock to control the output of this
							///      message, if sent raw, or simply to mark the max frequency for custom
							///      handlers to retrieve.*/
	bool force_send_changed_; ///< force_send_changed_ - If true, regardless of the frequency, it will send CAN
							///	message if it has changed when using raw passthrough.*/
	std::vector<uint8_t> last_value_; ///< last_value_ - The last received value of the message. Defaults to undefined.
										///	This is required for the forceSendChanged functionality, as the stack
										///	needs to compare an incoming CAN message with the previous frame.*/
	std::vector<std::shared_ptr<signal_t> > signals_; ///< signals_ - Vector holding signal_t object which share the same arbitration ID */

public:
	//message_definition_t(const message_definition_t& b);
	message_definition_t(const std::string bus);
	message_definition_t(const std::string bus, uint32_t id, frequency_clock_t frequency_clock, bool force_send_changed);
	message_definition_t(const std::string bus, uint32_t id, uint32_t flags, frequency_clock_t frequency_clock, bool force_send_changed);
	message_definition_t(const std::string bus,
				 uint32_t id,
				 uint32_t flags,
				 frequency_clock_t frequency_clock,
				 bool force_send_changed,
				 const std::vector<std::shared_ptr<signal_t> >& signals);
	message_definition_t(const std::string bus,
				 uint32_t id,
				 std::string name,
				 uint32_t length,
				 uint32_t flags,
				 frequency_clock_t frequency_clock,
				 bool force_send_changed,
				 const std::vector<std::shared_ptr<signal_t> >& signals);


	const std::string get_bus_name() const;
	const std::string get_bus_device_name() const;
	uint32_t get_id() const;
	bool is_fd() const;
	bool is_j1939() const;
	std::vector<std::shared_ptr<signal_t>>& get_signals();
	uint32_t get_length() const;
	uint32_t get_flags() const;

	void set_parent(std::shared_ptr<message_set_t> parent);
	void set_last_value(std::shared_ptr<message_t>  m);
};
