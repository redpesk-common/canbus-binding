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

#include "can-message.hpp"

#include <cstring>

#include "low-can-binding.hpp"

/********************************************************************************
*
*		CanMessage method implementation
*
*********************************************************************************/

can_message_t::can_message_t()
	: id_{0}, length_{0}, format_{CanMessageFormat::ERROR}, data_{0,0,0,0,0,0,0,0}
{}

uint32_t can_message_t::get_id() const
{
	return id_;
}

int can_message_t::get_format() const
{
	if (format_ != CanMessageFormat::STANDARD || format_ != CanMessageFormat::EXTENDED)
		return CanMessageFormat::ERROR;
	return format_;
}

const uint8_t* can_message_t::get_data() const
{
	return data_;
}
uint8_t can_message_t::get_length() const
{
	return length_;
}

bool can_message_t::is_correct_to_send()
{
	if (id_ != 0 && length_ != 0 && format_ != CanMessageFormat::ERROR)
	{
		int i;
		for(i=0;i<CAN_MESSAGE_SIZE;i++)
			if(data_[i] != 0)
				return true;
	}
	return false;
}

void can_message_t::set_id(const uint32_t new_id)
{
	switch(format_)
	{
		case CanMessageFormat::STANDARD:
			id_ = new_id & CAN_SFF_MASK;
			break;
		case CanMessageFormat::EXTENDED:
			id_ = new_id & CAN_EFF_MASK;
			break;
		default:
			ERROR(binder_interface, "ERROR: Can set id, not a compatible format or format not set prior to set id.");
			break;
	}
}

void can_message_t::set_format(const CanMessageFormat new_format)
{
	if(new_format == CanMessageFormat::STANDARD || new_format == CanMessageFormat::EXTENDED)
		format_ = new_format;
	else
		ERROR(binder_interface, "ERROR: Can set format, wrong format chosen");
}

void can_message_t::set_data(const uint8_t new_data)
{
	if ((sizeof(new_data) / sizeof(uint8_t) > CAN_MESSAGE_SIZE))
		ERROR(binder_interface, "Can set data, your data array is too big");
	else
	{
		::memcpy(&data_, &new_data, sizeof(new_data));
		length_ = sizeof(new_data);
	}
}

void can_message_t::convert_from_canfd_frame(const canfd_frame& frame)
{
	length_ = (frame.len > CAN_MAX_DLEN) ? (uint8_t)CAN_MAX_DLEN : frame.len;
	length_ = (frame.len > CANFD_MAX_DLEN) ? (uint8_t)CANFD_MAX_DLEN : frame.len;

	if (frame.can_id & CAN_ERR_FLAG)
	{
		id_ = frame.can_id & (CAN_ERR_MASK|CAN_ERR_FLAG);
		format_ = CanMessageFormat::ERROR;
	}
	else if (frame.can_id & CAN_EFF_FLAG)
	{
		id_ = frame.can_id & CAN_EFF_MASK;
		format_ = CanMessageFormat::EXTENDED;
	}
	else
	{
		id_ = frame.can_id & CAN_SFF_MASK;
		format_ = CanMessageFormat::STANDARD;
	}

	if (sizeof(frame.data) <= sizeof(data_))
		::memcpy(&data_, frame.data, length_);
	else if (sizeof(frame.data) >= CAN_MAX_DLEN)
		ERROR(binder_interface, "can_message_t: canfd_frame data too long to be stored into CanMessage object");
}

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

