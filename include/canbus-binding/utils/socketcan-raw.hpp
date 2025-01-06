/*
 * Copyright (C) 2015, 2016 , 2017, 2018, 2019 "IoT\.bzh"
 * Author: "Romain Forlot" <romain.forlot@iot.bzh>
 * Author: "Loïc Collignon" <loic.collignon@iot.bzh>
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

#include "socketcan.hpp"

namespace utils
{
	class socketcan_raw_t : public socketcan_t
	{
	public:
		using socketcan_t::socketcan_t;
		virtual int open(std::string device_name);
		virtual std::shared_ptr<message_t> read_message();
		virtual int write_message(message_t& cm);
	};

}
