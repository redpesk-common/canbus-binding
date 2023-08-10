/*
 * Copyright (C) 2019, 2020 "IoT.bzh"
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

#include <climits>
#include <sstream>
#include <iomanip>

#include "converter.hpp"

/**
 * @brief Convert data to hex string
 *
 * @param data An array of data
 * @param length The length of the data
 * @return std::string The hex string
 */
std::string converter_t::to_hex(const uint8_t data[], const size_t length)
{
	std::stringstream stream;
	stream << std::hex << std::setfill('0');
	stream << "0x";
	for(int i = 0; i < length; i++)
		stream << std::setfill('0') << std::setw(2) << std::hex << ((int) data[i]);

	return stream.str();
}

/**
 * @brief Convert data to ascii string
 *
 * @param data An array of data
 * @param length The length of the data
 * @return std::string The ascii string
 */
std::string converter_t::to_ascii(const uint8_t data[], const size_t length)
{
	std::stringstream stream;
	for(int i = 0; i < length; i++)
		stream << ((char) data[i]);
	return stream.str();
}

/**
 * @brief Translate bit_position and bit_size
 *
 *
 * @param bit_position The position in the frame
 * @param bit_size The size of the signal
 * @param new_start_byte The first bytes of the signal in the frame
 * @param new_end_byte The last byte of the signal in the frame
 * @param new_start_bit The first bit of the signal in the frame
 * @param new_end_bit The last bit of the signal in the frame
 */
void converter_t::signal_to_bits_bytes(unsigned int bit_position, unsigned int bit_size, int &new_start_byte, int &new_end_byte, uint8_t &new_start_bit, uint8_t &new_end_bit)
{
	new_start_byte = bit_position >> 3;
	new_start_bit = bit_position % CHAR_BIT;
	new_end_byte = (bit_position + bit_size - 1) >> 3;
	new_end_bit = (bit_position + bit_size - 1) % CHAR_BIT;
}
