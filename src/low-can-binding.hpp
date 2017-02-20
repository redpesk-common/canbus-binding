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
 
#pragma once

#include "can-signals.hpp"

extern "C"
{
	#include <afb/afb-binding.h>
	#include <afb/afb-service-itf.h>

	static void subscribe(struct afb_req request);
	static void unsubscribe(struct afb_req request);

	/**
	* @brief Register the binding.
	*
	* @desc - A binding V1 MUST have a function of this name and signature.
	* This function is called during loading of the binding. It
	* receives an 'interface' that should be recorded for later access to
	* functions provided by the framework.
	*
	* This function MUST return the address of a structure that describes
	* the binding and its implemented verbs.
	*
	* In case of initialisation error, NULL must be returned.
	* 
	* @param[in] const struct afb_binding_interface *itf - interface to the application framework binder.
	* 
	* @return pointer to the binding or NULL in case of error
	*/
	const struct afb_binding *afbBindingV1Register (const struct afb_binding_interface *itf);

	/**
	* @brief Initialize the binding.
	* 
	* @param[in] service Structure which represent the Application Framework Binder.
	* 
	* @return Exit code, zero if success.
	*/
	int afbBindingV1ServiceInit(struct afb_service service);
}

/*
 *	Interface between the daemon and the binding
 */
static const struct afb_binding_interface *interface;