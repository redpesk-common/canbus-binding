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

#include <queue>
#include <thread>
#include <linux/can.h>

#include "timer.hpp"
#include "openxc.pb.h"
#include "low-can-binding.hpp"

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
typedef openxc_DynamicField (*SignalDecoder)(const struct CanSignal& signal,
		const CanSignal& signals, float value, bool* send);

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
 * @class can_bus_t
 * @brief Object used to handle decoding and manage event queue to be pushed.
 *
 * This object is also used to initialize can_bus_dev_t object after reading 
 * json conf file describing the CAN devices to use. Thus, those object will read 
 * on the device the CAN frame and push them into the can_bus_t can_message_q_ queue.
 *
 * That queue will be later used to be decoded and pushed to subscribers.
 */
class can_bus_t {
	private:
		int conf_file_; /*!< conf_file_ - configuration file handle used to initialize can_bus_dev_t objects.*/
		
		std::thread th_decoding_; /*!< thread that'll handle decoding a can frame */
		bool is_decoding_; /*!< boolean member controling thread while loop*/
		std::thread th_pushing_; /*!<  thread that'll handle pushing decoded can frame to subscribers */
		bool is_pushing_; /*!< boolean member controling thread while loop*/

		bool has_can_message_; /*!< boolean members that control whether or not there is can_message into the queue */
		std::queue <can_message_t> can_message_q_; /*!< queue that'll store can_message_t to decoded */

		bool has_vehicle_message_; /*!< boolean members that control whether or not there is openxc_VehicleMessage into the queue */
		std::queue <openxc_VehicleMessage> vehicle_message_q_; /*!< queue that'll store openxc_VehicleMessage to pushed */

	public:
		/**
		 * @brief Class constructor
		 *
		 * @param struct afb_binding_interface *interface between daemon and binding
		 * @param int file handle to the json configuration file.
		 */
		can_bus_t(int& conf_file);
		
		/**
		 * @brief Will initialize can_bus_dev_t objects after reading 
		 * the configuration file passed in the constructor.
		 */
		int init_can_dev();
		
		/**
		 * @brief read the conf_file_ and will parse json objects
		 * in it searching for canbus objects devices name.
		 *
		 * @return Vector of can bus device name string.
		 */
		 std::vector<std::string> read_conf();
		
		/**
		 * @brief Will initialize threads that will decode
		 *  and push subscribed events.
		 */
		void start_threads();

		/**
		 * @brief Will stop all threads holded by can_bus_t object
		 *  which are decoding and pushing threads.
		 */
		void stop_threads();

		/**
		 * @brief Telling if the decoding thread is running.
		 *  This is the boolean value on which the while loop
		 *  take its condition. Set it to false will stop the 
		 *  according thread.
		 *
		 * @return true if decoding thread is running, false if not.
		 */
		bool is_decoding();

		/**
		 * @brief Telling if the pushing thread is running
		 *  This is the boolean value on which the while loop
		 *  take its condition. Set it to false will stop the 
		 *  according thread.
		 *
		 * @return true if pushing thread is running, false if not.
		 */
		bool is_pushing();

		/**
		 * @brief Return first can_message_t on the queue 
		 *
		 * @return a can_message_t 
		 */
		can_message_t next_can_message();
		
		/**
		 * @brief Push a can_message_t into the queue
		 *
		 * @param the const reference can_message_t object to push into the queue
		 */
		void push_new_can_message(const can_message_t& can_msg);		
		
		/**
		 * @brief Return a boolean telling if there is any can_message into the queue
		 *
		 * @return true if there is at least a can_message_t, false if not.
		 */
		bool has_can_message() const;
		
		/**
		 * @brief Return first openxc_VehicleMessage on the queue 
		 *
		 * @return a openxc_VehicleMessage containing a decoded can message
		 */
		openxc_VehicleMessage next_vehicle_message();
		
		/**
		 * @brief Push a openxc_VehicleMessage into the queue
		 *
		 * @param the const reference openxc_VehicleMessage object to push into the queue
		 */
		void push_new_vehicle_message(const openxc_VehicleMessage& v_msg);
		
		/**
		 * @brief Return a boolean telling if there is any openxc_VehicleMessage into the queue
		 *
		 * @return true if there is at least a openxc_VehicleMessage, false if not.
		 */
		bool has_vehicle_message() const;
};

/**
 * @class can_bus_dev_t 
 *
 * @brief Object representing a can device. Handle opening, closing and reading on the
 *  socket. This is the low level object to be use by can_bus_t.
 */
class can_bus_dev_t {
	private:
		std::string device_name_; /*!< std::string device_name_ - name of the linux device handling the can bus. Generally vcan0, can0, etc. */
		int can_socket_; /*!< socket handler for the can device */
		bool is_fdmode_on_; /*!< boolean telling if whether or not the can socket use fdmode. */
		struct sockaddr_can txAddress_; /*!< internal member using to bind to the socket */

		std::thread th_reading_; /*!< Thread handling read the socket can device filling can_message_q_ queue of can_bus_t */
		bool is_running_; /*!< boolean telling whether or not reading is running or not */

	public:
		/**
		 * @brief Class constructor 
		 * 
		 * @param const string representing the device name into the linux /dev tree
		 */
		can_bus_dev_t(const std::string& dev_name);

		/**
		 * @brief Open the can socket and returning it 
		 *
		 * @return 
		 */
		int open();
		
		/**
		 * @brief Open the can socket and returning it 
		 *
		 * @return 
		 */
		int close();
		
		/**
		 * @brief Telling if the reading thread is running
		 *  This is the boolean value on which the while loop
		 *  take its condition. Set it to false will stop the 
		 *  according thread.
		 *
		 * @return true if reading thread is running, false if not.
		 */
		bool is_running();
		
		/**
 		* @brief start reading threads and set flag is_running_
		*
		* @param can_bus_t reference can_bus_t. it will be passed to the thread 
		*  to allow using can_bus_t queue.
 		*/
		void start_reading(can_bus_t& can_bus);

		/**
 		* @brief Read the can socket and retrieve canfd_frame
		*
		* @param const struct afb_binding_interface* interface pointer. Used to be able to log 
		*  using application framework logger.
 		*/
		canfd_frame read();
		
		/**
		* @brief Send a can message from a can_message_t object.
		* 
		* @param const can_message_t& can_msg: the can message object to send 
		* @param const struct afb_binding_interface* interface pointer. Used to be able to log 
		*  using application framework logger.
		*/
		int send_can_message(can_message_t& can_msg);
};

/**
 * @struct CanSignalState
 *
 * @brief A state encoded (SED) signal's mapping from numerical values to
 * OpenXC state names.
 */
struct CanSignalState {
	const int value; /*!< int value - The integer value of the state on the CAN bus.*/
	const char* name; /*!< char* name  - The corresponding string name for the state in OpenXC. */
};
typedef struct CanSignalState CanSignalState;

/**
 * @struct CanSignal
 *
 * @brief A CAN signal to decode from the bus and output over USB.
 */
struct CanSignal {
	struct CanMessageDefinition* message; /*!< message	   - The message this signal is a part of. */
	const char* genericName; /*!< genericName - The name of the signal to be output over USB.*/
	uint8_t bitPosition; /*!< bitPosition - The starting bit of the signal in its CAN message (assuming
 						*	non-inverted bit numbering, i.e. the most significant bit of
 						*	each byte is 0) */
	uint8_t bitSize; /*!< bitSize - The width of the bit field in the CAN message. */
	float factor; /*!< factor - The final value will be multiplied by this factor. Use 1 if you
 				*	don't need a factor. */
	float offset; /*!< offset	   - The final value will be added to this offset. Use 0 if you
 				*	don't need an offset. */
	float minValue; /*!< minValue    - The minimum value for the processed signal.*/
	float maxValue; /*!< maxValue    - The maximum value for the processed signal. */
	FrequencyClock frequencyClock; /*!< frequencyClock - A FrequencyClock struct to control the maximum frequency to
 								*	process and send this signal. To process every value, set the
 								*	clock's frequency to 0. */
	bool sendSame; /*!< sendSame    - If true, will re-send even if the value hasn't changed.*/
	bool forceSendChanged; /*!< forceSendChanged - If true, regardless of the frequency, it will send the
 						*	value if it has changed. */
	const CanSignalState* states; /*!< states	   - An array of CanSignalState describing the mapping
 								*	between numerical and string values for valid states. */
	uint8_t stateCount; /*!< stateCount  - The length of the states array. */
	bool writable; /*!< writable    - True if the signal is allowed to be written from the USB host
 				*	back to CAN. Defaults to false.*/
	SignalDecoder decoder; /*!< decoder	   - An optional function to decode a signal from the bus to a human
 						*	readable value. If NULL, the default numerical decoder is used. */
	SignalEncoder encoder; /*!< encoder	   - An optional function to encode a signal value to be written to
 						*	CAN into a byte array. If NULL, the default numerical encoder
 						*	is used. */
	bool received; /*!< received    - True if this signal has ever been received.*/
	float lastValue; /*!< lastValue   - The last received value of the signal. If 'received' is false,
 					*	this value is undefined. */
};
typedef struct CanSignal CanSignal;

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
	const char* name; /*!< name - The name of the message set.*/
	uint8_t busCount; /*!< busCount - The number of CAN buses defined for this message set.*/
	unsigned short messageCount; /*!< messageCount - The number of CAN messages (across all buses) defined for
 									*	this message set.*/
	unsigned short signalCount; /*!< signalCount - The number of CAN signals (across all messages) defined for
 								*	this message set.*/
	unsigned short commandCount; /*!< commandCount - The number of CanCommmands defined for this message set.*/
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

/* @struct CanCommand
 * @brief The structure to represent a supported custom OpenXC command.
 *
 * @desc For completely customized CAN commands without a 1-1 mapping between an
 * OpenXC message from the host and a CAN signal, you can define the name of the
 * command and a custom function to handle it in the VI. An example is
 * the "turn_signal_status" command in OpenXC, which has a value of "left" or
 * "right". The vehicle may have separate CAN signals for the left and right
 * turn signals, so you will need to implement a custom command handler to send
 * the correct signals.
 *
 * Command handlers are also useful if you want to trigger multiple CAN messages
 * or signals from a signal OpenXC message.
 */
typedef struct {
	const char* genericName; /*!< genericName - The name of the command.*/
	CommandHandler handler; /*!< handler - An function to process the received command's data and perform some
 							*	action.*/
} CanCommand;

/**
 * @fn void pre_initialize(can_bus_dev_t* bus, bool writable, can_bus_dev_t* buses, const int busCount);
 * @brief Pre initialize actions made before CAN bus initialization
 *
 * @param[in] can_bus_dev_t bus - A CanBus struct defining the bus's metadata
 * @param[in] bool writable - configure the controller in a writable mode. If false, it will be
 *		configured as "listen only" and will not allow writes or even CAN ACKs.
 * @param[in] buses - An array of all CAN buses.
 * @param[in] int busCount - The length of the buses array.
 */
void pre_initialize(can_bus_dev_t* bus, bool writable, can_bus_dev_t* buses, const int busCount);

/**
 * @fn void post_initialize(can_bus_dev_t* bus, bool writable, can_bus_dev_t* buses, const int busCount);
 * @brief Post-initialize actions made after CAN bus initialization
 *
 * @param[in] bus - A CanBus struct defining the bus's metadata
 * @param[in] writable - configure the controller in a writable mode. If false, it will be
 *		configured as "listen only" and will not allow writes or even CAN ACKs.
 * @param[in] buses - An array of all CAN buses.
 * @param[in] busCount - The length of the buses array.
 */
void post_initialize(can_bus_dev_t* bus, bool writable, can_bus_dev_t* buses, const int busCount);

/**
 * @fn bool isBusActive(can_bus_dev_t* bus);
 * @brief Check if the device is connected to an active CAN bus, i.e. it's
 * received a message in the recent past.
 *
 * @return true if a message was received on the CAN bus within
 * CAN_ACTIVE_TIMEOUT_S seconds.
 */
bool isBusActive(can_bus_dev_t* bus);

/**
 * @fn void logBusStatistics(can_bus_dev_t* buses, const int busCount);
 * @brief Log transfer statistics about all active CAN buses to the debug log.
 *
 * @param[in] buses - an array of active CAN buses.
 * @param[in] busCount - the length of the buses array.
 */
void logBusStatistics(can_bus_dev_t* buses, const int busCount);

/**
 * @fn void can_reader(can_bus_dev_t& can_bus_dev, can_bus_t& can_bus);
 *
 * @brief Thread function used to read the can socket.
 *
 * @param[in] can_bus_dev_t object to be used to read the can socket
 * @param[in] can_bus_t object used to fill can_message_q_ queue
 */
void can_reader(can_bus_dev_t& can_bus_dev, can_bus_t& can_bus);

/**
 * @fn void can_decode_message(can_bus_t& can_bus);
 *
 * @brief Thread function used to decode can messages read into the can_message_q_
 *
 * @param[in] can_bus_t object used to pop can_message_q_ queue and fill decoded message
 * into vehicle_message_q_ queue.
 */
void can_decode_message(can_bus_t& can_bus);

/**
 * @fn void can_decode_message(can_bus_t& can_bus);
 *
 * @brief Thread function used to push afb_event
 *
 * @param[in] can_bus_t object used to pop can_message_q_ queue and fill decoded message
 * into vehicle_message_q_ queue.
 */
void can_event_push(can_bus_t& can_bus);