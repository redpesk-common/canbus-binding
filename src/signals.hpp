
#pragma once

#include <vector>
#include <string>
#include <fnmatch.h>

#include "openxc.pb.h"
#include "can-signals.hpp"
#include "obd2-signals.hpp"

template <typename T>
void lookup_signals_by_name(const std::string& key, std::vector<T>& signals, std::vector<T*>& found_signals)
{
	for(T& s : signals)
	{
		if(::fnmatch(key.c_str(), s.generic_name, FNM_CASEFOLD) == 0)
			found_signals.push_back(&s);
	}
}

template <typename T>
void lookup_signals_by_name(const std::string& key, std::vector<T>& signals, std::vector<std::string>& found_signals_name)
{
	for(const T& s : signals)
	{
		if(::fnmatch(key.c_str(), s.generic_name, FNM_CASEFOLD) == 0)
			found_signals_name.push_back(s.generic_name);
	}
}

template <typename T>
void lookup_signals_by_id(const double key, std::vector<T>& signals, std::vector<T*>& found_signals)
{
	for(T& s : signals)
	{
		if(get_signal_id(s) == key)
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
		if(get_signal_id(s) == key)
		{
			found_signals_name.push_back(s.generic_name);
		}
	}
}

std::vector<std::string> find_signals(const openxc_DynamicField &key);
