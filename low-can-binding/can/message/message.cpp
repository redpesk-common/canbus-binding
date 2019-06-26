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

///
/// @brief Class constructor
///
/// message_t class constructor.
///
message_t::message_t()
	: length_{0},
	 format_{message_format_t::INVALID},
	 timestamp_{0},
	 sub_id_{-1}
{}

message_t::message_t(uint8_t length,
	message_format_t format,
	std::vector<uint8_t>& data,
	uint64_t timestamp)
	: length_{length},
	format_{format},
	data_{data},
	timestamp_{timestamp},
	sub_id_{-1}
{}

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
uint8_t message_t::get_length() const
{
	return length_;
}

void message_t::set_sub_id(int sub_id)
{
	sub_id_ = sub_id;
}

uint64_t message_t::get_timestamp() const
{
	return timestamp_;
}

message_format_t message_t::get_msg_format()
{
	return format_;
}
