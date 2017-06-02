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

low_can_subscription_t::~low_can_subscription_t()
{
	socket_.close();
}

low_can_subscription_t::operator bool() const
{
	return ((can_signal_ != nullptr || ! diagnostic_message_.empty()) && afb_event_is_valid(event_));
}

struct afb_event& low_can_subscription_t::get_event()
{
	return event_;
}

int low_can_subscription_t::get_index() const
{
	return index_;
}

const std::shared_ptr<can_signal_t> low_can_subscription_t::get_can_signal() const
{
	return can_signal_;
}

const std::vector<std::shared_ptr<diagnostic_message_t> > low_can_subscription_t::get_diagnostic_message() const
{
	return diagnostic_message_;
}

const std::shared_ptr<diagnostic_message_t> low_can_subscription_t::get_diagnostic_message(uint32_t pid) const
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

const std::shared_ptr<diagnostic_message_t> low_can_subscription_t::get_diagnostic_message(const std::string& name) const
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

const std::string low_can_subscription_t::get_name() const
{
	if (can_signal_ != nullptr)
		return can_signal_->get_name();

	return "";
}

const std::string low_can_subscription_t::get_name(uint32_t pid) const
{
	if (!diagnostic_message_.empty())
		return get_diagnostic_message(pid)->get_name() ;

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

void low_can_subscription_t::set_event(struct afb_event event)
{
	event_ = event;
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

int low_can_subscription_t::open_socket()
{
	int ret = 0;
	if(! socket_)
	{
		if( can_signal_ != nullptr)
			{ret = socket_.open(can_signal_->get_message()->get_bus_device_name());}
		else if (! diagnostic_message_ .empty())
			{ret = socket_.open(application_t::instance().get_diagnostic_manager().get_bus_device_name());}
		index_ = (int)socket_.socket();
	}
	return ret;
}

struct utils::simple_bcm_msg low_can_subscription_t::make_bcm_head(uint32_t can_id, uint32_t flags, const struct timeval& timeout, const struct timeval& frequency_thinning) const
{
	struct utils::simple_bcm_msg bcm_msg;

	memset(&bcm_msg, 0, sizeof(bcm_msg));

	bcm_msg.msg_head.opcode  = RX_SETUP;
	bcm_msg.msg_head.can_id  = can_id;
	bcm_msg.msg_head.flags = flags;
	bcm_msg.msg_head.ival1.tv_sec = timeout.tv_sec ;
	bcm_msg.msg_head.ival1.tv_usec = timeout.tv_usec;
	bcm_msg.msg_head.ival2.tv_sec = frequency_thinning.tv_sec ;
	bcm_msg.msg_head.ival2.tv_usec = frequency_thinning.tv_usec;

	return bcm_msg;
}

void low_can_subscription_t::add_bcm_frame(const struct can_frame& cfd, struct utils::simple_bcm_msg& bcm_msg) const
{
	for(int i=0; i < CAN_MAX_DLEN; i++)
	{
		if(cfd.data[i] != 0)
		{
			bcm_msg.msg_head.nframes = 1;
			bcm_msg.frames = cfd;
			return;
		}
	}
}

/// @brief Create a RX_SETUP receive job used by the BCM socket.
///
/// @return 0 if ok else -1
int low_can_subscription_t::create_rx_filter()
{
	int ret = -1;
	if ( can_signal_ != nullptr)
		{ret = create_rx_filter(can_signal_);}
	else if (! diagnostic_message_ .empty())
		{ret = create_rx_filter(diagnostic_message_.front());}

	return ret;
}

/// @brief Create a RX_SETUP receive job used by the BCM socket.
///
/// @return 0 if ok else -1
int low_can_subscription_t::create_rx_filter(std::shared_ptr<can_signal_t> sig)
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
	frequency_clock_t f = std::isnan(event_filter_.frequency) ? can_signal_->get_frequency() : frequency_clock_t(event_filter_.frequency);
	freq = f.get_timeval_from_period();

	utils::simple_bcm_msg bcm_msg = make_bcm_head(can_signal_->get_message()->get_id(), SETTIMER|RX_NO_AUTOTIMER, timeout, freq);
	add_bcm_frame(cfd, bcm_msg);

	return create_rx_filter(bcm_msg);
}

/// @brief Create a RX_SETUP receive job used by the BCM socket.
///
/// @return 0 if ok else -1
int low_can_subscription_t::create_rx_filter(std::shared_ptr<diagnostic_message_t> sig)
{
	diagnostic_message_.push_back(sig);

	struct timeval freq = frequency_clock_t(event_filter_.frequency).get_timeval_from_period();
	//struct timeval timeout = frequency_clock_t(10).get_timeval_from_period();
	struct timeval timeout = {0,0};

	utils::simple_bcm_msg bcm_msg =  make_bcm_head(OBD2_FUNCTIONAL_BROADCAST_ID, SETTIMER|RX_NO_AUTOTIMER|RX_FILTER_ID, timeout, freq);
	return create_rx_filter(bcm_msg);
}

/// @brief Create a RX_SETUP receive job used by the BCM socket.
///
/// @return 0 if ok else -1
int low_can_subscription_t::create_rx_filter(utils::simple_bcm_msg& bcm_msg) 
{
	// Make sure that socket has been opened.
	if(open_socket() < 0)
		{return -1;}

	// If it isn't an OBD2 CAN ID then just add a simple RX_SETUP job
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

///******************************************************************************
///
///		SystemD event loop Callbacks
///
///*******************************************************************************/

void on_no_clients(std::shared_ptr<low_can_subscription_t> can_subscription, uint32_t pid)
{
	if( ! can_subscription->get_diagnostic_message().empty() && can_subscription->get_diagnostic_message(pid) != nullptr)
	{
		DiagnosticRequest diag_req = can_subscription->get_diagnostic_message(pid)->build_diagnostic_request();
		active_diagnostic_request_t* adr = application_t::instance().get_diagnostic_manager().find_recurring_request(diag_req);
		if( adr != nullptr)
			application_t::instance().get_diagnostic_manager().cleanup_request(adr, true);
	}

	on_no_clients(can_subscription);
}

void on_no_clients(std::shared_ptr<low_can_subscription_t> can_subscription)
{
	utils::signals_manager_t& sm = utils::signals_manager_t::instance();
	std::lock_guard<std::mutex> subscribed_signals_lock(sm.get_subscribed_signals_mutex());
	std::map<int, std::shared_ptr<low_can_subscription_t> >& s = sm.get_subscribed_signals();
	auto it = s.find(can_subscription->get_index());
	s.erase(it);
}

static void push_n_notify(const can_message_t& cm)
{
	can_bus_t& cbm = application_t::instance().get_can_bus_manager();
	{
		std::lock_guard<std::mutex> can_message_lock(cbm.get_can_message_mutex());
	 	cbm.push_new_can_message(cm);
	}
	cbm.get_new_can_message_cv().notify_one();
}

int read_message(sd_event_source *event_source, int fd, uint32_t revents, void *userdata)
{
	low_can_subscription_t* can_subscription = (low_can_subscription_t*)userdata;
	if ((revents & EPOLLIN) != 0)
	{
		can_message_t cm;
		utils::socketcan_bcm_t& s = can_subscription->get_socket();
		s >> cm;

		push_n_notify(cm);
	}

	/* check if error or hangup */
	if ((revents & (EPOLLERR|EPOLLRDHUP|EPOLLHUP)) != 0)
	{
		sd_event_source_unref(event_source);
		can_subscription->get_socket().close();
	}
	return 0;
}

///******************************************************************************
///
///		Subscription and unsubscription
///
///*******************************************************************************/

static int make_subscription_unsubscription(struct afb_req request, std::shared_ptr<low_can_subscription_t>& can_subscription, std::map<int, std::shared_ptr<low_can_subscription_t> >& s, bool subscribe)
{
	/* Make the subscription or unsubscription to the event */
	if (((subscribe ? afb_req_subscribe : afb_req_unsubscribe)(request, s[can_subscription->get_index()]->get_event())) < 0)
	{
		ERROR(binder_interface, "%s: Operation goes wrong for signal: %s", __FUNCTION__, can_subscription->get_name().c_str());
		return -1;
	}
	return 0;
}

static int create_event_handle(std::shared_ptr<low_can_subscription_t>& can_subscription, std::map<int, std::shared_ptr<low_can_subscription_t> >& s)
{
	int sub_index = can_subscription->get_index();
	can_subscription->set_event(afb_daemon_make_event(binder_interface->daemon, can_subscription->get_name().c_str()));
	s[sub_index] = can_subscription;
	if (!afb_event_is_valid(s[sub_index]->get_event()))
	{
		ERROR(binder_interface, "%s: Can't create an event for %s, something goes wrong.", __FUNCTION__, can_subscription->get_name().c_str());
		return -1;
	}
	return 0;
}

/// @brief Will determine if it is needed or not to create the event handle and checks it to be sure that
/// we got a valid afb_event to get subscribe or unsubscribe. Then launch the subscription or unsubscription
/// against the application framework using that event handle.
static int subscribe_unsubscribe_signal(struct afb_req request, bool subscribe, std::shared_ptr<low_can_subscription_t>& can_subscription, std::map<int, std::shared_ptr<low_can_subscription_t> >& s)
{
	int ret;
	int sub_index = can_subscription->get_index();

	if (can_subscription && s.find(sub_index) != s.end())
	{
		if (!afb_event_is_valid(s[sub_index]->get_event()) && !subscribe)
		{
			NOTICE(binder_interface, "%s: Event isn't valid, no need to unsubscribed.", __FUNCTION__);
			ret = -1;
		}
	}
	else
	{
		/* Event doesn't exist , so let's create it */
		can_subscription->set_event({nullptr, nullptr});
		s[sub_index] = can_subscription;
		ret = create_event_handle(can_subscription, s);
	}

	// Check whether or not the event handler has been correctly created and
	// make the subscription/unsubscription operation is so.
	if (ret < 0)
		return ret;
	return make_subscription_unsubscription(request, can_subscription, s, subscribe);
}

static int add_to_event_loop(std::shared_ptr<low_can_subscription_t>& can_subscription)
{
		struct sd_event_source* event_source = nullptr;
		return ( sd_event_add_io(afb_daemon_get_event_loop(binder_interface->daemon),
			&event_source,
			can_subscription->get_socket().socket(),
			EPOLLIN,
			read_message,
			can_subscription.get()));
}

static int subscribe_unsubscribe_diagnostic_messages(struct afb_req request, bool subscribe, std::vector<std::shared_ptr<diagnostic_message_t> > diagnostic_messages, struct event_filter_t& event_filter, std::map<int, std::shared_ptr<low_can_subscription_t> >& s)
{
	int rets = 0;
	application_t& app = application_t::instance();
	diagnostic_manager_t& diag_m = app.get_diagnostic_manager();

	for(const auto& sig : diagnostic_messages)
	{
		DiagnosticRequest* diag_req = new DiagnosticRequest(sig->build_diagnostic_request());
		event_filter.frequency = std::isnan(event_filter.frequency) ? sig->get_frequency() : event_filter.frequency;
		std::shared_ptr<low_can_subscription_t> can_subscription;

		auto it =  std::find_if(s.begin(), s.end(), [&sig](std::pair<int, std::shared_ptr<low_can_subscription_t> > sub){ return (! sub.second->get_diagnostic_message().empty());});
		can_subscription = it != s.end() ?
			it->second : 
			std::make_shared<low_can_subscription_t>(low_can_subscription_t(event_filter));
		// If the requested diagnostic message isn't supported by the car then unsubcribe it
		// no matter what we want, worse case will be a fail unsubscription but at least we don't
		// poll a PID for nothing.
		//TODO: Adding callback requesting ignition status:	diag_req, sig.c_str(), false, diagnostic_message_t::decode_obd2_response, diagnostic_message_t::check_ignition_status, frequency);
		if(sig->get_supported() && subscribe)
		{
			diag_m.add_recurring_request(diag_req, sig->get_name().c_str(), false, sig->get_decoder(), sig->get_callback(), event_filter.frequency);
			if(can_subscription->create_rx_filter(sig) < 0)
				{return -1;}
			DEBUG(binder_interface, "%s: Signal: %s subscribed", __FUNCTION__, sig->get_name().c_str());
			if(it == s.end() && add_to_event_loop(can_subscription) < 0)
			{
				diag_m.cleanup_request(
					diag_m.find_recurring_request(*diag_req), true);
				WARNING(binder_interface, "%s: signal: %s isn't supported. Canceling operation.", __FUNCTION__,  sig->get_name().c_str());
				return -1;
			}
		}
		else
		{
			diag_m.cleanup_request(
					diag_m.find_recurring_request(*diag_req), true);
			if(sig->get_supported())
			{DEBUG(binder_interface, "%s: %s cancelled due to unsubscribe", __FUNCTION__, sig->get_name().c_str());}
			else
			{
				WARNING(binder_interface, "%s: signal: %s isn't supported. Canceling operation.", __FUNCTION__, sig->get_name().c_str());
				return -1;
			}
		}
		int ret = subscribe_unsubscribe_signal(request, subscribe, can_subscription, s);
		if(ret < 0)
			return ret;

		rets++;
	}

	return rets;
}

static int subscribe_unsubscribe_can_signals(struct afb_req request, bool subscribe, std::vector<std::shared_ptr<can_signal_t> > can_signals, struct event_filter_t& event_filter, std::map<int, std::shared_ptr<low_can_subscription_t> >& s)
{
	int rets = 0;
	for(const auto& sig: can_signals)
	{
		auto it =  std::find_if(s.begin(), s.end(), [&sig](std::pair<int, std::shared_ptr<low_can_subscription_t> > sub){ return sub.second->get_can_signal() == sig; });
		std::shared_ptr<low_can_subscription_t> can_subscription;
		if(it != s.end())
		{
			 can_subscription = it->second;
		}
		else
		{
			 can_subscription = std::make_shared<low_can_subscription_t>(low_can_subscription_t(event_filter));
			if(can_subscription->create_rx_filter(sig) < 0)
				{return -1;}
		}

		if(subscribe_unsubscribe_signal(request, subscribe, can_subscription, s) < 0)
			{return -1;}

		if(add_to_event_loop(can_subscription) < 0)
			{return -1;}
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
	utils::signals_manager_t& sm = utils::signals_manager_t::instance();

	std::lock_guard<std::mutex> subscribed_signals_lock(sm.get_subscribed_signals_mutex());
	std::map<int, std::shared_ptr<low_can_subscription_t> >& s = sm.get_subscribed_signals();

	rets += subscribe_unsubscribe_diagnostic_messages(request, subscribe, signals.diagnostic_messages, event_filter, s);
	rets += subscribe_unsubscribe_can_signals(request, subscribe, signals.can_signals, event_filter, s);

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
