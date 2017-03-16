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

#include "diagnostic-manager.hpp"

#include "../utils/openxc-utils.hpp"
#include "../configuration.hpp"

#define MAX_RECURRING_DIAGNOSTIC_FREQUENCY_HZ 10
#define MAX_SIMULTANEOUS_DIAG_REQUESTS 50
#define TIMERFD_ACCURACY 0
#define MICRO 1000000

diagnostic_manager_t::diagnostic_manager_t()
	: initialized_{false}
{}

/// @brief Diagnostic manager isn't initialized at launch but after
///  CAN bus devices initialization. For the moment, it is only possible
///  to have 1 diagnostic bus which are the first bus declared in the JSON
///  description file. Configuration instance will return it.
///
/// @desc this will initialize DiagnosticShims and cancel all active requests 
///  if there are any.
bool diagnostic_manager_t::initialize()
{
	// Mandatory to set the bus before intialize shims.
	bus_ = configuration_t::instance().get_diagnostic_bus();

	init_diagnostic_shims();
	reset();

	initialized_ = true;
	DEBUG(binder_interface, "initialize: Diagnostic Manager initialized");
	return initialized_;
}

/// @brief initialize shims used by UDS lib and set initialized_ to true.
///  It is needed before used the diagnostic manager fully because shims are
///  required by most member functions.
void diagnostic_manager_t::init_diagnostic_shims()
{
	shims_ = diagnostic_init_shims(shims_logger, shims_send, NULL);
	DEBUG(binder_interface, "init_diagnostic_shims: Shims initialized");
}

/// @brief Force cleanup all active requests.
void diagnostic_manager_t::reset()
{
	DEBUG(binder_interface, "Clearing existing diagnostic requests");
	cleanup_active_requests(true);
}

/// @brief send function use by diagnostic library. Only one bus used for now
///  so diagnostic request is sent using the default diagnostic bus not matter of
///  which is specified in the diagnostic message definition.
///
/// @param[in] arbitration_id - CAN arbitration ID to use when send message. OBD2 broadcast ID
///  is 0x7DF by example.
/// @param[in] data - The data payload for the message. NULL is valid if size is also 0.
/// @param[in] size - The size of the data payload, in bytes.
///
/// @return true if the CAN message was sent successfully. 
bool diagnostic_manager_t::shims_send(const uint32_t arbitration_id, const uint8_t* data, const uint8_t size)
{
	std::shared_ptr<can_bus_dev_t> can_bus_dev = can_bus_t::get_can_device(configuration_t::instance().get_diagnostic_manager().bus_);
	return can_bus_dev->shims_send(arbitration_id, data, size);
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

	DEBUG(binder_interface, "shims_logger: %s", buffer);
}

/// @brief The type signature for a... OpenXC TODO: not used yet.
void diagnostic_manager_t::shims_timer()
{}

std::shared_ptr<can_bus_dev_t> diagnostic_manager_t::get_can_bus_dev()
{
	return can_bus_t::get_can_device(bus_);
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

// @brief TODO: implement cancel_request if needed... Don't know.
void diagnostic_manager_t::cancel_request(active_diagnostic_request_t* entry)
{

	/* TODO: implement acceptance filters.
	if(entry.arbitration_id_ == OBD2_FUNCTIONAL_BROADCAST_ID) {
		for(uint32_t filter = OBD2_FUNCTIONAL_RESPONSE_START;
				filter < OBD2_FUNCTIONAL_RESPONSE_START +
					OBD2_FUNCTIONAL_RESPONSE_COUNT;
				filter++) {
			removeAcceptanceFilter(entry.bus_, filter,
					CanMessageFormat::STANDARD, getCanBuses(),
					getCanBusCount());
		}
	} else {
		removeAcceptanceFilter(entry.bus_,
				entry.arbitration_id_ +
					DIAGNOSTIC_RESPONSE_ARBITRATION_ID_OFFSET,
				CanMessageFormat::STANDARD, getCanBuses(), getCanBusCount());
	}*/
}

/// @brief Cleanup a specific request if it isn't running and get complete. As it is almost
/// impossible to get that state for a recurring request without waiting for that, you can 
/// force the cleaning operation.
///
/// @param[in] entry - the request to clean
/// @param[in] force - Force the cleaning or not ?
void diagnostic_manager_t::cleanup_request(active_diagnostic_request_t* entry, bool force)
{
	if(force || (entry->get_in_flight() && entry->request_completed()))
	{
		entry->set_in_flight(false);

		char request_string[128] = {0};
		diagnostic_request_to_string(&entry->get_handle()->request,
			request_string, sizeof(request_string));
		if(entry->get_recurring())
		{
			find_and_erase(entry, recurring_requests_);
			if(force)
				cancel_request(entry);
			DEBUG(binder_interface, "cleanup_request: Cancelling completed, recurring request: %s", request_string);
		}
		else
		{
			DEBUG(binder_interface, "cleanup_request: Cancelling completed, non-recurring request: %s", request_string);
			find_and_erase(entry, non_recurring_requests_);
			cancel_request(entry);
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
		if (entry != nullptr)
			cleanup_request(entry, force);

	for(auto& entry : recurring_requests_)
		if (entry != nullptr)
			cleanup_request(entry, force);
}

/// @brief Will return the active_diagnostic_request_t pointer for theDiagnosticRequest or nullptr if
/// not found.
///
/// @param[in] request - Search key, method will go through recurring list to see if it find that request
///  holded by the DiagnosticHandle member.
active_diagnostic_request_t* diagnostic_manager_t::find_recurring_request(const DiagnosticRequest* request)
{
	for (auto& entry : recurring_requests_)
	{
		if(entry != nullptr)
		{
			if(diagnostic_request_equals(&entry->get_handle()->request, request))
			{
				return entry;
				break;
			}
		}
	}
	return nullptr;
}

/// @brief Add and send a new one-time diagnostic request.
///
/// @desc A one-time (aka non-recurring) request can existing in parallel with a
/// recurring request for the same PID or mode, that's not a problem.
///
/// For an example, see the docs for addRecurringRequest. This function is very
/// similar but leaves out the frequencyHz parameter.
///
/// @param[in] request - The parameters for the request.
/// @param[in] name - Human readable name this response, to be used when
///      publishing received responses. TODO: If the name is NULL, the published output
///      will use the raw OBD-II response format.
/// @param[in] waitForMultipleResponses - If false, When any response is received
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
/// wasn't a free active request entry, if the frequency was too high or if the
/// CAN acceptance filters could not be configured,
bool diagnostic_manager_t::add_request(DiagnosticRequest* request, const std::string name,
	bool wait_for_multiple_responses, const DiagnosticResponseDecoder decoder,
	const DiagnosticResponseCallback callback)
{
	cleanup_active_requests(false);

	bool added = true;

	if (non_recurring_requests_.size() <= MAX_SIMULTANEOUS_DIAG_REQUESTS)
	{
		// TODO: implement Acceptance Filter
		//	if(updateRequiredAcceptanceFilters(bus, request)) {
			active_diagnostic_request_t* entry = new active_diagnostic_request_t(bus_, request, name,
					wait_for_multiple_responses, decoder, callback, 0);
			entry->set_handle(shims_, request);

			char request_string[128] = {0};
			diagnostic_request_to_string(&entry->get_handle()->request, request_string,
					sizeof(request_string));

			find_and_erase(entry, non_recurring_requests_);
			DEBUG(binder_interface, "Added one-time diagnostic request on bus %s: %s",
					bus_, request_string);

			non_recurring_requests_.push_back(entry);
	}
	else
	{
		WARNING(binder_interface, "There isn't enough request entry. Vector exhausted %d/%d", (int)non_recurring_requests_.size());
		non_recurring_requests_.resize(MAX_SIMULTANEOUS_DIAG_REQUESTS);
		added = false;
	}
	return added;
}

bool diagnostic_manager_t::validate_optional_request_attributes(float frequencyHz)
{
	if(frequencyHz > MAX_RECURRING_DIAGNOSTIC_FREQUENCY_HZ) {
		DEBUG(binder_interface, "Requested recurring diagnostic frequency %d is higher than maximum of %d",
			frequencyHz, MAX_RECURRING_DIAGNOSTIC_FREQUENCY_HZ);
		return false;
	}
	return true;
}

/// @brief Add and send a new recurring diagnostic request.
///
/// At most one recurring request can be active for the same arbitration ID, mode
/// and (if set) PID on the same bus at one time. If you try and call
/// addRecurringRequest with the same key, it will return an error.
///
/// TODO: This also adds any neccessary CAN acceptance filters so we can receive the
/// response. If the request is to the functional broadcast ID (0x7df) filters
/// are added for all functional addresses (0x7e8 to 0x7f0).
///
/// Example:
///
///     // Creating a functional broadcast, mode 1 request for PID 2.
///     DiagnosticRequest request = {
///         arbitration_id: 0x7df,
///         mode: 1,
///         has_pid: true,
///         pid: 2
///     };
///
///     // Add a recurring request, to be sent at 1Hz, and published with the
///     // name "my_pid_request"
///     addRecurringRequest(&getConfiguration()->diagnosticsManager,
///          canBus,
///          &request,
///          "my_pid_request",
///          false,
///          NULL,
///          NULL,
///          1);
///
/// @param[in] request - The parameters for the request.
/// @param[in] name - An optional human readable name this response, to be used when
///      publishing received responses. If the name is NULL, the published output
///      will use the raw OBD-II response format.
/// @param[in] waitForMultipleResponses - If false, When any response is received
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
/// was too much already running requests, if the frequency was too high TODO:or if the
/// CAN acceptance filters could not be configured,
///
bool diagnostic_manager_t::add_recurring_request(DiagnosticRequest* request, const char* name,
		bool wait_for_multiple_responses, const DiagnosticResponseDecoder decoder,
		const DiagnosticResponseCallback callback, float frequencyHz)
{
	if(!validate_optional_request_attributes(frequencyHz))
		return false;

	cleanup_active_requests(false);

	bool added = true;
	if(find_recurring_request(request) == nullptr)
	{
		if(recurring_requests_.size() <= MAX_SIMULTANEOUS_DIAG_REQUESTS)
		{
			sd_event_source *source;
			// TODO: implement Acceptance Filter
			//if(updateRequiredAcceptanceFilters(bus, request)) {
			active_diagnostic_request_t* entry = new active_diagnostic_request_t(bus_, request, name,
					wait_for_multiple_responses, decoder, callback, frequencyHz);
			entry->set_handle(shims_, request);

			char request_string[128] = {0};
			diagnostic_request_to_string(&entry->get_handle()->request, request_string,
					sizeof(request_string));

			DEBUG(binder_interface, "add_recurring_request: Added recurring diagnostic request (freq: %f) on bus %s: %s",
					frequencyHz, bus_.c_str(), request_string);

			uint64_t usec;
			sd_event_now(afb_daemon_get_event_loop(binder_interface->daemon), CLOCK_MONOTONIC, &usec);
			if(sd_event_add_time(afb_daemon_get_event_loop(binder_interface->daemon), &source,
					CLOCK_MONOTONIC, usec, TIMERFD_ACCURACY, send_request, request) < 0)
			{
				ERROR(binder_interface, "add_recurring_request: Request fails to be schedule through event loop");
				added = false;
			}
			recurring_requests_.push_back(entry);
		}
		else
		{
			WARNING(binder_interface, "add_recurring_request: There isn't enough request entry. Vector exhausted %d/%d", (int)recurring_requests_.size(), MAX_SIMULTANEOUS_DIAG_REQUESTS);
			recurring_requests_.resize(MAX_SIMULTANEOUS_DIAG_REQUESTS);
			added = false;
		}
	}
	else
	{
		DEBUG(binder_interface, "add_recurring_request: Can't add request, one already exists with same key");
		added = false;
	}
	return added;
}

/// @brief Returns true if there are two active requests running for the same arbitration ID.
bool diagnostic_manager_t::conflicting(active_diagnostic_request_t* request, active_diagnostic_request_t* candidate) const
{
	return (candidate->get_in_flight() && candidate != request &&
			candidate->get_can_bus_dev() == request->get_can_bus_dev() &&
			candidate->get_id() == request->get_id());
}


/// @brief Returns true if there are no other active requests to the same arbitration ID.
bool diagnostic_manager_t::clear_to_send(active_diagnostic_request_t* request) const
{
	for ( auto entry : non_recurring_requests_)
	{
		if(conflicting(request, entry))
			return false;
	}

	for ( auto entry : recurring_requests_)
	{
		if(conflicting(request, entry))
			return false;
	}
	return true;
}

/// @brief Systemd timer event callback use to send CAN messages at regular interval. Depending
/// on the diagnostic message frequency.
///
/// This should be called from systemd binder event loop and the event is created on add_recurring_request
///
/// @param[in] s - Systemd event source pointer used to reschedule the new iteration.
/// @param[in] usec - previous call timestamp in microseconds.
/// @param[in] userdata - the DiagnosticRequest struct, use to retrieve the active request from the list.
///
/// @return positive integer if sent and rescheduled or negative value if something wrong. If an error occurs
/// event will be disabled.
int diagnostic_manager_t::send_request(sd_event_source *s, uint64_t usec, void *userdata)
{
	diagnostic_manager_t& dm = configuration_t::instance().get_diagnostic_manager();
	DiagnosticRequest* request = (DiagnosticRequest*)userdata;
	active_diagnostic_request_t* adr = dm.find_recurring_request(request);

//	if(adr != nullptr && adr->get_can_bus_dev() == dm.get_can_bus_dev() && adr->should_send() &&
//		dm.clear_to_send(adr))
	if(adr != nullptr && adr->get_can_bus_dev() == dm.get_can_bus_dev())
	{
		adr->get_frequency_clock().tick();
		start_diagnostic_request(&dm.shims_, adr->get_handle());
		if(adr->get_handle()->completed && !adr->get_handle()->success)
		{
			DEBUG(binder_interface, "send_request: Fatal error sending diagnostic request");
			sd_event_source_unref(s);
			return -1;
		}
		adr->get_timeout_clock().tick();
		adr->set_in_flight(true);

		if(adr->get_recurring())
		{
			usec = usec + (uint64_t)(frequency_clock_t::frequency_to_period(adr->get_frequency_clock().get_frequency())*MICRO);
			DEBUG(binder_interface, "send_request: Event loop state: %d. usec: %ld", sd_event_get_state(afb_daemon_get_event_loop(binder_interface->daemon)), usec);
			if(sd_event_source_set_time(s, usec) >= 0)
				if(sd_event_source_set_enabled(s, SD_EVENT_ON) >= 0)
					return 0;
			sd_event_source_unref(s);
			return -1;
		}
	}
	sd_event_source_unref(s);
	ERROR(binder_interface, "send_request: Something goes wrong when submitting a new request to the CAN bus");
	return -2;
}

/// @brief Will decode the diagnostic response and build the final openxc_VehicleMessage to return.
///
/// @param[in] adr - A pointer to an active diagnostic request holding a valid diagnostic handle
/// @param[in] response - The response to decode from which the Vehicle message will be built and returned
///
/// @return A filled openxc_VehicleMessage or a zeroed struct if there is an error.
openxc_VehicleMessage diagnostic_manager_t::relay_diagnostic_response(active_diagnostic_request_t* adr, const DiagnosticResponse& response)
{
	openxc_VehicleMessage message = build_VehicleMessage();
	float value = (float)diagnostic_payload_to_integer(&response);
	if(adr->get_decoder() != nullptr)
	{
		value = adr->get_decoder()(&response, value);
	}

	if((response.success && strnlen(adr->get_name().c_str(), adr->get_name().size())) > 0)
	{
		// If name, include 'value' instead of payload, and leave of response
		// details.
		message = build_VehicleMessage(build_SimpleMessage(adr->get_name(), build_DynamicField(value)));
	}
	else
	{
		// If no name, send full details of response but still include 'value'
		// instead of 'payload' if they provided a decoder. The one case you
		// can't get is the full detailed response with 'value'. We could add
		// another parameter for that but it's onerous to carry that around.
		message = build_VehicleMessage(adr, response, value);
	}

	// If not success but completed then the pid isn't supported
	if(!response.success)
	{
		std::vector<diagnostic_message_t*> found_signals;
		configuration_t::instance().find_diagnostic_messages( build_DynamicField(adr->get_name()), found_signals );
		found_signals.front()->set_supported(false);
		cleanup_request(adr, true);
		NOTICE(binder_interface, "relay_diagnostic_response: PID not supported or ill formed. Please unsubscribe from it. Error code : %d", response.negative_response_code);
	}

	if(adr->get_callback() != nullptr)
	{
		adr->get_callback()(adr, &response, value);
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
			return relay_diagnostic_response(entry, response);
	}
	else if(!response.completed && response.multi_frame)
	{
		// Reset the timeout clock while completing the multi-frame receive
		entry->get_timeout_clock().tick();
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
