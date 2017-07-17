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

#include "ini-config.hpp"

bool starts_with(const std::string& text, const std::string& token)
{
	if(text.length() < token.length()) return false;
	return (text.compare(0, token.length(), token) == 0);
}

void ini_config::read_file(const std::string& filename)
{
	std::ifstream f(filename);
	if (f)
	{
		std::regex r_section("^\\s*\\[([^\\]]+)\\]\\s*(#.*)?$");
		std::regex r_key("^\\s*([^\\s]+)\\s*=\\s*\"([^\"]+)\"\\s*(#.*)?$");
		std::string current_section;
		std::string line;
		while (std::getline(f, line))
		{
			std::smatch mr;

			switch (qualify(line))
			{
			case line_type::section:
				if (std::regex_match(line, mr, r_section) && mr.size() >= 2 && mr[1].matched)
				{
					current_section = mr[1].str();
				}
				break;
			case line_type::key:
				if(std::regex_match(line, mr, r_key) && mr.size() >= 2 && mr[1].matched)
				{
					std::string key = current_section + '/' + mr[1].str();
					config_[key] = (mr.size() >= 3 && mr[2].matched) ? mr[2].str() : "";
				}
				break;
			case line_type::ignore:
				break;
			}
		}
	}
}

ini_config::map ini_config::get_keys(const std::string& section, bool wo_prefix)
{
	map ret;
	std::string key;

	std::string prefix = section + '/';
	for(auto i = config_.begin();
		i != config_.end();
		++i)
	{
		if (starts_with(i->first, prefix))
		{
			if(wo_prefix)
				key = i->first.substr(section.size()+1);
			else
				key = i->first;
			ret[key] = i->second;
		}
	}
	return ret;
}

std::string ini_config::get_value(const std::string& section, const std::string& key)
{
	return config_[section + '/' + key];
}

ini_config::line_type ini_config::qualify(std::string& line)
{
	if (line.size())
	{
		for (std::string::value_type c : line)
		{
			if (c == '#') return line_type::ignore;
			if (c == '[') return line_type::section;
			if (!std::isspace(c, std::locale("C"))) return line_type::key;
		}
	}
	return line_type::ignore;
}
