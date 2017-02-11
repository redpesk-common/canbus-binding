/*
 * Copyright (C) 2015, 2016 "IoT.bzh"
 * Author "Romain Forlot" <romain.forlot@iot.bzh>
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/*
 *  A representation of an OBD-II PID.
 *
 * pid - The 1 byte PID.
 * name - A human readable name to use for this PID when published.
 * frequency - The frequency to request this PID if supported by the vehicle
 *      when automatic, recurring OBD-II requests are enabled.
 */

/*
 * Will scan for supported Obd2 pids
 */
Obd2Handler::Obd2Handler() {

}

Obd2Handler::isObd2Request(DiagnosticRequest* request) {
    return request->mode == 0x1 && request->has_pid && request->pid < 0xff;
}

Obd2Handler::decodeObd2Response(obd2response responce)