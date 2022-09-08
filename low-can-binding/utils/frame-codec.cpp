/*
 * Copyright (C) 2015-2022 "IoT.bzh"
 * Author "José Bollo" <jose.bollo@iot.bzh>
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

#include <low-can/utils/frame-codec.hpp>

namespace frame_codec {

void encode(uint64_t value, uint8_t data[], uint32_t bit_position, uint32_t bit_size, int bigendian)
{
	uint32_t rbits = bit_size;
	uint32_t ibyte = bit_position >> 3;

	if (bigendian) {
		/* default is big endian */
		uint32_t ipos = bit_position & 7;
		uint32_t nbits = ipos + 1;
		uint8_t mask = (uint8_t)(1 << nbits) - 1;
		if (rbits < nbits) {
			mask &= ~((1 << (nbits - rbits)) - 1);
			data[ibyte] = (data[ibyte] & ~mask) | (uint8_t)((value << (nbits - rbits)) & mask);
		}
		else {
			rbits -= nbits;
			data[ibyte] = (data[ibyte] & ~mask) | (uint8_t)((value >> rbits) & mask);
			while(rbits >= 8) {
				rbits -= 8;
				data[++ibyte] = (uint8_t)(value >> rbits);
			}
			if (rbits) {
				mask = 0xff << (8 - rbits);
				ibyte++;
				data[ibyte] = (data[ibyte] & ~mask) | (uint8_t)((value << (8 - rbits)) & mask);
			}
		}
	}
	else {
		/* little endian */
		uint32_t ipos = bit_position & 7;
		uint32_t nbits = 8 - ipos;
		uint8_t mask = (uint8_t)(0xff << ipos);
		if (rbits < nbits) {
			mask &= ((1 << (ipos + rbits)) - 1);
			data[ibyte] = (data[ibyte] & ~mask) | (uint8_t)((value << ipos) & mask);
		}
		else {
			data[ibyte] = (data[ibyte] & ~mask) | (uint8_t)((value << ipos) & mask);
			rbits -= nbits;
			while(rbits >= 8) {
				data[++ibyte] = (uint8_t)(value >> (bit_size - rbits));
				rbits -= 8;
			}
			if (rbits) {
				mask = ((1 << rbits) - 1);
				ibyte++;
				data[ibyte] = (data[ibyte] & ~mask) | (uint8_t)((value >> (bit_size - rbits)) & mask);
			}
		}
	}
}

uint64_t decode(const uint8_t data[], uint32_t bit_position, uint32_t bit_size, int bigendian, int sign)
{
	uint32_t rbits = bit_size;
	uint32_t ibyte = bit_position >> 3;
	uint64_t value = 0;

	if (bigendian) {
		/* default is big endian */
		uint32_t ipos = bit_position & 7;
		uint32_t nbits = ipos + 1;
		uint8_t mask = (uint8_t)(1 << nbits) - 1;
		if (rbits < nbits) {
			mask &= ~((1 << (nbits - rbits)) - 1);
			value = ((uint64_t)(data[ibyte] & mask)) >> (nbits - rbits);
		}
		else {
			rbits -= nbits;
			value = ((uint64_t)(data[ibyte] & mask)) << rbits;
			while(rbits >= 8) {
				rbits -= 8;
				value |= ((uint64_t)data[++ibyte]) << rbits;
			}
			if (rbits) {
				mask = 0xff << (8 - rbits);
				value |= ((uint64_t)(data[++ibyte] & mask)) >> (8 - rbits);
			}
		}
	}
	else {
		/* little endian */
		uint32_t ipos = bit_position & 7;
		uint32_t nbits = 8 - ipos;
		uint8_t mask = (uint8_t)(0xff << ipos);
		if (rbits < nbits) {
			mask &= ((1 << (ipos + rbits)) - 1);
			value = ((uint64_t)(data[ibyte] & mask)) >> ipos;
		}
		else {
			value = ((uint64_t)(data[ibyte] & mask)) >> ipos;
			rbits -= nbits;
			while(rbits >= 8) {
				value |= ((uint64_t)data[++ibyte]) << (bit_size - rbits);
				rbits -= 8;
			}
			if (rbits) {
				mask = ((1 << rbits) - 1);
				value |= ((uint64_t)(data[++ibyte] & mask)) << (bit_size - rbits);
			}
		}
	}
	if (sign)
		value |= (-(value >> (bit_size - 1)) << (bit_size - 1));
	return value;
}

};