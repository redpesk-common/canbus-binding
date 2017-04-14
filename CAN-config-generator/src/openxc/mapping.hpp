#pragma once

#include <string>
#include <json.hpp>

namespace openxc
{
	class mapping
	{
	private:
		std::string		mapping_name_;
		std::string		bus_;
		std::string		database_;
		bool			bit_numbering_inverted_;
		bool			enabled_;
	public:
		std::string mapping_name() const;
		std::string bus() const;
		std::string database() const;
		bool bit_numbering_inverted() const;
		bool enabled() const;
		
		void from_json(const nlohmann::json& j);
		nlohmann::json to_json() const;
	};

	void to_json(nlohmann::json& j, const mapping& p);
	void from_json(const nlohmann::json& j, mapping& p);
}
