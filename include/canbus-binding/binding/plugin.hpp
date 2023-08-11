/*
 * Copyright (C) 2015-2023 "IoT.bzh"
 * Author Iot Team <team@iot.bzh>
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

#include <cstdint>
#include <cstring>

class application_t;

struct canbus_plugin_t
{
	uint32_t magic[2];
	const char *tag;
	const char *id;
	int (*init)(application_t&);
};

#define CANBUS_PLUGIN_MAGIC0 0x78459865
#define CANBUS_PLUGIN_MAGIC1 0x41526358
#define CANBUS_PLUGIN_TAG    "canbus-plugin"

static inline const canbus_plugin_t *to_canbus_plugin(const void *ptr)
{
	const canbus_plugin_t *desc = reinterpret_cast<const canbus_plugin_t*>(ptr);
	return desc != nullptr
		&& desc->magic[0] == CANBUS_PLUGIN_MAGIC0
		&& desc->magic[1] == CANBUS_PLUGIN_MAGIC1
		&& 0 == strcmp(desc->tag, CANBUS_PLUGIN_TAG)
		? desc : nullptr;
}

#define CANBUS_PLUGIN_DECLARE(id, init)                           \
	const canbus_plugin_t canbus_plugin_export =              \
		{ { CANBUS_PLUGIN_MAGIC0, CANBUS_PLUGIN_MAGIC1 }, \
		  CANBUS_PLUGIN_TAG,                              \
		  id, init };

#define CANBUS_PLUGIN_EXPORT "canbus_plugin_export"
