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

#include "converter.hpp"
#include <sstream>
#include <net/if.h>
#include <afb/afb-binding>

/**
 * @brief Convert hex data to string
 *
 * @param data An array of data
 * @param length The length of the data
 * @return std::string The string data
 */
std::string converter_t::to_hex(const uint8_t data[], const size_t length)
{
	std::stringstream stream;
	stream << std::hex << std::setfill('0');
	for(int i = 0; i < length; i++)
		stream << std::hex << ((int) data[i]);

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
void converter_t::signal_to_bits_bytes(uint32_t bit_position, uint32_t bit_size, int &new_start_byte, int &new_end_byte, uint8_t &new_start_bit, uint8_t &new_end_bit)
{
	new_start_byte = bit_position >> 3;
	new_start_bit = bit_position % 8;
	new_end_byte = (bit_position + bit_size - 1) >> 3;
	new_end_bit = (bit_position + bit_size - 1) % 8;
}


/**
 * @brief 	This is to use when you have a big endian CAN frame layout.
 * 		  	It converts the bit position so it matches with little endiant CAN frame layout.
 *
 * @param bit_position 	Original bit position.
 * @param bit_size 		Size of the data.
 * @return uint32_t 	New little endian bit position.
 */
uint32_t converter_t::bit_position_swap(uint32_t bit_position,uint32_t bit_size)
{
	uint32_t start_byte_position = (uint32_t)(bit_position/8);
	uint32_t bit_size_rest = bit_size;

	if((int)(bit_size-(8 + start_byte_position*8-bit_position%8))>0)
	{
		AFB_ERROR("Error: bit_position and bit_size getting out of range");
		return bit_position;
	}

	if(bit_size<=8 && ((bit_position+bit_size)%8==bit_size || (bit_position+bit_size)%8==0))
	{
		return (uint32_t)(start_byte_position*8 + (8-bit_size));
	}
	else
	{
		do
		{
			bit_size_rest = bit_size_rest - ((start_byte_position+1)*8-bit_position);
			start_byte_position--;
			bit_position = start_byte_position*8;
		} while (bit_size_rest>8);
		return (uint32_t)(start_byte_position*8 + (8-bit_size_rest));
	}

}
