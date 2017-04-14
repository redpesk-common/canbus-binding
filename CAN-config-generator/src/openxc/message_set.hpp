#pragma once

#include <string>
#include <vector>
#include <map>
#include <json.hpp>

#include "can_bus.hpp"
#include "can_message.hpp"
#include "diagnostic_message.hpp"
#include "mapping.hpp"
#include "command.hpp"

namespace openxc
{
	class message_set
	{
	private:
		std::string							name_;
		bool								bit_numbering_inverted_;
		float								max_message_frequency_;
		can_bus_mode						raw_can_mode_;
		std::vector<std::string>			parents_;
		std::vector<std::string>			initializers_;
		std::vector<std::string>			loopers_;
		std::map<std::string, can_bus>		buses_;
		//std::map<std::string, can_message>	messages_;
		std::vector<can_message>			messages_;
		std::vector<diagnostic_message>		diagnostic_messages_;
		std::vector<mapping>				mappings_;
		std::vector<std::string>			extra_sources_;
		std::vector<command>				commands_;

	public:
		message_set();
		message_set(const message_set&) = default;
		message_set(message_set&&) = default;
		
		std::string name() const;
		bool bit_numbering_inverted() const;
		float max_message_frequency() const;
		can_bus_mode raw_can_mode() const;
		const std::vector<std::string>& parents() const;
		const std::vector<std::string>& initializers() const;
		const std::vector<std::string>& loopers() const;
		const std::map<std::string, can_bus>& buses() const;
		const std::vector<can_message>& messages() const;
		const std::vector<diagnostic_message>& diagnostic_messages() const;
		const std::vector<mapping>& mappings() const;
		const std::vector<std::string>& extra_sources() const;
		const std::vector<command>& commands() const;
		
		void from_json(const nlohmann::json& j);
		nlohmann::json to_json() const;
	};

	void to_json(nlohmann::json& j, const message_set& p);
	void from_json(const nlohmann::json& j, message_set& p);
}
