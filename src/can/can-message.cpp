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

#include "can/can-message.hpp"

#include <cstring>

#include "low-can-binding.hpp"

/********************************************************************************
*
*		CanMessage method implementation
*
*********************************************************************************/
/**
* @brief Class constructor
*
* Constructor about can_message_t class.
*/
can_message_t::can_message_t()
	: id_{0}, rtr_flag_{false}, length_{0}, flags_{0}, format_{can_message_format::ERROR}, maxdlen_{0}
{}

/**
* @brief Retrieve id_ member value.
*
* @return id_ class member
*/
uint32_t can_message_t::get_id() const
{
	return id_;
}

/**
* @brief Retrieve RTR flag member.
*
* @return rtr_flags_ class member
*/
bool can_message_t::get_rtr_flag_() const
{
	return rtr_flag_;
}

/**
* @brief Retrieve format_ member value.
*
* @return format_ class member
*/
int can_message_t::get_format() const
{
	if (format_ != can_message_format::STANDARD || format_ != can_message_format::EXTENDED)
		return can_message_format::ERROR;
	return format_;
}

/**
* @brief Retrieve flags_ member value.
*
* @return flags_ class member 
*/
uint8_t can_message_t::get_flags() const
{
	return flags_;
}

/**
* @brief Retrieve data_ member value.
*
* @return pointer to the first element 
*  of class member data_
*/
const uint8_t* can_message_t::get_data() const
{
	return data_.data();
}

/**
* @brief Retrieve length_ member value.
*
* @return length_ class member
*/
uint8_t can_message_t::get_length() const
{
	return length_;
}

/**
* @brief Control whether the object is correctly initialized
*  to be sent over the CAN bus
*
* @return true if object correctly initialized and false if not.
*/
bool can_message_t::is_correct_to_send()
{
	if (id_ != 0 && length_ != 0 && format_ != can_message_format::ERROR)
	{
		int i;
		for(i=0;i<CAN_MESSAGE_SIZE;i++)
			if(data_[i] != 0)
				return true;
	}
	return false;
}

/**
* @brief Set format_ member value.
*
* Preferred way to initialize these members by using 
* convert_from_canfd_frame method.
*
* @param[in] new_format - class member
*/
void can_message_t::set_format(const can_message_format new_format)
{
	if(new_format == can_message_format::STANDARD || new_format == can_message_format::EXTENDED || new_format == can_message_format::ERROR)
		format_ = new_format;
	else
		ERROR(binder_interface, "ERROR: Can set format, wrong format chosen");
}

/**
* @brief Take a canfd_frame struct to initialize class members
*
* This is the preferred way to initialize class members.
*
* @param[in] args - struct read from can bus device.
*/
can_message_t can_message_t::convert_to_canfd_frame(const struct canfd_frame& frame, size_t nbytes)
{
 	switch(nbytes)
	{
		case CANFD_MTU:
			DEBUG(binder_interface, "set_max_data_length: Got an CAN FD frame");
			maxdlen_ = CANFD_MAX_DLEN;
			break;
		case CAN_MTU:
			DEBUG(binder_interface, "set_max_data_length: Got a legacy CAN frame");
			maxdlen_ = CAN_MAX_DLEN;
			break;
		default:
			ERROR(binder_interface, "set_max_data_length: unsupported CAN frame");
			break;
	}

	if(rtr_flag_)
		length_ = frame.len& 0xF;
	else
	{
		length_ = (frame.len > maxdlen_) ? maxdlen_ : frame.len;
	}
	
	if (frame.can_id & CAN_ERR_FLAG)
		format_ = can_message_format::ERROR;
	else if (frame.can_id & CAN_EFF_FLAG)
		format_ = can_message_format::EXTENDED;
	else
		format_ = can_message_format::STANDARD;
		
	switch(format_)
	{
		case can_message_format::STANDARD:
			id_ = frame.can_id & CAN_SFF_MASK;
			break;
		case can_message_format::EXTENDED:
			id_ = frame.can_id & CAN_EFF_MASK;
			break;
		case can_message_format::ERROR:
			id_ = frame.can_id & (CAN_ERR_MASK|CAN_ERR_FLAG);
			break;
		default:
			ERROR(binder_interface, "ERROR: Can set id, not a compatible format or format not set prior to set id.");
			break;
	}
	
	/* Overwrite lenght_ if RTR flags is detected.
	 * standard CAN frames may have RTR enabled. There are no ERR frames with RTR */
	if (frame.can_id & CAN_RTR_FLAG)
	{
		rtr_flag_ = true;
		if(frame.len && frame.len <= CAN_MAX_DLC)
		{
    	if(rtr_flag_)
    		length_ = frame.len& 0xF;
    	else
    	{
    		length_ = (frame.len > maxdlen_) ? maxdlen_ : frame.len;
    	}	
		}
	}
	else
	{
  	/* Flags field only present for CAN FD frames*/
  	if(maxdlen_ == CANFD_MAX_DLEN)
  			flags_ = frame.flags & 0xF;
  
  	if (data_.capacity() < maxdlen_)
  		data_.reserve(maxdlen_);
  			int i;

		data_.clear();
		/* maxdlen_ is now set at CAN_MAX_DLEN or CANFD_MAX_DLEN, respectively 8 and 64 bytes*/
		for(i=0;i<maxdlen_;i++)
		{
			data_.push_back(frame.data[i]);
		};
  
  	DEBUG(binder_interface, "convert_from_canfd_frame: Found id: %X, format: %X, length: %X, data %02X%02X%02X%02X%02X%02X%02X%02X", id_, format_, length_,
  							data_[0], data_[1], data_[2], data_[3], data_[4], data_[5], data_[6], data_[7]);
	}
}

/**
* @brief Take all initialized class's members and build an 
* canfd_frame struct that can be use to send a CAN message over
* the bus.
*
* @return canfd_frame struct built from class members.
*/
canfd_frame can_message_t::convert_to_canfd_frame()
{
	canfd_frame frame;

	if(is_correct_to_send())
	{
		frame.can_id = get_id();
		frame.len = get_length();
		::memcpy(frame.data, get_data(), length_);
	}
	else
		ERROR(binder_interface, "can_message_t not correctly initialized to be sent");
	
	return frame;
}

/********************************************************************************
*
*		can_message_definition_t method implementation
*
*********************************************************************************/

can_message_definition_t::can_message_definition_t()
  : last_value_(CAN_MESSAGE_SIZE)
{}

uint32_t get_id() const
{
  return id_;
}
}
