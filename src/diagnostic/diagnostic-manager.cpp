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

#include <algorithm>

#include "diagnostic-manager.hpp"

#include "uds/uds.h"
#include "../configuration.hpp"

#define MAX_RECURRING_DIAGNOSTIC_FREQUENCY_HZ 10
#define MAX_SIMULTANEOUS_DIAG_REQUESTS 50
#define MAX_REQUEST_ENTRIES 50

diagnostic_manager_t::diagnostic_manager_t()
	: request_list_entries_(MAX_REQUEST_ENTRIES), initialized_{false}
{}

bool diagnostic_manager_t::initialize(std::shared_ptr<can_bus_dev_t> cbd)
{
	// Mandatory to set the bus before intiliaze shims.
	bus_ = cbd;

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

	for(int i = 0; i < MAX_SIMULTANEOUS_DIAG_REQUESTS; i++)
		free_request_entries_.push_back(request_list_entries_[i]);
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
	free_request_entries_.push_back(entry);
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
			else
			{
				DEBUG(binder_interface, "Moving completed recurring request to the back of the queue: %s", request_string);
				recurring_requests_.push_back(entry);
			}
		}
		else
		{
			DEBUG(binder_interface, "Cancelling completed, non-recurring request: %s", request_string);
			find_and_erase(entry, non_recurring_requests_);
			cancel_request(entry);
		}
	}
}

/// @brief Clean up the request list, move as many to the free list as possible
void diagnostic_manager_t::cleanup_active_requests(bool force)
{
	for(auto& entry : non_recurring_requests_)
		cleanup_request(entry, force);

	for(auto& entry : recurring_requests_)
		cleanup_request(entry, force);
}

/// @brief Note that this pops it off of whichver list it was on and returns it, so make
/// sure to add it to some other list or it'll be lost.
bool diagnostic_manager_t::lookup_recurring_request(const DiagnosticRequest* request)
{
	active_diagnostic_request_t existingEntry;
	for (auto& entry : recurring_requests_)
	{
		active_diagnostic_request_t* candidate = entry;
		if(candidate->get_can_bus_dev()->get_device_name() == bus_->get_device_name() &&
			diagnostic_request_equals(&candidate->get_handle()->request, request))
		{
			find_and_erase(entry, recurring_requests_);
			return true;
			break;
		}
	}
	return false;
}

std::shared_ptr<can_bus_dev_t> diagnostic_manager_t::get_can_bus_dev()
{
	return bus_;
}

active_diagnostic_request_t* diagnostic_manager_t::get_free_entry()
{
	if (request_list_entries_.empty())
		return nullptr;

	active_diagnostic_request_t* adr = request_list_entries_.back();
	request_list_entries_.pop_back();
	return adr;
}

bool diagnostic_manager_t::add_request(DiagnosticRequest* request, const std::string name,
	bool wait_for_multiple_responses, const DiagnosticResponseDecoder decoder,
	const DiagnosticResponseCallback callback)
{
	cleanup_active_requests(false);

	bool added = true;
	active_diagnostic_request_t* entry = get_free_entry();

	if (entry != nullptr)
	{
		// TODO: implement Acceptance Filter
		//	if(updateRequiredAcceptanceFilters(bus, request)) {
			entry = new active_diagnostic_request_t(bus_, request, name,
					wait_for_multiple_responses, decoder, callback, 0);
			entry->set_handle(shims_, request);

			char request_string[128] = {0};
			diagnostic_request_to_string(&entry->get_handle()->request, request_string,
					sizeof(request_string));

			find_and_erase(entry, non_recurring_requests_);
			DEBUG(binder_interface, "Added one-time diagnostic request on bus %s: %s",
					bus_->get_device_name(), request_string);

			non_recurring_requests_.push_back(entry);
	}
	else
	{
		WARNING(binder_interface, "There isn't enough request entry. Vector exhausted %d/%d", (int)request_list_entries_.size(), (int)request_list_entries_.max_size());
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
	if(lookup_recurring_request(request))
	{
		active_diagnostic_request_t* entry = get_free_entry();

		if(entry != nullptr)
		{
			// TODO: implement Acceptance Filter
			//if(updateRequiredAcceptanceFilters(bus, request)) {
				entry = new active_diagnostic_request_t(bus_, request, name,
						wait_for_multiple_responses, decoder, callback, frequencyHz);
				entry->set_handle(shims_, request);

				char request_string[128] = {0};
				diagnostic_request_to_string(&entry->get_handle()->request, request_string,
						sizeof(request_string));

				find_and_erase(entry, recurring_requests_);
				DEBUG(binder_interface, "Added recurring diagnostic request (freq: %f) on bus %d: %s",
						frequencyHz, bus_->get_device_name(), request_string);

				recurring_requests_.push_back(entry);
		}
		else
		{
			WARNING(binder_interface, "There isn't enough request entry. Vector exhausted %d/%d", (int)request_list_entries_.size(), (int)request_list_entries_.max_size());
			added = false;
		}
	}
	else
	{
		DEBUG(binder_interface, "Can't add request, one already exists with same key");
		added = false;
	}
	return added;
}

bool diagnostic_manager_t::shims_send(const uint32_t arbitration_id, const uint8_t* data, const uint8_t size)
{
	std::shared_ptr<can_bus_dev_t> can_bus_dev = configuration_t::instance().get_diagnostic_manager().get_can_bus_dev();
	return can_bus_dev->shims_send(arbitration_id, data, size);
}

void diagnostic_manager_t::shims_logger(const char* m, ...)
{
	DEBUG(binder_interface, "%s", m);
}

void diagnostic_manager_t::shims_timer()
{}

