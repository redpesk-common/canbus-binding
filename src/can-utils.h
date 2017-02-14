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
#include "timer.h"
#include "openxc.pb.h"

// TODO actual max is 32 but dropped to 24 for memory considerations
#define MAX_ACCEPTANCE_FILTERS 24
// TODO this takes up a ton of memory
#define MAX_DYNAMIC_MESSAGE_COUNT 12

#define CAN_MESSAGE_SIZE 8

#define CAN_ACTIVE_TIMEOUT_S 30

#define QUEUE_DECLARE(type, max_length) \
static const int queue_##type##_max_length = max_length; \
static const int queue_##type##_max_internal_length = max_length + 1; \
typedef struct queue_##type##_s { \
	int head; \
	int tail; \
	type elements[max_length + 1]; \
} queue_##type; \
\
bool queue_##type##_push(queue_##type* queue, type value); \
\
type queue_##type##_pop(queue_##type* queue); \
\
type queue_##type##_peek(queue_##type* queue); \
void queue_##type##_init(queue_##type* queue); \
int queue_##type##_length(queue_##type* queue); \
int queue_##type##_available(queue_##type* queue); \
bool queue_##type##_full(queue_##type* queue); \
bool queue_##type##_empty(queue_##type* queue); \
void queue_##type##_snapshot(queue_##type* queue, type* snapshot, int max);

/* Public: The type signature for a CAN signal decoder.
 *
 * A SignalDecoder transforms a raw floating point CAN signal into a number,
 * string or boolean.
 *
 * signal - The CAN signal that we are decoding.
 * signals - The list of all signals.
 * signalCount - The length of the signals array.
 * value - The CAN signal parsed from the message as a raw floating point
 *		value.
 * send - An output parameter. If the decoding failed or the CAN signal should
 *		not send for some other reason, this should be flipped to false.
 *
 * Returns a decoded value in an openxc_DynamicField struct.
 */
typedef openxc_DynamicField (*SignalDecoder)(struct CanSignal* signal,
		CanSignal* signals, int signalCount, float value, bool* send);

/* Public: The type signature for a CAN signal encoder.
 *
 * A SignalEncoder transforms a number, string or boolean into a raw floating
 * point value that fits in the CAN signal.
 *
 * signal - The CAN signal to encode. 
 * value - The dynamic field to encode.
 * send - An output parameter. If the encoding failed or the CAN signal should
 * not be encoded for some other reason, this will be flipped to false.
 */
typedef uint64_t (*SignalEncoder)(struct CanSignal* signal,
		openxc_DynamicField* value, bool* send);

/* 
 * CanBus represent a can device definition gotten from configuraiton file 
 */
class CanBus_c {
	private:
		afb_binding_interface *interface;

		/* Got from conf file */
		std::string deviceName;

		int socket;
		bool is_fdmode_on;
		struct sockaddr_can txAddress;

		std::thread th_reading;
		std::thread th_decoding;
		std::thread th_pushing;

		std::queue <CanMessage_c> can_message_q;
		std::queue <openxc_VehicleMessage> vehicle_message_q;

	public:
		int open();
		int close();

		void start_threads();
		int send_can_message(CanMessage_c can_msg);

		CanMessage_c* next_can_message();
		void insert_new_can_message(CanMessage_c *can_msg);

		openxc_VehicleMessage* next_vehicle_message();
		void insert_new_vehicle_message(openxc_VehicleMessage *v_msg);
};

/* A compact representation of a single CAN message, meant to be used in in/out
 * buffers.
 *
 * id - The ID of the message.
 * format - the format of the message's ID.
 * data  - The message's data field.
 * length - the length of the data array (max 8).
struct CanMessage {
	uint32_t id;
	CanMessageFormat format;
	uint8_t data[CAN_MESSAGE_SIZE];
	uint8_t length;
};
typedef struct CanMessage CanMessage;
*/
class CanMessage_c {
	private:
		uint32_t id;
		CanMessageFormat format;
		uint8_t data[CAN_MESSAGE_SIZE];
		uint8_t length;

	public:
		uint32_t get_id();
		int get_format();
		uint8_t get_data();
		uint8_t get_lenght();

		void set_id(uint32_t id);
		void set_format(CanMessageFormat format);
		void set_data(uint8_t data);
		void set_lenght(uint8_t length);

		void convert_from_canfd_frame(canfd_frame frame);
		canfd_frame convert_to_canfd_frame();
};

QUEUE_DECLARE(CanMessage_c, 8);

/* Public: The ID format for a CAN message.
 *
 * STANDARD - standard 11-bit CAN arbitration ID.
 * EXTENDED - an extended frame, with a 29-bit arbitration ID.
 */
enum CanMessageFormat {
	STANDARD,
	EXTENDED,
};
typedef enum CanMessageFormat CanMessageFormat;

/* Public: A state encoded (SED) signal's mapping from numerical values to
 * OpenXC state names.
 *
 * value - The integer value of the state on the CAN bus.
 * name  - The corresponding string name for the state in OpenXC.
 */
struct CanSignalState {
	const int value;
	const char* name;
};
typedef struct CanSignalState CanSignalState;

/* Public: A CAN signal to decode from the bus and output over USB.
 *
 * message	   - The message this signal is a part of.
 * genericName - The name of the signal to be output over USB.
 * bitPosition - The starting bit of the signal in its CAN message (assuming
 *				 non-inverted bit numbering, i.e. the most significant bit of
 *				 each byte is 0)
 * bitSize	   - The width of the bit field in the CAN message.
 * factor	   - The final value will be multiplied by this factor. Use 1 if you
 *				 don't need a factor.
 * offset	   - The final value will be added to this offset. Use 0 if you
 *				 don't need an offset.
 * minValue    - The minimum value for the processed signal.
 * maxValue    - The maximum value for the processed signal.
 * frequencyClock - A FrequencyClock struct to control the maximum frequency to
 *				process and send this signal. To process every value, set the
 *				clock's frequency to 0.
 * sendSame    - If true, will re-send even if the value hasn't changed.
 * forceSendChanged - If true, regardless of the frequency, it will send the
 *				value if it has changed.
 * states	   - An array of CanSignalState describing the mapping
 *				 between numerical and string values for valid states.
 * stateCount  - The length of the states array.
 * writable    - True if the signal is allowed to be written from the USB host
 *				 back to CAN. Defaults to false.
 * decoder	   - An optional function to decode a signal from the bus to a human
 *		readable value. If NULL, the default numerical decoder is used.
 * encoder	   - An optional function to encode a signal value to be written to
 *				  CAN into a byte array. If NULL, the default numerical encoder
 *				  is used.
 * received    - True if this signal has ever been received.
 * lastValue   - The last received value of the signal. If 'received' is false,
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

	struct afb_event event;
};
typedef struct CanSignal CanSignal;

/* Public: The definition of a CAN message. This includes a lot of metadata, so
 * to save memory this struct should not be used for storing incoming and
 * outgoing CAN messages.
 *
 * bus - A pointer to the bus this message is on.
 * id - The ID of the message.
 * format - the format of the message's ID.
 * clock - an optional frequency clock to control the output of this
 *		message, if sent raw, or simply to mark the max frequency for custom
 *		handlers to retrieve.
 * forceSendChanged - If true, regardless of the frequency, it will send CAN
 *		message if it has changed when using raw passthrough.
 * lastValue - The last received value of the message. Defaults to undefined.
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

/* Private: An entry in the list of acceptance filters for each CanBus.
 *
 * This struct is meant to be used with a LIST type from <sys/queue.h>.
 *
 * filter - the value for the CAN acceptance filter.
 * activeUserCount - The number of active consumers of this filter's messages.
 *		When 0, this filter can be removed.
 * format - the format of the ID for the filter.
struct AcceptanceFilterListEntry {
	uint32_t filter;
	uint8_t activeUserCount;
	CanMessageFormat format;
	LIST_ENTRY(AcceptanceFilterListEntry) entries;
};
 */

/* Private: A type of list containing CAN acceptance filters.
LIST_HEAD(AcceptanceFilterList, AcceptanceFilterListEntry);

struct CanMessageDefinitionListEntry {
	CanMessageDefinition definition;
	LIST_ENTRY(CanMessageDefinitionListEntry) entries;
};
LIST_HEAD(CanMessageDefinitionList, CanMessageDefinitionListEntry);
 */

/** Public: A parent wrapper for a particular set of CAN messages and associated
 *	CAN buses(e.g. a vehicle or program).
 *
 *	index - A numerical ID for the message set, ideally the index in an array
 *		for fast lookup
 *	name - The name of the message set.
 *	busCount - The number of CAN buses defined for this message set.
 *	messageCount - The number of CAN messages (across all buses) defined for
 *		this message set.
 *	signalCount - The number of CAN signals (across all messages) defined for
 *		this message set.
 *	commandCount - The number of CanCommmands defined for this message set.
 */
 typedef struct {
	uint8_t index;
	const char* name;
	uint8_t busCount;
	unsigned short messageCount;
	unsigned short signalCount;
	unsigned short commandCount;
} CanMessageSet;

/* Public: The type signature for a function to handle a custom OpenXC command.
 *
 * name - the name of the received command.
 * value - the value of the received command, in a DynamicField. The actual type
 *		may be a number, string or bool.
 * event - an optional event from the received command, in a DynamicField. The
 *		actual type may be a number, string or bool.
 * signals - The list of all signals.
 * signalCount - The length of the signals array.
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
typedef struct {
	const char* genericName;
	CommandHandler handler;
} CanCommand;
 */

class CanCommand_c {
	private:
		const char* genericName;
		CommandHandler handler;
};

/* Pre initialize actions made before CAN bus initialization
 *
 * bus - A CanBus struct defining the bus's metadata
 * writable - configure the controller in a writable mode. If false, it will be
 *		configured as "listen only" and will not allow writes or even CAN ACKs.
 * buses - An array of all CAN buses.
 * busCount - The length of the buses array.
 */
void pre_initialize(CanBus* bus, bool writable, CanBus* buses, const int busCount);

/* Post-initialize actions made after CAN bus initialization and before the
 * event loop connection.
 *
 * bus - A CanBus struct defining the bus's metadata
 * writable - configure the controller in a writable mode. If false, it will be
 *		configured as "listen only" and will not allow writes or even CAN ACKs.
 * buses - An array of all CAN buses.
 * busCount - The length of the buses array.
 */
void post_initialize(CanBus* bus, bool writable, CanBus* buses, const int busCount);

/* Public: Check if the device is connected to an active CAN bus, i.e. it's
 * received a message in the recent past.
 *
 * Returns true if a message was received on the CAN bus within
 * CAN_ACTIVE_TIMEOUT_S seconds.
 */
bool isBusActive(CanBus* bus);

/* Public: Log transfer statistics about all active CAN buses to the debug log.
 *
 * buses - an array of active CAN buses.
 * busCount - the length of the buses array.
 */
void logBusStatistics(CanBus* buses, const int busCount);
