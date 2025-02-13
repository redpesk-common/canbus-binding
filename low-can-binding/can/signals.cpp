/*
 * Copyright (C) 2015-2025 IoT.bzh Company
 * Author: "Romain Forlot" <romain.forlot@iot.bzh>
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *	 http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <fnmatch.h>
#include <memory>

#include <canbus-binding/can/signals.hpp>
#include <canbus-binding/binding/application.hpp>
#include <canbus-binding/can/can-decoder.hpp>
#include <canbus-binding/can/can-encoder.hpp>
#include <canbus-binding/can/can-bus.hpp>
#include <canbus-binding/diagnostic/diagnostic-message.hpp>
#include <canbus-binding/utils/signals.hpp>
#include <canbus-binding/utils/openxc-utils.hpp>

#include "canutil/write.h"

signal_t::signal_t(
	std::string name,
	uint32_t bit_position,
	uint32_t bit_size,
	float factor,
	float offset,
	float min_value,
	float max_value,
	frequency_clock_t frequency,
	bool send_same,
	bool force_send_changed,
	std::map<uint8_t, std::string> states,
	bool writable,
	signal_decoder decoder,
	signal_encoder encoder,
	bool received,
	std::pair<bool, int> multiplex,
	sign_t sign,
	int32_t bit_sign_position,
	std::string unit,
	std::string permission,
	endian_t endian)
	: parent_{nullptr},
	 name_{ name }
	, bit_position_{ bit_position }
	, bit_size_{ bit_size }
	, factor_{ factor }
	, offset_{ offset }
	, min_value_{min_value}
	, max_value_{max_value}
	, frequency_{frequency}
	, send_same_{send_same}
	, force_send_changed_{force_send_changed}
	, states_{states}
	, writable_{writable}
	, decoder_{decoder}
	, encoder_{encoder}
	, received_{received}
	, last_value_{.0f}
	, last_raw_value_{0}
	, multiplex_{multiplex}
	, sign_{sign}
	, bit_sign_position_{bit_sign_position}
	, unit_{unit}
	, permission_{permission}
	, endian_{endian}
{}

signal_t::signal_t(
	std::string name,
	uint32_t bit_position,
	uint32_t bit_size,
	float factor,
	float offset,
	float min_value,
	float max_value,
	frequency_clock_t frequency,
	bool send_same,
	bool force_send_changed,
	std::map<uint8_t, std::string> states,
	bool writable,
	signal_decoder decoder,
	signal_encoder encoder,
	bool received,
	std::pair<bool, int> multiplex,
	sign_t sign,
	int32_t bit_sign_position,
	std::string unit,
	std::string permission)
	: parent_{nullptr},
	 name_{ name }
	, bit_position_{ bit_position }
	, bit_size_{ bit_size }
	, factor_{ factor }
	, offset_{ offset }
	, min_value_{min_value}
	, max_value_{max_value}
	, frequency_{frequency}
	, send_same_{send_same}
	, force_send_changed_{force_send_changed}
	, states_{states}
	, writable_{writable}
	, decoder_{decoder}
	, encoder_{encoder}
	, received_{received}
	, last_value_{.0f}
	, last_raw_value_{0}
	, multiplex_{multiplex}
	, sign_{sign}
	, bit_sign_position_{bit_sign_position}
	, unit_{unit}
	, permission_{permission}
	, endian_{UnsetEndian}
{}

signal_t::signal_t(
	std::string name,
	uint32_t bit_position,
	uint32_t bit_size,
	float factor,
	float offset,
	float min_value,
	float max_value,
	frequency_clock_t frequency,
	bool send_same,
	bool force_send_changed,
	std::map<uint8_t, std::string> states,
	bool writable,
	signal_decoder decoder,
	signal_encoder encoder,
	bool received,
	std::pair<bool, int> multiplex,
	sign_t sign,
	int32_t bit_sign_position,
	std::string unit)
	: parent_{nullptr},
	 name_{ name }
	, bit_position_{ bit_position }
	, bit_size_{ bit_size }
	, factor_{ factor }
	, offset_{ offset }
	, min_value_{min_value}
	, max_value_{max_value}
	, frequency_{frequency}
	, send_same_{send_same}
	, force_send_changed_{force_send_changed}
	, states_{states}
	, writable_{writable}
	, decoder_{decoder}
	, encoder_{encoder}
	, received_{received}
	, last_value_{.0f}
	, last_raw_value_{0}
	, multiplex_{multiplex}
	, sign_{sign}
	, bit_sign_position_{bit_sign_position}
	, unit_{unit}
	, endian_{UnsetEndian}
{}


signal_t::signal_t(
	std::string name,
	uint32_t bit_position,
	uint32_t bit_size,
	float factor,
	float offset,
	float min_value,
	float max_value,
	frequency_clock_t frequency,
	bool send_same,
	bool force_send_changed,
	std::map<uint8_t, std::string> states,
	bool writable,
	signal_decoder decoder,
	signal_encoder encoder,
	bool received)
	: name_{ name }
	, bit_position_{ bit_position }
	, bit_size_{ bit_size }
	, factor_{ factor }
	, offset_{ offset }
	, min_value_{min_value}
	, max_value_{max_value}
	, frequency_{frequency}
	, send_same_{send_same}
	, force_send_changed_{force_send_changed}
	, states_{states}
	, writable_{writable}
	, decoder_{decoder}
	, encoder_{encoder}
	, received_{received}
	, endian_{UnsetEndian}
{}

std::shared_ptr<signal_t> signal_t::get_shared_ptr()
{
	return shared_from_this();
}

std::shared_ptr<message_definition_t> signal_t::get_message() const
{
	return parent_;
}

const std::string signal_t::get_name() const
{
	return name_;
}

uint32_t signal_t::get_bit_position() const
{
	return bit_position_;
}

uint32_t signal_t::get_bit_size() const
{
	return bit_size_;
}

float signal_t::get_factor() const
{
	return factor_;
}

float signal_t::get_offset() const
{
	return offset_;
}

float signal_t::get_min_value() const
{
	return min_value_;
}

float signal_t::get_max_value() const
{
	return max_value_;
}

frequency_clock_t& signal_t::get_frequency()
{
	return frequency_;
}

bool signal_t::get_send_same() const
{
	return send_same_;
}

const std::string signal_t::get_states(uint8_t value)
{
	if ( states_.count(value) > 0 )
		return states_[value];
	return std::string();
}

inline bool caseInsCharCompareN(char a, char b) {
	return(toupper(a) == toupper(b));
}

inline bool caseInsCompare(const std::string& s1, const std::string& s2) {
	return((s1.size() == s2.size()) &&
	equal(s1.begin(), s1.end(), s2.begin(), caseInsCharCompareN));
}

uint64_t signal_t::get_states(const std::string& value) const
{
	uint64_t ret = -1;
	for( const auto& state: states_)
	{
		if(caseInsCompare(state.second, value))
		{
			ret = (uint64_t)state.first;
			break;
		}
	}
	return ret;
}

bool signal_t::get_writable() const
{
	return writable_;
}

signal_decoder& signal_t::get_decoder()
{
	return decoder_;
}

signal_encoder& signal_t::get_encoder()
{
	return encoder_;
}

bool signal_t::get_received() const
{
	return received_;
}

float signal_t::get_last_value() const
{
	return last_value_;
}

uint64_t signal_t::get_last_raw_value() const
{
	return last_raw_value_;
}

json_object* signal_t::afb_verb_get_last_value()
{
	json_object *jobj = json_object_new_object();

	if(! received_)
		return nullptr;

	if(states_.empty())
	{
		json_object_object_add(jobj, get_name().c_str(),
					json_object_new_double(last_value_));
	}
	else
	{
		std::string val = states_[(uint8_t) last_value_];
		json_object_object_add(jobj, get_name().c_str(),
					json_object_new_string(val.c_str()));

	}
	json_object_object_add(jobj, "timestamp",
				json_object_new_double(frequency_.get_last_tick()));

	return jobj;
}

int signal_t::afb_verb_write_on_bus(json_object *json_value)
{
	openxc_DynamicField dynafield_value = build_DynamicField(json_value);
	std::shared_ptr<signal_t> sig = get_shared_ptr();
	std::string bus_name = sig->get_message()->get_bus_device_name();
	bool send = true;

	uint64_t value = encoder_ ?
			encoder_(*this, dynafield_value, &send) :
			encoder_t::encode_DynamicField(*this, dynafield_value, &send);

	uint32_t flags = INVALID_FLAG;

	if(get_message()->is_j1939())
		flags = J1939_PROTOCOL;
	else if(get_message()->is_isotp())
		flags = ISOTP_PROTOCOL;
	else
		flags = CAN_PROTOCOL;

	message_t *message = encoder_t::build_message(sig, value, false, false);

	std::unique_ptr<low_can_subscription_t> sub = std::make_unique<low_can_subscription_t>(low_can_subscription_t());
	sub->set_signal(sig);

	if(flags & CAN_PROTOCOL)
		return sub->tx_send(message, bus_name);
#ifdef USE_FEATURE_ISOTP
	else if(flags & ISOTP_PROTOCOL)
		return sub->isotp_send(message, bus_name);
#endif
#ifdef USE_FEATURE_J1939
	else if(flags & J1939_PROTOCOL)
		return sub->j1939_send(message, bus_name);
#endif
	else
		return -1;

	if(get_message()->is_j1939())
#ifdef USE_FEATURE_J1939
		delete (j1939_message_t*) message;
#else
		AFB_WARNING("Warning: You do not have J1939 feature activated.");
#endif
	else
		delete (can_message_t*) message;

	return -1;
}

std::pair<float, uint64_t> signal_t::get_last_value_with_timestamp() const
{
	return std::make_pair(last_value_, frequency_.get_last_tick());
}

void signal_t::set_parent(std::shared_ptr<message_definition_t> parent)
{
	parent_ = parent;
}

void signal_t::set_received(bool r)
{
	received_ = r;
}

void signal_t::set_last_value(float val)
{
	last_value_ = val;
	last_raw_value_ = (uint64_t)((int64_t)roundf((val - offset_) / factor_));
}

void signal_t::set_last_raw_value(uint64_t val)
{
	last_raw_value_ = val;
	last_value_ = (factor_ * ((int64_t)val)) + offset_;
}

void signal_t::set_timestamp(uint64_t timestamp)
{
	frequency_.tick(timestamp);
}

void signal_t::set_bit_position(uint32_t bit_position)
{
	bit_position_=bit_position;
}

std::pair<bool,int> signal_t::get_multiplex() const
{
	return multiplex_;
}

sign_t signal_t::get_sign() const
{
	return sign_;
}

int32_t signal_t::get_bit_sign_position() const
{
	return bit_sign_position_;
}

const std::string signal_t::get_unit() const
{
	return unit_;
}

const std::string signal_t::get_permission() const
{
	return permission_;
}

endian_t signal_t::get_endian() const
{
	return endian_;
}
