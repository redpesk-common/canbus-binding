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

#include <stdint.h>

namespace frame_codec {
	void encode(uint64_t value, uint8_t data[], uint32_t bit_position, uint32_t bit_size, int bigendian);
	uint64_t decode(const uint8_t data[], uint32_t bit_position, uint32_t bit_size, int bigendian, int sign);
};
