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

#include <vector>
#include <string>
#include <cstdint>
#include <linux/can.h>

#include "utils/timer.hpp"

#define CAN_MESSAGE_SIZE 8

class can_bus_dev_t;

/**
 * @enum can_message_format_t
 * @brief The ID format for a CAN message.
 */
enum class can_message_format_t {
	STANDARD, /*!< STANDARD - standard 11-bit CAN arbitration ID. */
	EXTENDED, /*!< EXTENDED - an extended frame, with a 29-bit arbitration ID. */
	ERROR,    /*!< ERROR - ERROR code used at initialization to signify that it isn't usable'*/
};

/**
 * @class can_message_t
 *
 * @brief A compact representation of a single CAN message, meant to be used in in/out
 * buffers.
 */
class can_message_t {
	private:
		uint32_t id_; /*!< id_ - The ID of the message. */
		bool rtr_flag_; /*!< rtr_flag_ - Telling if the frame has RTR flag positionned. Then frame hasn't data field*/
		uint8_t length_; /*!< length_ - the length of the data array (max 8). */
		uint8_t flags_; /*!< flags_ - flags of a CAN FD frame. Needed if we catch FD frames.*/
		can_message_format_t format_; /*!< format_ - the format of the message's ID.*/
		std::vector<uint8_t> data_; /*!< data_ - The message's data field with a size of 8 which is the standard about CAN bus messages.*/

		uint8_t maxdlen_; /*!< maxdlen_ - Max data length deduce from number of bytes read from the socket.*/

	public:
		can_message_t();

		uint32_t get_id() const;
		bool get_rtr_flag_() const;
		can_message_format_t get_format() const;
		uint8_t get_flags() const;
		const uint8_t* get_data() const;
		uint8_t get_length() const;

		void set_format(const can_message_format_t new_format);

		bool is_correct_to_send();

    static can_message_t convert_to_canfd_frame(const struct canfd_frame& frame, size_t nbytes);
		canfd_frame convert_to_canfd_frame();
};

/**
 * @class can_message_definition_t
 *
 * @brief The definition of a CAN message. This includes a lot of metadata, so
 * to save memory this struct should not be used for storing incoming and
 * outgoing CAN messages.
 */
class can_message_definition_t
{
	private:
	can_bus_dev_t& bus_; /*!< bus_ - A pointer to the bus this message is on. */
	uint32_t id_; /*!< id_ - The ID of the message.*/
	can_message_format_t format_; /*!< format_ - the format of the message's ID.*/
	FrequencyClock clock_; /*!<  clock_ - an optional frequency clock to control the output of this
							*      message, if sent raw, or simply to mark the max frequency for custom
							*      handlers to retrieve.*/
	bool force_send_changed_; /*!< force_send_changed_ - If true, regardless of the frequency, it will send CAN
 							 *	message if it has changed when using raw passthrough.*/
	std::vector<uint8_t> last_value_; /*!< last_value_ - The last received value of the message. Defaults to undefined.
 										  *	This is required for the forceSendChanged functionality, as the stack
 										  *	needs to compare an incoming CAN message with the previous frame.*/
	
	public:
	  can_message_definition_t();
		uint32_t get_id() const;
};

/**
 * @class can_message_set_t
 *
 * @brief A parent wrapper for a particular set of CAN messages and associated
 *	CAN buses(e.g. a vehicle or program).
 */
class can_message_set_t
{
	private:
		uint8_t index_; /*!<index_ - A numerical ID for the message set, ideally the index in an array
						*	for fast lookup*/
		const std::string name_; /*!< name_ - The name of the message set.*/
		uint8_t can_bus_count_; /*!< can_bus_count_ - The number of CAN buses defined for this message set.*/
		unsigned short can_message_count_; /*!< can_message_count_ - The number of CAN messages (across all buses) defined for
										*	this message set.*/
		unsigned short can_signal_count_; /*!< can_signal_count_ - The number of CAN signals (across all messages) defined for
									*	this message set.*/
		unsigned short can_command_count_; /*!< can_command_count_ - The number of CanCommmands defined for this message set.*/
		unsigned short obd2_signal_count_; /*!< obd2_signal_count_ - The number of obd2 signals defined for this message set.*/
};

/** Public: Retrive a list of all possible CAN configurations.
 *
 * Returns a pointer to an array of all configurations.
 */
can_message_set_t* getMessageSets();

/** Public: Return the length of the array returned by getMessageSets() */
int getMessageSetCount();

/* Public: Return an array of all CAN messages to be processed in the active
 * configuration.
 */
can_message_definition_t* getMessages();

/* Public: Return the length of the array returned by getMessages(). */
int getMessageCount();