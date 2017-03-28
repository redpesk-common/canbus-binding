/*
 * Copyright (C) 2015, 2016 ,2017 "IoT.bzh"
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

#pragma once

#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <json-c/json.h>

#include <string>

namespace utils
{
	class config_parser_t
	{
	private:
		int conf_file_; /*!< conf_file_ - file that handle the binding configuration file */
		std::string config_content_; /*!< config_content_ - String that contains the content of config file */
		std::vector<std::string> devices_name_; /*!< devices_name - Found devices name after reading configuration file */

		void parse_devices_name();
	public:
		config_parser_t(int conf_file);

		void read_conf();
		std::vector<std::string> get_devices_name();
	};
}
