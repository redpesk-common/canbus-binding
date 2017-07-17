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

#include <time.h>
#include <stdlib.h>
#include <cmath>

#include "timer.hpp"

long long int system_time_us()
{
	struct timespec t_usec;
	long long int timestamp_usec = 0;

	if(!::clock_gettime(CLOCK_MONOTONIC, &t_usec))
		timestamp_usec = (t_usec.tv_nsec / 1000ll) + (t_usec.tv_sec* 1000000ll);
	return timestamp_usec;
}

long long int system_time_ms()
{
	struct timespec t_msec;
	long long int timestamp_msec = 0;

	if(!::clock_gettime(CLOCK_MONOTONIC, &t_msec))
		timestamp_msec = (t_msec.tv_nsec / 1000000ll) + (t_msec.tv_sec* 1000ll);
	return timestamp_msec;
}

long long int system_time_s()
{
	struct timespec t_sec;
	long long int timestamp_sec = 0;

	if(!::clock_gettime(CLOCK_MONOTONIC, &t_sec))
		timestamp_sec = t_sec.tv_sec;
	return timestamp_sec;
}

frequency_clock_t::frequency_clock_t()
	: unit_{1000000}, frequency_{10.0}, last_tick_{0}, time_function_{nullptr}
{}

frequency_clock_t::frequency_clock_t(float frequency)
	: unit_{1000000}, frequency_{frequency}, last_tick_{0}, time_function_{nullptr}
{
	if(frequency_ <= 0)
		frequency_ = 2000;
}

frequency_clock_t::frequency_clock_t(float frequency, uint64_t last_tick, time_function_t time_function)
	: unit_{1000000}, frequency_{frequency}, last_tick_{0}, time_function_{nullptr}
{
	if(frequency_ <= 0)
		frequency_ = 1;
}

/// @brief Return the period in ms given the frequency in hertz.
/// @param[in] frequency - Frequency to convert, in hertz
float frequency_clock_t::frequency_to_period() const
{
	return frequency_ == 0 ? 0 : 1 / frequency_;
}

/// @brief Return a timeval struct based on the frequency_ member. used to
/// specified CAN BCM timers.
const struct timeval frequency_clock_t::get_timeval_from_period() const
{
	struct timeval freq = {0, 0};
	float f;
	freq.tv_usec = (long int)(std::modf(frequency_to_period(), &f) * unit_);
	freq.tv_sec = (time_t)f;

	return freq;
}

bool frequency_clock_t::started()
{
	return last_tick_ != 0;
}

time_function_t frequency_clock_t::get_time_function()
{
	return time_function_ != nullptr ? time_function_ : system_time_us;
}

bool frequency_clock_t::elapsed(bool stagger)
{
	float period = frequency_to_period();
	float elapsed_time = 0;
	if(!started() && stagger)
		last_tick_ = get_time_function()() - (rand() % int(period));

	// Make sure it ticks the the first call
	elapsed_time = !started() ? period : (float)get_time_function()() - (float)last_tick_;

	return frequency_ == 0 || elapsed_time >= period;
}

float frequency_clock_t::get_frequency() const
{
	return frequency_;
}

uint64_t frequency_clock_t::get_last_tick() const
{
	return last_tick_;
}

/// @brief Force the clock to tick, regardless of it its time has actually
/// elapsed.
void frequency_clock_t::tick(uint64_t timestamp)
{
	last_tick_ = timestamp;
}
