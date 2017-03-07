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

#include "can/can-bus.hpp"

#include <map>
#include <cerrno>
#include <vector>
#include <string>
#include <fcntl.h>
#include <unistd.h>
#include <net/if.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <json-c/json.h>
#include <linux/can/raw.h>

#include "can/can-decoder.hpp"
#include "utils/openxc-utils.hpp"
#include "obd2/diagnostic-manager.hpp"

extern "C"
{
	#include <afb/afb-binding.h>
}

/********************************************************************************
*
*		can_bus_t method implementation
*
*********************************************************************************/
/**
* @brief Class constructor
*
* @param struct afb_binding_interface *interface between daemon and binding
* @param int file handle to the json configuration file.
*/
can_bus_t::can_bus_t(int conf_file)
	: conf_file_{conf_file}
{
}

/**
* @brief thread to decoding raw CAN messages. 
*
* @desc It will take from the can_message_q_ queue the next can message to process then it will search 
*  about signal subscribed if there is a valid afb_event for it. We only decode signal for which a 
*  subscription has been made. Can message will be decoded using translateSignal that will pass it to the
*  corresponding decoding function if there is one assigned for that signal. If not, it will be the default
*  noopDecoder function that will operate on it.
*/
void can_bus_t::can_decode_message()
{
	can_message_t can_message;
	std::vector <CanSignal*> signals;
	openxc_VehicleMessage vehicle_message;
	openxc_DynamicField search_key, decoded_message;

	decoder_t decoder;

	while(is_decoding_)
	{
		std::unique_lock<std::mutex> can_message_lock(can_message_mutex_);
		new_can_message_cv_.wait(can_message_lock);
		can_message = next_can_message();
	
		/* First we have to found which CanSignal it is */
		search_key = build_DynamicField((double)can_message.get_id());
		signals.clear();
		find_can_signals(search_key, signals);

		/* Decoding the message ! Don't kill the messenger ! */
		for(auto& sig : signals)
		{
			std::lock_guard<std::mutex> subscribed_signals_lock(get_subscribed_signals_mutex());
			std::map<std::string, struct afb_event>& s = get_subscribed_signals();
			
			/* DEBUG message to make easier debugger STL containers...
			DEBUG(binder_interface, "Operator[] key char: %s, event valid? %d", sig.generic_name, afb_event_is_valid(s[sig.generic_name]));
			DEBUG(binder_interface, "Operator[] key string: %s, event valid? %d", sig.generic_name, afb_event_is_valid(s[std::string(sig.generic_name)]));
			DEBUG(binder_interface, "Nb elt matched char: %d", (int)s.count(sig.generic_name));
			DEBUG(binder_interface, "Nb elt matched string: %d", (int)s.count(std::string(sig.generic_name))); */
			if( s.find(sig->generic_name) != s.end() && afb_event_is_valid(s[sig->generic_name]))
			{
				decoded_message = decoder.translateSignal(*sig, can_message, get_can_signals());

				openxc_SimpleMessage s_message = build_SimpleMessage(sig->generic_name, decoded_message);
				vehicle_message = build_VehicleMessage_with_SimpleMessage(openxc_DynamicField_Type::openxc_DynamicField_Type_NUM, s_message);

				std::lock_guard<std::mutex> decoded_can_message_lock(decoded_can_message_mutex_);
				push_new_vehicle_message(vehicle_message);
				new_decoded_can_message_.notify_one();
			}
		}
	}
}

/**
* @brief thread to push events to suscribers. It will read subscribed_signals map to look 
* which are events that has to be pushed.
*/
void can_bus_t::can_event_push()
{
	openxc_VehicleMessage v_message;
	openxc_SimpleMessage s_message;
	json_object* jo;
	
	while(is_pushing_)
	{
		std::unique_lock<std::mutex> decoded_can_message_lock(decoded_can_message_mutex_);
		new_decoded_can_message_.wait(decoded_can_message_lock);
		v_message = next_vehicle_message();

		s_message = get_simple_message(v_message);
		{
			std::lock_guard<std::mutex> subscribed_signals_lock(get_subscribed_signals_mutex());
			std::map<std::string, struct afb_event>& s = get_subscribed_signals();
			if(s.find(std::string(s_message.name)) != s.end() && afb_event_is_valid(s[std::string(s_message.name)]))
			{
				jo = json_object_new_object();
				jsonify_simple(s_message, jo);
				afb_event_push(s[std::string(s_message.name)], jo);
			}
		}
	}
}

/**
	* @brief Will initialize threads that will decode
	*  and push subscribed events.
	*/
void can_bus_t::start_threads()
{
	is_decoding_ = true;
	th_decoding_ = std::thread(&can_bus_t::can_decode_message, this);
	if(!th_decoding_.joinable())
		is_decoding_ = false;
	
	is_pushing_ = true;
	th_pushing_ = std::thread(&can_bus_t::can_event_push, this);
	if(!th_pushing_.joinable())
		is_pushing_ = false;
}

/**
* @brief Will stop all threads holded by can_bus_t object
*  which are decoding and pushing then will wait that's 
* they'll finish their job.
*/
void can_bus_t::stop_threads()
{
	is_decoding_ = false;
	is_pushing_ = false;
}

/**
* @brief Will initialize can_bus_dev_t objects after reading 
* the configuration file passed in the constructor.
*/
int can_bus_t::init_can_dev()
{
	std::vector<std::string> devices_name;
	int i;
	size_t t;

	devices_name = read_conf();

	if (! devices_name.empty())
	{
		t = devices_name.size();
		i=0;

		for(const auto& device : devices_name)
		{
			can_devices_m_[device] = std::make_shared<can_bus_dev_t>(device);
			if (can_devices_m_[device]->open() == 0)
			{
				i++;
				DEBUG(binder_interface, "Start reading thread");
				NOTICE(binder_interface, "%s device opened and reading", device.c_str());
				can_devices_m_[device]->start_reading(*this);
			}
			else
				ERROR(binder_interface, "Can't open device %s", device.c_str());
		}

		NOTICE(binder_interface, "Initialized %d/%d can bus device(s)", i, t);
		return 0;
	}
	ERROR(binder_interface, "init_can_dev: Error at CAN device initialization. No devices read from configuration file. Did you specify canbus JSON object ?");
	return 1;
}

/**
* @brief read the conf_file_ and will parse json objects
* in it searching for canbus objects devices name.
*
* @return Vector of can bus device name string.
*/
std::vector<std::string> can_bus_t::read_conf()
{
	std::vector<std::string> ret;
	json_object *jo, *canbus;
	int n, i;
	const char* taxi;

	FILE *fd = fdopen(conf_file_, "r");
	if (fd)
	{
		std::string fd_conf_content;
		std::fseek(fd, 0, SEEK_END);
		fd_conf_content.resize(std::ftell(fd));
		std::rewind(fd);
		std::fread(&fd_conf_content[0], 1, fd_conf_content.size(), fd);
		std::fclose(fd);

		DEBUG(binder_interface, "Configuration file content : %s", fd_conf_content.c_str());
		jo = json_tokener_parse(fd_conf_content.c_str());

		if (jo == NULL || !json_object_object_get_ex(jo, "canbus", &canbus))
		{/**
* @brief Telling if the pushing thread is running
*  This is the boolean value on which the while loop
*  take its condition. Set it to false will stop the 
*  according thread.
*
* @return true if pushing thread is running, false if not.
*/

			ERROR(binder_interface, "Can't find canbus node in the configuration file. Please review it.");
			ret.clear();
		}
		else if (json_object_get_type(canbus) != json_type_array)
		{
			taxi = json_object_get_string(canbus);
			DEBUG(binder_interface, "Can bus found: %s", taxi);
			ret.push_back(std::string(taxi));
		}
		else
		{
			n = json_object_array_length(canbus);
			for (i = 0 ; i < n ; i++)
				ret.push_back(json_object_get_string(json_object_array_get_idx(canbus, i)));
		}
		return ret;
	}
	ERROR(binder_interface, "Problem at reading the conf file");
	ret.clear();
	return ret;
}

/**
* @brief return new_can_message_cv_ member
*
* @return  return new_can_message_cv_ member
*/
std::condition_variable& can_bus_t::get_new_can_message_cv()
{
	return new_can_message_cv_;
}

/**
* @brief return can_message_mutex_ member
*
* @return  return can_message_mutex_ member
*/
std::mutex& can_bus_t::get_can_message_mutex()
{
	return can_message_mutex_;
}

/**
* @brief Return first can_message_t on the queue 
*
* @return a can_message_t 
*/
can_message_t can_bus_t::next_can_message()
{
	can_message_t can_msg;

	if(!can_message_q_.empty())
	{
		can_msg = can_message_q_.front();
		can_message_q_.pop();
		DEBUG(binder_interface, "next_can_message: Here is the next can message : id %X, length %X, data %02X%02X%02X%02X%02X%02X%02X%02X", can_msg.get_id(), can_msg.get_length(),
			can_msg.get_data()[0], can_msg.get_data()[1], can_msg.get_data()[2], can_msg.get_data()[3], can_msg.get_data()[4], can_msg.get_data()[5], can_msg.get_data()[6], can_msg.get_data()[7]);
		return can_msg;
	}
	
	return can_msg;
}

/**
* @brief Push a can_message_t into the queue
*
* @param the const reference can_message_t object to push into the queue
*/
void can_bus_t::push_new_can_message(const can_message_t& can_msg)
{
	can_message_q_.push(can_msg);
}

/**
* @brief Return first openxc_VehicleMessage on the queue 
*
* @return a openxc_VehicleMessage containing a decoded can message
*/
openxc_VehicleMessage can_bus_t::next_vehicle_message()
{
	openxc_VehicleMessage v_msg;

	if(! vehicle_message_q_.empty())
	{
		v_msg = vehicle_message_q_.front();
		vehicle_message_q_.pop();
		DEBUG(binder_interface, "next_vehicle_message: next vehicle message poped");
		return v_msg;
	}
	
	return v_msg;
}

/**
* @brief Push a openxc_VehicleMessage into the queue
*
* @param the const reference openxc_VehicleMessage object to push into the queue
*/
void can_bus_t::push_new_vehicle_message(const openxc_VehicleMessage& v_msg)
{
	vehicle_message_q_.push(v_msg);
}

/**
* @brief Return a map with the can_bus_dev_t initialized
*
* @return map can_bus_dev_m_ map
*/
std::map<std::string, std::shared_ptr<can_bus_dev_t>> can_bus_t::get_can_devices()
{
	return can_devices_m_;
}

/********************************************************************************
*
*		can_bus_dev_t method implementation
*
*********************************************************************************/
/**
* @brief Class constructor 
*
* @param const string representing the device name into the linux /dev tree
*/
can_bus_dev_t::can_bus_dev_t(const std::string& dev_name)
	: device_name_{dev_name}, can_socket_{-1}, diagnostic_manager_{diagnostic_manager_t(*this)}
{}

/**
* @brief Open the can socket and returning it 
*
* @return 
*/
int can_bus_dev_t::open()
{
	const int canfd_on = 1;
	const int timestamp_on = 1;
	struct ifreq ifr;
	struct timeval timeout;

	DEBUG(binder_interface, "CAN Handler socket : %d", can_socket_);
	if (can_socket_ >= 0)
		return 0;

	can_socket_ = ::socket(PF_CAN, SOCK_RAW, CAN_RAW);
	DEBUG(binder_interface, "CAN Handler socket correctly initialized : %d", can_socket_);
	if (can_socket_ < 0)
		ERROR(binder_interface, "socket could not be created. Error was : %s", ::strerror(errno));
	else
	{
		/* Set timeout for read */
		::setsockopt(can_socket_, SOL_SOCKET, SO_RCVTIMEO, (char *)&timeout, sizeof(timeout));
		/* Set timestamp for receveid frame */
		if (::setsockopt(can_socket_, SOL_SOCKET, SO_TIMESTAMP, &timestamp_on, sizeof(timestamp_on)) < 0)
			WARNING(binder_interface, "setsockopt SO_TIMESTAMP error: %s", ::strerror(errno));
		DEBUG(binder_interface, "Switch CAN Handler socket to use fd mode");
		/* try to switch the socket into CAN_FD mode */
		if (::setsockopt(can_socket_, SOL_CAN_RAW, CAN_RAW_FD_FRAMES, &canfd_on, sizeof(canfd_on)) < 0)
		{
			NOTICE(binder_interface, "Can not switch into CAN Extended frame format.");
			is_fdmode_on_ = false;
		} else {
			DEBUG(binder_interface, "Correctly set up CAN socket to use FD frames.");
			is_fdmode_on_ = true;
		}

		/* Attempts to open a socket to CAN bus */
		::strcpy(ifr.ifr_name, device_name_.c_str());
		DEBUG(binder_interface, "ifr_name is : %s", ifr.ifr_name);
		if(::ioctl(can_socket_, SIOCGIFINDEX, &ifr) < 0)
			ERROR(binder_interface, "ioctl failed. Error was : %s", strerror(errno));
		else
		{
			txAddress_.can_family = AF_CAN;
			txAddress_.can_ifindex = ifr.ifr_ifindex;

			/* And bind it to txAddress */
			DEBUG(binder_interface, "Bind the socket");
			if (::bind(can_socket_, (struct sockaddr *)&txAddress_, sizeof(txAddress_)) < 0)
				ERROR(binder_interface, "Bind failed. %s", strerror(errno));
			else
				return 0;
		}
		close();
	}
	return -1;
}

/**
	* @brief Open the can socket and returning it 
	*
	* @return 
	*/
int can_bus_dev_t::close()
{
	::close(can_socket_);
	can_socket_ = -1;
	return can_socket_;
}

/**
* @brief Read the can socket and retrieve canfd_frame
*
* @param const struct afb_binding_interface* interface pointer. Used to be able to log 
*  using application framework logger.
*/
std::pair<struct canfd_frame&, size_t> can_bus_dev_t::read()
{
	ssize_t nbytes;
	//int maxdlen;
	struct canfd_frame cfd;

	/* Test that socket is really opened */
	if (can_socket_ < 0)
	{
		ERROR(binder_interface, "read_can: Socket unavailable. Closing thread.");
		is_running_ = false;
	}

	nbytes = ::read(can_socket_, &cfd, CANFD_MTU);

	/* if we did not fit into CAN sized messages then stop_reading. */
	if (nbytes != CANFD_MTU && nbytes != CAN_MTU)
	{
		if (errno == ENETDOWN)
			ERROR(binder_interface, "read: %s CAN device down", device_name_);
		ERROR(binder_interface, "read: Incomplete CAN(FD) frame");
		::memset(&cfd, 0, sizeof(cfd));
	}
	
	DEBUG(binder_interface, "read: Found id: %X, length: %X, data %02X%02X%02X%02X%02X%02X%02X%02X", cfd.can_id, cfd.len,
							cfd.data[0], cfd.data[1], cfd.data[2], cfd.data[3], cfd.data[4], cfd.data[5], cfd.data[6], cfd.data[7]);
	return std::pair<struct canfd_frame&, size_t>(cfd, nbytes);
}

/**
* @brief start reading threads and set flag is_running_
*
* @param can_bus_t reference can_bus_t. it will be passed to the thread 
*  to allow using can_bus_t queue.
*/
void can_bus_dev_t::start_reading(can_bus_t& can_bus)
{
	DEBUG(binder_interface, "Launching reading thread");
	is_running_ = true;
	th_reading_ = std::thread(&can_bus_dev_t::can_reader, this, std::ref(can_bus));
	if(!th_reading_.joinable())
		is_running_ = false;
}

/**
* @brief stop the reading thread setting flag is_running_ to false and
* and wait that the thread finish its job.
*/
void can_bus_dev_t::stop_reading()
{
	is_running_ = false;
}

/**
*
* @brief Thread function used to read the can socket.
*
* @param[in] can_bus_dev_t object to be used to read the can socket
* @param[in] can_bus_t object used to fill can_message_q_ queue
*/
void can_bus_dev_t::can_reader(can_bus_t& can_bus)
{
	can_message_t can_message;

	while(is_running_)
	{
		can_message.convert_from_canfd_frame(read());

		{
			std::lock_guard<std::mutex> can_message_lock(can_bus.get_can_message_mutex());
			can_bus.push_new_can_message(can_message);
		}
		can_bus.get_new_can_message_cv().notify_one();
	}
}

/**
* @brief Send a can message from a can_message_t object.
* 
* @param const can_message_t& can_msg: the can message object to send 
* @param const struct afb_binding_interface* interface pointer. Used to be able to log 
*  using application framework logger.
*/
int can_bus_dev_t::send_can_message(can_message_t& can_msg)
{
	ssize_t nbytes;
	canfd_frame f;

	f = can_msg.convert_to_canfd_frame();

	if(can_socket_ >= 0)
	{
		nbytes = ::sendto(can_socket_, &f, sizeof(struct canfd_frame), 0,
				(struct sockaddr*)&txAddress_, sizeof(txAddress_));
		if (nbytes == -1)
		{
			ERROR(binder_interface, "send_can_message: Sending CAN frame failed.");
			return -1;
		}
		return (int)nbytes;
	}
	else
	{
		ERROR(binder_interface, "send_can_message: socket not initialized. Attempt to reopen can device socket.");
		open();
	}
	return 0;
}