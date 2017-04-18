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

#include "../low-can-binding.hpp"

namespace utils
{
	/// @brief constructor using a POSIX file handle as input.
	///
	/// @param conf_file - a POSIX file handle to the INI configuration file
	config_parser_t::config_parser_t(int conf_file)
		: config_content_(fdopen(conf_file, "r"))
	{
		::close(conf_file);
	}

	/// @brief constructor using path to file
	config_parser_t::config_parser_t(std::string conf_file)
		: config_content_{INIReader(conf_file)}
	{}

	/// @brief read the conf_file_ and parse it into an INIReader object
	/// to search into later.
	bool config_parser_t::check_conf()
	{
		if (config_content_.ParseError() < 0)
		{
			ERROR(binder_interface, "read_conf: Can't load the INI config file.");
			return false;
		}
			DEBUG(binder_interface, "read_conf: Configuration file parsed");
			return true;
	}

	/// @brief Public method to access devices_name_ vector. If vector size equal 0
	/// then it will parses the configuration file content to fill it. It could be empty even
	/// after parsing if content file just don't have a correct "canbus" directive so you
	/// have to test the returned value.
	///
	/// @return A const vector with string of linux CAN devices.
	const std::vector<std::string> config_parser_t::get_devices_name()
	{
		std::vector<std::string> devices_name;

		std::set<std::string> sections = config_content_.GetSections();
		for(const auto& sectionsIt : sections)
		{
			if(sectionsIt == "CANbus-mapping")
			{
				std::set<std::string> fields = config_content_.GetFields(sectionsIt);
				for(const auto& fieldsIt : fields)
				{
					std::string val = config_content_.Get(sectionsIt, fieldsIt, "INVALID");
					devices_name.push_back(val);
				}	
			}
		}

		return devices_name;
	}
}