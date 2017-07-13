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
#include "low-can-subscription.hpp"

#include <map>
#include <queue>
#include <mutex>
#include <vector>
#include <json-c/json.h>

#include "application.hpp"
#include "../can/can-bus.hpp"

extern "C"
{
	static int initv2();

	static constexpr struct afb_auth loa_afb_auth(const unsigned loa)
	{
		struct afb_auth a = {};
		a.type = afb_auth_LOA;
		a.loa = loa;
		return a;
	}

	static constexpr struct afb_auth perm_afb_auth(const char* permission)
	{
		struct afb_auth a = {};
		a.type = afb_auth_Permission;
		a.text = permission;
		return a;
	}

	static const struct afb_auth afb_auth_loa_1 = { loa_afb_auth(1) };
	static const struct afb_auth afb_auth_perm = { perm_afb_auth("urn:AGL:permission::platform:can:write") };

	static const struct afb_verb_v2 verbs[]=
	{
		{ .verb= "auth", .callback= auth, .auth= &afb_auth_perm, .info="Authentification against service to get the required level of confidence", .session= AFB_SESSION_NONE},
		{ .verb= "subscribe", .callback= subscribe, .auth= NULL, .info="Let subscribe to signals", .session= AFB_SESSION_NONE},
		{ .verb= "unsubscribe", .callback= unsubscribe, .auth= NULL, .info="Let unsubscribe signals", .session= AFB_SESSION_NONE},
		{ .verb= "swrite", .callback= swrite, .auth= &afb_auth_loa_1, .info="Write a single CAN message on a CAN bus", .session= AFB_SESSION_LOA_1},
		{ .verb= NULL, .callback= NULL, .auth= NULL, .info=NULL, .session= 0}
	};

	const struct afb_binding_v2 afbBindingV2 {
		.api = "low-can",
		.specification = NULL,
		.info = "API to Low level CAN service, read and decode the bus",
		.verbs = verbs,
		.preinit = NULL,
		.init = initv2,
		.onevent = NULL,
		.noconcurrency = 0
	};

	/// @brief Initialize the binding.
	///
	/// @param[in] service Structure which represent the Application Framework Binder.
	///
	/// @return Exit code, zero if success.
	static int initv2()
	{
		can_bus_t& can_bus_manager = application_t::instance().get_can_bus_manager();

		can_bus_manager.set_can_devices();
		can_bus_manager.start_threads();

		/// Initialize Diagnostic manager that will handle obd2 requests.
		/// We pass by default the first CAN bus device to its Initialization.
		/// TODO: be able to choose the CAN bus device that will be use as Diagnostic bus.
		if(application_t::instance().get_diagnostic_manager().initialize())
			return 0;

		AFB_ERROR("There was something wrong with CAN device Initialization.");
		return 1;
	}
};
