#pragma once

#include <string>
#include <json.hpp>

namespace openxc
{
	class diagnostic_message
	{
	private:
		std::string		bus_;
		std::uint32_t	id_;
		std::uint32_t	mode_;
		float			frequency_;
		std::uint32_t	pid_;
		std::string		name_;
		std::string		decoder_;
		std::string		callback_;
	public:
		std::string bus() const;
		std::uint32_t id() const;
		std::uint32_t mode() const;
		float frequency() const;
		std::uint32_t pid() const;
		std::string name() const;
		std::string decoder() const;
		std::string callback() const;
		
		void from_json(const nlohmann::json& j);
		nlohmann::json to_json() const;
	};

	void to_json(nlohmann::json& j, const diagnostic_message& p);
	void from_json(const nlohmann::json& j, diagnostic_message& p);
}
