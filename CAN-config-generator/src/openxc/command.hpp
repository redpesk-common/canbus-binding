#pragma once

#include <string>
#include <json.hpp>

namespace openxc
{
	class command
	{
	private:
		std::string		name_;
		bool			enabled_;
		std::string		handler_;
	public:
		std::string name() const;
		bool enabled() const;
		std::string handler() const;
		
		void from_json(const nlohmann::json& j);
		nlohmann::json to_json() const;
	};

	void to_json(nlohmann::json& j, const command& p);
	void from_json(const nlohmann::json& j, command& p);
}
