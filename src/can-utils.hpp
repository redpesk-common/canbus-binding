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

#include <map>
#include <queue>
#include <vector>
#include <cstdio>
#include <string>
#include <thread>
#include <fcntl.h>
#include <unistd.h>
#include <net/if.h>
#include <sys/ioctl.h>
#include <linux/can.h>
#include <sys/socket.h>
#include <json-c/json.h>
#include <linux/can/raw.h>

#include "timer.hpp"
#include "openxc.pb.h"

extern "C"
{
	#include <afb/afb-binding.h>
	#include <afb/afb-service-itf.h>
}

// TODO actual max is 32 but dropped to 24 for memory considerations
#define MAX_ACCEPTANCE_FILTERS 24
// TODO this takes up a ton of memory
#define MAX_DYNAMIC_MESSAGE_COUNT 12

#define CAN_MESSAGE_SIZE 8

#define CAN_ACTIVE_TIMEOUT_S 30

/**
 * @brief The type signature for a CAN signal decoder.
 *
 * @desc A SignalDecoder transforms a raw floating point CAN signal into a number,
 * string or boolean.
 *
 * @param[in] CanSignal signal - The CAN signal that we are decoding.
 * @param[in] CanSignal signals - The list of all signals.
 * @param[in] int signalCount - The length of the signals array.
 * @param[in] float value - The CAN signal parsed from the message as a raw floating point
 *	value.
 * @param[out] bool send - An output parameter. If the decoding failed or the CAN signal should
 *	not send for some other reason, this should be flipped to false.
 *
 * @return a decoded value in an openxc_DynamicField struct.
 */
typedef openxc_DynamicField (*SignalDecoder)(struct CanSignal* signal,
		CanSignal* signals, int signalCount, float value, bool* send);

/**
 * @brief: The type signature for a CAN signal encoder.
 *
 * @desc A SignalEncoder transforms a number, string or boolean into a raw floating
 * point value that fits in the CAN signal.
 *
 * @params[signal] - The CAN signal to encode. 
 * @params[value] - The dynamic field to encode.
 * @params[send] - An output parameter. If the encoding failed or the CAN signal should
 * not be encoded for some other reason, this will be flipped to false.
 */
typedef uint64_t (*SignalEncoder)(struct CanSignal* signal,
		openxc_DynamicField* value, bool* send);

/**
 * @brief The ID format for a CAN message.
 *
 * STANDARD - standard 11-bit CAN arbitration ID.
 * EXTENDED - an extended frame, with a 29-bit arbitration ID.
 */
enum CanMessageFormat {
	STANDARD,
	EXTENDED,
};
typedef enum CanMessageFormat CanMessageFormat;

/**
 * @brief A compact representation of a single CAN message, meant to be used in in/out
 * buffers.
 *
 * param[in] uint32_t id - The ID of the message.
 * param[in] CanMessageFormat format - the format of the message's ID.
 * param[in] uint8_t data  - The message's data field.
 * @param[in] uint8_t length - the length of the data array (max 8).
*************************
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
		const struct afb_binding_interface* interface_;
		uint32_t id_;
		CanMessageFormat format_;
		uint8_t data_;
		uint8_t length_;

	public:
		can_message_t(const struct afb_binding_interface* interface);

		uint32_t get_id() const;
		int get_format() const;
		const uint8_t* get_data() const;
		uint8_t get_length() const;

		void set_id(const uint32_t new_id);
		void set_format(const CanMessageFormat format);
		void set_data(const uint8_t new_data);
		void set_length(const uint8_t new_length);

		void convert_from_canfd_frame(const canfd_frame& frame);
		canfd_frame convert_to_canfd_frame();
};

/** 
 * @brief Object representing a can device. Handle opening, closing and reading on the
 * socket. This is the low level object to be use by can_bus_t.
 *
 * @params[in] std::string device_name_ - name of the linux device handling the can bus. Generally vcan0, can0, etc.
 */
class can_bus_dev_t {
	private:
		std::string device_name_;
		int can_socket_;
		bool is_fdmode_on_;
		struct sockaddr_can txAddress_;

		bool has_can_message_;
		std::queue <can_message_t> can_message_q_;

		std::thread th_reading_;
		bool is_running_;

	public:
		can_bus_dev_t(const std::string& dev_name);

		int open(const struct afb_binding_interface* interface);
		int close();
		bool is_running();
		void start_reading();
		canfd_frame read(const struct afb_binding_interface *interface);
		
		can_message_t next_can_message(const struct afb_binding_interface* interface);
		void push_new_can_message(const can_message_t& can_msg);		
		bool has_can_message() const;

		int send_can_message(can_message_t& can_msg, const struct afb_binding_interface* interface);
};

/** 
 * @brief Object used to handle decoding and manage event queue to be pushed.
 *
 * @params[in] interface_ - afb_binding_interface pointer to the binder. Used to log messages
 * @params[in] conf_file_ - configuration file handle used to initialize can_bus_dev_t objects.
 */
class can_bus_t {
	private:
		const struct afb_binding_interface *interface_;
		int conf_file_;
		
		std::thread th_decoding_;
		std::thread th_pushing_;

		bool has_vehicle_message_;
		std::queue <openxc_VehicleMessage> vehicle_message_q_;

	public:
		can_bus_t(const struct afb_binding_interface *itf, int& conf_file);
		int init_can_dev();
		std::vector<std::string> read_conf();
		
		void start_threads();
		
		openxc_VehicleMessage next_vehicle_message();
		void push_new_vehicle_message(const openxc_VehicleMessage& v_msg);
		bool has_vehicle_message() const;
};

/**
 * @brief A state encoded (SED) signal's mapping from numerical values to
 * OpenXC state names.
 *
 * @param[in] in value - The integer value of the state on the CAN bus.
 * @param[in] char* name  - The corresponding string name for the state in OpenXC.
 */
struct CanSignalState {
	const int value;
	const char* name;
};
typedef struct CanSignalState CanSignalState;

/**
 * @brief A CAN signal to decode from the bus and output over USB.
 *
 * @param[in] message	   - The message this signal is a part of.
 * @param[in] genericName - The name of the signal to be output over USB.
 * @param[in] bitPosition - The starting bit of the signal in its CAN message (assuming
 *				 non-inverted bit numbering, i.e. the most significant bit of
 *				 each byte is 0)
 * @param[in] bitSize	   - The width of the bit field in the CAN message.
 * @param[in] factor	   - The final value will be multiplied by this factor. Use 1 if you
 *				 don't need a factor.
 * @param[in] offset	   - The final value will be added to this offset. Use 0 if you
 *				 don't need an offset.
 * @param[in] minValue    - The minimum value for the processed signal.
 * @param[in] maxValue    - The maximum value for the processed signal.
 * @param[in] frequencyClock - A FrequencyClock struct to control the maximum frequency to
 *				process and send this signal. To process every value, set the
 *				clock's frequency to 0.
 * @param[in] sendSame    - If true, will re-send even if the value hasn't changed.
 * @param[in] forceSendChanged - If true, regardless of the frequency, it will send the
 *				value if it has changed.
 * @param[in] states	   - An array of CanSignalState describing the mapping
 *				 between numerical and string values for valid states.
 * @param[in] stateCount  - The length of the states array.
 * @param[in] writable    - True if the signal is allowed to be written from the USB host
 *				 back to CAN. Defaults to false.
 * @param[in] decoder	   - An optional function to decode a signal from the bus to a human
 *				readable value. If NULL, the default numerical decoder is used.
 * @param[in] encoder	   - An optional function to encode a signal value to be written to
 *				  CAN into a byte array. If NULL, the default numerical encoder
 *				  is used.
 * @param[in] received    - True if this signal has ever been received.
 * @param[in] lastValue   - The last received value of the signal. If 'received' is false,
 *		this value is undefined.
 */
struct CanSignal {
	struct CanMessageDefinition* message;
	const char* genericName;
	uint8_t bitPosition;
	uint8_t bitSize;
	float factor;
	float offset;
	float minValue;
	float maxValue;
	FrequencyClock frequencyClock;
	bool sendSame;
	bool forceSendChanged;
	const CanSignalState* states;
	uint8_t stateCount;
	bool writable;
	SignalDecoder decoder;
	SignalEncoder encoder;
	bool received;
	float lastValue;
};
typedef struct CanSignal CanSignal;

/**
 * @brief The definition of a CAN message. This includes a lot of metadata, so
 * to save memory this struct should not be used for storing incoming and
 * outgoing CAN messages.
 *
 * @param[in] bus - A pointer to the bus this message is on.
 * @param[in] id - The ID of the message.
 * @param[in] format - the format of the message's ID.
 * @param[in] clock - an optional frequency clock to control the output of this
 *		message, if sent raw, or simply to mark the max frequency for custom
 *		handlers to retriec++ if ? syntaxve.
 * @param[in] forceSendChanged - If true, regardless of the frequency, it will send CAN
 *		message if it has changed when using raw passthrough.
 * @param[in] lastValue - The last received value of the message. Defaults to undefined.
 *		This is required for the forceSendChanged functionality, as the stack
 *		needs to compare an incoming CAN message with the previous frame.
 */
struct CanMessageDefinition {
	struct CanBus* bus;
	uint32_t id;
	CanMessageFormat format;
	FrequencyClock frequencyClock;
	bool forceSendChanged;
	uint8_t lastValue[CAN_MESSAGE_SIZE];
};
typedef struct CanMessageDefinition CanMessageDefinition;

/**
 * @brief A parent wrapper for a particular set of CAN messages and associated
 *	CAN buses(e.g. a vehicle or program).
 *
 *	@param[in] index - A numerical ID for the message set, ideally the index in an array
 *		for fast lookup
 *	@param[in] name - The name of the message set.
 *	@param[in] busCount - The number of CAN buses defined for this message set.
 *	@param[in] messageCount - The number of CAN messages (across all buses) defined for
 *		this message set.
 *	@param[in] signalCount - The number of CAN signals (across all messages) defined for
 *		this message set.
 *	@param[in] commandCount - The number of CanCommmands defined for this message set.
 */
 typedef struct {
	uint8_t index;
	const char* name;
	uint8_t busCount;
	unsigned short messageCount;
	unsigned short signalCount;
	unsigned short commandCount;
} CanMessageSet;

/**
 * @brief The type signature for a function to handle a custom OpenXC command.
 *
 * @param[in] char* name - the name of the received command.
 * @param[in] openxc_DynamicField* value - the value of the received command, in a DynamicField. The actual type
 *		may be a number, string or bool.
 * @param[in] openxc_DynamicField* event - an optional event from the received command, in a DynamicField. The
 *		actual type may be a number, string or bool.
 * @param[in] CanSignal* signals - The list of all signals.
 * @param[in] int signalCount - The length of the signals array.
 */
typedef void (*CommandHandler)(const char* name, openxc_DynamicField* value,
		openxc_DynamicField* event, CanSignal* signals, int signalCount);

/* Public: The structure to represent a supported custom OpenXC command.
 *
 * For completely customized CAN commands without a 1-1 mapping between an
 * OpenXC message from the host and a CAN signal, you can define the name of the
 * command and a custom function to handle it in the VI. An example is
 * the "turn_signal_status" command in OpenXC, which has a value of "left" or
 * "right". The vehicle may have separate CAN signals for the left and right
 * turn signals, so you will need to implement a custom command handler to send
 * the correct signals.
 *
 * Command handlers are also useful if you want to trigger multiple CAN messages
 * or signals from a signal OpenXC message.
 *
 * genericName - The name of the command.
 * handler - An function to process the received command's data and perform some
 *		action.
 */
typedef struct {
	const char* genericName;
	CommandHandler handler;
} CanCommand;

/**
 * @brief Pre initialize actions made before CAN bus initialization
 *
 * @param[in] can_bus_dev_t bus - A CanBus struct defining the bus's metadata
 * @param[in] bool writable - configure the controller in a writable mode. If false, it will be
 *		configured as "listen only" and will not allow writes or even CAN ACKs.
 * @param[in] buses - An array of all CAN buses.
 * @param[in] int busCount - The length of the buses array.
 */
void pre_initialize(can_bus_dev_t* bus, bool writable, can_bus_dev_t* buses, const int busCount);

/* Post-initialize actions made after CAN bus initialization and before the
 * event loop connection.
 *
 * bus - A CanBus struct defining the bus's metadata
 * writable - configure the controller in a writable mode. If false, it will be
 *		configured as "listen only" and will not allow writes or even CAN ACKs.
 * buses - An array of all CAN buses.
 * busCount - The length of the buses array.
 */
void post_initialize(can_bus_dev_t* bus, bool writable, can_bus_dev_t* buses, const int busCount);

/* Public: Check if the device is connected to an active CAN bus, i.e. it's
 * received a message in the recent past.
 *
 * Returns true if a message was received on the CAN bus within
 * CAN_ACTIVE_TIMEOUT_S seconds.
 */
bool isBusActive(can_bus_dev_t* bus);

/* Public: Log transfer statistics about all active CAN buses to the debug log.
 *
 * buses - an array of active CAN buses.
 * busCount - the length of the buses array.
 */
void logBusStatistics(can_bus_dev_t* buses, const int busCount);

/**
 * @brief Function representing thread activated by can bus objects
 */
void can_reader(can_bus_dev_t& can_bus);
void can_decode_message(can_bus_t& can_bus);
void can_event_push(can_bus_t& can_bus);