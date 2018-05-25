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

#include "low-can-socket.hpp"
#include "../can/can-signals.hpp"
#include "../diagnostic/diagnostic-message.hpp"
#include "../utils/socketcan-bcm.hpp"

/// @brief The subscription class has a context that can handle all needed values to describe a subscription
/// to the low-can binding. It can hold a CAN signal or a diagnostic message. A diagnostic message for OBD2 is
/// special because there is only 1 listener to retrieve OBD2 requests. It is required that all diagnostic message
/// subscriptions are in 1 object.
class low_can_subscription_t : public low_can_socket_t
{
private:
	struct afb_event event_; ///< event_ - application framework event used to push on client

public:
	using low_can_socket_t::low_can_socket_t;

	struct afb_event& get_event();
	void set_event(struct afb_event event);
};
