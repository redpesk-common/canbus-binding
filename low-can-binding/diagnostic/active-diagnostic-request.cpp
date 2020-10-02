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


#include <fnmatch.h>
#include <map>

#include <low-can/diagnostic/active-diagnostic-request.hpp>
#include <low-can/binding/application.hpp>

#define AFB_ERROR_PID 0xFF

std::string active_diagnostic_request_t::prefix_ = "diagnostic_messages";

active_diagnostic_request_t::active_diagnostic_request_t(const std::string& bus, uint32_t id,
		const std::string& name,
		bool wait_for_multiple_responses,
		const DiagnosticResponseDecoder decoder,
		const DiagnosticResponseCallback callback,
		float frequencyHz,
		bool permanent)
	: bus_{bus},
	  id_{id},
	  handle_{nullptr},
	  name_{name},
	  decoder_{decoder},
	  callback_{callback},
	  recurring_{frequencyHz ? true : false},
	  permanent_{permanent},
	  wait_for_multiple_responses_{wait_for_multiple_responses},
	  frequency_clock_{frequency_clock_t(frequencyHz)},
	  timeout_clock_{frequency_clock_t(10)},
	  socket_{}
{}

active_diagnostic_request_t::~active_diagnostic_request_t()
{
	socket_.close();
	delete handle_;
	handle_ = nullptr;
}

uint32_t active_diagnostic_request_t::get_id() const
{
	return id_;
}

uint16_t active_diagnostic_request_t::get_pid() const
{
	if (handle_->request.has_pid)
		return handle_->request.pid;
	return AFB_ERROR_PID;
}

DiagnosticRequestHandle* active_diagnostic_request_t::get_handle()
{
	return handle_;
}

const std::string active_diagnostic_request_t::get_name() const
{
	return name_;
}

std::string& active_diagnostic_request_t::get_prefix()
{
	return active_diagnostic_request_t::prefix_;
}

DiagnosticResponseDecoder& active_diagnostic_request_t::get_decoder()
{
	return decoder_;
}

DiagnosticResponseCallback& active_diagnostic_request_t::get_callback()
{
	return callback_;
}

bool active_diagnostic_request_t::get_recurring() const
{
	return recurring_;
}

bool active_diagnostic_request_t::get_permanent() const
{
	return permanent_;
}

frequency_clock_t& active_diagnostic_request_t::get_frequency_clock()
{
	return frequency_clock_;
}

frequency_clock_t& active_diagnostic_request_t::get_timeout_clock()
{
	return timeout_clock_;
}

utils::socketcan_bcm_t& active_diagnostic_request_t::get_socket()
{
	return socket_;
}

void active_diagnostic_request_t::set_handle(DiagnosticShims& shims, DiagnosticRequest* request)
{
	handle_ = new DiagnosticRequestHandle(generate_diagnostic_request(&shims, request, nullptr));
}

/// @brief Returns true if a sufficient response has been received for a
/// diagnostic request.
///
/// This is true when at least one response has been received and the request is
/// configured to not wait for multiple responses. Functional broadcast requests
/// may often wish to wait the full 100ms for modules to respond.
bool active_diagnostic_request_t::response_received() const
{
	return !wait_for_multiple_responses_ &&
				handle_->completed && handle_->success;
}
