#pragma once

#include <cstdint>
#include <json.hpp>

namespace openxc
{
	enum class can_bus_mode
	{
		off,
		filtered,
		unfiltered
	};

	class can_bus
	{
	private:
		std::uint32_t	controller_;
		std::uint32_t	speed_;
		can_bus_mode	raw_can_mode_;
		bool			raw_writable_;
		float			max_message_frequency_;
		bool			force_send_changed_;

	public:
		std::uint32_t controller() const;
		std::uint32_t speed() const;
		can_bus_mode raw_can_mode() const;
		bool raw_writable() const;
		float max_message_frequency() const;
		bool force_send_changed() const;
		
		void from_json(const nlohmann::json& j);
		nlohmann::json to_json() const;
	};

	void to_json(nlohmann::json& j, const can_bus& p);
	void from_json(const nlohmann::json& j, can_bus& p);

	void to_json(nlohmann::json& j, const can_bus_mode& p);
	void from_json(const nlohmann::json& j, can_bus_mode& p);
}
