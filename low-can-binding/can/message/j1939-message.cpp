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

#include <cstring>
#include <iomanip>
#include <net/if.h>
#include "../../binding/low-can-hat.hpp"
#include "../../utils/converter.hpp"
#include "j1939-message.hpp"

/**
 * @brief Construct a new j1939 message t::j1939 message t object
 *
 */
j1939_message_t::j1939_message_t():
	message_t(),
	name_{0},
	pgn_{0},
	addr_{0}
{}

/**
 * @brief Construct a new j1939 message t::j1939 message t object
 *
 * @param length The length of the message
 * @param format The format of the message
 * @param data The vector data of the message
 * @param timestamp The timetamp of the message
 * @param name The name of the message
 * @param pgn The PGN of the message
 * @param addr The address of the message
 */
j1939_message_t::j1939_message_t(uint32_t length,
	std::vector<uint8_t>& data,
	uint64_t timestamp,
	name_t name,
	pgn_t pgn,
	uint8_t addr):
	message_t(J1939_MAX_DLEN, length, J1939_PROTOCOL, data, timestamp),
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
	int i;
	uint32_t length = 0;
	std::vector<uint8_t> data_vector;

	if(nbytes > J1939_MAX_DLEN)
	{
		AFB_DEBUG("Unsupported j1939 frame");
		return std::make_shared<j1939_message_t>(j1939_message_t());
	}

	length = (uint32_t) nbytes;
	data_vector.reserve(length);

	data_vector.clear();

	std::string data_string;
	data_string = converter_t::to_hex(data,length);

	for(i=0;i<length;i++)
	{
		data_vector.push_back(data[i]);
	};

	AFB_DEBUG("Found pgn: %X, length: %X, data %s",
							addr.can_addr.j1939.pgn, length, data_string.c_str());

	return std::make_shared<j1939_message_t>(j1939_message_t(length, data_vector, timestamp,addr.can_addr.j1939.name,addr.can_addr.j1939.pgn,addr.can_addr.j1939.addr));
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
	AFB_DEBUG("Prefer method get_pgn() for j1939 messages");
	return get_pgn();
}

void j1939_message_t::set_id(const canid_t id)
{
	pgn_ = id;
}


/**
 * @brief Return the sockname of the message
 *
 * @return struct sockaddr_can The sockname of the message
 */
struct sockaddr_can j1939_message_t::get_sockname()
{
	return sockname_;
}

/**
 * @brief Allows to set a sockname at a message to send it after
 *
 * @param sockname The sockname of the message
 */
void j1939_message_t::set_sockname(struct sockaddr_can sockname)
{
	sockname_ = sockname;
}

/**
 * @brief Allows to generate a sockname for the message
 *
 * @param pgn The pgn for the sockname
 * @param name The name for the sockname
 * @param addr The address for the sockname
 */
void j1939_message_t::set_sockname(pgn_t pgn, name_t name, uint8_t addr)
{
	memset(&sockname_, 0, sizeof(sockname_));
	sockname_.can_family = AF_CAN;
	sockname_.can_ifindex = 0;

	if(addr <= 0 || addr >= UINT8_MAX )
	{
		sockname_.can_addr.j1939.addr = J1939_NO_ADDR;
	}
	else
	{
		sockname_.can_addr.j1939.addr = addr;
	}

	if(name <= 0 || name >= UINT64_MAX )
	{
		sockname_.can_addr.j1939.name = J1939_NO_NAME;
	}
	else
	{
		sockname_.can_addr.j1939.name = name;
	}

	if(pgn <= 0 || pgn > J1939_PGN_MAX)
	{
		sockname_.can_addr.j1939.pgn = J1939_NO_PGN;
	}
	else
	{
		sockname_.can_addr.j1939.pgn = pgn;
	}
}