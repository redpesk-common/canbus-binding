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

/********************************************************************************
*
*		Subscription and unsubscription
*
*********************************************************************************/

std::vector<std::string> get_name(struct afb_req request)
{
	int i, n;
	std::vector<std::string> ret;
	struct json_object *args, *a, *x;

	args = afb_req_json(request);
	if (args == NULL || !json_object_object_get_ex(args, "event", &a))
		ret.clear();
	else if (json_object_get_type(a) != json_type_array)
		ret.push_back(json_object_get_string(a));
	else
	{
		n = json_object_array_length(a);
		for (i = 0 ; i < n ; i++) {
			x = json_object_array_get_idx(a, i);
			ret.push_back(json_object_get_string(x));
		}
	}

	return ret;
}

static const struct afb_verb_desc_v1 verbs[]=
{
	{ .name= "subscribe",	.session= AFB_SESSION_NONE, .callback= subscribe,	.info= "subscribe to notification of CAN bus messages." },
	{ .name= "unsubscribe",	.session= AFB_SESSION_NONE, .callback= unsubscribe,	.info= "unsubscribe a previous subscription." }
};

static const struct afb_binding binding_desc {
	AFB_BINDING_VERSION_1,
	{
		"CAN bus service",
		"can",
		verbs
	}
};

extern "C"
{
	static void subscribe(struct afb_req request)
	{
		int ret;
		std::vector<std::string> names;
		
		names = get_name(request);
		if(names.empty())
			ret = subscribe_signals(request, "*");
		else
			ret = subscribe_signals(request, names);
	}

	static void unsubscribe(struct afb_req request)
	{
		subscribe_unsubscribe(request, false);
	}

	const struct afb_binding *afbBindingV1Register (const struct afb_binding_interface *itf)
	{
		interface = itf;

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
		int fd_conf;
		fd_conf = afb_daemon_rootdir_open_locale(interface->daemon, "can_bus.json", O_RDONLY, NULL);

		/* Open CAN socket */
		can_bus_t can_bus_handler(interface, fd_conf);
		if(can_bus_handler.init_can_dev() == 0)
		{
			can_bus_handler.start_threads();
			return 0;
		}

		return 1;
	}
};