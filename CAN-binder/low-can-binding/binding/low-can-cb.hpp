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

#include <string>
#include <cmath>
#include <utility>

#include "../can/can-signals.hpp"
#include "../utils/socketcan-bcm.hpp"

struct event_filter_t
{
	float frequency;
	float min;
	float max;
	event_filter_t() : frequency{NAN}, min{NAN}, max{NAN} {}
};

class low_can_subscription_t
{
private:
	int index_;

	/// Signal part
	std::shared_ptr<can_signal_t> can_signal_;

	/// Filtering part
	struct event_filter_t event_filter_;

	utils::socketcan_bcm_t socket_;
public:
	low_can_subscription_t();
	low_can_subscription_t(struct event_filter_t event_filter);
	low_can_subscription_t(const low_can_subscription_t& s) = delete;
	low_can_subscription_t(low_can_subscription_t&& s);

	low_can_subscription_t& operator=(const low_can_subscription_t& s);
	explicit operator bool() const;

	int get_index() const;
	const std::shared_ptr<can_signal_t> get_can_signal() const;
	const std::string get_sig_name() const;
	float get_frequency() const;
	float get_min() const;
	float get_max() const;
	utils::socketcan_bcm_t& get_socket();

	void set_frequency(float freq);
	void set_min(float min);
	void set_max(float max);

	int create_rx_filter();
	int create_rx_filter(std::shared_ptr<can_signal_t> sig);
};
