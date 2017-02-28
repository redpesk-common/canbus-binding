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

#include <string>
#include <cstdint>
#include <linux/can.h>

#include "timer.hpp"

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
		uint32_t id_; /*!< uint32_t id - The ID of the message. */
		uint8_t length_; /*!<  uint8_t length - the length of the data array (max 8). */
		CanMessageFormat format_; /*!< CanMessageFormat format - the format of the message's ID.*/
		uint8_t data_[CAN_MESSAGE_SIZE]; /*!< uint8_t data  - The message's data field with a size of 8 which is the standard about CAN bus messages.*/

	public:
		/**
		 * @brief Class constructor
		 *
		 * Constructor about can_message_t class.
		 */
		can_message_t();

		/**
		 * @brief Retrieve id_ member value.
		 *
		 * @return uint32_t id_ class member
		 */
		uint32_t get_id() const;
		
		/**
		 * @brief Retrieve format_ member value.
		 *
		 * @return CanMessageFormat format_ class member
		 */
		int get_format() const;
		
		/**
		 * @brief Retrieve data_ member value.
		 *
		 * @return uint8_t data_ pointer class member
		 */
		const uint8_t* get_data() const;
		
		/**
		 * @brief Retrieve length_ member value.
		 *
		 * @return uint8_t length_ class member
		 */
		uint8_t get_length() const;

		/**
		 * @brief Control whether the object is correctly initialized
		 *  to be sent over the CAN bus
		 *
		 * @return true if object correctly initialized and false if not...
		 */
		bool is_correct_to_send();
		
		/**
		 * @brief Set id_ member value.
		 *
		 * Preferred way to initialize these members by using 
		 * convert_from_canfd_frame method.
		 *
		 * @param uint32_t id_ class member
		 */
		void set_id(const uint32_t new_id);
		
		/**
		 * @brief Set format_ member value.
		 *
		 * Preferred way to initialize these members by using 
		 * convert_from_canfd_frame method.
		 *
		 * @param CanMessageFormat format_ class member
		 */
		void set_format(const CanMessageFormat format);
		
		/**
		 * @brief Set data_ member value.
		 *
		 * Preferred way to initialize these members by using 
		 * convert_from_canfd_frame method.
		 *
		 * @param uint8_t data_ array with a max size of 8 elements.
		 */
		void set_data(const uint8_t new_data);
		
		/**
		 * @brief Set length_ member value.
		 *
		 * Preferred way to initialize these members by using 
		 * convert_from_canfd_frame method.
		 *
		 * @param uint8_t length_ array with a max size of 8 elements.
		 */
		void set_length(const uint8_t new_length);

		/**
		 * @brief Take a canfd_frame struct to initialize class members
		 *
		 * This is the preferred way to initialize class members.
		 *
		 * @param canfd_frame struct read from can bus device.
		 */
		void convert_from_canfd_frame(const canfd_frame& frame);
		
		/**
		 * @brief Take all initialized class's members and build an 
		 * canfd_frame struct that can be use to send a CAN message over
		 * the bus.
		 *
		 * @return canfd_frame struct built from class members.
		 */
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