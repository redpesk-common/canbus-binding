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
#include <json-c/json.h>

#include "configuration.hpp"
#include "../can/can-bus.hpp"

extern "C"
{
	#include <afb/afb-service-itf.h>
};

// Interface between the daemon and the binding
const struct afb_binding_interface *binder_interface;

extern "C"
{
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

	/// @brief Initialize the binding.
	///
	/// @param[in] service Structure which represent the Application Framework Binder.
	///
	/// @return Exit code, zero if success.
	int afbBindingV1ServiceInit(struct afb_service service)
	{
		can_bus_t& can_bus_manager = configuration_t::instance().get_can_bus_manager();

		can_bus_manager.set_can_devices();
		can_bus_manager.start_threads();

		/// Initialize Diagnostic manager that will handle obd2 requests.
		/// We pass by default the first CAN bus device to its Initialization.
		/// TODO: be able to choose the CAN bus device that will be use as Diagnostic bus.
		if(configuration_t::instance().get_diagnostic_manager().initialize())
			return 0;

		ERROR(binder_interface, "%s: There was something wrong with CAN device Initialization.", __FUNCTION__);
		return 1;
	}
};
