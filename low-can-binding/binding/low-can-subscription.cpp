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
#include "../utils/socketcan-bcm.hpp"
#include "../can/can-encoder.hpp"

#ifdef USE_FEATURE_ISOTP
#include "../utils/socketcan-isotp.hpp"
#endif

#ifdef USE_FEATURE_J1939
#include "../utils/socketcan-j1939/socketcan-j1939-data.hpp"
#endif

low_can_subscription_t::low_can_subscription_t()
	: index_{-1},
	event_filter_{},
	event_{},
	socket_{}
{}

low_can_subscription_t::low_can_subscription_t(struct event_filter_t event_filter)
	: index_{-1},
	  event_filter_{event_filter},
	  event_{},
	  socket_{}
 {}

low_can_subscription_t::low_can_subscription_t( low_can_subscription_t&& s)
	: index_{s.index_},
	event_filter_{s.event_filter_},
	event_{},
	socket_{std::move(s.socket_)}
{}

low_can_subscription_t& low_can_subscription_t::operator=(const low_can_subscription_t& s)
{
	socket_ = std::move(s.socket_);
	return *this;
}

low_can_subscription_t::~low_can_subscription_t()
{
	if(socket_)
		socket_->close();
}

low_can_subscription_t::operator bool() const
{
	return ((signal_ != nullptr || ! diagnostic_message_.empty()) && ! socket_);
}
afb_event_t low_can_subscription_t::get_event()
{
	return event_;
}

/**
 * @brief Set the event calling the afb_daemon_make_event function to
 * create it and the checks its validity.
 *
 * @return int - 0 if OK, -1 if not
 */
int low_can_subscription_t::set_event()
{
	std::string event_name = get_name();
	event_ = afb_daemon_make_event(event_name.c_str());
	if (! afb_event_is_valid(event_))
	{
		AFB_ERROR("Can't create an event for %s, something goes wrong.", event_name.c_str());
		return -1;
	}

	return 0;
}

/**
 * @brief Subscribe to the event member of the object
 *
 * @param request the subscribe AFB client request which want to
 * subscribe
 *
 * @return int - 0 if OK, -1 if not
 */
int low_can_subscription_t::subscribe(afb_req_t request)
{
	if(! afb_event_is_valid(event_))
		if(set_event() < 0)
			return -1;

	return afb_req_subscribe(request, event_);
}

/**
 * @brief Unsubscribe to the event member of the object
 *
 * @param request the unsubscribe AFB client request which want to
 * unsubscribe
 *
 * @return int - 0 if OK, -1 if not
 */
int low_can_subscription_t::unsubscribe(afb_req_t request)
{
	return afb_req_unsubscribe(request, event_);
}

/**
 * @brief Getter of index of subscription
 *
 * @return int Index
 */
int low_can_subscription_t::get_index() const
{
	return index_;
}

/**
 * @brief Getter of signal of subscription
 *
 * @return const std::shared_ptr<signal_t> A shared pointer of the signal
 */
const std::shared_ptr<signal_t> low_can_subscription_t::get_signal() const
{
	return signal_;
}

/**
 * @brief Check if the signal and event are the same that the subscription
 *
 * @param signal the signal compared
 * @param event_filter the event_filter compared
 * @return true if they are equal
 * @return false if they are not equal
 */
bool low_can_subscription_t::is_signal_subscription_corresponding(const std::shared_ptr<signal_t> signal, const struct event_filter_t& event_filter) const
{
	return signal_ == signal && event_filter_ == event_filter;
}

/**
 * @brief Getter for diagnostic messages of subscription
 *
 * @return const vector_ptr_diag_msg_t Vector of pointer of diagnostic message
 */
const vect_ptr_diag_msg_t low_can_subscription_t::get_diagnostic_message() const
{
	return diagnostic_message_;
}

/// @brief Retrieve a diagnostic_message subscribed from a pid
///
/// @param[in] pid - Diagnostic messages PID to search for
///
/// @return shared_ptr diagnostic_message_ if found and nullptr if not found
const std::shared_ptr<diagnostic_message_t> low_can_subscription_t::get_diagnostic_message(uint32_t pid) const
{
	for(const auto& diag: diagnostic_message_)
		if(diag->get_pid() == pid)
			return diag;

	return nullptr;
}

/// @brief Retrieve a diagnostic message search from its name
///
/// @return shared_ptr diagnostic_message_ if found and nullptr if not found
const std::shared_ptr<diagnostic_message_t> low_can_subscription_t::get_diagnostic_message(const std::string& name) const
{
	for(const auto& diag: diagnostic_message_)
		if(diag->get_name() == name)
			return diag;

	return nullptr;
}

/// @brief Return the CAN signal name and empty string if not found
/// or no CAN signal subscribed
const std::string low_can_subscription_t::get_name() const
{
	if (signal_ != nullptr)
		return signal_->get_name();
	else if (!diagnostic_message_.empty())
		return "diagnostic_messages";

	AFB_WARNING("No diagnostics messages nor CAN signals registered in that subscription. Name empty ! It's a bug to be reported.");
	return "";
}

/// @brief Return name from a diagnostic message from a PID
///
/// @param[in] pid - Diagnostic message PID
const std::string low_can_subscription_t::get_name(uint32_t pid) const
{
	if (!diagnostic_message_.empty())
		return get_diagnostic_message(pid)->get_name() ;

	AFB_WARNING("No diagnostics messages nor CAN signals registered in that subscription. Name empty ! It's a bug to be reported.");
	return "";
}

/**
 * @brief Getter of the frequency of the event_filter
 *
 * @return float The frequency
 */
float low_can_subscription_t::get_frequency() const
{
	return event_filter_.frequency;
}

/**
 * @brief Getter of the min of the event_filter
 *
 * @return float The min value filtered
 */
float low_can_subscription_t::get_min() const
{
	return event_filter_.min;
}

/**
 * @brief Getter of the max of the event_filter
 *
 * @return float The max value filtered
 */
float low_can_subscription_t::get_max() const
{
	return event_filter_.max;
}

bool low_can_subscription_t::get_promisc() const
{
	return event_filter_.promisc;
}

/**
 * @brief Getter of the rx_id of the event_filter
 *
 * @return canid_t The rx_id value
 */
canid_t low_can_subscription_t::get_rx_id() const
{
	return event_filter_.rx_id;
}

/**
 * @brief Getter of the tx_id of the event_filter
 *
 * @return canid_t The tx_id value
 */
canid_t low_can_subscription_t::get_tx_id() const
{
	return event_filter_.tx_id;
}

/**
 * @brief Getter of the socket of the subscription
 *
 * @return std::shared_ptr<utils::socketcan_t> Pointer of the socket object
 */
std::shared_ptr<utils::socketcan_t> low_can_subscription_t::get_socket()
{
	return socket_;
}

std::shared_ptr<message_definition_t> low_can_subscription_t::get_message_definition()
{
	return message_;
}

/**
 * @brief Setter for the frequency of the event_filter
 *
 * @param freq The new frequency
 */
void low_can_subscription_t::set_frequency(float freq)
{
	event_filter_.frequency = freq;
}

/**
 * @brief Setter for the min of the event_filter
 *
 * @param min The new min
 */
void low_can_subscription_t::set_min(float min)
{
	event_filter_.min = min;
}

/**
 * @brief Setter for the max of the event_filter
 *
 * @param max The new max
 */
void low_can_subscription_t::set_max(float max)
{
	event_filter_.max = max;
}

void low_can_subscription_t::set_promisc(bool promisc)
{
	event_filter_.promisc = promisc;
}

/**
 * @brief Setter for the rx_id of the event_filter
 *
 * @param rx_id The new rx_id
 */
void low_can_subscription_t::set_rx_id(canid_t rx_id)
{
	event_filter_.rx_id = rx_id;
}

/**
 * @brief Setter for the tx_id of the event_filter
 *
 * @param tx_id The new tx_id
 */
void low_can_subscription_t::set_tx_id(canid_t tx_id)
{
	event_filter_.tx_id = tx_id;
}

/**
 * @brief Setter for the index of the subscription
 *
 * @param index The new index
 */
void low_can_subscription_t::set_index(int index)
{
	index_ = index;
}

/**
 * @brief Setter for the signal of the subscription
 *
 * @param signal The new signal
 */
void low_can_subscription_t::set_signal(std::shared_ptr<signal_t> signal)
{
	signal_ = signal;
}

void low_can_subscription_t::set_message_definition(std::shared_ptr<message_definition_t> message)
{
	message_ = message;
}

/// @brief Based upon which object is a subscribed CAN signal or diagnostic message
/// it will open the socket with the required CAN bus device name.
///
/// @return INVALID_SOCKET on failure, else positive integer
int low_can_subscription_t::open_socket(low_can_subscription_t &subscription, const std::string& bus_name,  uint32_t flags)
{
	int ret = -1;
	if(! subscription.socket_)
	{
		if(flags & CAN_PROTOCOL)
		{
			subscription.socket_ = std::make_shared<utils::socketcan_bcm_t>();
			if( subscription.signal_ )
				ret = subscription.socket_->open(subscription.signal_->get_message()->get_bus_device_name());
			else if(! subscription.diagnostic_message_.empty())
				ret = subscription.socket_->open(application_t::instance().get_diagnostic_manager().get_bus_device_name());
			else if(! bus_name.empty())
				ret = subscription.socket_->open(bus_name);

			subscription.index_ = (int)subscription.socket_->socket();
		}
#ifdef USE_FEATURE_ISOTP
		else if(flags & ISOTP_PROTOCOL)
		{
			std::shared_ptr<utils::socketcan_isotp_t> socket = std::make_shared<utils::socketcan_isotp_t>();
			if(subscription.signal_ )
			{
				canid_t rx = NO_CAN_ID;
				canid_t tx = NO_CAN_ID;
				if(flags & ISOTP_SEND)
				{
					rx = subscription.get_rx_id();
					tx = subscription.signal_->get_message()->get_id();
				}
				else if(flags & ISOTP_RECEIVE)
				{
					rx = subscription.signal_->get_message()->get_id();
					tx = subscription.get_tx_id();
				}
				ret = socket->open(subscription.signal_->get_message()->get_bus_device_name(), rx, tx);
				subscription.socket_ = socket;
			}
			else if(! bus_name.empty())
			{
				ret = socket->open(bus_name, subscription.get_rx_id(), subscription.get_tx_id());
				subscription.socket_ = socket;
			}
			subscription.index_ = (int)subscription.socket_->socket();
		}
#endif
#ifdef USE_FEATURE_J1939
		else if(flags & J1939_ADDR_CLAIM_PROTOCOL)
		{
			pgn_t pgn = J1939_NO_PGN;
			std::shared_ptr<utils::socketcan_j1939_addressclaiming_t> socket = std::make_shared<utils::socketcan_j1939_addressclaiming_t>();
			if(!bus_name.empty())
				ret = socket->open(bus_name, pgn);
			subscription.socket_ = socket;
			subscription.index_ = (int)subscription.socket_->socket();
		}
		else if(flags & J1939_PROTOCOL)
		{
			pgn_t pgn = J1939_NO_PGN;
			std::shared_ptr<utils::socketcan_j1939_data_t> socket = std::make_shared<utils::socketcan_j1939_data_t>();
			if(subscription.signal_)
			{
				pgn = subscription.signal_->get_message()->get_id();
				ret = socket->open(subscription.signal_->get_message()->get_bus_device_name(), pgn);
			}
			else if(!bus_name.empty())
			{
				ret = socket->open(bus_name, pgn);
			}

			if(ret)
				socket->define_opt(!j1939_pgn_is_pdu1(pgn),subscription.event_filter_.promisc);

			subscription.socket_ = socket;
			subscription.index_ = (int)subscription.socket_->socket();
		}
#endif
		else
		{
			AFB_ERROR("Socket format not supported");
			return INVALID_SOCKET;
		}
	}
	else{
		ret = subscription.socket_->socket();
	}
	return ret;
}


/// @brief Builds a BCM message head but doesn't set can_frame.
///
/// @returns a bcm_msg with the msg_head parts set and can_frame
/// zeroed.
struct bcm_msg low_can_subscription_t::make_bcm_head(uint32_t opcode, uint32_t can_id, uint32_t flags, const struct timeval& timeout, const struct timeval& frequency_thinning)
{
	struct bcm_msg bcm_msg;
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

/// @brief Take an existing bcm_msg struct and add a can_frame.
/// Currently only 1 uniq can_frame can be added, it's not possible to build
/// a multiplexed message with several can_frame.
void low_can_subscription_t::add_one_bcm_frame(struct canfd_frame& cfd, struct bcm_msg& bcm_msg)
{
	struct can_frame cf;

	if (bcm_msg.msg_head.flags & CAN_FD_FRAME)
		bcm_msg.fd_frames[bcm_msg.msg_head.nframes] = cfd;
	else
	{
		cf.can_id = cfd.can_id;
		cf.can_dlc = cfd.len;
		::memcpy(&cf.data, cfd.data, cfd.len);
		bcm_msg.frames[bcm_msg.msg_head.nframes] = cf;
	}
	bcm_msg.msg_head.nframes++;
}

/// @brief Take an existing bcm_msg struct and add a can_frame.
/// Currently only 1 uniq can_frame can be added, it's not possible to build
/// a multiplexed message with several can_frame.
void low_can_subscription_t::remove_last_bcm_frame(struct bcm_msg& bcm_msg)
{
	struct canfd_frame cf;
	memset(&cf, 0, sizeof(cf));
	bcm_msg.fd_frames[bcm_msg.msg_head.nframes] = cf;
	bcm_msg.msg_head.nframes--;
}

#ifdef USE_FEATURE_J1939
/**
 * @brief Create a j1939 socket to read message
 *
 * @param subscription The subscription
 * @param sig The signal subscribed
 * @return int 0 if ok else -1
 */
int low_can_subscription_t::create_rx_filter_j1939(low_can_subscription_t &subscription, std::shared_ptr<signal_t> sig)
{
	subscription.signal_= sig;

	// Make sure that socket is opened.
	if(open_socket(subscription, "", J1939_PROTOCOL) < 0)
			return -1;

	return 0;
}
#endif

/**
 * @brief Create an iso tp socket to read message
 *
 * @param subscription The subscription
 * @param sig The signal subscribed
 * @return int 0 if ok else -1
 */
int low_can_subscription_t::create_rx_filter_isotp(low_can_subscription_t &subscription, std::shared_ptr<signal_t> sig)
{
	subscription.signal_= sig;

	// Make sure that socket is opened.
	if(open_socket(subscription, "", ISOTP_PROTOCOL|ISOTP_RECEIVE) < 0)
			return -1;

	return 0;
}

/// @brief Create a RX_SETUP receive job to be used by the BCM socket for a CAN signal
/// subscription
///
/// @return 0 if ok else -1
int low_can_subscription_t::create_rx_filter_can(low_can_subscription_t &subscription, std::shared_ptr<signal_t> sig)
{
	uint32_t flags_bcm;
	struct timeval freq, timeout = {0, 0};
	subscription.signal_= sig;
	bool is_fd = sig->get_message()->is_fd();
	uint32_t max_dlen = 0;

	uint32_t length_msg = sig->get_message()->get_length();
	std::vector<uint8_t> data(length_msg);
	can_message_t cm;

	if(! length_msg)
	{
		AFB_ERROR("Error in the length of message with id %d", sig->get_message()->get_id());
		return -1;
	}

	encoder_t::encode_data(subscription.signal_, data, true, false, true);

	if (is_fd)
	{
		flags_bcm = SETTIMER|RX_NO_AUTOTIMER|CAN_FD_FRAME;
		max_dlen = CANFD_MAX_DLEN;
	}
	else
	{
		flags_bcm = SETTIMER|RX_NO_AUTOTIMER;
		max_dlen = CAN_MAX_DLEN;
	}

	cm = can_message_t( max_dlen,
			    sig->get_message()->get_id(),
			    length_msg,
			    false,
			    sig->get_message()->get_flags(),
			    data,
			    0);

	frequency_clock_t f = subscription.event_filter_.frequency == 0 ? subscription.signal_->get_frequency() : frequency_clock_t(subscription.event_filter_.frequency);
	freq = f.get_timeval_from_period();

	struct bcm_msg bcm_msg = subscription.make_bcm_head(RX_SETUP, subscription.signal_->get_message()->get_id(), flags_bcm, timeout, freq);

	std::vector<canfd_frame> cfd_vect = cm.convert_to_canfd_frame_vector();

	if(cfd_vect.size() > 1) //multi
	{
		AFB_ERROR("Not implemented yet");
		return -1;
	}
	else if(cfd_vect.size() == 1)
	{
		subscription.add_one_bcm_frame(cfd_vect[0], bcm_msg);
	}
	else
	{
		AFB_ERROR("No data available");
		return -1;
	}

	return create_rx_filter_bcm(subscription, bcm_msg);
}


int low_can_subscription_t::create_rx_filter(std::shared_ptr<message_definition_t> msg)
{
	std::shared_ptr<signal_t> signal_message =
		std::make_shared<signal_t>(signal_t{msg->get_name(),
						    0,
						    msg->get_length() * 8,
						    1.00000f,
						    0.00000f,
						    0,
						    0,
						    frequency_clock_t(0.00000f),
						    true,
						    false,
						    {},
						    true,
						    nullptr,
						    nullptr,
						    false,
						    std::make_pair<bool, int>(false, 0),
						    static_cast<sign_t>(0),
						    -1,
						    ""});

	signal_message->set_parent(msg);
	return create_rx_filter(signal_message);
}


/**
 * @brief Create the good socket to read message
 * depending on the signal
 *
 * @param sig The signal subscribed
 * @return  0 if ok else -1
 */
int low_can_subscription_t::create_rx_filter(std::shared_ptr<signal_t> sig)
{
	if(!sig->get_message()->is_isotp() && !sig->get_message()->is_j1939())
		return low_can_subscription_t::create_rx_filter_can(*this, sig);
#ifdef USE_FEATURE_ISOTP
	else if(sig->get_message()->is_isotp())
		return low_can_subscription_t::create_rx_filter_isotp(*this, sig);
#endif
#ifdef USE_FEATURE_J1939
	else if(sig->get_message()->is_j1939())
		return low_can_subscription_t::create_rx_filter_j1939(*this, sig);
#endif
	AFB_ERROR("Signal can't be created (check config)");
	return -1;
}


/// @brief Create a RX_SETUP receive job to be used by the BCM socket for a
/// diagnostic message subscription.
///
/// @return 0 if ok else -1
int low_can_subscription_t::create_rx_filter(std::shared_ptr<diagnostic_message_t> sig)
{
	diagnostic_message_.push_back(sig);

	struct timeval freq = frequency_clock_t(event_filter_.frequency).get_timeval_from_period();
	struct timeval timeout = {0, 0};

	struct bcm_msg bcm_msg =  make_bcm_head(RX_SETUP, OBD2_FUNCTIONAL_BROADCAST_ID, SETTIMER | RX_NO_AUTOTIMER | RX_FILTER_ID, timeout, freq);
	return create_rx_filter_bcm(*this, bcm_msg);
}

/// @brief Create a RX_SETUP receive job used by the BCM socket directly from
/// a bcm_msg. The method should not be used directly but rather through the
/// two previous method with signal_t or diagnostic_message_t object.
///
/// If the CAN arbitration ID is the OBD2 functional broadcast id the subscribed
/// to the 8 classics OBD2 functional response ID
///
/// @return 0 if ok else -1
int low_can_subscription_t::create_rx_filter_bcm(low_can_subscription_t &subscription, struct bcm_msg& bcm_msg)
{
	// Make sure that socket is opened.
	if(subscription.open_socket(subscription, "", CAN_PROTOCOL) < 0)
		return -1;

	can_message_t msg = can_message_t();
	msg.set_bcm_msg(bcm_msg);

	// If it's not an OBD2 CAN ID then just add a simple RX_SETUP job
	// else monitor all standard 8 CAN OBD2 ID response.
	if(bcm_msg.msg_head.can_id != OBD2_FUNCTIONAL_BROADCAST_ID)
	{
		subscription.socket_->write_message(msg);
		if(! subscription.socket_)
			return -1;
	}
	else
	{
		for(uint8_t i = 0; i < 8; i++)
		{
			bcm_msg.msg_head.can_id = OBD2_FUNCTIONAL_RESPONSE_START + i;
			msg.set_bcm_msg(bcm_msg);
			subscription.socket_->write_message(msg);
			if(! subscription.socket_)
				return -1;
		}
	}
	return 0;
}

/// @brief Creates a TX_SEND job that is used by the BCM socket to
/// send a message
///
/// @return 0 if ok else -1
int low_can_subscription_t::tx_send(low_can_subscription_t &subscription, message_t *message, const std::string& bus_name)
{
	can_message_t *cm = static_cast<can_message_t*>(message);

	struct bcm_msg bcm_msg = subscription.make_bcm_head(TX_SEND, cm->get_id(), cm->get_flags()|TX_CP_CAN_ID); // TX_CP_CAN_ID -> copy in cfd the id of bcm
	cm->set_bcm_msg(bcm_msg);

	std::vector<canfd_frame> cfd_vect = cm->convert_to_canfd_frame_vector();

	if(subscription.open_socket(subscription, bus_name, CAN_PROTOCOL) < 0)
		return -1;

	struct bcm_msg &bcm_cm = cm->get_bcm_msg();

	if(cfd_vect.size() > 1)
	{
		AFB_ERROR("Multi frame BCM not implemented");
		return -1;
	}
	else if(cfd_vect.size() == 1) // raw or fd
	{
		subscription.add_one_bcm_frame(cfd_vect[0], bcm_cm);

		if(subscription.socket_->write_message(*cm) < 0)
		{
			AFB_ERROR("Error write message id : %d", cfd_vect[0].can_id);
			return -1;
		}
	}
	else // error
	{
		AFB_ERROR("Error no data available");
		return -1;
	}

	if(! subscription.socket_.get())
		return -1;

	return 0;
}

#ifdef USE_FEATURE_J1939
/**
 * @brief Allows to open socket j1939 and send j1939 messsage
 *
 * @param subscription The subscription
 * @param message The j1939 message to send
 * @param bus_name The bus name where to send message
 * @return int  0 if ok else -1
 */
int low_can_subscription_t::j1939_send(low_can_subscription_t &subscription, message_t *message, const std::string& bus_name)
{
	//struct bcm_msg bcm_msg = subscription.make_bcm_head(TX_SEND, cfd.can_id);
	//subscription.add_one_bcm_frame(cfd, bcm_msg);

	if(subscription.open_socket(subscription, bus_name, J1939_PROTOCOL) < 0)
		return -1;

	j1939_message_t *jm = static_cast<j1939_message_t*>(message);
	jm->set_sockname(jm->get_pgn(), J1939_NO_NAME, J1939_NO_ADDR);
	if(subscription.socket_->write_message(*jm) < 0)
	{
		AFB_ERROR("Error write j1939 message");
		return -1;
	}

	return 0;
}
#endif


/**
 * @brief Allows to open socket isotp and send can messsage
 *
 * @param subscription The subscription
 * @param message The can message to send
 * @param bus_name The bus name where to send message
 * @return int  0 if ok else -1
 */
int low_can_subscription_t::isotp_send(low_can_subscription_t &subscription, message_t *message, const std::string& bus_name)
{
	//struct bcm_msg bcm_msg = subscription.make_bcm_head(TX_SEND, cfd.can_id);
	//subscription.add_one_bcm_frame(cfd, bcm_msg);

	if(subscription.open_socket(subscription, bus_name, ISOTP_PROTOCOL|ISOTP_SEND) < 0)
		return -1;

	can_message_t *cm = static_cast<can_message_t*>(message);
	if(subscription.socket_->write_message(*cm) < 0)
	{
		AFB_ERROR("Error write iso tp message");
		return -1;
	}

	return 0;
}
