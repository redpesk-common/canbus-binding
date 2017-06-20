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

#include <string>
#include <vector>
#include <utility>
#include "ini-config.hpp"

namespace utils
{
	/// @brief A configuration file parser that handle INI configuration
	/// file format.
	class config_parser_t
	{
	private:
		const std::string filepath_; /*!< filepath_ - Path to the config file*/
		ini_config config_content_; /*!< config_content_ - Parsed content of INI file.*/

	public:
		config_parser_t(config_parser_t&&) = default;
		config_parser_t(const config_parser_t&) = default;
		explicit config_parser_t(std::string conf_file);

		const std::string& filepath() const;
		bool check_conf();
		const std::vector<std::pair<std::string, std::string> > get_devices_name();
	};
}
