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
	: id_{0}, rtr_flag_{false}, length_{0}, flags_{0}, format_{CanMessageFormat::ERROR}
{}

uint32_t can_message_t::get_id() const
{
	return id_;
}

bool can_message_t::get_rtr_flag_() const
{
	return rtr_flag_;
}

int can_message_t::get_format() const
{
	if (format_ != CanMessageFormat::STANDARD || format_ != CanMessageFormat::EXTENDED)
		return CanMessageFormat::ERROR;
	return format_;
}

uint8_t can_message_t::get_flags() const
{
	return flags_;
}

const uint8_t* can_message_t::get_data() const
{
	return data_.data();
}

uint8_t can_message_t::get_length() const
{
	return length_;
}

void can_message_t::set_max_data_length(size_t nbytes)
{
	maxdlen_ = 0;

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

void can_message_t::set_id_and_format(const uint32_t new_id)
{
	set_format(new_id);
	switch(format_)
	{
		case CanMessageFormat::STANDARD:
			id_ = new_id & CAN_SFF_MASK;
			break;
		case CanMessageFormat::EXTENDED:
			id_ = new_id & CAN_EFF_MASK;
			break;
		case CanMessageFormat::ERROR:
			id_ = new_id & (CAN_ERR_MASK|CAN_ERR_FLAG);
			break;
		default:
			ERROR(binder_interface, "ERROR: Can set id, not a compatible format or format not set prior to set id.");
			break;
	}
}

void can_message_t::set_format(const CanMessageFormat new_format)
{
	if(new_format == CanMessageFormat::STANDARD || new_format == CanMessageFormat::EXTENDED || new_format == CanMessageFormat::ERROR)
		format_ = new_format;
	else
		ERROR(binder_interface, "ERROR: Can set format, wrong format chosen");
}

void can_message_t::set_format(const uint32_t can_id)
{
	if (can_id & CAN_ERR_FLAG)
		format_ = CanMessageFormat::ERROR;
	else if (can_id & CAN_EFF_FLAG)
		format_ = CanMessageFormat::EXTENDED;
	else
		format_ = CanMessageFormat::STANDARD;
}

void can_message_t::set_flags(const uint8_t flags)
{
	flags_ = flags & 0xF;
}

void can_message_t::set_length(const uint8_t new_length)
{
	if(rtr_flag_)
		length_ = new_length & 0xF;
	else
	{
		length_ = (new_length > maxdlen_) ? maxdlen_ : new_length;
	}
}

void can_message_t::set_data(const __u8* new_data)
{
		int i;

		data_.clear();
		/* maxdlen_ is now set at CAN_MAX_DLEN or CANFD_MAX_DLEN, respectively 8 and 64 bytes*/
		for(i=0;i<maxdlen_;i++)
		{
			data_.push_back(new_data[i]);
		}
}

void can_message_t::convert_from_canfd_frame(const std::pair<struct canfd_frame&, size_t>args)
{
	// May be it's overkill to assign member of the pair... May be it will change...
	struct canfd_frame frame = args.first;
	size_t nbytes = args.second;
	set_max_data_length(nbytes);
	set_length(frame.len);
	set_id_and_format(frame.can_id);

	/* Overwrite lenght_ if RTR flags is detected.
	 * standard CAN frames may have RTR enabled. There are no ERR frames with RTR */
	if (frame.can_id & CAN_RTR_FLAG)
	{
		rtr_flag_ = true;
		if(frame.len && frame.len <= CAN_MAX_DLC)
			set_length(frame.len);
		return;
	}

	/* Flags field only present for CAN FD frames*/
	if(maxdlen_ == CANFD_MAX_DLEN)
		set_flags(frame.flags);

	if ( data_.capacity() < maxdlen_)
		data_.reserve(maxdlen_);
	set_data(frame.data);

	DEBUG(binder_interface, "convert_from_canfd_frame: Found id: %X, format: %X, length: %X, data %02X%02X%02X%02X%02X%02X%02X%02X", id_, format_, length_,
							data_[0], data_[1], data_[2], data_[3], data_[4], data_[5], data_[6], data_[7]);
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

