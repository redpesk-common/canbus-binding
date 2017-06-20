/*
 * Copyright (C) 2015, 2016 ,2017 "IoT.bzh"
 * Author "Loïc Collignon" <loic.collignon@iot.bzh>
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

#ifndef INI_CONFIG_HPP
#define INI_CONFIG_HPP

#include <string>
#include <fstream>
#include <map>
#include <regex>
#include <algorithm>

// Représente un fichier de configuration.
class ini_config
{
public:
	using map = std::map<std::string, std::string>;

	void read_file(const std::string& filename);

	map get_keys(const std::string& section, bool wo_prefix=true);
	std::string get_value(const std::string& section, const std::string& key);

	typename map::size_type size() const { return config_.size(); }
	typename map::iterator begin() { return config_.begin(); }
	typename map::iterator end() { return config_.end(); }
	typename map::const_iterator cbegin() const { return config_.cbegin(); }
	typename map::const_iterator cend() const { return config_.cend(); }
	typename map::reverse_iterator rbegin() { return config_.rbegin(); }
	typename map::reverse_iterator rend() { return config_.rend(); }
	typename map::const_reverse_iterator crbegin() const { return config_.crbegin(); }
	typename map::const_reverse_iterator crend() const { return config_.crend(); }

private:
	map config_;

	enum class line_type
	{
		ignore = 0,
		section = 1,
		key = 2
	};

	line_type qualify(std::string& line);
};

#endif // INI_CONFIG_HPP
