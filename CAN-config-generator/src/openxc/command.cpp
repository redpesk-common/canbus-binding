#include "command.hpp"

namespace openxc
{
	std::string command::name() const
	{
		return name_;
	}
	
	bool command::enabled() const
	{
		return enabled_;
	}
	
	std::string command::handler() const
	{
		return handler_;
	}
	
	void command::from_json(const nlohmann::json& j)
	{
		name_ = j.count("name") ? j["name"].get<std::string>() : "";
		enabled_ = j.count("enabled") ? j["enabled"].get<bool>() : true;
		handler_ = j.count("handler") ? j["handler"].get<std::string>() : "";
	}

	nlohmann::json command::to_json() const
	{
		nlohmann::json j;
		j["name"] = name_;
		j["enabled"] = enabled_;
		j["handler"] = handler_;
		return j;
	}

	void to_json(nlohmann::json& j, const command& p)
	{
		j = p.to_json();
	}

	void from_json(const nlohmann::json& j, command& p)
	{
		p.from_json(j);
	}
}
