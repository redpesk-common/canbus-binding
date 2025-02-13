/*
 * Copyright (C) 2015-2025 IoT.bzh Company
 * Author: "Romain Forlot" <romain.forlot@iot.bzh>
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

#include <canbus-binding/can/message/message.hpp>
#include <canbus-binding/binding/low-can-hat.hpp>
#include <cstring>

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
	sub_id_{-1},
	is_timeout_{false}
{}

message_t::message_t(uint64_t timestamp, bool is_timeout)
	: maxdlen_{0},
	length_{0},
	flags_{0},
	data_{0},
	timestamp_{timestamp},
	sub_id_{-1},
	is_timeout_{is_timeout}
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

///
/// @brief Retrieve is_timeout value.
///
/// @return is_timeout_ class member
///
bool message_t::is_timeout() const
{
	return is_timeout_;
}

/**
 * @brief Set data vector of the message
 *
 * @param data A vector of data
 */
void message_t::set_data(std::vector<uint8_t> data)
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

void message_t::erase_flags()
{
	flags_ = 0;
}

uint32_t message_t::get_maxdlen()
{
	return maxdlen_;
}

void message_t::set_maxdlen(uint32_t maxdlen)
{
	maxdlen_ = maxdlen;
}

void message_t::set_length(uint32_t length)
{
	length_ = length;
}

void message_t::frame_swap()
{
	int i;
	uint8_t *temp = (uint8_t*)alloca(length_);

	for(i = 0; i < length_; i++)
		temp[i] = data_[length_ - i - 1];

	memcpy(data_.data(), temp, length_);
}
