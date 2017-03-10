/*
 * Copyright (C) 2015, 2016 "IoT.bzh"
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

#include <vector>
#include <string>
#include <fnmatch.h>

#include "openxc.pb.h"
#include "../configuration.hpp"
#include "../can/can-signals.hpp"
#include "../obd2/obd2-signals.hpp"

#include "../low-can-binding.hpp"

extern std::mutex subscribed_signals_mutex;
std::mutex& get_subscribed_signals_mutex();

/**
 * @brief return the subscribed_signals map.
 * 
 * return Map of subscribed signals.
 */
extern std::map<std::string, struct afb_event> subscribed_signals;
std::map<std::string, struct afb_event>& get_subscribed_signals();

template <typename T>
void lookup_signals_by_name(const std::string& key, std::vector<T>& signals, std::vector<T*>& found_signals)
{
	for(T& s : signals)
	{
		if(::fnmatch(key.c_str(), s.get_generic_name().c_str(), FNM_CASEFOLD) == 0)
			found_signals.push_back(&s);
	}
}

template <typename T>
void lookup_signals_by_name(const std::string& key, std::vector<T>& signals, std::vector<std::string>& found_signals_name)
{
	for(T& s : signals)
	{
		if(::fnmatch(key.c_str(), s.get_generic_name().c_str(), FNM_CASEFOLD) == 0)
			found_signals_name.push_back(s.get_name());
	}
}

template <typename T>
void lookup_signals_by_id(const double key, std::vector<T>& signals, std::vector<T*>& found_signals)
{
	for(T& s : signals)
	{
		if(configuration_t::instance().get_signal_id(s) == key)
		{
			found_signals.push_back(&s);
		}
	}
}

template <typename T>
void lookup_signals_by_id(const double key, std::vector<T>& signals, std::vector<std::string>& found_signals_name)
{
	for(T& s : signals)
	{
		if(configuration_t::instance().get_signal_id(s) == key)
		{
			found_signals_name.push_back(s.get_name());
		}
	}
}

std::vector<std::string> find_signals(const openxc_DynamicField &key);
