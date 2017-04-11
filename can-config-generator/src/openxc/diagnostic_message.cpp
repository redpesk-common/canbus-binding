#include "diagnostic_message.hpp"

namespace openxc
{
	std::string diagnostic_message::bus() const
	{
		return bus_;
	}
	
	std::uint32_t diagnostic_message::id() const
	{
		return id_;
	}
	
	std::uint32_t diagnostic_message::mode() const
	{
		return mode_;
	}
	
	float diagnostic_message::frequency() const
	{
		return frequency_;
	}
	
	std::uint32_t diagnostic_message::pid() const
	{
		return pid_;
	}
	
	std::string diagnostic_message::name() const
	{
		return name_;
	}
	
	std::string diagnostic_message::decoder() const
	{
		return decoder_;
	}
	
	std::string diagnostic_message::callback() const
	{
		return callback_;
	}
	
	void diagnostic_message::from_json(const nlohmann::json& j)
	{
		bus_ = j.count("bus") ? j["bus"].get<std::string>() : "";
		id_ = j.count("id") ? j["id"].get<std::uint32_t>() : 0;
		mode_ = j.count("mode") ? j["mode"].get<std::uint32_t>() : 1;
		frequency_ = j.count("frequency") ? j["frequency"].get<float>() : 0;
		pid_ = j.count("pid") ? j["pid"].get<std::uint32_t>() : 0;
		name_ = j.count("name") ? j["name"].get<std::string>() : "";
		decoder_ = j.count("decoder") ? j["decoder"].get<std::string>() : "";
		callback_ = j.count("callback") ? j["callback"].get<std::string>() : "";
	}

	nlohmann::json diagnostic_message::to_json() const
	{
		nlohmann::json j;
		j["bus"] = bus_;
		j["id"] = id_;
		j["mode"] = mode_;
		j["frequency"] = frequency_;
		j["pid"] = pid_;
		j["name"] = name_;
		j["decoder"] = decoder_;
		j["callback"] = callback_;
		return j;
	}

	void to_json(nlohmann::json& j, const diagnostic_message& p)
	{
		j = p.to_json();
	}

	void from_json(const nlohmann::json& j, diagnostic_message& p)
	{
		p.from_json(j);
	}
}
