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

#pragma once

#include <string>
#include <cmath>
#include <utility>

#include "../can/signals.hpp"
#include "../diagnostic/diagnostic-message.hpp"
#include "../utils/socketcan-bcm.hpp"

#define OBDII_MAX_SIMULTANEOUS_RESPONSES 8

/// @brief Filtering values. Theses values have to be tested in
/// can_bus_t::apply_filter method.
struct event_filter_t
{
	float frequency; ///< frequency - Maximum frequency which will be received and pushed to a subscribed event.
	float min; ///< min - Minimum value that the signal doesn't have to go below to be pushed.
	float max; ///< max - Maximum value that the signal doesn't have to go above to be pushed.

	event_filter_t() : frequency{0}, min{-__FLT_MAX__}, max{__FLT_MAX__} {};
	bool operator==(const event_filter_t& ext) const {
		return frequency == ext.frequency && min == ext.min && max == ext.max;
	}
};

/// @brief The object stores socket to CAN to be used to write on it.
/// This is a simple access to a CAN bus device without any subscriptions attached
class low_can_subscription_t
{
private:
	int index_; ///< index_ - index number is the socket (int) casted
	struct event_filter_t event_filter_;
	afb_event_t event_; ///< event_ - application framework event used to push on client

	/// Signal part
	std::shared_ptr<signal_t> signal_; ///< signal_ - the CAN signal subscribed
	std::vector<std::shared_ptr<diagnostic_message_t> > diagnostic_message_; ///< diagnostic_message_ - diagnostic messages meant to receive OBD2
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

	afb_event_t get_event();
	int subscribe(afb_req_t request);
	int unsubscribe(afb_req_t request);

	int get_index() const;
	const std::shared_ptr<signal_t> get_signal() const;
	bool is_signal_subscription_corresponding(const std::shared_ptr<signal_t>, const struct event_filter_t& event_filter) const;
	const std::shared_ptr<diagnostic_message_t> get_diagnostic_message(uint32_t pid) const;
	const std::vector<std::shared_ptr<diagnostic_message_t> > get_diagnostic_message() const;
	const std::shared_ptr<diagnostic_message_t> get_diagnostic_message(const std::string& name) const;
	const std::string get_name() const;
	const std::string get_name(uint32_t pid) const;
	float get_frequency() const;
	float get_min() const;
	float get_max() const;
	std::shared_ptr<utils::socketcan_t> get_socket();

	void set_frequency(float freq);
	void set_min(float min);
	void set_max(float max);

	static struct bcm_msg make_bcm_head(uint32_t opcode, uint32_t can_id = 0, uint32_t flags = 0, const struct timeval& timeout = {0,0}, const struct timeval& frequency_thinning = {0,0});
	static void add_one_bcm_frame(struct canfd_frame& cfd, struct bcm_msg& bcm_msg);

	static int open_socket(low_can_subscription_t &subscription, const std::string& bus_name = "");

	int create_rx_filter(std::shared_ptr<signal_t> sig);
	int create_rx_filter(std::shared_ptr<diagnostic_message_t> sig);
	static int create_rx_filter_can(low_can_subscription_t &subscription, std::shared_ptr<signal_t> sig);
	static int create_rx_filter_j1939(low_can_subscription_t &subscription, std::shared_ptr<signal_t> sig);
	static int create_rx_filter_bcm(low_can_subscription_t &subscription, bcm_msg& bcm_msg);

	static int tx_send(low_can_subscription_t &subscription, struct canfd_frame& cfd, const std::string& bus_name);
};
