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

#include "../binding/low-can-hat.hpp"

///
/// @brief Class constructor
///
/// can_message_t class constructor.
///
can_message_t::can_message_t()
	: maxdlen_{0},
	 id_{0},
	 length_{0},
	 format_{can_message_format_t::INVALID},
	 rtr_flag_{false},
	 flags_{0},
	 timestamp_{0},
	 sub_id_{-1}
{}

can_message_t::can_message_t(uint8_t maxdlen,
	uint32_t id,
	uint8_t length,
	can_message_format_t format,
	bool rtr_flag,
	uint8_t flags,
	std::vector<uint8_t>& data,
	uint64_t timestamp)
	:  maxdlen_{maxdlen},
	id_{id},
	length_{length},
	format_{format},
	rtr_flag_{rtr_flag},
	flags_{flags},
	data_{data},
	timestamp_{timestamp},
	sub_id_{-1}
{}

///
/// @brief Retrieve id_ member value.
///
/// @return id_ class member
///
uint32_t can_message_t::get_id() const
{
	return id_;
}

int can_message_t::get_sub_id() const
{
	return sub_id_;
}

///
/// @brief Retrieve RTR flag member.
///
/// @return rtr_flags_ class member
///
bool can_message_t::get_rtr_flag_() const
{
	return rtr_flag_;
}

///
/// @brief Retrieve format_ member value.
///
/// @return format_ class member. Default to INVALID.
///
can_message_format_t can_message_t::get_format() const
{
	if (format_ != can_message_format_t::STANDARD || format_ != can_message_format_t::EXTENDED)
		return can_message_format_t::INVALID;
	return format_;
}

///
/// @brief Retrieve flags_ member value.
///
/// @return flags_ class member
///
uint8_t can_message_t::get_flags() const
{
	return flags_;
}

///
/// @brief Retrieve data_ member value.
///
/// @return pointer to the first element
///  of class member data_
///
const uint8_t* can_message_t::get_data() const
{
	return data_.data();
}

///
/// @brief Retrieve data_ member whole vector
///
/// @return the vector as is
///
const std::vector<uint8_t> can_message_t::get_data_vector() const
{
	return data_;
}

///
/// @brief Retrieve length_ member value.
///
/// @return length_ class member
///
uint8_t can_message_t::get_length() const
{
	return length_;
}

void can_message_t::set_sub_id(int sub_id)
{
	sub_id_ = sub_id;
}

uint64_t can_message_t::get_timestamp() const
{
	return timestamp_;
}

void can_message_t::set_timestamp(uint64_t timestamp)
{
	timestamp_ = timestamp;
}

/// @brief Control whether the object is correctly initialized
///  to be sent over the CAN bus
///
/// @return True if object correctly initialized and false if not.
bool can_message_t::is_correct_to_send()
{
	if (id_ != 0 && length_ != 0 && format_ != can_message_format_t::INVALID)
	{
		int i;
		for(i=0;i<CAN_MESSAGE_SIZE;i++)
			if(data_[i] != 0)
				return true;
	}
	return false;
}

/// @brief Set format_ member value.
///
/// Preferred way to initialize these members is to use
/// convert_from_canfd_frame method.
///
/// @param[in] new_format - class member
void can_message_t::set_format(const can_message_format_t new_format)
{
	if(new_format == can_message_format_t::STANDARD || new_format == can_message_format_t::EXTENDED || new_format == can_message_format_t::INVALID)
		format_ = new_format;
	else
		AFB_ERROR("Can set format, wrong format chosen");
}

/// @brief Take a canfd_frame struct to initialize class members
///
/// This is the preferred way to initialize class members.
///
/// @param[in] frame - canfd_frame to convert coming from a read of CAN socket
/// @param[in] nbytes - bytes read from socket read operation.
///
/// @return A can_message_t object fully initialized with canfd_frame values.
can_message_t can_message_t::convert_from_frame(const struct canfd_frame& frame, size_t nbytes, uint64_t timestamp)
{
	uint8_t maxdlen = 0, length = 0, flags = 0;
	uint32_t id;
	can_message_format_t format;
	bool rtr_flag;
	std::vector<uint8_t> data;

	switch(nbytes)
	{
		case CANFD_MTU:
			AFB_DEBUG("Got an CAN FD frame");
			maxdlen = CANFD_MAX_DLEN;
			break;
		case CAN_MTU:
			AFB_DEBUG("Got a legacy CAN frame");
			maxdlen = CAN_MAX_DLEN;
			break;
		default:
			AFB_ERROR("unsupported CAN frame");
			break;
	}

	if (frame.can_id & CAN_ERR_FLAG)
	{
		format = can_message_format_t::INVALID;
		id = frame.can_id & (CAN_ERR_MASK|CAN_ERR_FLAG);
	}
	else if (frame.can_id & CAN_EFF_FLAG)
	{
		format = can_message_format_t::EXTENDED;
		id = frame.can_id & CAN_EFF_MASK;
	}
	else
	{
		format = can_message_format_t::STANDARD;
		id = frame.can_id & CAN_SFF_MASK;
	}

	/* Overwrite length_ if RTR flags is detected.
	 * standard CAN frames may have RTR enabled. There are no ERR frames with RTR */
	if (frame.can_id & CAN_RTR_FLAG)
	{
		rtr_flag = true;
		if(frame.len && frame.len <= CAN_MAX_DLC)
		{
			if(rtr_flag)
				length = frame.len& 0xF;
			else
			{
				length = (frame.len > maxdlen) ? maxdlen : frame.len;
			}
		}
	}
	else
	{
		length = (frame.len > maxdlen) ? maxdlen : frame.len;

		/* Flags field only present for CAN FD frames*/
		if(maxdlen == CANFD_MAX_DLEN)
				flags = frame.flags & 0xF;

		if (data.capacity() < maxdlen)
			data.reserve(maxdlen);
				int i;

			data.clear();
			/* maxdlen_ is now set at CAN_MAX_DLEN or CANFD_MAX_DLEN, respectively 8 and 64 bytes*/
			for(i=0;i<maxdlen;i++)
			{
				data.push_back(frame.data[i]);
			};

		AFB_DEBUG("Found id: %X, format: %X, length: %X, data %02X%02X%02X%02X%02X%02X%02X%02X",
								id, (uint8_t)format, length, data[0], data[1], data[2], data[3], data[4], data[5], data[6], data[7]);
	}

	return can_message_t(maxdlen, id, length, format, rtr_flag, flags, data, timestamp);
}

/// @brief Take a can_frame struct to initialize class members
///
/// This is the preferred way to initialize class members.
///
/// @param[in] frame - can_frame to convert coming from a read of CAN socket
/// @param[in] nbytes - bytes read from socket read operation.
///
/// @return A can_message_t object fully initialized with can_frame values.
can_message_t can_message_t::convert_from_frame(const struct can_frame& frame, size_t nbytes, uint64_t timestamp)
{
	uint8_t maxdlen = 0, length = 0, flags = 0;
	uint32_t id;
	can_message_format_t format;
	bool rtr_flag;
	std::vector<uint8_t> data;

	if(nbytes <= CAN_MTU)
	{
			AFB_DEBUG("Got a legacy CAN frame");
			maxdlen = CAN_MAX_DLEN;
	}
	else
	{
			AFB_ERROR("unsupported CAN frame");
	}

	if (frame.can_id & CAN_ERR_FLAG)
	{
		format = can_message_format_t::INVALID;
		id = frame.can_id & (CAN_ERR_MASK|CAN_ERR_FLAG);
	}
	else if (frame.can_id & CAN_EFF_FLAG)
	{
		format = can_message_format_t::EXTENDED;
		id = frame.can_id & CAN_EFF_MASK;
	}
	else
	{
		format = can_message_format_t::STANDARD;
		id = frame.can_id & CAN_SFF_MASK;
	}

	/* Overwrite length_ if RTR flags is detected.
	 * standard CAN frames may have RTR enabled. There are no ERR frames with RTR */
	if (frame.can_id & CAN_RTR_FLAG)
	{
		rtr_flag = true;
		if(frame.can_dlc && frame.can_dlc <= CAN_MAX_DLC)
		{
			if(rtr_flag)
				length = frame.can_dlc& 0xF;
			else
			{
				length = (frame.can_dlc > maxdlen) ? maxdlen : frame.can_dlc;
			}
		}
	}
	else
	{
		length = (frame.can_dlc > maxdlen) ? maxdlen : frame.can_dlc;

		if (data.capacity() < maxdlen)
			data.reserve(maxdlen);
				int i;

			data.clear();
			/* maxdlen_ is now set at CAN_MAX_DLEN or CANFD_MAX_DLEN, respectively 8 and 64 bytes*/
			for(i=0;i<maxdlen;i++)
			{
				data.push_back(frame.data[i]);
			};

//		AFB_DEBUG("Found id: %X, format: %X, length: %X, data %02X%02X%02X%02X%02X%02X%02X%02X",
//								id, (uint8_t)format, length, data[0], data[1], data[2], data[3], data[4], data[5], data[6], data[7]);
	}

	return can_message_t(maxdlen, id, length, format, rtr_flag, flags, data, timestamp);
}

/// @brief Take all initialized class members and build a
/// canfd_frame struct that can be use to send a CAN message over
/// the bus.
///
/// @return canfd_frame struct built from class members.
struct canfd_frame can_message_t::convert_to_canfd_frame()
{
	canfd_frame frame;

	if(is_correct_to_send())
	{
		frame.can_id = get_id();
		frame.len = get_length();
		::memcpy(frame.data, get_data(), length_);
	}
	else
		AFB_ERROR("can_message_t not correctly initialized to be sent");

	return frame;
}

/// @brief Take all initialized class members and build a
/// can_frame struct that can be use to send a CAN message over
/// the bus.
///
/// @return can_frame struct built from class members.
struct can_frame can_message_t::convert_to_can_frame()
{
	can_frame frame;

	if(is_correct_to_send())
	{
		frame.can_id = get_id();
		frame.can_dlc = get_length();
		::memcpy(frame.data, get_data(), length_);
	}
	else
		AFB_ERROR("can_message_t not correctly initialized to be sent");

	return frame;
}
