#pragma once

#include <string>
#include <vector>
#include <json.hpp>

#include "signal.hpp"

namespace openxc
{
	class can_message
	{
	private:
		std::string						id_;
		std::string						bus_;
		bool							bit_numbering_inverted_;
		std::vector<signal>				signals_;
		std::string						name_;
		std::vector<std::string>		handlers_;
		bool							enabled_;
		float							max_frequency_;
		float							max_signal_frequency_;
		bool							force_send_changed_;
		bool							force_send_changed_signals_;

	public:
		std::string id() const;
		void id(const std::string& id);
		std::string	bus() const;
		bool bit_numbering_inverted() const;
		const std::vector<signal>& signals() const;
		std::string name() const;
		std::vector<std::string> handlers() const;
		bool enabled() const;
		float max_frequency() const;
		float max_signal_frequency() const;
		bool force_send_changed() const;
		bool force_send_changed_signals() const;
		
		void from_json(const nlohmann::json& j);
		nlohmann::json to_json() const;
		
		std::uint32_t get_signals_count() const;
	};

	void to_json(nlohmann::json& j, const can_message& p);
	void from_json(const nlohmann::json& j, can_message& p);
	void from_json(const nlohmann::json& j, can_message& p, const std::string& id);
}
