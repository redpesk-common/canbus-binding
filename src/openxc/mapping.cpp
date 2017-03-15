#include "mapping.hpp"

namespace openxc
{
	std::string mapping::mapping_name() const
	{
		return mapping_name_;
	}
	
	std::string mapping::bus() const
	{
		return bus_;
	}
	
	std::string mapping::database() const
	{
		return database_;
	}
	
	bool mapping::bit_numbering_inverted() const
	{
		return bit_numbering_inverted_;
	}
	
	bool mapping::enabled() const
	{
		return enabled_;
	}
	
	void mapping::from_json(const nlohmann::json& j)
	{
		mapping_name_ = j.count("mapping") ? j["mapping"].get<std::string>() : "";
		bus_ = j.count("bus") ? j["bus"].get<std::string>() : "";
		database_ = j.count("database") ? j["database"].get<std::string>() : "";
		bit_numbering_inverted_ = j.count("bit_numbering_inverted") ? j["bit_numbering_inverted"].get<bool>() : false;
		enabled_ = j.count("enabled") ? j["enabled"].get<bool>() : true;
	}

	nlohmann::json mapping::to_json() const
	{
		nlohmann::json j;
		j["mapping"] = mapping_name_;
		j["bus"] = bus_;
		j["database"] = database_;
		j["bit_numbering_inverted"] = bit_numbering_inverted_;
		j["enabled"] = enabled_;
		return j;
	}

	void to_json(nlohmann::json& j, const mapping& p)
	{
		j = p.to_json();
	}

	void from_json(const nlohmann::json& j, mapping& p)
	{
		p.from_json(j);
	}
}
