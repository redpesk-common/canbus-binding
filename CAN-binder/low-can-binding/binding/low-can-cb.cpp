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

#include "low-can-hat.hpp"

#include <map>
#include <memory>
#include <queue>
#include <mutex>
#include <vector>
#include <thread>
#include <json-c/json.h>
#include <systemd/sd-event.h>

#include "openxc.pb.h"
#include "application.hpp"
#include "../can/can-bus.hpp"
#include "../can/can-signals.hpp"
#include "../can/can-message.hpp"
#include "../utils/timer.hpp"
#include "../utils/signals.hpp"
#include "../diagnostic/diagnostic-message.hpp"
#include "../utils/openxc-utils.hpp"
#include "canutil/write.h"

extern "C"
{
	#include <afb/afb-service-itf.h>
};

///******************************************************************************
///
///	low_can_subscription_t object
///
///*******************************************************************************/

low_can_subscription_t::low_can_subscription_t()
	: index_{-1},
	event_filter_{event_filter_t()},
	socket_{}
{}

low_can_subscription_t::low_can_subscription_t(struct event_filter_t event_filter)
	: event_filter_{event_filter}
{}

low_can_subscription_t::low_can_subscription_t(struct event_filter_t event_filter, std::shared_ptr<diagnostic_message_t> diagnostic_message)
	: diagnostic_message_{diagnostic_message}, event_filter_{event_filter}
{}

low_can_subscription_t::low_can_subscription_t( low_can_subscription_t&& s)
	: index_{s.index_},
	event_filter_{s.event_filter_},
	socket_{std::move(s.socket_)}
{}

	low_can_subscription_t& low_can_subscription_t::operator=(const low_can_subscription_t& s)
{
	socket_ = std::move(s.socket_);
	return *this;
}

low_can_subscription_t::operator bool() const
{
	return socket_.socket() != INVALID_SOCKET;
}

int low_can_subscription_t::get_index() const
{
	return index_;
}

const std::shared_ptr<can_signal_t> low_can_subscription_t::get_can_signal() const
{
	return can_signal_;
}

const std::string low_can_subscription_t::get_name() const
{
	if (can_signal_ != nullptr)
		return can_signal_->get_name();
	if (diagnostic_message_ != nullptr)
		return diagnostic_message_->get_name() ;

	return "";
}

float low_can_subscription_t::get_frequency() const
{
	return event_filter_.frequency;
}

float low_can_subscription_t::get_min() const
{
	return event_filter_.min;
}

float low_can_subscription_t::get_max() const
{
	return event_filter_.max;
}

utils::socketcan_bcm_t& low_can_subscription_t::get_socket()
{
	return socket_;
}

void low_can_subscription_t::set_frequency(float freq)
{
	event_filter_.frequency = freq;
}

void low_can_subscription_t::set_min(float min)
{
	event_filter_.min = min;
}

void low_can_subscription_t::set_max(float max)
{
	event_filter_.max = max;
}

/// @brief Create a RX_SETUP receive job used by the BCM socket.
///
/// @return 0 if ok else -1
int low_can_subscription_t::create_rx_filter(std::shared_ptr<can_signal_t> sig)
{
	can_signal_= sig;
	return create_rx_filter();
}

/// @brief Create a RX_SETUP receive job used by the BCM socket.
///
/// @return 0 if ok else -1
int low_can_subscription_t::create_rx_filter()
{
	if (can_signal_ == nullptr)
		{return -1;}

	// Make sure that socket has been opened.
	if(! socket_)
	{
		socket_.open(can_signal_->get_message()->get_bus_device_name());
		index_ = (int)socket_.socket();
	}

	struct utils::simple_bcm_msg bcm_msg;
	struct can_frame cfd;

	memset(&cfd, 0, sizeof(cfd));
	memset(&bcm_msg.msg_head, 0, sizeof(bcm_msg.msg_head));
	float val = (float)(1 << can_signal_->get_bit_size()) - 1;

	struct timeval freq;
	frequency_clock_t f = std::isnan(event_filter_.frequency) ? can_signal_->get_frequency() : frequency_clock_t(event_filter_.frequency);
	freq = f.get_timeval_from_period();

	bcm_msg.msg_head.opcode  = RX_SETUP;
	bcm_msg.msg_head.can_id  = can_signal_->get_message()->get_id();
	bcm_msg.msg_head.flags = SETTIMER|RX_NO_AUTOTIMER;
	bcm_msg.msg_head.ival2.tv_sec = freq.tv_sec ;
	bcm_msg.msg_head.ival2.tv_usec = freq.tv_usec;
	bcm_msg.msg_head.nframes = 1;
	bitfield_encode_float(val,
							can_signal_->get_bit_position(),
							can_signal_->get_bit_size(),
							can_signal_->get_factor(),
							can_signal_->get_offset(),
							cfd.data,
							CAN_MAX_DLEN);

	bcm_msg.frames = cfd;

	if(socket_ << bcm_msg)
		return 0;
	return -1;
}

///******************************************************************************
///
///		SystemD event loop Callbacks
///
///*******************************************************************************/

void on_no_clients(const std::string& message)
{
	DiagnosticRequest* diag_req = application_t::instance().get_request_from_diagnostic_message(message);
	if(diag_req != nullptr)
	{
		active_diagnostic_request_t* adr = application_t::instance().get_diagnostic_manager().find_recurring_request(diag_req);
		if( adr != nullptr)
			application_t::instance().get_diagnostic_manager().cleanup_request(adr, true);
	}
	delete diag_req;
	diag_req = nullptr;
}

static void push_n_notify(const can_message_t& cm)
{
	can_bus_t& cbm = application_t::instance().get_can_bus_manager();
	std::lock_guard<std::mutex> can_message_lock(cbm.get_can_message_mutex());
	{ cbm.push_new_can_message(cm); }
	cbm.get_new_can_message_cv().notify_one();
}

int read_message(sd_event_source *s, int fd, uint32_t revents, void *userdata)
{
	can_message_t cm;
	low_can_subscription_t* can_subscription;
	diagnostic_manager_t& diag_m = application_t::instance().get_diagnostic_manager();

	if(userdata != nullptr)
	{
		can_subscription = (low_can_subscription_t*)userdata;
		utils::socketcan_bcm_t& s = can_subscription->get_socket();
		s >> cm;
	}
	else
	{
		utils::socketcan_bcm_t& s = diag_m.get_socket();
		s >> cm;
	}

	push_n_notify(cm);

	/* check if error or hangup */
	if ((revents & (EPOLLERR|EPOLLRDHUP|EPOLLHUP)) != 0)
	{
		sd_event_source_unref(s);
		if(userdata != nullptr)
		{
			can_subscription->get_socket().close();
			can_subscription->create_rx_filter();
			NOTICE(binder_interface, "%s: Recreated RX_SETUP BCM job for can_subscription: %s", __FUNCTION__, can_subscription->get_name().c_str());
		}
		else
		{
			diag_m.get_socket().close();
			diag_m.cleanup_active_requests(true);
			ERROR(binder_interface, "%s: Error on diagnostic manager socket, cancelling active requests.", __FUNCTION__);
		}
		return -1;
	}

	return 0;
}

///******************************************************************************
///
///		Subscription and unsubscription
///
///*******************************************************************************/

static int make_subscription_unsubscription(struct afb_req request, std::shared_ptr<low_can_subscription_t>& can_subscription, std::map<int, std::pair<std::shared_ptr<low_can_subscription_t>, struct afb_event> >& s, bool subscribe)
{
	/* Make the subscription or unsubscription to the event */
	if (((subscribe ? afb_req_subscribe : afb_req_unsubscribe)(request, s[can_subscription->get_index()].second)) < 0)
	{
		ERROR(binder_interface, "%s: Operation goes wrong for signal: %s", __FUNCTION__, can_subscription->get_name().c_str());
		return -1;
	}
	return 0;
}

static int create_event_handle(std::shared_ptr<low_can_subscription_t>& can_subscription, std::map<int, std::pair<std::shared_ptr<low_can_subscription_t>, struct afb_event> >& s)
{
	int sub_index = can_subscription->get_index();
	struct afb_event event = afb_daemon_make_event(binder_interface->daemon, can_subscription->get_name().c_str());
	s[sub_index] = std::make_pair(can_subscription, event);
	if (!afb_event_is_valid(s[sub_index].second))
	{
		ERROR(binder_interface, "%s: Can't create an event for %s, something goes wrong.", __FUNCTION__, can_subscription->get_name().c_str());
		return -1;
	}
	return 0;
}

/// @brief Will determine if it is needed or not to create the event handle and checks it to be sure that
/// we got a valid afb_event to get subscribe or unsubscribe. Then launch the subscription or unsubscription
/// against the application framework using that event handle.
static int subscribe_unsubscribe_signal(struct afb_req request, bool subscribe, std::shared_ptr<low_can_subscription_t>& can_subscription)
{
	int ret;
	int sub_index = can_subscription->get_index();
	utils::signals_manager_t& sm = utils::signals_manager_t::instance();

	std::lock_guard<std::mutex> subscribed_signals_lock(sm.get_subscribed_signals_mutex());
	std::map<int, std::pair<std::shared_ptr<low_can_subscription_t>, struct afb_event> >& s = sm.get_subscribed_signals();
	if (can_subscription && s.find(sub_index) != s.end())
	{
		if (!afb_event_is_valid(s[sub_index].second) && !subscribe)
		{
			NOTICE(binder_interface, "%s: Event isn't valid, no need to unsubscribed.", __FUNCTION__);
			ret = -1;
		}
		else
		{
			// Event it isn't valid anymore, recreate it
			ret = create_event_handle(can_subscription, s);
		}
	}
	else
	{
		/* Event doesn't exist , so let's create it */
		struct afb_event empty_event = {nullptr, nullptr};
		s[sub_index] = std::make_pair(can_subscription, empty_event);
		ret = create_event_handle(can_subscription, s);
	}

	// Check whether or not the event handler has been correctly created and
	// make the subscription/unsubscription operation is so.
	if (ret < 0)
		return ret;
	return make_subscription_unsubscription(request, can_subscription, s, subscribe);
}

int subscribe_unsubscribe_diagnostic_messages(struct afb_req request, bool subscribe, std::vector<std::shared_ptr<diagnostic_message_t> > diagnostic_messages, struct event_filter_t& event_filter)
{
	int rets = 0;
	application_t& conf = application_t::instance();
	diagnostic_manager_t& diag_m = conf.get_diagnostic_manager();

	for(const auto& sig : diagnostic_messages)
	{
		active_diagnostic_request_t* adr;
		DiagnosticRequest* diag_req = conf.get_request_from_diagnostic_message(sig->get_name());

		// If the requested diagnostic message isn't supported by the car then unsubcribe it
		// no matter what we want, worse case will be a fail unsubscription but at least we don't
		// poll a PID for nothing.
		if(sig->get_supported() && subscribe)
		{
			float frequency = std::isnan(event_filter.frequency) ? sig->get_frequency() : event_filter.frequency;

			adr = diag_m.add_recurring_request(diag_req, sig->get_name().c_str(), false, sig->get_decoder(), sig->get_callback(), frequency);
			//TODO: Adding callback requesting ignition status:	diag_req, sig.c_str(), false, diagnostic_message_t::decode_obd2_response, diagnostic_message_t::check_ignition_status, frequency);
		}
		else
		{
			diag_m.cleanup_request(
				diag_m.find_recurring_request(diag_req), true);
			WARNING(binder_interface, "%s: signal: %s isn't supported. Canceling operation.", __FUNCTION__, sig->get_name().c_str());
			delete diag_req;
			diag_req = nullptr;
			return -1;
		}

		std::shared_ptr<low_can_subscription_t> can_subscription(new low_can_subscription_t(event_filter, sig));
		int ret = subscribe_unsubscribe_signal(request, subscribe, can_subscription);
		if(ret < 0)
			return ret;
		rets++;
		DEBUG(binder_interface, "%s: Signal: %s subscribed", __FUNCTION__, sig->get_name().c_str());
	}

	return rets;
}

int subscribe_unsubscribe_can_signals(struct afb_req request, bool subscribe, std::vector<std::shared_ptr<can_signal_t> > can_signals, struct event_filter_t& event_filter)
{
	int rets = 0;
	for(const auto& sig: can_signals)
	{
		std::shared_ptr<low_can_subscription_t> can_subscription(new low_can_subscription_t(event_filter));
		if(can_subscription->create_rx_filter(sig) < 0)
			{return -1;}
		else if(subscribe_unsubscribe_signal(request, subscribe, can_subscription) < 0)
			{return -1;}

		struct sd_event_source* e_source;
		sd_event_add_io(afb_daemon_get_event_loop(binder_interface->daemon), &e_source, can_subscription->get_socket().socket(), EPOLLIN, read_message, can_subscription.get());
		rets++;
		DEBUG(binder_interface, "%s: signal: %s subscribed", __FUNCTION__, sig->get_name().c_str());
	}
	return rets;
}

///
/// @brief subscribe to all signals in the vector signals
///
/// @param[in] afb_req request : contain original request use to subscribe or unsubscribe
/// @param[in] subscribe boolean value used to chose between a subscription operation or an unsubscription
/// @param[in] signals -  struct containing vectors with can_signal_t and diagnostic_messages to subscribe
///
/// @return Number of correctly subscribed signal
///
static int subscribe_unsubscribe_signals(struct afb_req request, bool subscribe, const struct utils::signals_found& signals, struct event_filter_t& event_filter)
{
	int rets = 0;

	rets += subscribe_unsubscribe_diagnostic_messages(request, subscribe, signals.diagnostic_messages, event_filter);
	rets += subscribe_unsubscribe_can_signals(request, subscribe, signals.can_signals, event_filter);

	return rets;
}

static int one_subscribe_unsubscribe(struct afb_req request, bool subscribe, const std::string& tag, json_object* args)
{
	int ret = 0;
	struct event_filter_t event_filter;
	struct json_object  *filter, *obj;
	struct utils::signals_found sf;

	// computes the filter
	if (json_object_object_get_ex(args, "filter", &filter))
	{
		if (json_object_object_get_ex(filter, "frequency", &obj)
		&& (json_object_is_type(obj, json_type_double) || json_object_is_type(obj, json_type_int)))
		{
			event_filter.frequency = (float)json_object_get_double(obj);
			ret += 1;
		}
		if (json_object_object_get_ex(filter, "min", &obj)
		&& (json_object_is_type(obj, json_type_double) || json_object_is_type(obj, json_type_int)))
		{
			event_filter.min = (float)json_object_get_double(obj);
			ret += 2;
		}
		if (json_object_object_get_ex(filter, "max", &obj)
		&& (json_object_is_type(obj, json_type_double) || json_object_is_type(obj, json_type_int)))
		{
			event_filter.max = (float)json_object_get_double(obj);
			ret += 4;
		}
	}

	// subscribe or unsubscribe
	openxc_DynamicField search_key = build_DynamicField(tag);
	sf = utils::signals_manager_t::instance().find_signals(search_key);
	if (sf.can_signals.empty() && sf.diagnostic_messages.empty())
		NOTICE(binder_interface, "%s: No signal(s) found for %s.", __FUNCTION__, tag.c_str());
	else
		ret = subscribe_unsubscribe_signals(request, subscribe, sf, event_filter);

	return ret;
}

static void do_subscribe_unsubscribe(struct afb_req request, bool subscribe)
{
	int i, n, rc, rc2;
	struct json_object *args, *event, *x;

	args = afb_req_json(request);
	if (args == NULL || !json_object_object_get_ex(args, "event", &event))
	{
		rc = one_subscribe_unsubscribe(request, subscribe, "*", args);
	}
	else if (json_object_get_type(event) != json_type_array)
	{
		rc = one_subscribe_unsubscribe(request, subscribe, json_object_get_string(event), args);
	}
	else
	{
		rc = 0;
		n = json_object_array_length(event);
		for (i = 0 ; i < n ; i++)
		{
			x = json_object_array_get_idx(event, i);
			rc2 = one_subscribe_unsubscribe(request, subscribe, json_object_get_string(x), args);
			if (rc >= 0)
				rc = rc2 >= 0 ? rc + rc2 : rc2;
		}
	}

	if (rc >= 0)
		afb_req_success(request, NULL, NULL);
	else
		afb_req_fail(request, "error", NULL);
}

void subscribe(struct afb_req request)
{
	do_subscribe_unsubscribe(request, true);
}

void unsubscribe(struct afb_req request)
{
	do_subscribe_unsubscribe(request, false);
}
