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

#include "uds/uds.h"
#include "../utils/openxc-utils.hpp"
#include "../configuration.hpp"

#define MAX_RECURRING_DIAGNOSTIC_FREQUENCY_HZ 10
#define MAX_SIMULTANEOUS_DIAG_REQUESTS 50
#define TIMERFD_ACCURACY 0
#define MICRO 1000000

diagnostic_manager_t::diagnostic_manager_t()
	: initialized_{false}
{}

bool diagnostic_manager_t::initialize()
{
	// Mandatory to set the bus before intiliaze shims.
	bus_ = configuration_t::instance().get_diagnostic_bus();

	init_diagnostic_shims();
	reset();

	initialized_ = true;
	DEBUG(binder_interface, "initialize: Diagnostic Manager initialized");
	return initialized_;
}

/**
 * @brief initialize shims used by UDS lib and set initialized_ to true.
 *  It is needed before used the diagnostic manager fully because shims are
 *  required by most member functions.
 */
void diagnostic_manager_t::init_diagnostic_shims()
{
	shims_ = diagnostic_init_shims(shims_logger, shims_send, NULL);
	DEBUG(binder_interface, "init_diagnostic_shims: Shims initialized");
}

void diagnostic_manager_t::reset()
{
	if(initialized_)
	{
		DEBUG(binder_interface, "Clearing existing diagnostic requests");
		cleanup_active_requests(true);
	}
}


void diagnostic_manager_t::find_and_erase(active_diagnostic_request_t* entry, std::vector<active_diagnostic_request_t*>& requests_list)
{
	auto i = std::find(requests_list.begin(), requests_list.end(), entry);
	if ( i != requests_list.end())
		requests_list.erase(i);
}

/// Move the entry to the free list and decrement the lock count for any
/// CAN filters it used.
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
		}
		else
		{
			DEBUG(binder_interface, "cleanup_request: Cancelling completed, non-recurring request: %s", request_string);
			find_and_erase(entry, non_recurring_requests_);
			cancel_request(entry);
		}
	}
}

/// @brief Clean up the request list
void diagnostic_manager_t::cleanup_active_requests(bool force)
{
	for(auto& entry : non_recurring_requests_)
		if (entry != nullptr)
			cleanup_request(entry, force);

	for(auto& entry : recurring_requests_)
		if (entry != nullptr)
			cleanup_request(entry, force);
}

/// @brief Will return the active_diagnostic_request_t pointer of found request or nullptr if
/// not found.
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

std::shared_ptr<can_bus_dev_t> diagnostic_manager_t::get_can_bus_dev()
{
	return can_bus_t::get_can_device(bus_);
}

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
			return 0;
		}
		adr->get_timeout_clock().tick();
		adr->set_in_flight(true);

		if(adr->get_recurring())
		{
			usec = usec + (uint64_t)(frequency_clock_t::frequency_to_period(adr->get_frequency_clock().get_frequency())*MICRO);
			DEBUG(binder_interface, "send_request: Event loop state: %d. usec: %ld", sd_event_get_state(afb_daemon_get_event_loop(binder_interface->daemon)), usec);
			if(sd_event_source_set_time(s, usec+1000000) >= 0)
				if(sd_event_source_set_enabled(s, SD_EVENT_ON) >= 0)
					return 1;
		}
	}
	sd_event_source_unref(s);
	ERROR(binder_interface, "send_request: Something goes wrong when submitting a new request to the CAN bus");
	return -1;
}

openxc_VehicleMessage diagnostic_manager_t::relay_diagnostic_response(active_diagnostic_request_t* adr, const DiagnosticResponse& response) const
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

DiagnosticShims& diagnostic_manager_t::get_shims()
{
	return shims_;
}

bool diagnostic_manager_t::shims_send(const uint32_t arbitration_id, const uint8_t* data, const uint8_t size)
{
	std::shared_ptr<can_bus_dev_t> can_bus_dev = can_bus_t::get_can_device(configuration_t::instance().get_diagnostic_manager().bus_);
	return can_bus_dev->shims_send(arbitration_id, data, size);
}

void diagnostic_manager_t::shims_logger(const char* format, ...)
{
	va_list args;
	va_start(args, format);

	char buffer[256];
	vsnprintf(buffer, 256, format, args);

	DEBUG(binder_interface, "shims_logger: %s", buffer);
}

void diagnostic_manager_t::shims_timer()
{}
