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

#include "config-parser.hpp"

#include "../binding/low-can-hat.hpp"

namespace utils
{
	/// @brief constructor using path to file
	config_parser_t::config_parser_t(std::string conf_file)
		: filepath_{conf_file}, config_content_{}
	{
		config_content_.read_file(conf_file);
	}

	const std::string& config_parser_t::filepath() const
	{
		return filepath_;
	}

	/// @brief read the conf_file_ and parse it into an INIReader object
	/// to search into later.
	bool config_parser_t::check_conf()
	{
		if (config_content_.size() <= 0)
		{
			AFB_ERROR("Can't load the INI config file: /etc/dev-mapping.conf.");
			return false;
		}
			AFB_DEBUG("Configuration file parsed");
			return true;
	}

	/// @brief Public method to access devices_name_ vector. If vector size equal 0
	/// then it will parses the configuration file content to fill it. It could be empty even
	/// after parsing if content file just don't have a correct "canbus" directive so you
	/// have to test the returned value.
	///
	/// @return A const vector with string of linux CAN devices.
	const std::vector<std::pair<std::string, std::string> > config_parser_t::get_devices_name()
	{
		std::vector<std::pair<std::string, std::string> > devices_name;

		std::map<std::string, std::string> bus_mapping = config_content_.get_keys("CANbus-mapping");
		for(const auto& busIt : bus_mapping )
		{
			devices_name.push_back(std::make_pair(busIt.first, busIt.second));
		}

		return devices_name;
	}
}
