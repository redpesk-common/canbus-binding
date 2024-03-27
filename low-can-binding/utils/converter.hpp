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

#pragma once

#include <string>
#include <stdint.h>
#include <cstdint>

class converter_t
{
	public:
		static std::string to_hex(const uint8_t data[], const size_t length);
		static std::string to_ascii(const uint8_t data[], const size_t length);
		static void signal_to_bits_bytes(unsigned int bit_position,
						 unsigned int bit_size,
						 int &new_start_byte,
						 int &new_end_byte,
						 uint8_t &new_start_bit,
						 uint8_t &new_end_bit);
};
