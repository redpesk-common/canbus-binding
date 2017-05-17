#include <sstream>
#include "message_set.hpp"

namespace openxc
{
	message_set::message_set()
		: name_{""}
		, bit_numbering_inverted_{false}
		, max_message_frequency_{0}
		, raw_can_mode_{can_bus_mode::off}
		, parents_{}
		, initializers_{}
		, loopers_{}
		, buses_{}
		, messages_{}
		, diagnostic_messages_{}
		, mappings_{}
		, extra_sources_{}
		, commands_{}
	{
	}
	
	std::string message_set::name() const
	{
		return name_;
	}
	
	bool message_set::bit_numbering_inverted() const
	{
		return bit_numbering_inverted_;
	}
	
	float message_set::max_message_frequency() const
	{
		return max_message_frequency_;
	}
	
	can_bus_mode message_set::raw_can_mode() const
	{
		return raw_can_mode_;
	}
	
	const std::vector<std::string>& message_set::parents() const
	{
		return parents_;
	}
	
	const std::vector<std::string>& message_set::initializers() const
	{
		return initializers_;
	}
	
	const std::vector<std::string>& message_set::loopers() const
	{
		return loopers_;
	}
	
	const std::map<std::string, can_bus>& message_set::buses() const
	{
		return buses_;
	}
	
	const std::vector<can_message>& message_set::messages() const
	{
		return messages_;
	}
	
	const std::vector<diagnostic_message>& message_set::diagnostic_messages() const
	{
		return diagnostic_messages_;
	}
	
	const std::vector<mapping>& message_set::mappings() const
	{
		return mappings_;
	}
	
	const std::vector<std::string>& message_set::extra_sources() const
	{
		return extra_sources_;
	}
	
	const std::vector<command>& message_set::commands() const
	{
		return commands_;
	}

	void message_set::from_json(const nlohmann::json& j)
	{
		name_ = j["name"].get<std::string>();
		bit_numbering_inverted_ = j.count("bit_numbering_inverted") ? j["bit_numbering_inverted"].get<bool>() : false; // TODO: should be true by default if database-backed.
		max_message_frequency_ = j.count("max_message_frequency") ? j["max_message_frequency"].get<float>() : 0.0f;
		raw_can_mode_ = j.count("raw_can_mode") ? j["raw_can_mode"].get<can_bus_mode>() : can_bus_mode::off;
		parents_ = j.count("parents") ? j["parents"].get<std::vector<std::string>>() : std::vector<std::string>();
		initializers_ = j.count("initializers") ? j["initializers"].get<std::vector<std::string>>() : std::vector<std::string>();
		loopers_ = j.count("loopers") ? j["loopers"].get<std::vector<std::string>>() : std::vector<std::string>();
		buses_ = j.count("buses") ? j["buses"].get<std::map<std::string, can_bus>>() : std::map<std::string, can_bus>();
		//messages_ = j.count("messages") ? j["messages"].get<std::map<std::string, can_message>>() : std::map<std::string, can_message>();
		diagnostic_messages_ = j.count("diagnostic_messages") ? j["diagnostic_messages"].get<std::vector<diagnostic_message>>() : std::vector<diagnostic_message>();
		mappings_ = j.count("mappings") ? j["mappings"].get<std::vector<mapping>>() : std::vector<mapping>();
		extra_sources_ = j.count("extra_sources") ? j["extra_sources"].get<std::vector<std::string>>() : std::vector<std::string>();
		commands_ = j.count("commands") ? j["commands"].get<std::vector<command>>() : std::vector<command>();
		
		
		if (j.count("messages"))
		{
			std::map<std::string, nlohmann::json> messages = j["messages"];
			for(const std::map<std::string, nlohmann::json>::value_type& m : messages)
			{
				can_message cm = m.second.get<can_message>();
				cm.id(m.first);
				messages_.push_back(cm);
			}
		}
	}

	nlohmann::json message_set::to_json() const
	{
		nlohmann::json j;
		j["name_"]						= name_;
		j["bit_numbering_inverted"]		= bit_numbering_inverted_;
		j["max_message_frequency"]		= max_message_frequency_;
		j["raw_can_mode"]				= raw_can_mode_;
		j["parents"]					= parents_;
		j["initializers"]				= initializers_;
		j["loopers"]					= loopers_;
		j["buses"]						= buses_;
		j["messages"]					= messages_;
		j["diagnostic_messages"]		= diagnostic_messages_;
		j["mappings"]					= mappings_;
		j["extra_sources"]				= extra_sources_;
		j["commands"]					= commands_;
		return j;
	}

	void to_json(nlohmann::json& j, const message_set& p)
	{
		j = p.to_json();
	}

	void from_json(const nlohmann::json& j, message_set& p)
	{
		p.from_json(j);
	}
}
