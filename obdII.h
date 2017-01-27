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
typedef struct _Obd2Pid {
    uint8_t pid;
    const char* name;
    float frequency;
} Obd2Pid;

/*
 * Pre-defined OBD-II PIDs to query for if supported by the vehicle.
 */
const Obd2Pid OBD2_PIDS[] = {
    { pid: 0xc, name: "engine_speed", frequency: 5 },
    { pid: 0xd, name: "vehicle_speed", frequency: 5 },
    { pid: 0x4, name: "engine_load", frequency: 5 },
    { pid: 0x5, name: "engine_coolant_temperature", frequency: 1 },
    { pid: 0x33, name: "barometric_pressure", frequency: 1 },
    { pid: 0x4c, name: "commanded_throttle_position", frequency: 1 },
    { pid: 0x27, name: "fuel_level", frequency: 1 },
    { pid: 0xf, name: "intake_air_temperature", frequency: 1 },
    { pid: 0xb, name: "intake_manifold_pressure", frequency: 1 },
    { pid: 0x1f, name: "running_time", frequency: 1 },
    { pid: 0x11, name: "throttle_position", frequency: 5 },
    { pid: 0xa, name: "fuel_pressure", frequency: 1 },
    { pid: 0x10, name: "mass_airflow", frequency: 5 },
    { pid: 0x5a, name: "accelerator_pedal_position", frequency: 5 },
    { pid: 0x52, name: "ethanol_fuel_percentage", frequency: 1 },
    { pid: 0x5c, name: "engine_oil_temperature", frequency: 1 },
    { pid: 0x63, name: "engine_torque", frequency: 1 },
};
