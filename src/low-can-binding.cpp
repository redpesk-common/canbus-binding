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

#include "low-can-binding.hpp"

#include <queue>
#include <mutex>
#include <vector>
#include <thread>
#include <time.h>
#include <linux/can.h>
#include <json-c/json.h>

#include "openxc.pb.h"
#include "configuration.hpp"
#include "can/can-bus.hpp"
#include "can/can-signals.hpp"
#include "can/can-message.hpp"
#include "utils/timer.hpp"
#include "utils/signals.hpp"
#include "diagnostic/diagnostic-message.hpp"
#include "utils/openxc-utils.hpp"

extern "C"
{
	#include <afb/afb-service-itf.h>
};

// Interface between the daemon and the binding
const struct afb_binding_interface *binder_interface;

/********************************************************************************
*
*		Subscription and unsubscription
*
*********************************************************************************/

static int make_subscription_unsubscription(struct afb_req request, const std::string& sig_name, std::map<std::string, struct afb_event>& s, bool subscribe)
{
	/* Make the subscription or unsubscription to the event */
	if (((subscribe ? afb_req_subscribe : afb_req_unsubscribe)(request, s[sig_name])) < 0)
	{
		ERROR(binder_interface, "make_subscription_unsubscription: Operation goes wrong for signal: %s", sig_name.c_str());
		return 0;
	}
	return 1;

}

static int create_event_handle(const std::string& sig_name, std::map<std::string, struct afb_event>& s)
{
	s[sig_name] = afb_daemon_make_event(binder_interface->daemon, sig_name.c_str());
	if (!afb_event_is_valid(s[sig_name]))
	{
		ERROR(binder_interface, "create_event_handle: Can't create an event for %s, something goes wrong.", sig_name.c_str());
		return 0;
	}
	return 1;
}

static int subscribe_unsubscribe_signal(struct afb_req request, bool subscribe, const std::string& sig)
{
	int ret;

	std::lock_guard<std::mutex> subscribed_signals_lock(get_subscribed_signals_mutex());
	std::map<std::string, struct afb_event>& s = get_subscribed_signals();
	if (s.find(sig) != s.end())
	{
		if (!afb_event_is_valid(s[sig]) && !subscribe)
		{
			NOTICE(binder_interface, "subscribe_unsubscribe_signal: Event isn't valid, it can't be unsubscribed.");
			ret = -1;
		}
		/*else
		{
			// Event it isn't valid annymore, recreate it
			ret = create_event_handle(sig, s);
		}*/
	}
	else
	{
		/* Event doesn't exist , so let's create it */
		struct afb_event empty_event = {nullptr, nullptr};
		subscribed_signals[sig] = empty_event;
		ret = create_event_handle(sig, s);
	}

	/* Check whether or not the event handler has been correctly created and
	 * make the subscription/unsubscription operation is so.
	 */
	if (ret <= 0)
		return ret;
	return make_subscription_unsubscription(request, sig, s, subscribe);
}

/**
 * @fn static int subscribe_unsubscribe_signals(struct afb_req request, bool subscribe, const std::vector<can_signal_t>& signals)
 * @brief subscribe to all signals in the vector signals
 *
 * @param[in] afb_req request : contain original request use to subscribe or unsubscribe
 * @param[in] subscribe boolean value used to chose between a subscription operation or an unsubscription
 * @param[in] can_signal_t  vector with can_signal_t to subscribe
 *
 * @return Number of correctly subscribed signal
 */
static int subscribe_unsubscribe_signals(struct afb_req request, bool subscribe, const std::vector<std::string>& signals)
{
	int rets = 0;

	//TODO: Implement way to dynamically call the right function no matter
	// how much signals types we have.

	for(const std::string& sig : signals)
	{
		int ret;
		if (active_diagnostic_request_t::is_diagnostic_signal(sig))
		{
			std::vector<diagnostic_message_t*> found;
			configuration_t::instance().find_diagnostic_messages(build_DynamicField(sig), found);
			DiagnosticRequest* diag_req = new DiagnosticRequest(found.front()->build_diagnostic_request());

			// If the requested diagnostic message isn't supported by the car then unssubcribe.
			// no matter what we want, worse case will be a fail unsubscription but at least we don't
			// poll a PID for nothing.
			if(found.front()->get_supported())
				subscribe = false;
			if(subscribe)
			{
				float frequency = found.front()->get_frequency();
				configuration_t::instance().get_diagnostic_manager().add_recurring_request(
					diag_req, sig.c_str(), false, found.front()->get_decoder(), found.front()->get_callback(), (float)frequency);
					//TODO: Adding callback requesting ignition status:	diag_req, sig.c_str(), false, diagnostic_message_t::decode_obd2_response, diagnostic_message_t::check_ignition_status, frequency);
			}
			else
			{
				configuration_t::instance().get_diagnostic_manager().cleanup_request(
					configuration_t::instance().get_diagnostic_manager().find_recurring_request(diag_req), true);
			}
		}

		ret = subscribe_unsubscribe_signal(request, subscribe, sig);
		if(ret <= 0)
			return ret;

		rets++;
		DEBUG(binder_interface, "Signal: %s subscribed", sig.c_str());
	}
	return rets;
}

static int subscribe_unsubscribe_name(struct afb_req request, bool subscribe, const char *name)
{
	std::vector<std::string> signals;
	int ret = 0;

	openxc_DynamicField search_key = build_DynamicField(std::string(name));
	signals = find_signals(search_key);
	if (signals.empty())
		ret = 0;

	ret = subscribe_unsubscribe_signals(request, subscribe, signals);
	NOTICE(binder_interface, "Subscribed/unsubscribe correctly to %d/%d signal(s).", ret, (int)signals.size());

	return ret;
}

static void subscribe_unsubscribe(struct afb_req request, bool subscribe)
{
	int ok, i, n;
	struct json_object *args, *a, *x;

	/* makes the subscription/unsubscription */
	args = afb_req_json(request);
	if (args == NULL || !json_object_object_get_ex(args, "event", &a)) {
		ok = subscribe_unsubscribe_name(request, subscribe, "*");
	} else if (json_object_get_type(a) != json_type_array) {
		ok = subscribe_unsubscribe_name(request, subscribe, json_object_get_string(a));
	} else {
		n = json_object_array_length(a);
		ok = 0;
		for (i = 0 ; i < n ; i++) {
			x = json_object_array_get_idx(a, i);
			if (subscribe_unsubscribe_name(request, subscribe, json_object_get_string(x)))
				ok++;
		}
		ok = (ok == n);
	}

	/* send the report */
	if (ok)
		afb_req_success(request, NULL, NULL);
	else
		afb_req_fail(request, "error", NULL);
}

extern "C"
{
	static void subscribe(struct afb_req request)
	{
		subscribe_unsubscribe(request, true);
	}

	static void unsubscribe(struct afb_req request)
	{
		subscribe_unsubscribe(request, false);
	}

	static const struct afb_verb_desc_v1 verbs[]=
	{
		{ .name= "subscribe",	.session= AFB_SESSION_NONE, .callback= subscribe,	.info= "subscribe to notification of CAN bus messages." },
		{ .name= "unsubscribe",	.session= AFB_SESSION_NONE, .callback= unsubscribe,	.info= "unsubscribe a previous subscription." }
	};

	static const struct afb_binding binding_desc {
		AFB_BINDING_VERSION_1,
		{
			"Low level CAN bus service",
			"low-can",
			verbs
		}
	};

	const struct afb_binding *afbBindingV1Register (const struct afb_binding_interface *itf)
	{
		binder_interface = itf;

		return &binding_desc;
	}

	/**
	* @brief Initialize the binding.
	*
	* @param[in] service Structure which represent the Application Framework Binder.
	*
	* @return Exit code, zero if success.
	*/
	int afbBindingV1ServiceInit(struct afb_service service)
	{
		can_bus_t& can_bus_manager = configuration_t::instance().get_can_bus_manager();

		/// Initialize CAN socket
		if(can_bus_manager.init_can_dev() == 0)
			can_bus_manager.start_threads();

		/// Initialize Diagnostic manager that will handle obd2 requests.
		/// We pass by default the first CAN bus device to its Initialization.
		/// TODO: be able to choose the CAN bus device that will be use as Diagnostic bus.
		if(configuration_t::instance().get_diagnostic_manager().initialize())
			return 0;

		ERROR(binder_interface, "There was something wrong with CAN device Initialization. Check your config file maybe");
		return 1;
	}
};
