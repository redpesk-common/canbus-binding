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

namespace utils
{
	config_parser_t::config_parser_t(int conf_file)
		: conf_file_{conf_file}, devices_name{}
	{}

	/// @brief read the conf_file_ and will parse json objects
	/// in it searching for canbus objects devices name.
	///
	/// @return Vector of can bus device name string.
	void can_bus_t::read_conf()
	{
		FILE *fd = fdopen(conf_file_, "r");
		if (fd)
		{
			std::fseek(fd, 0, SEEK_END);
			config_content_.resize(std::ftell(fd));
			std::rewind(fd);
			std::fread(&config_content_[0], 1, config_content_.size(), fd);
			std::fclose(fd);

			DEBUG(binder_interface, "Configuration file content : %s", config_content_.c_str());
		}
		ERROR(binder_interface, "Problem at reading the conf file");
	}

	void parse_devices_name()
	{
		json_object *jo, *canbus;
		const char* taxi;

		jo = json_tokener_parse(config_content_.c_str());

			if (jo == NULL || !json_object_object_get_ex(jo, "canbus", &canbus))
			{
				ERROR(binder_interface, "Can't find canbus node in the configuration file. Please review it.");
				devices_name_.clear();
			}
			else if (json_object_get_type(canbus) != json_type_array)
			{
				taxi = json_object_get_string(canbus);
				DEBUG(binder_interface, "Can bus found: %s", taxi);
				devices_name_.push_back(std::string(taxi));
			}
			else
			{
				int n, i;
				n = json_object_array_length(canbus);
				for (i = 0 ; i < n ; i++)
					devices_name_.push_back(json_object_get_string(json_object_array_get_idx(canbus, i)));
			}
	}

	/// @brief Public method to access devices_name_ vector. If vector size equal 0
	/// then it will parses the configuration file content to fill it. It could be empty even
	/// after parsing if content file just don't have a correct "canbus" directive so you
	/// have to test the returned value.
	///
	/// @return A const vector with string of linux CAN devices.
	const std::vector<std::string>& get_devices_name()
	{
		if(devices_name_.empty())
			parse_devices_name();

		return devices_name_;
	}
}