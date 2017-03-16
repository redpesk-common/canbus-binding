#include "can_bus.hpp"

namespace openxc
{
	std::uint32_t can_bus::controller() const
	{
		return controller_;
	}
	
	std::uint32_t can_bus::speed() const
	{
		return speed_;
	}
	
	can_bus_mode can_bus::raw_can_mode() const
	{
		return raw_can_mode_;
	}
	
	bool can_bus::raw_writable() const
	{
		return raw_writable_;
	}
	
	float can_bus::max_message_frequency() const
	{
		return max_message_frequency_;
	}
	
	bool can_bus::force_send_changed() const
	{
		return force_send_changed_;
	}
	
	void can_bus::from_json(const nlohmann::json& j)
	{
		controller_ = j.count("controller") ? j["controller"].get<std::uint32_t>() : 1;
		speed_ = j.count("speed") ? j["speed"].get<std::uint32_t>() : 0;
		raw_can_mode_ = j.count("raw_can_mode") ? j["raw_can_mode"].get<can_bus_mode>() : can_bus_mode::off;
		raw_writable_ = j.count("raw_writable") ? j["raw_writable"].get<bool>() : false;
		max_message_frequency_ = j.count("max_message_frequency") ? j["max_message_frequency"].get<float>() : 0;
		force_send_changed_ = j.count("force_send_changed") ? j["force_send_changed"].get<bool>() : false;
	}

	nlohmann::json can_bus::to_json() const
	{
		nlohmann::json j;
		j["controller"] = controller_;
		j["speed"] = speed_;
		j["raw_can_mode"] = raw_can_mode_;
		j["raw_writable"] = raw_writable_;
		j["max_message_frequency"] = max_message_frequency_;
		j["force_send_changed"] = force_send_changed_;
		return j;
	}

	void to_json(nlohmann::json& j, const can_bus& p)
	{
		j = p.to_json();
	}

	void from_json(const nlohmann::json& j, can_bus& p)
	{
		p.from_json(j);
	}

	void to_json(nlohmann::json& j, const can_bus_mode& p)
	{
		switch (p)
		{
		case can_bus_mode::off:
			j = std::string("off");
			break;
		case can_bus_mode::filtered:
			j = std::string("filtered");
			break;
		case can_bus_mode::unfiltered:
			j = std::string("unfiltered");
			break;
		}
	}

	void from_json(const nlohmann::json& j, can_bus_mode& p)
	{
		if (j.get<std::string>() == "off") p = can_bus_mode::off;
		else if (j.get<std::string>() == "filtered") p = can_bus_mode::filtered;
		else p = can_bus_mode::unfiltered;
	}
}
