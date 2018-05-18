/*
 * Copyright (C) 2015, 2016 "IoT.bzh"
 * Author "Romain Forlot" <romain.forlot@iot.bzh>
 * Author "Loic Collignon" <loic.collignon@iot.bzh>
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

#include "low-can-subscription.hpp"
#include "application.hpp"
#include "canutil/write.h"

low_can_socket_t::low_can_socket_t()
	: index_{-1},
	event_filter_{},
	socket_{}
{}

low_can_socket_t::low_can_socket_t(struct event_filter_t event_filter)
	: event_filter_{event_filter}
 {}

low_can_socket_t::low_can_socket_t( low_can_socket_t&& s)
	: index_{s.index_},
	event_filter_{s.event_filter_},
	socket_{std::move(s.socket_)}
{}

low_can_socket_t& low_can_socket_t::operator=(const low_can_socket_t& s)
{
	socket_ = std::move(s.socket_);
	return *this;
}

low_can_socket_t::~low_can_socket_t()
{
	socket_.close();
}

low_can_socket_t::operator bool() const
{
	return ((can_signal_ != nullptr || ! diagnostic_message_.empty()) && ! socket_);
}

int low_can_socket_t::get_index() const
{
	return index_;
}

const std::shared_ptr<can_signal_t> low_can_socket_t::get_can_signal() const
{
	return can_signal_;
}

bool low_can_socket_t::is_signal_subscription_corresponding(const std::shared_ptr<can_signal_t> can_signal, const struct event_filter_t& event_filter) const
{
	return can_signal_ == can_signal && event_filter_ == event_filter;
}

const std::vector<std::shared_ptr<diagnostic_message_t> > low_can_socket_t::get_diagnostic_message() const
{
	return diagnostic_message_;
}

/// @brief Retrieve a diagnostic_message subscribed from a pid
///
/// @param[in] pid - Diagnostic messages PID to search for
///
/// @return shared_ptr diagnostic_message_ if found and nullptr if not found
const std::shared_ptr<diagnostic_message_t> low_can_socket_t::get_diagnostic_message(uint32_t pid) const
{
	for(const auto& diag: diagnostic_message_)
	{
		if(diag->get_pid() == pid)
		{
			return diag;
		}
	}
	return nullptr;
}

/// @brief Retrieve a diagnostic message search from its name
///
/// @return shared_ptr diagnostic_message_ if found and nullptr if not found
const std::shared_ptr<diagnostic_message_t> low_can_socket_t::get_diagnostic_message(const std::string& name) const
{
	for(const auto& diag: diagnostic_message_)
	{
		if(diag->get_name() == name)
		{
			return diag;
		}
	}
	return nullptr;
}

/// @brief Return the CAN signal name and empty string if not found
/// or no CAN signal subscribed
const std::string low_can_socket_t::get_name() const
{
	if (can_signal_ != nullptr)
		return can_signal_->get_name();
	else if (!diagnostic_message_.empty())
		return "diagnostic_messages";

	AFB_WARNING("No diagnostics messages nor CAN signals registered in that subscription. Name empty ! It's a bug to be reported.");
	return "";
}

/// @brief Return name from a diagnostic message from a PID
///
/// @param[in] pid - Diagnostic message PID
const std::string low_can_socket_t::get_name(uint32_t pid) const
{
	if (!diagnostic_message_.empty())
		return get_diagnostic_message(pid)->get_name() ;

	AFB_WARNING("No diagnostics messages nor CAN signals registered in that subscription. Name empty ! It's a bug to be reported.");
	return "";
}

float low_can_socket_t::get_frequency() const
{
	return event_filter_.frequency;
}

float low_can_socket_t::get_min() const
{
	return event_filter_.min;
}

float low_can_socket_t::get_max() const
{
	return event_filter_.max;
}

utils::socketcan_bcm_t& low_can_socket_t::get_socket()
{
	return socket_;
}

void low_can_socket_t::set_frequency(float freq)
{
	event_filter_.frequency = freq;
}

void low_can_socket_t::set_min(float min)
{
	event_filter_.min = min;
}

void low_can_socket_t::set_max(float max)
{
	event_filter_.max = max;
}
/// @brief Based upon which object is a subscribed CAN signal or diagnostic message
/// it will open the socket with the required CAN bus device name.
///
/// @return INVALID_SOCKET on failure, else positive integer
int low_can_socket_t::open_socket(const std::string& bus_name)
{
	int ret = 0;
	if(! socket_)
	{
		if( can_signal_ != nullptr)
			{ret = socket_.open(can_signal_->get_message()->get_bus_device_name());}
		else if (! diagnostic_message_ .empty())
			{ret = socket_.open(application_t::instance().get_diagnostic_manager().get_bus_device_name());}
		else if ( ! bus_name.empty())
			{ ret = socket_.open(bus_name);}
		index_ = (int)socket_.socket();
	}
	return ret;
}

/// @brief Builds a BCM message head but doesn't set can_frame.
///
/// @returns a simple_bcm_msg with the msg_head parts set and can_frame
/// zeroed.
struct utils::simple_bcm_msg low_can_socket_t::make_bcm_head(uint32_t opcode, uint32_t can_id, uint32_t flags, const struct timeval& timeout, const struct timeval& frequency_thinning) const
{
	struct utils::simple_bcm_msg bcm_msg;
	::memset(&bcm_msg, 0, sizeof(bcm_msg));

	bcm_msg.msg_head.opcode  = opcode;
	bcm_msg.msg_head.can_id  = can_id;
	bcm_msg.msg_head.flags = flags;
	bcm_msg.msg_head.ival1.tv_sec = timeout.tv_sec ;
	bcm_msg.msg_head.ival1.tv_usec = timeout.tv_usec;
	bcm_msg.msg_head.ival2.tv_sec = frequency_thinning.tv_sec ;
	bcm_msg.msg_head.ival2.tv_usec = frequency_thinning.tv_usec;

	return bcm_msg;
}

/// @brief Take an existing simple_bcm_msg struct and add a can_frame.
/// Currently only 1 uniq can_frame can be added, it's not possible to build
/// a multiplexed message with several can_frame.
void low_can_socket_t::add_bcm_frame(const struct can_frame& cf, struct utils::simple_bcm_msg& bcm_msg) const
{
	for(int i=0; i < CAN_MAX_DLEN; i++)
	{
		if(cf.data[i] != 0)
		{
			bcm_msg.msg_head.nframes = 1;
			bcm_msg.frames = cf;
			return;
		}
	}
}

/// @brief Create a RX_SETUP receive job to be used by the BCM socket for a CAN signal
/// subscription
///
/// @return 0 if ok else -1
int low_can_socket_t::create_rx_filter(std::shared_ptr<can_signal_t> sig)
{
	can_signal_= sig;

	struct can_frame cfd;
	memset(&cfd, 0, sizeof(cfd));

	float val = (float)(1 << can_signal_->get_bit_size()) - 1;
	bitfield_encode_float(val,
							can_signal_->get_bit_position(),
							can_signal_->get_bit_size(),
							can_signal_->get_factor(),
							can_signal_->get_offset(),
							cfd.data,
							CAN_MAX_DLEN);

	struct timeval freq, timeout = {0, 0};
	frequency_clock_t f = event_filter_.frequency == 0 ? can_signal_->get_frequency() : frequency_clock_t(event_filter_.frequency);
	freq = f.get_timeval_from_period();

	utils::simple_bcm_msg bcm_msg = make_bcm_head(RX_SETUP, can_signal_->get_message()->get_id(), SETTIMER|RX_NO_AUTOTIMER, timeout, freq);
	add_bcm_frame(cfd, bcm_msg);

	return create_rx_filter(bcm_msg);
}

/// @brief Create a RX_SETUP receive job to be used by the BCM socket for a
/// diagnostic message subscription.
///
/// @return 0 if ok else -1
int low_can_socket_t::create_rx_filter(std::shared_ptr<diagnostic_message_t> sig)
{
	diagnostic_message_.push_back(sig);

	struct timeval freq = frequency_clock_t(event_filter_.frequency).get_timeval_from_period();
	//struct timeval timeout = frequency_clock_t(10).get_timeval_from_period();
	struct timeval timeout = {0,0};

	utils::simple_bcm_msg bcm_msg =  make_bcm_head(RX_SETUP, OBD2_FUNCTIONAL_BROADCAST_ID, SETTIMER|RX_NO_AUTOTIMER|RX_FILTER_ID, timeout, freq);
	return create_rx_filter(bcm_msg);
}

/// @brief Create a RX_SETUP receive job used by the BCM socket directly from
/// a simple_bcm_msg. The method should not be used directly but rather through the
/// two previous method with can_signal_t or diagnostic_message_t object.
///
/// If the CAN arbitration ID is the OBD2 functional broadcast id the subscribed
/// to the 8 classics OBD2 functional response ID
///
/// @return 0 if ok else -1
int low_can_socket_t::create_rx_filter(utils::simple_bcm_msg& bcm_msg)
{
	// Make sure that socket is opened.
	if(open_socket() < 0)
		{return -1;}

	// If it's not an OBD2 CAN ID then just add a simple RX_SETUP job
	// else monitor all standard 8 CAN OBD2 ID response.
	if(bcm_msg.msg_head.can_id != OBD2_FUNCTIONAL_BROADCAST_ID)
	{
		socket_ << bcm_msg;
			if(! socket_)
				return -1;
	}
	else
	{
		for(uint8_t i = 0; i < 8; i++)
		{
			bcm_msg.msg_head.can_id  =  OBD2_FUNCTIONAL_RESPONSE_START + i;

			socket_ << bcm_msg;
			if(! socket_)
				return -1;
		}
	}

	return 0;
}

/// @brief Creates a TX_SEND job that is used by the BCM socket to
/// send a message
///
/// @return 0 if ok, else -1
int low_can_socket_t::tx_send(const struct can_frame& cf, std::shared_ptr<can_signal_t> sig)
{
	can_signal_ = sig;

	utils::simple_bcm_msg bcm_msg =  make_bcm_head(TX_SEND);
	add_bcm_frame(cf, bcm_msg);

	if(open_socket() < 0)
		{return -1;}

	socket_ << bcm_msg;
	if(! socket_)
		return -1;

	return 0;
}

/// @brief Creates a TX_SEND job that is used by the BCM socket to
/// send a message
///
/// @return 0 if ok else -1
int low_can_socket_t::tx_send(const struct can_frame& cf, const std::string& bus_name)
{
	can_signal_ = nullptr;

	utils::simple_bcm_msg bcm_msg =  make_bcm_head(TX_SEND);
	add_bcm_frame(cf, bcm_msg);

	if(open_socket(bus_name) < 0)
		{return -1;}

	socket_ << bcm_msg;
	if(! socket_)
		return -1;

	return 0;
}
