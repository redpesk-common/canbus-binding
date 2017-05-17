#include "can_message.hpp"

namespace openxc
{
	std::string can_message::id() const
	{
		return id_;
	}
	
	void can_message::id(const std::string& id)
	{
		id_ = id;
	}
	
	std::string can_message::bus() const
	{
		return bus_;
	}
	
	bool can_message::bit_numbering_inverted() const
	{
		return bit_numbering_inverted_;
	}
	
	const std::vector<signal>& can_message::signals() const
	{
		return signals_;
	}
	
	std::string can_message::name() const
	{
		return name_;
	}
	
	std::vector<std::string> can_message::handlers() const
	{
		return handlers_;
	}
	
	bool can_message::enabled() const
	{
		return enabled_;
	}
	
	float can_message::max_frequency() const
	{
		return max_frequency_;
	}
	
	float can_message::max_signal_frequency() const
	{
		return max_signal_frequency_;
	}
	
	bool can_message::force_send_changed() const
	{
		return force_send_changed_;
	}
	
	bool can_message::force_send_changed_signals() const
	{
		return force_send_changed_;
	}
	
	void can_message::from_json(const nlohmann::json& j)
	{
		bus_ = j.count("bus") ? j["bus"].get<std::string>() : "";
		bit_numbering_inverted_ = j.count("bit_numbering_inverted") ? j["bit_numbering_inverted"].get<bool>() : false;
		name_ = j.count("name") ? j["name"].get<std::string>() : "";
		handlers_ = j.count("handlers") ? j["handlers"].get<std::vector<std::string>>() : std::vector<std::string>();
		enabled_ = j.count("enabled") ? j["enabled"].get<bool>() : true;
		max_frequency_ = (float)j.count("max_frequency") ? j["max_frequency"].get<float>() : 5;
		max_signal_frequency_ = j.count("max_signal_frequency") ? j["max_signal_frequency"].get<float>() : 5;
		force_send_changed_ = j.count("force_send_changed") ? j["force_send_changed"].get<bool>() : true;
		force_send_changed_signals_ = j.count("force_send_changed_signals") ? j["force_send_changed_signals"].get<bool>() : false;
		
		if(j.count("signals"))
		{
			std::map<std::string, nlohmann::json> signals = j["signals"];
			for(const auto& s : signals)
			{
				signal sig = s.second.get<signal>();
				sig.id(s.first);
				signals_.push_back(sig);
			}
		}
		
	}
	
	std::uint32_t can_message::get_signals_count() const
	{
		return (uint32_t)signals_.size();
	}

	nlohmann::json can_message::to_json() const
	{
		nlohmann::json j;
		j["bus"] = bus_;
		j["bit_numbering_inverted"] = bit_numbering_inverted_;
		j["signals"] = signals_;
		j["name"] = name_;
		j["handlers"] = handlers_;
		j["enabled"] = enabled_;
		j["max_frequency"] = max_frequency_;
		j["max_signal_frequency"] = max_signal_frequency_;
		j["force_send_changed"] = force_send_changed_;
		j["force_send_changed_signals"] = force_send_changed_signals_;
		return j;
	}

	void to_json(nlohmann::json& j, const can_message& p)
	{
		j = p.to_json();
	}
	
	void from_json(const nlohmann::json& j, can_message& p)
	{
		p.from_json(j);
	}

	void from_json(const nlohmann::json& j, can_message& p, const std::string& id)
	{
		p.from_json(j);
		p.id(id);
	}
}
