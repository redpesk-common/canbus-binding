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

#include "./can-message.hpp"

#include <cstring>

#include "../../binding/low-can-hat.hpp"

///
/// @brief Class constructor
///
/// can_message_t class constructor.
///
can_message_t::can_message_t()
	: message_t(),
	 id_{0},
	 rtr_flag_{false},
	 flags_{0}
{}

can_message_t::can_message_t(uint32_t maxdlen,
	uint32_t id,
	uint32_t length,
	message_format_t format,
	bool rtr_flag,
	uint32_t flags,
	std::vector<uint8_t>& data,
	uint64_t timestamp)
	: message_t(maxdlen, length, format, data, timestamp),
	id_{id},
	rtr_flag_{rtr_flag},
	flags_{flags}
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


/// @brief Control whether the object is correctly initialized
///  to be sent over the CAN bus
///
/// @return True if object correctly initialized and false if not.
bool can_message_t::is_correct_to_send()
{
	if (id_ != 0 && length_ != 0 && format_ != message_format_t::INVALID)
	{
		int i;
		for(i=0;i<length_;i++)
			if(data_[i] != 0)
				return true;
	}
	return false;
}

/// @brief Take a canfd_frame struct to initialize class members
///
/// This is the preferred way to initialize class members.
///
/// @param[in] frame - canfd_frame to convert coming from a read of CAN socket
/// @param[in] nbytes - bytes read from socket read operation.
///
/// @return A can_message_t object fully initialized with canfd_frame values.
std::shared_ptr<can_message_t> can_message_t::convert_from_frame(const struct canfd_frame& frame, size_t nbytes, uint64_t timestamp)
{
	uint32_t maxdlen = 0, length = 0;
	uint8_t flags = 0;
	uint32_t id;
	message_format_t format;
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
		format = message_format_t::INVALID;
		id = frame.can_id & (CAN_ERR_MASK|CAN_ERR_FLAG);
	}
	else if (frame.can_id & CAN_EFF_FLAG)
	{
		format = message_format_t::EXTENDED;
		id = frame.can_id & CAN_EFF_MASK;
	}
	else
	{
		format = message_format_t::STANDARD;
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

	return std::make_shared<can_message_t>(can_message_t(maxdlen, id, length, format, rtr_flag, flags, data, timestamp));
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
		frame.len = (uint8_t) get_length();
		::memcpy(frame.data, get_data(), length_);
	}
	else
		AFB_ERROR("can_message_t not correctly initialized to be sent");

	return frame;
}

/// @brief Take all initialized class members and build a
/// canfd_frame struct that can be use to send a CAN message over
/// the bus.
///
/// @return canfd_frame struct built from class members.
struct std::vector<canfd_frame> can_message_t::convert_to_canfd_frame_vector()
{
	std::vector<canfd_frame> ret;
	if(is_correct_to_send())
	{
		if(flags_ & CAN_FD_FRAME)
		{
			int i=0;
			do
			{
				canfd_frame frame;
				frame.can_id = id_;
				frame.len = 64;
				std::vector<uint8_t> data = get_data_vector((i*64),(i*64)+63);
				if(data.size()<64)
				{
					::memset(frame.data,0,sizeof(frame.data));
					::memcpy(frame.data,data.data(),data.size());
				}
				else
				{
					::memcpy(frame.data,data.data(),64);
				}
				ret.push_back(frame);
				i++;
			} while (i<(length_ >> 6));
		}
		else
		{
			int i=0;
			do
			{
				canfd_frame frame;
				frame.can_id = id_;
				frame.len = 8;
				std::vector<uint8_t> data = get_data_vector(i*8,(i*8)+7);
				if(data.size()<8)
				{
					::memset(frame.data,0,sizeof(frame.data));
					::memcpy(frame.data,data.data(),data.size());
				}
				else
				{
					::memset(frame.data,0,sizeof(frame.data));
					::memcpy(frame.data,data.data(),8);
				}
				ret.push_back(frame);
				i++;
			} while (i<(length_ >> 3));
		}
	}
	else
		AFB_ERROR("can_message_t not correctly initialized to be sent");

	return ret;
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
		frame.can_dlc = (uint8_t) get_length();
		::memcpy(frame.data, get_data(), length_);
	}
	else
		AFB_ERROR("can_message_t not correctly initialized to be sent");

	return frame;
}

bool can_message_t::is_set()
{
	return (id_ != 0 && length_ != 0);
}

std::string can_message_t::get_debug_message()
{
	std::string ret = "";
	ret = ret + "Here is the next can message : id " + std::to_string(id_)  + " length " + std::to_string(length_) + ", data ";
	for (size_t i = 0; i < data_.size(); i++)
	{
		ret = ret + std::to_string(data_[i]);
	}

	return ret;
}

struct bcm_msg can_message_t::get_bcm_msg()
{
	return bcm_msg_;
}

void can_message_t::set_bcm_msg(struct bcm_msg bcm_msg)
{
	bcm_msg_ = bcm_msg;
}

uint32_t can_message_t::get_flags()
{
	return flags_;
}