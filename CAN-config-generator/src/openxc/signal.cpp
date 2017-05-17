#include "signal.hpp"

namespace openxc
{
	std::string signal::id() const
	{
		return id_;
	}
	
	void signal::id(const std::string& id)
	{
		id_ = id;
	}
	
	void id(const std::string& id);
	
	std::string signal::generic_name() const
	{
		return generic_name_;
	}
	
	std::uint32_t signal::bit_position() const
	{
		return bit_position_;
	}
	
	std::uint32_t signal::bit_size() const
	{
		return bit_size_;
	}
	
	float signal::factor() const
	{
		return factor_;
	}
	
	float signal::offset() const
	{
		return offset_;
	}
	
	std::string signal::decoder() const
	{
		return decoder_;
	}
	
	bool signal::ignore() const
	{
		return ignore_;
	}
	
	bool signal::enabled() const
	{
		return enabled_;
	}
	
	const std::map<std::string, std::vector<std::uint32_t>>& signal::states() const
	{
		return states_;
	}
	
	float signal::max_frequency() const
	{
		return max_frequency_;
	}
	
	bool signal::send_same() const
	{
		return send_same_;
	}
	
	bool signal::force_send_changed() const
	{
		return force_send_changed_;
	}
	
	bool signal::writable() const
	{
		return writable_;
	}
	
	std::string signal::encoder() const
	{
		return encoder_;
	}
	
	void signal::from_json(const nlohmann::json& j)
	{
		generic_name_ = j.count("generic_name") ? j["generic_name"].get<std::string>() : "";
		bit_position_ = j.count("bit_position") ? j["bit_position"].get<std::uint32_t>() : 0;
		bit_size_ = j.count("bit_size") ? j["bit_size"].get<std::uint32_t>() : 0;
		factor_ = j.count("factor") ? j["factor"].get<float>() : 1.0f;
		offset_ = j.count("offset") ? j["offset"].get<float>() : 0.0f;
		decoder_ = j.count("decoder") ? j["decoder"].get<std::string>() : "";
		ignore_ = j.count("ignore") ? j["ignore"].get<bool>() : false;
		enabled_ = j.count("enabled") ? j["enabled"].get<bool>() : true;
		max_frequency_ = j.count("max_frequency") ? j["max_frequency"].get<float>() : 0.0f;
		send_same_ = j.count("send_same") ? j["send_same"].get<bool>() : true;
		force_send_changed_ = j.count("force_send_changed") ? j["force_send_changed"].get<bool>() : false;
		writable_ = j.count("writable") ? j["writable"].get<bool>() : false;
		encoder_ = j.count("encoder") ? j["encoder"].get<std::string>() : "";

		if (j.count("states"))
		{
			std::map<std::string, nlohmann::json> items = j["states"];
			for(const auto& i : items)
			{
				states_[i.first] = i.second.get<std::vector<std::uint32_t>>();
			}
		}
	}

	nlohmann::json signal::to_json() const
	{
		nlohmann::json j;
		j["generic_name"] = generic_name_;
		j["bit_position"] = bit_position_;
		j["bit_size"] = bit_size_;
		j["factor"] = factor_;
		j["offset"] = offset_;
		j["decoder"] = decoder_;
		j["ignore"] = ignore_;
		j["enabled"] = enabled_;
		j["states"] = states_;
		j["max_frequency"] = max_frequency_;
		j["send_same"] = send_same_;
		j["force_send_changed"] = force_send_changed_;
		j["writable"] = writable_;
		j["encoder"] = encoder_;
		return j;
	}

	void to_json(nlohmann::json& j, const signal& p)
	{
		j = p.to_json();
	}

	void from_json(const nlohmann::json& j, signal& p)
	{
		p.from_json(j);
	}
}
