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

#pragma once

#include <string>
#include <memory>
#include <cmath>
#include <linux/can.h>

#include <afb/afb-binding>

#include <canbus-binding/utils/socketcan.hpp>
#include <canbus-binding/can/message/message.hpp>
#include <canbus-binding/can/message-set.hpp>

class signal_t;
class diagnostic_message_t;

#define OBDII_MAX_SIMULTANEOUS_RESPONSES 8

/// @brief Filtering values. Theses values have to be tested in
/// can_bus_t::apply_filter method.
struct event_filter_t
{
	float frequency; ///< frequency - Maximum frequency which will be received and pushed to a subscribed event.
	float min; ///< min - Minimum value that the signal doesn't have to go below to be pushed.
	float max; ///< max - Maximum value that the signal doesn't have to go above to be pushed.
	bool promisc; //<promisc - Active mode promisc on socket that permit this feature (j1939)
	canid_t rx_id; ///< rx_id - RX_ID for ISO_TP protocol
	canid_t tx_id; ///< tx_id - TX_ID for ISO_TP protocol

	event_filter_t() : frequency{0}, min{-__FLT_MAX__}, max{__FLT_MAX__}, promisc{false}, rx_id{NO_CAN_ID}, tx_id{NO_CAN_ID} {};
	bool operator==(const event_filter_t& ext) const {
		return frequency == ext.frequency && min == ext.min && max == ext.max && promisc == ext.promisc && rx_id == ext.rx_id && tx_id == ext.tx_id;
	}
};


/// @brief The object stores socket to CAN to be used to write on it.
/// This is a simple access to a CAN bus device without any subscriptions attached
class low_can_subscription_t
{
private:
	int index_; ///< index_ - index number is the socket (int) casted
	struct event_filter_t event_filter_;
	afb::event event_; ///< event_ - application framework event used to push on client

	/// Signal part
	std::shared_ptr<message_definition_t> message_; ///< message_ - the CAN message subscribed
	std::shared_ptr<signal_t> signal_; ///< signal_ - the CAN signal subscribed
	vect_ptr_diag_msg_t diagnostic_message_; ///< diagnostic_message_ - diagnostic messages meant to receive OBD2
										 /// responses. Normal diagnostic request and response are not tested for now.
	std::shared_ptr<utils::socketcan_t> socket_; ///< socket_ - socket_ that receives CAN messages.

	int set_event();

public:
	low_can_subscription_t();
	low_can_subscription_t(struct event_filter_t event_filter);
	low_can_subscription_t(const low_can_subscription_t& s) = delete;
	low_can_subscription_t(low_can_subscription_t&& s);
	~low_can_subscription_t();

	low_can_subscription_t& operator=(const low_can_subscription_t& s);
	explicit operator bool() const;

	afb::event get_event();
	int subscribe(afb::req request);
	int unsubscribe(afb::req request);

	int get_index() const;
	const std::shared_ptr<signal_t> get_signal() const;
	bool is_signal_subscription_corresponding(const std::shared_ptr<signal_t>, const struct event_filter_t& event_filter) const;
	const std::shared_ptr<diagnostic_message_t> get_diagnostic_message(uint32_t pid) const;
	const vect_ptr_diag_msg_t get_diagnostic_message() const;
	const std::shared_ptr<diagnostic_message_t> get_diagnostic_message(const std::string& name) const;
	const std::string get_name() const;
	const std::string get_name(uint32_t pid) const;
	float get_frequency() const;
	float get_min() const;
	float get_max() const;
	bool get_promisc() const;
	canid_t get_rx_id() const;
	canid_t get_tx_id() const;
	std::shared_ptr<utils::socketcan_t> get_socket();
	std::shared_ptr<message_definition_t> get_message_definition();

	void set_frequency(float freq);
	void set_min(float min);
	void set_max(float max);
	void set_index(int index);
	void set_promisc(bool promisc);
	void set_rx_id(canid_t rx_id);
	void set_tx_id(canid_t tx_id);
	void set_signal(std::shared_ptr<signal_t> signal);
	void set_message_definition(std::shared_ptr<message_definition_t> message);

	static void make_bcm_head(uint32_t opcode, union bcm_msg& bcm_msg, uint32_t can_id = 0, uint32_t flags = 0, const struct timeval& timeout = {0, 0}, const struct timeval& frequency_thinning = {0, 0});
	static void add_one_bcm_frame(struct canfd_frame& cfd, union bcm_msg& bcm_msg);
	static void remove_last_bcm_frame(union bcm_msg& bcm_msg);

	int open_socket(const std::string& bus_name = "", uint32_t flags = INVALID_FLAG);

	int create_rx_filter(std::shared_ptr<signal_t> sig);
	int create_rx_filter(std::shared_ptr<message_definition_t> msg);
	int create_rx_filter(std::shared_ptr<diagnostic_message_t> sig);
	int create_rx_filter_can(std::shared_ptr<signal_t> sig);
	int create_rx_filter_j1939(std::shared_ptr<signal_t> sig);
	int create_rx_filter_isotp(std::shared_ptr<signal_t> sig);
	int create_rx_filter_bcm(bcm_msg& bcm_msg);

	int tx_send(message_t *message, const std::string& bus_name);
	int j1939_send(message_t *message, const std::string& bus_name);
	int isotp_send(message_t *message, const std::string& bus_name);
};
