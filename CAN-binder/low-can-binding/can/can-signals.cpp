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

#include "can-signals.hpp"

#include "../binding/configuration.hpp"
#include "../utils/signals.hpp"
#include "can-decoder.hpp"
#include "can-message.hpp"
#include "can-bus.hpp"
#include "../diagnostic/diagnostic-message.hpp"
#include "canutil/write.h"

std::string can_signal_t::prefix_ = "messages";

can_signal_t::can_signal_t(std::uint8_t message_set_id,
	std::uint8_t message_id,
	std::string generic_name,
	uint8_t bit_position,
	uint8_t bit_size,
	float factor,
	float offset,
	float min_value,
	float max_value,
	frequency_clock_t frequency,
	bool send_same,
	bool force_send_changed,
	std::map<uint8_t, std::string> states,
	bool writable,
	SignalDecoder decoder,
	SignalEncoder encoder,
	bool received)
	: message_set_id_{ message_set_id }
	, message_id_{ message_id }
	, generic_name_{ generic_name }
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
{}

utils::socketcan_bcm_t can_signal_t::get_socket() const
{
	return socket_;
}

can_message_definition_t& can_signal_t::get_message() const
{
	return configuration_t::instance().get_can_message_definition(message_set_id_, message_id_);
}

const std::string& can_signal_t::get_generic_name() const
{
	return generic_name_;
}

const std::string can_signal_t::get_name() const
{
	return prefix_ + "." + generic_name_;
}

const std::string& can_signal_t::get_prefix() const
{
	return prefix_;
}

uint8_t can_signal_t::get_bit_position() const
{
	return bit_position_;
}

uint8_t can_signal_t::get_bit_size() const
{
	return bit_size_;
}

float can_signal_t::get_factor() const
{
	return factor_;
}

float can_signal_t::get_offset() const
{
	return offset_;
}

float can_signal_t::get_min_value() const
{
	return min_value_;
}	

float can_signal_t::get_max_value() const
{
	return max_value_;
}

frequency_clock_t& can_signal_t::get_frequency()
{
	return frequency_;
}

bool can_signal_t::get_send_same() const
{
	return send_same_;
}

bool can_signal_t::get_force_send_changed() const
{
	return force_send_changed_;
}

const std::map<uint8_t, std::string>& can_signal_t::get_states() const
{
	return states_;
}

const std::string can_signal_t::get_states(uint8_t value)
{
	if (value < states_.size())
		return states_[value];
	return std::string();
}

size_t can_signal_t::get_state_count() const
{
	return states_.size();
}

bool can_signal_t::get_writable() const
{
	return writable_;
}

SignalDecoder& can_signal_t::get_decoder()
{
	return decoder_;
}

SignalEncoder& can_signal_t::get_encoder()
{
	return encoder_;
}

bool can_signal_t::get_received() const
{
	return received_;
}
float can_signal_t::get_last_value() const
{
	return last_value_;
}

void can_signal_t::set_prefix(std::string val)
{
	prefix_ = val;
}

void can_signal_t::set_received(bool r)
{
	received_ = r;
}

void can_signal_t::set_last_value(float val)
{
	last_value_ = val;
}

/// @brief Create a RX_SETUP receive job using the BCM socket.
///
/// @return 0 if ok else -1
int can_signal_t::create_rx_filter()
{
	// Make sure that socket has been opened.
	if(! socket_)
		socket_.open(
			get_message().get_bus_name());

	uint32_t can_id  = get_message().get_id();

	struct utils::simple_bcm_msg bcm_msg;
	struct can_frame cfd;

	memset(&cfd, 0, sizeof(cfd));
	memset(&bcm_msg.msg_head, 0, sizeof(bcm_msg.msg_head));
	float val = (float)(1 << bit_size_)-1;
	float freq = frequency_.frequency_to_period();
	if(freq <= 0)
		freq = 0.000001f;

	bcm_msg.msg_head.opcode  = RX_SETUP;
	bcm_msg.msg_head.can_id  = can_id;
	bcm_msg.msg_head.flags = SETTIMER;
	bcm_msg.msg_head.ival2.tv_sec = long(freq);
	bcm_msg.msg_head.ival2.tv_usec = (freq - (long)freq) * 1000000;
	bcm_msg.msg_head.nframes = 1;
	bitfield_encode_float(val,
										bit_position_,
										bit_size_,
										factor_,
										offset_,
										cfd.data,
										CAN_MAX_DLEN);

	bcm_msg.frames = cfd;

	if(socket_ << bcm_msg)
		return 0;
	return -1;
}

void can_signal_t::read_socket()
{
	can_message_t msg;
	can_bus_t& cbm = configuration_t::instance().get_can_bus_manager();
	socket_ >> msg;
	std::lock_guard<std::mutex> can_message_lock(cbm.get_can_message_mutex());
	{ cbm.push_new_can_message(msg); }
	cbm.get_new_can_message_cv().notify_one();
}