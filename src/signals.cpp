
#include "signals.hpp"

/**
 * @fn std::vector<std::string> find_signals(const openxc_DynamicField &key)
 * @brief return signals name found searching through CAN_signals and OBD2 pid
 * 
 * @param[in] const openxc_DynamicField : can contain numeric or string value in order to search against 
 *   can signals or obd2 signals name.
 *
 * @return std::vector<std::string> Vector of signals name found. 
 */
std::vector<std::string> find_signals(const openxc_DynamicField &key)
{
	std::vector<std::string> found_signals_name;

	switch(key.type)
	{
		case openxc_DynamicField_Type::openxc_DynamicField_Type_STRING:
				lookup_signals_by_name(key.string_value, get_can_signals(), found_signals_name);
				lookup_signals_by_name(key.string_value, get_obd2_signals(), found_signals_name);
			break;
		case openxc_DynamicField_Type::openxc_DynamicField_Type_NUM:
				lookup_signals_by_id(key.numeric_value, get_can_signals(), found_signals_name);
				lookup_signals_by_id(key.numeric_value, get_obd2_signals(), found_signals_name);
			break;
		default:
			ERROR(binder_interface, "find_signals: wrong openxc_DynamicField specified. Use openxc_DynamicField_Type_NUM or openxc_DynamicField_Type_STRING type only.");
			break;
	}
	DEBUG(binder_interface, "Found %d signal(s)", (int)found_signals_name.size());
	return found_signals_name;
}
