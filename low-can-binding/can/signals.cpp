/*
 * Copyright (C) 2015, 2016 "IoT.bzh"
 * Author "Romain Forlot" <romain.forlot@iot.bzh>
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

#include "signals.hpp"

#include "../binding/application.hpp"
#include "../utils/signals.hpp"
#include "can-decoder.hpp"
#include "can-bus.hpp"
#include "../diagnostic/diagnostic-message.hpp"
#include "canutil/write.h"

std::string signal_t::prefix_ = "messages";

signal_t::signal_t(
	std::string generic_name,
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
	bool is_big_endian,
	bool is_signed,
	std::string unit)
	: parent_{nullptr},
	 generic_name_{ generic_name }
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
	, multiplex_{multiplex}
	, is_big_endian_{is_big_endian}
	, is_signed_{is_signed}
	, unit_{unit}
{}

signal_t::signal_t(
	std::string generic_name,
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
	: generic_name_{ generic_name }
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
{}

std::shared_ptr<message_definition_t> signal_t::get_message() const
{
	return parent_;
}

const std::string signal_t::get_generic_name() const
{
	return generic_name_;
}

const std::string signal_t::get_name() const
{
	return prefix_ + "." + generic_name_;
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

bool signal_t::get_is_big_endian() const
{
	return is_big_endian_;
}

bool signal_t::get_is_signed() const
{
	return is_signed_;
}

const std::string signal_t::get_unit() const
{
	return unit_;
}
