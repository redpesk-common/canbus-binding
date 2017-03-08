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

#include "can/can-bus.hpp"
#include "utils/timer.hpp"

#define CAN_MESSAGE_SIZE 8

/**
 * @enum CanMessageFormat
 * @brief The ID format for a CAN message.
 */
enum CanMessageFormat {
	STANDARD, /*!< STANDARD - standard 11-bit CAN arbitration ID. */
	EXTENDED, /*!< EXTENDED - an extended frame, with a 29-bit arbitration ID. */
	ERROR,    /*!< ERROR - ERROR code used at initialization to signify that it isn't usable'*/
};
typedef enum CanMessageFormat CanMessageFormat;

/**
 * @class can_message_t
 *
 * @brief A compact representation of a single CAN message, meant to be used in in/out
 * buffers.
 */

/*************************
 * old CanMessage struct *
 *************************
struct CanMessage {
	uint32_t id;
	CanMessageFormat format;
	uint8_t data[CAN_MESSAGE_SIZE];
	uint8_t length;
};
typedef struct CanMessage CanMessage;
*/
class can_message_t {
	private:
		uint32_t id_; /*!< id_ - The ID of the message. */
		bool rtr_flag_; /*!< rtr_flag_ - Telling if the frame has RTR flag positionned. Then frame hasn't data field*/
		uint8_t length_; /*!< length_ - the length of the data array (max 8). */
		uint8_t flags_; /*!< flags_ - flags of a CAN FD frame. Needed if we catch FD frames.*/
		CanMessageFormat format_; /*!< format_ - the format of the message's ID.*/
		std::vector<uint8_t> data_; /*!< data_ - The message's data field with a size of 8 which is the standard about CAN bus messages.*/

		uint8_t maxdlen_; /*!< maxdlen_ - Max data length deduce from number of bytes read from the socket.*/

	public:
		can_message_t();

		uint32_t get_id() const;
		bool get_rtr_flag_() const;
		int get_format() const;
		uint8_t get_flags() const;
		const uint8_t* get_data() const;
		uint8_t get_length() const;

		void set_max_data_length(size_t nbytes);
		void set_id_and_format(const uint32_t new_id);
		void set_format(const CanMessageFormat new_format);
		void set_format(const uint32_t can_id);
		void set_flags(const uint8_t flags);
		void set_data(const __u8* new_data);
		void set_length(const uint8_t new_length);

		bool is_correct_to_send();

		void convert_from_canfd_frame(const std::pair<struct canfd_frame&, size_t>args);
		canfd_frame convert_to_canfd_frame();
};

/**
 * @struct CanMessageDefinition
 *
 * @brief The definition of a CAN message. This includes a lot of metadata, so
 * to save memory this struct should not be used for storing incoming and
 * outgoing CAN messages.
 */
struct CanMessageDefinition {
	//can_bus_dev_t bus; /*!< bus - A pointer to the bus this message is on. */
	uint32_t id; /*!<  id - The ID of the message.*/
	CanMessageFormat format; /*!< format - the format of the message's ID.*/
	FrequencyClock frequencyClock; /*!<  clock - an optional frequency clock to control the output of this
 									*	message, if sent raw, or simply to mark the max frequency for custom
 									*	handlers to retriec++ if ? syntaxve.*/
	bool forceSendChanged; /*!< forceSendChanged - If true, regardless of the frequency, it will send CAN
 							*	message if it has changed when using raw passthrough.*/
	uint8_t lastValue[CAN_MESSAGE_SIZE]; /*!< lastValue - The last received value of the message. Defaults to undefined.
 										*	This is required for the forceSendChanged functionality, as the stack
 										*	needs to compare an incoming CAN message with the previous frame.*/
};
typedef struct CanMessageDefinition CanMessageDefinition;

class can_message_definition_t
{
	private:
	can_bus_dev_t& bus_; /*!< bus_ - A pointer to the bus this message is on. */
	uint32_t id_; /*!< id_ - The ID of the message.*/
	CanMessageFormat format_; /*!< format_ - the format of the message's ID.*/
	FrequencyClock clock_; /*!<  clock_ - an optional frequency clock to control the output of this
 							*	message, if sent raw, or simply to mark the max frequency for custom
 							*	handlers to retriec++ if ? syntaxve.*/
	bool force_send_changed_; /*!< force_send_changed_ - If true, regardless of the frequency, it will send CAN
 							 *	message if it has changed when using raw passthrough.*/
	uint8_t last_value_[CAN_MESSAGE_SIZE]; /*!< last_value_ - The last received value of the message. Defaults to undefined.
 										  *	This is required for the forceSendChanged functionality, as the stack
 										  *	needs to compare an incoming CAN message with the previous frame.*/
	
	public:
		uint32_t get_id();
};

/**
 * @struct CanMessageSet
 *
 * @brief A parent wrapper for a particular set of CAN messages and associated
 *	CAN buses(e.g. a vehicle or program).
 */
 typedef struct {
	uint8_t index; /*!<index - A numerical ID for the message set, ideally the index in an array
 					*	for fast lookup*/
	const std::string name; /*!< name - The name of the message set.*/
	uint8_t busCount; /*!< busCount - The number of CAN buses defined for this message set.*/
	unsigned short messageCount; /*!< messageCount - The number of CAN messages (across all buses) defined for
 									*	this message set.*/
	unsigned short signalCount; /*!< signalCount - The number of CAN signals (across all messages) defined for
 								*	this message set.*/
	unsigned short commandCount; /*!< commandCount - The number of CanCommmands defined for this message set.*/
} CanMessageSet;

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

/** Public: Return the currently active CAN configuration. */
CanMessageSet* getActiveMessageSet();

/** Public: Retrive a list of all possible CAN configurations.
 *
 * Returns a pointer to an array of all configurations.
 */
CanMessageSet* getMessageSets();

/** Public: Return the length of the array returned by getMessageSets() */
int getMessageSetCount();

/* Public: Return an array of all CAN messages to be processed in the active
 * configuration.
 */
CanMessageDefinition* getMessages();

/* Public: Return the length of the array returned by getMessages(). */
int getMessageCount();