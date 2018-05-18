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

#include <systemd/sd-event.h>
#include <algorithm>
#include <string.h>

#include "diagnostic-manager.hpp"

#include "../utils/openxc-utils.hpp"
#include "../utils/signals.hpp"
#include "../binding/application.hpp"

#define MAX_RECURRING_DIAGNOSTIC_FREQUENCY_HZ 10
#define MAX_SIMULTANEOUS_DIAG_REQUESTS 50
// There are only 8 slots of in flight diagnostic requests
#define MAX_SIMULTANEOUS_IN_FLIGHT_REQUESTS 8
#define TIMERFD_ACCURACY 0
#define MICRO 1000000

diagnostic_manager_t::diagnostic_manager_t()
	: initialized_{false}
{}


diagnostic_manager_t::~diagnostic_manager_t()
{
	for(auto r: recurring_requests_)
	{
		delete(r);
	}
	for(auto r: non_recurring_requests_)
	{
		delete(r);
	}
}

/// @brief Diagnostic manager is not initialized at launch but after
///  the initialization of CAN bus devices. For the moment, it is only possible
///  to have 1 diagnostic bus which are the first bus declared in the JSON
///  description file. Configuration instance will return it.
///
/// this will initialize DiagnosticShims and cancel all active requests
///  if there are any.
bool diagnostic_manager_t::initialize()
{
	// Mandatory to set the bus before intialize shims.
	bus_ = application_t::instance().get_diagnostic_bus();

	init_diagnostic_shims();
	reset();

	initialized_ = true;
	AFB_DEBUG("Diagnostic Manager initialized");
	return initialized_;
}

/// @brief initialize shims used by UDS lib and set initialized_ to true.
///  It is needed before used the diagnostic manager fully because shims are
///  required by most member functions.
void diagnostic_manager_t::init_diagnostic_shims()
{
	shims_ = diagnostic_init_shims(shims_logger, shims_send, NULL);
	AFB_DEBUG("Shims initialized");
}

/// @brief Force cleanup all active requests.
void diagnostic_manager_t::reset()
{
	AFB_DEBUG("Clearing existing diagnostic requests");
	cleanup_active_requests(true);
}

/// @brief send function use by diagnostic library. It will open a BCM CAN socket TX_SETUP type.
/// That socket will send cyclic messages configured from a diagnostic request.
///
/// @param[in] arbitration_id - CAN arbitration ID to use when send message. OBD2 broadcast ID
///  is 0x7DF by example.
/// @param[in] data - The data payload for the message. NULL is valid if size is also 0.
/// @param[in] size - The size of the data payload, in bytes.
///
/// @return true if the CAN message was sent successfully.
bool diagnostic_manager_t::shims_send(const uint32_t arbitration_id, const uint8_t* data, const uint8_t size)
{
	diagnostic_manager_t& dm = application_t::instance().get_diagnostic_manager();
	active_diagnostic_request_t* current_adr = dm.get_last_recurring_requests();
	utils::socketcan_bcm_t& tx_socket = current_adr->get_socket();

	// Make sure that socket has been opened.
	if(! tx_socket)
		tx_socket.open(
			dm.get_bus_device_name());

	struct utils::simple_bcm_msg bcm_msg;
	struct can_frame cfd;

	memset(&cfd, 0, sizeof(cfd));
	memset(&bcm_msg.msg_head, 0, sizeof(bcm_msg.msg_head));

	struct timeval freq = current_adr->get_frequency_clock().get_timeval_from_period();

	bcm_msg.msg_head.opcode  = TX_SETUP;
	bcm_msg.msg_head.can_id  = arbitration_id;
	bcm_msg.msg_head.flags = SETTIMER|STARTTIMER|TX_CP_CAN_ID;
	bcm_msg.msg_head.ival2.tv_sec = freq.tv_sec;
	bcm_msg.msg_head.ival2.tv_usec = freq.tv_usec;
	bcm_msg.msg_head.nframes = 1;
	cfd.can_dlc = size;
	::memcpy(cfd.data, data, size);

	bcm_msg.frames = cfd;

	tx_socket << bcm_msg;
	if(tx_socket)
		return true;
	return false;
}

/// @brief The type signature for an optional logging function, if the user
/// wishes to provide one. It should print, store or otherwise display the
/// message.
///
/// message - A format string to log using the given parameters.
/// ... (vargs) - the parameters for the format string.
///
void diagnostic_manager_t::shims_logger(const char* format, ...)
{
	va_list args;
	va_start(args, format);

	char buffer[256];
	vsnprintf(buffer, 256, format, args);

	AFB_DEBUG("%s", buffer);
	va_end(args);
}

const std::string diagnostic_manager_t::get_bus_name() const
{
	return bus_;
}

const std::string diagnostic_manager_t::get_bus_device_name() const
{
	return application_t::instance().get_can_bus_manager()
		.get_can_device_name(bus_);
}

active_diagnostic_request_t* diagnostic_manager_t::get_last_recurring_requests() const
{
	return recurring_requests_.back();
}

/// @brief Return diagnostic manager shims member.
DiagnosticShims& diagnostic_manager_t::get_shims()
{
	return shims_;
}

/// @brief Search for a specific active diagnostic request in the provided requests list
/// and erase it from the vector. This is useful at unsubscription to clean up the list otherwize
/// all received CAN messages will be passed to DiagnosticRequestHandle of all active diagnostic request
/// contained in the vector but no event if connected to, so we will decode uneeded request.
///
/// @param[in] entry - a pointer of an active_diagnostic_request instance to clean up
/// @param[in] requests_list - a vector where to make the search and cleaning.
void diagnostic_manager_t::find_and_erase(active_diagnostic_request_t* entry, std::vector<active_diagnostic_request_t*>& requests_list)
{
	auto i = std::find(requests_list.begin(), requests_list.end(), entry);
	if ( i != requests_list.end())
		requests_list.erase(i);
}

/// @brief Free memory allocated on active_diagnostic_request_t object and close the socket.
void diagnostic_manager_t::cancel_request(active_diagnostic_request_t* entry)
{
	entry->get_socket().close();
	delete entry;
	entry = nullptr;
}

/// @brief Cleanup a specific request if it isn't running and get complete. As it is almost
/// impossible to get that state for a recurring request without waiting for that, you can
/// force the cleaning operation.
///
/// @param[in] entry - the request to clean
/// @param[in] force - Force the cleaning or not ?
void diagnostic_manager_t::cleanup_request(active_diagnostic_request_t* entry, bool force)
{
	if(entry != nullptr && (force || entry->response_received()))
	{
		char request_string[128] = {0};
		diagnostic_request_to_string(&entry->get_handle()->request,
			request_string, sizeof(request_string));
		if(force && entry->get_recurring())
		{
			cancel_request(entry);
			find_and_erase(entry, recurring_requests_);
			AFB_DEBUG("Cancelling completed, recurring request: %s", request_string);
		}
		else if (!entry->get_recurring())
		{
			AFB_DEBUG("Cancelling completed, non-recurring request: %s", request_string);
			cancel_request(entry);
			find_and_erase(entry, non_recurring_requests_);
		}
	}
}

/// @brief Clean up all requests lists, recurring and not recurring.
///
/// @param[in] force - Force the cleaning or not ? If true, that will do
/// the same effect as a call to reset().
void diagnostic_manager_t::cleanup_active_requests(bool force)
{
	for(auto& entry : non_recurring_requests_)
	{
		if (entry != nullptr)
			cleanup_request(entry, force);
	}

	for(auto& entry : recurring_requests_)
	 {
		if (entry != nullptr)
			cleanup_request(entry, force);
	 }
}

/// @brief Will return the active_diagnostic_request_t pointer for theDiagnosticRequest or nullptr if
/// not found.
///
/// @param[in] request - Search key, method will go through recurring list to see if it find that request
///  holded by the DiagnosticHandle member.
active_diagnostic_request_t* diagnostic_manager_t::find_recurring_request(DiagnosticRequest& request)
{
	for (auto& entry : recurring_requests_)
	{
		if(entry != nullptr)
		{
			if(diagnostic_request_equals(&entry->get_handle()->request, &request))
				{return entry;}
		}
	}
	return nullptr;
}

/// @brief Add and send a new one-time diagnostic request. DON'T USED AT THIS TIME
///
/// A one-time (aka non-recurring) request can existing in parallel with a
/// recurring request for the same PID or mode, that's not a problem.
///
/// For an example, see the docs for addRecurringRequest. This function is very
/// similar but leaves out the frequencyHz parameter.
///
/// @param[in] request - The parameters for the request.
/// @param[in] name - Human readable name this response, to be used when
///      publishing received responses.
/// @param[in] wait_for_multiple_responses - If false, When any response is received
///      for this request it will be removed from the active list. If true, the
///      request will remain active until the timeout clock expires, to allow it
///      to receive multiple response. Functional broadcast requests will always
///      waint for the timeout, regardless of this parameter.
/// @param[in] decoder - An optional DiagnosticResponseDecoder to parse the payload of
///      responses to this request. If the decoder is NULL, the output will
///      include the raw payload instead of a parsed value.
/// @param[in] callback - An optional DiagnosticResponseCallback to be notified whenever a
///      response is received for this request.
///
/// @return true if the request was added successfully. Returns false if there
/// wasn't a free active request entry.
active_diagnostic_request_t* diagnostic_manager_t::add_request(DiagnosticRequest* request, const std::string& name,
	bool wait_for_multiple_responses, const DiagnosticResponseDecoder decoder,
	const DiagnosticResponseCallback callback)
{
	cleanup_active_requests(false);

	active_diagnostic_request_t* entry = nullptr;

	if (non_recurring_requests_.size() <= MAX_SIMULTANEOUS_DIAG_REQUESTS)
	{
		active_diagnostic_request_t* entry = new active_diagnostic_request_t(bus_, request->arbitration_id, name,
				wait_for_multiple_responses, decoder, callback, 0, false);
		entry->set_handle(shims_, request);

		char request_string[128] = {0};
		diagnostic_request_to_string(&entry->get_handle()->request, request_string,
				sizeof(request_string));

		// Erase any existing request not already cleaned.
		cleanup_request(entry, true);
		AFB_DEBUG("Added one-time diagnostic request on bus %s: %s",
				bus_.c_str(), request_string);

		non_recurring_requests_.push_back(entry);
	}
	else
	{
		AFB_WARNING("There isn't enough request entry. Vector exhausted %d/%d", (int)non_recurring_requests_.size(), MAX_SIMULTANEOUS_DIAG_REQUESTS);
		non_recurring_requests_.resize(MAX_SIMULTANEOUS_DIAG_REQUESTS);
	}
	return entry;
}

/// @brief Validate frequency asked don't get higher than the maximum of a classical
/// CAN bus OBD2 request.
///
/// @param[in] frequencyHz - frequency asked for sending diagnostic requests.
///
/// @return True if frequency is below the Maximum false if not.
bool diagnostic_manager_t::validate_optional_request_attributes(float frequencyHz)
{
	if(frequencyHz > MAX_RECURRING_DIAGNOSTIC_FREQUENCY_HZ) {
		AFB_DEBUG("Requested recurring diagnostic frequency %lf is higher than maximum of %d",
			frequencyHz, MAX_RECURRING_DIAGNOSTIC_FREQUENCY_HZ);
		return false;
	}
	return true;
}

/// @brief Add and send a new recurring diagnostic request.
///
/// At most one recurring request can be active for the same arbitration ID, mode
/// and (if set) PID on the same bus at one time. If you try and call
/// add_recurring_request with the same key, it will return an error.
///
/// @param[in] request - The parameters for the request.
/// @param[in] name - An optional human readable name this response, to be used when
///      publishing received responses. If the name is NULL, the published output
///      will use the raw OBD-II response format.
/// @param[in] wait_for_multiple_responses - If false, When any response is received
///      for this request it will be removed from the active list. If true, the
///      request will remain active until the timeout clock expires, to allow it
///      to receive multiple response. Functional broadcast requests will always
///      waint for the timeout, regardless of this parameter.
/// @param[in] decoder - An optional DiagnosticResponseDecoder to parse the payload of
///      responses to this request. If the decoder is NULL, the output will
///      include the raw payload instead of a parsed value.
/// @param[in] callback - An optional DiagnosticResponseCallback to be notified whenever a
///      response is received for this request.
/// @param[in] frequencyHz - The frequency (in Hz) to send the request. A frequency above
///      MAX_RECURRING_DIAGNOSTIC_FREQUENCY_HZ is not allowed, and will make this
///      function return false.
///
/// @return true if the request was added successfully. Returns false if there
/// was too much already running requests, or if the frequency was too high.
active_diagnostic_request_t* diagnostic_manager_t::add_recurring_request(DiagnosticRequest* request, const char* name,
		bool wait_for_multiple_responses, const DiagnosticResponseDecoder decoder,
		const DiagnosticResponseCallback callback, float frequencyHz, bool permanent)
{
	active_diagnostic_request_t* entry = nullptr;

	if(!validate_optional_request_attributes(frequencyHz))
		return entry;

	cleanup_active_requests(false);

	if(find_recurring_request(*request) == nullptr)
	{
		if(recurring_requests_.size() <= MAX_SIMULTANEOUS_DIAG_REQUESTS)
		{
			entry = new active_diagnostic_request_t(bus_, request->arbitration_id, name,
					wait_for_multiple_responses, decoder, callback, frequencyHz, permanent);
			recurring_requests_.push_back(entry);

			entry->set_handle(shims_, request);
			start_diagnostic_request(&shims_, entry->get_handle());
		}
		else
		{
			AFB_WARNING("There isn't enough request entry. Vector exhausted %d/%d", (int)recurring_requests_.size(), MAX_SIMULTANEOUS_DIAG_REQUESTS);
			recurring_requests_.resize(MAX_SIMULTANEOUS_DIAG_REQUESTS);
		}
	}
	else
		{ AFB_DEBUG("Can't add request, one already exists with same key");}
	return entry;
}

/// @brief Will decode the diagnostic response and build the final openxc_VehicleMessage to return.
///
/// @param[in] adr - A pointer to an active diagnostic request holding a valid diagnostic handle
/// @param[in] response - The response to decode from which the Vehicle message will be built and returned
///
/// @return A filled openxc_VehicleMessage or a zeroed struct if there is an error.
openxc_VehicleMessage diagnostic_manager_t::relay_diagnostic_response(active_diagnostic_request_t* adr, const DiagnosticResponse& response, const uint64_t timestamp)
{
	openxc_VehicleMessage message = build_VehicleMessage();
	float value = (float)diagnostic_payload_to_integer(&response);

	struct utils::signals_found found_signals;
	found_signals = utils::signals_manager_t::instance().find_signals(build_DynamicField((double) adr->get_pid()));

	if(adr->get_decoder() != nullptr)
	{
		value = adr->get_decoder()(&response, value);
	}

	if((response.success && adr->get_name().size()) > 0)
	{
		// If name, include 'value' instead of payload, and leave of response
		// details.
		message = build_VehicleMessage(build_SimpleMessage(adr->get_name(), build_DynamicField(value)));
		message.has_diagnostic_response = true;
		message.diagnostic_response = build_VehicleMessage(adr, response, value).diagnostic_response;
	}
	else
	{
		// If no name, only send full details of response but still include 'value'
		// instead of 'payload' if they provided a decoder. The one case you
		// can't get is the full detailed response with 'value'. We could add
		// another parameter for that but it's onerous to carry that around.
		message = build_VehicleMessage(adr, response, value);
	}

	// If not success but completed then the pid isn't supported
	if(!response.success)
	{
		found_signals.diagnostic_messages.front()->set_supported(false);
		cleanup_request(adr, true);
		AFB_NOTICE("PID not supported or ill formed. Please unsubscribe from it. Error code : %d", response.negative_response_code);
		message = build_VehicleMessage(build_SimpleMessage(adr->get_name(), build_DynamicField("This PID isn't supported by your vehicle.")));
	}

	if(adr->get_callback() != nullptr)
	{
		adr->get_callback()(adr, &response, value);
	}

	// Reset the completed flag handle to make sure that it will be reprocessed the next time.
	adr->get_handle()->success = false;

	// Save value and timestamp of diagnostic message
	if(!found_signals.diagnostic_messages.empty())
	{
		// Then, for each diag_message found
		for(const auto& diag_mess: found_signals.diagnostic_messages)
		{
			// Save value and timestamp for this message
			diag_mess->set_received(true);
			diag_mess->set_last_value(value);
			diag_mess->set_timestamp(timestamp);
		}
	}

	return message;
}

/// @brief Will take the CAN message and pass it to the receive functions that will process
/// diagnostic handle for each active diagnostic request then depending on the result we will
/// return pass the diagnostic response to decode it.
///
/// @param[in] entry - A pointer to an active diagnostic request holding a valid diagnostic handle
/// @param[in] cm - A raw CAN message.
///
/// @return A pointer to a filled openxc_VehicleMessage or a nullptr if nothing has been found.
openxc_VehicleMessage diagnostic_manager_t::relay_diagnostic_handle(active_diagnostic_request_t* entry, const can_message_t& cm)
{
	DiagnosticResponse response = diagnostic_receive_can_frame(&shims_, entry->get_handle(), cm.get_id(), cm.get_data(), cm.get_length());
	if(response.completed && entry->get_handle()->completed)
	{
		if(entry->get_handle()->success)
			return relay_diagnostic_response(entry, response, cm.get_timestamp());
	}
	else if(!response.completed && response.multi_frame)
	{
		// Reset the timeout clock while completing the multi-frame receive
		entry->get_timeout_clock().tick(
			entry->get_timeout_clock().get_time_function()());
	}

	return build_VehicleMessage();
}

/// @brief Find the active diagnostic request with the correct DiagnosticRequestHandle
/// member that will understand the CAN message using diagnostic_receive_can_frame function
/// from UDS-C library. Then decode it with an ad-hoc method.
///
/// @param[in] cm - Raw CAN message received
///
/// @return VehicleMessage with decoded value.
openxc_VehicleMessage diagnostic_manager_t::find_and_decode_adr(const can_message_t& cm)
{
	openxc_VehicleMessage vehicle_message = build_VehicleMessage();

	for ( auto entry : non_recurring_requests_)
	{
		vehicle_message = relay_diagnostic_handle(entry, cm);
		if (is_valid(vehicle_message))
			return vehicle_message;
	}

	for ( auto entry : recurring_requests_)
	{
		vehicle_message = relay_diagnostic_handle(entry, cm);
		if (is_valid(vehicle_message))
			return vehicle_message;
	}

	return vehicle_message;
}

/// @brief Tell if the CAN message received is a diagnostic response.
/// Request broadcast ID use 0x7DF and assigned ID goes from 0x7E0 to Ox7E7. That allows up to 8 ECU to respond
/// at the same time. The response is the assigned ID + 0x8, so response ID can goes from 0x7E8 to 0x7EF.
///
/// @param[in] cm - CAN message received from the socket.
///
/// @return True if the active diagnostic request match the response.
bool diagnostic_manager_t::is_diagnostic_response(const can_message_t& cm)
{
	if (cm.get_id() >= 0x7e8 && cm.get_id() <= 0x7ef)
			return true;
	return false;
}
