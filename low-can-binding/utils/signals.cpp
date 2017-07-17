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

#include "signals.hpp"

namespace utils
{
	signals_manager_t::signals_manager_t()
	{}

	/// @brief Return singleton instance of configuration object.
	signals_manager_t& signals_manager_t::instance()
	{
		static signals_manager_t sm;
		return sm;
	}

	/// @brief Return Subscribed signals map mutex.
	std::mutex& signals_manager_t::get_subscribed_signals_mutex()
	{
		return subscribed_signals_mutex_;
	}

	///
	/// @brief return the subscribed_signals map.
	///
	/// @return Map of subscribed signals.
	std::map<int, std::shared_ptr<low_can_subscription_t> >& signals_manager_t::get_subscribed_signals()
	{
		return subscribed_signals_;
	}

	///
	/// @fn std::vector<std::string> find_signals(const openxc_DynamicField &key)
	/// @brief return signals name found searching through CAN_signals and OBD2 pid
	///
	/// @param[in] key : can contain numeric or string value in order to search against
	///   can signals or obd2 signals name.
	///
	/// @return Vector of signals name found.
	///
	struct signals_found signals_manager_t::find_signals(const openxc_DynamicField &key)
	{
		struct signals_found sf;

		switch(key.type)
		{
			case openxc_DynamicField_Type::openxc_DynamicField_Type_STRING:
					lookup_signals_by_name(key.string_value, application_t::instance().get_all_can_signals(), sf.can_signals);
					lookup_signals_by_name(key.string_value, application_t::instance().get_diagnostic_messages(), sf.diagnostic_messages);
				break;
			case openxc_DynamicField_Type::openxc_DynamicField_Type_NUM:
					lookup_signals_by_id(key.numeric_value, application_t::instance().get_all_can_signals(), sf.can_signals);
					lookup_signals_by_id(key.numeric_value, application_t::instance().get_diagnostic_messages(), sf.diagnostic_messages);
				break;
			default:
				AFB_ERROR("wrong openxc_DynamicField specified. Use openxc_DynamicField_Type_NUM or openxc_DynamicField_Type_STRING type only.");
				break;
		}
		AFB_DEBUG("Found %d signal(s)", (int)(sf.can_signals.size() + sf.diagnostic_messages.size()));
		return sf;
	}
}
