/*
 * Copyright (C) 2015, 2016, 2017 "IoT.bzh"
 * Author "Romain Forlot" <romain.forlot@iot.bzh>
 * Author "Loïc Collignon" <loic.collignon@iot.bzh>
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

#include <cstdint>
#include <string>
#include <vector>
#include <memory>

class can_signal_t;
class can_message_definition_t;
class diagnostic_message_t;

/// @brief A parent wrapper for a particular set of CAN messages and diagnostic messages
/// (e.g. a vehicle or program).
class can_message_set_t
{
private:
	uint8_t index_; /// < A numerical ID for the message set, ideally the index is in an array for fast lookup
	const std::string name_; /// < The name of the message set.
	std::vector<std::shared_ptr<can_message_definition_t> > can_messages_definition_; ///< Vector holding all message definitions handled by the message set.
	std::vector<std::shared_ptr<diagnostic_message_t> > diagnostic_messages_; ///< Vector holding all diagnostics messages from JSON signals description file. First vector map to message set

public:
	can_message_set_t(
			uint8_t index,
			const std::string& name,
			const std::vector<std::shared_ptr<can_message_definition_t> >& can_messages_definition,
			const std::vector<std::shared_ptr<diagnostic_message_t> >& diagnostic_messages);

	std::vector<std::shared_ptr<can_message_definition_t> >& get_can_message_definition();
	std::vector<std::shared_ptr<can_signal_t> > get_all_can_signals() const;
	std::vector<std::shared_ptr<diagnostic_message_t> >& get_diagnostic_messages();
};
