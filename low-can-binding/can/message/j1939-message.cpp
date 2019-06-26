/*
 * Copyright (C) 2018, 2019 "IoT.bzh"
 * Author "Arthur Guyader" <arthur.guyader@iot.bzh>
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

#include "./j1939-message.hpp"
#include <cstring>
#include "../../binding/low-can-hat.hpp"


///
/// @brief Class constructor
///
/// j1939_message_t class constructor.
///
j1939_message_t::j1939_message_t():
    message_t(),
    name_{0},
    pgn_{0},
    addr_{0}
{}

j1939_message_t::j1939_message_t(uint8_t length,
    message_format_t format,
    std::vector<uint8_t>& data,
    uint64_t timestamp,
    name_t name,
    pgn_t pgn,
    uint8_t addr):
    message_t(length, format, data, timestamp),
    name_{name},
    pgn_{pgn},
    addr_{addr}
{}

///
/// @brief Retrieve name_ member value.
///
/// @return name_ class member
///
uint64_t j1939_message_t::get_name() const {
    return name_;
}

///
/// @brief Retrieve pgn_ member value.
///
/// @return pgn_ class member
///
uint32_t j1939_message_t::get_pgn() const{
    return pgn_;
}

///
/// @brief Retrieve addr_ member value.
///
/// @return addr_ class member
///
uint8_t j1939_message_t::get_addr() const{
    return addr_;
}


/// @brief Take a sockaddr_can struct and array of data to initialize class members
///
/// This is the preferred way to initialize class members.
///
/// @param[in] addr - sockaddr_can to get pgn, name and addr
/// @param[in] data - array of data get from the j1939 socket
/// @param[in] nbytes - size of the array of data
/// @param[in] timestamp - timestamp of the message
///
/// @return A j1939_message_t object fully initialized with sockaddr_can and data values.
std::shared_ptr<j1939_message_t> j1939_message_t::convert_from_addr(struct sockaddr_can& addr, uint8_t (&data)[128],size_t nbytes, uint64_t timestamp)
{
	uint8_t length = 0;
	message_format_t format;
	std::vector<uint8_t> dataVector;

    if(nbytes > J1939_MAX_DLEN)
    {
        AFB_DEBUG("Unsupported j1939 frame");
        format = message_format_t::INVALID;
    }
    else
    {
        AFB_DEBUG("Got a j1939 frame");
        format = message_format_t::J1939;
    }

    length = (uint8_t) nbytes;
    dataVector.reserve(length);
    int i;
    dataVector.clear();
    for(i=0;i<length;i++)
    {
        dataVector.push_back(data[i]);
    };

    AFB_DEBUG("Found pgn: %X, format: %X, length: %X, data %02X%02X%02X%02X%02X%02X%02X%02X",
                            addr.can_addr.j1939.pgn, (uint8_t)format, length, data[0], data[1], data[2], data[3], data[4], data[5], data[6], data[7]);

	return std::make_shared<j1939_message_t>(j1939_message_t(length, format, dataVector, timestamp,addr.can_addr.j1939.name,addr.can_addr.j1939.pgn,addr.can_addr.j1939.addr));
}

/// @brief Test if members pgn_ and length are set.
///
/// @return boolean - true = set - false = not set
bool j1939_message_t::is_set()
{
	return (pgn_ != 0 && length_ != 0);
}

/// @brief Generate a string with informations about the message
///
/// @return Debug message with informations about members
std::string j1939_message_t::get_debug_message()
{
	std::string ret = "";
    ret = ret + "Here is the next j1939 message : pgn " + std::to_string(pgn_)  + " length " + std::to_string(length_) + ", data ";
    for (size_t i = 0; i < data_.size(); i++)
    {
        ret = ret + std::to_string(data_[i]);
    }
    return ret;
}

///
/// @brief Retrieve pgn_ member value.
///
/// @return pgn_ class member
///
uint32_t j1939_message_t::get_id() const
{
    AFB_WARNING("Prefer method get_pgn() for j1939 messages");
	return get_pgn();
}


struct bcm_msg j1939_message_t::get_bcm_msg()
{
    AFB_WARNING("Not implemented");
    struct bcm_msg bcm_msg;
    ::memset(&bcm_msg, 0, sizeof(struct bcm_msg));
	return bcm_msg;
}

void j1939_message_t::set_bcm_msg(struct bcm_msg bcm_msg)
{
	AFB_WARNING("Not implemented");
}
