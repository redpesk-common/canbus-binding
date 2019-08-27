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

#include "./message.hpp"

#include <cstring>

#include "../../binding/low-can-hat.hpp"

/**
 * @brief Construct a new message t::message t object
 *
 */
message_t::message_t()
	: maxdlen_{0},
	 length_{0},
	 flags_{INVALID_FLAG},
	 timestamp_{0},
	 sub_id_{-1}
{}

/**
 * @brief Construct a new message t::message t object
 *
 * @param maxdlen The maxdlen of a message
 * @param length The length of the message
 * @param format The format of the message
 * @param data The data vector of the message
 * @param timestamp The timestamp of the message
 */
message_t::message_t(uint32_t maxdlen,
	uint32_t length,
	uint32_t flags,
	std::vector<uint8_t>& data,
	uint64_t timestamp)
	: maxdlen_{maxdlen},
	length_{length},
	flags_{flags},
	data_{data},
	timestamp_{timestamp},
	sub_id_{-1}
{}

/**
 * @brief Return the sub_id of the message
 *
 * @return int The sub_id of the message
 */
int message_t::get_sub_id() const
{
	return sub_id_;
}

///
/// @brief Retrieve data_ member value.
///
/// @return pointer to the first element
///  of class member data_
///
const uint8_t* message_t::get_data() const
{
	return data_.data();
}


///
/// @brief Retrieve data_ member value.
///
/// @return pointer to the first element
///  of class member data_
///
const std::vector<uint8_t> message_t::get_data_vector(int start,int end) const
{
	std::vector<uint8_t> ret;
	if(start >= 0)
	{
		if(end<length_)
		{
			for(int i=start;i<=end;i++)
			{
				ret.push_back(data_[i]);
			}
		}
		else
		{
			for(int i=start;i<length_;i++)
			{
				ret.push_back(data_[i]);
			}
		}
	}
	else
	{
		AFB_ERROR("Error index to get data vector, [%d-%d] - for length %d",start,end,length_);
	}
	return ret;
}

///
/// @brief Retrieve data_ member whole vector
///
/// @return the vector as is
///
const std::vector<uint8_t> message_t::get_data_vector() const
{
	return data_;
}

///
/// @brief Retrieve length_ member value.
///
/// @return length_ class member
///
uint32_t message_t::get_length() const
{
	return length_;
}

/**
 * @brief Set data vector of the message
 *
 * @param data A vector of data
 */
void message_t::set_data(std::vector<uint8_t> &data)
{
	data_ = data;
}

/**
 * @brief Set sub_id of the message
 *
 * @param sub_id The sub_id to set
 */
void message_t::set_sub_id(int sub_id)
{
	sub_id_ = sub_id;
}

/**
 * @brief Return the timestamp of the message
 *
 * @return uint64_t The timestamp
 */
uint64_t message_t::get_timestamp() const
{
	return timestamp_;
}

uint32_t message_t::get_flags()
{
	return flags_;
}

void message_t::set_flags(uint32_t flags)
{
	flags_ = flags_ | flags;
}

uint32_t message_t::get_maxdlen()
{
	return maxdlen_;
}


void message_t::set_maxdlen(uint32_t maxdlen)
{
	maxdlen_ = maxdlen;
}