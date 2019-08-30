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

#include <cstddef>
#include <map>
#include <string>
#include <memory>
#include <systemd/sd-event.h>
#include <cctype>

#include <afb/afb-binding>

class low_can_subscription_t;

typedef std::map<int, std::shared_ptr<low_can_subscription_t>> map_subscription;

void on_no_clients(std::shared_ptr<low_can_subscription_t> can_subscription, map_subscription& s);
void on_no_clients(std::shared_ptr<low_can_subscription_t> can_subscription, uint32_t pid, map_subscription& s);
int read_message(sd_event_source *s, int fd, uint32_t revents, void *userdata);

inline bool caseInsCharCompareN(char a, char b) {
	return(toupper(a) == toupper(b));
}

inline bool caseInsCompare(const std::string& s1, const std::string& s2) {
	return((s1.size() == s2.size()) &&
	equal(s1.begin(), s1.end(), s2.begin(), caseInsCharCompareN));
}
