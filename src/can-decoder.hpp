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

#pragma once

#include "can-utils.hpp"

class decoder_t
{
	private:

	public:
		/* Public: Parse the signal's bitfield from the given data and return the raw
		* value.
		*
		* signal - The signal to parse from the data.
		* data - The data to parse the signal from.
		* length - The length of the data array.
		*
		* Returns the raw value of the signal parsed as a bitfield from the given byte
		* array.
		*/
		float parseSignalBitfield(CanSignal& signal, const can_message_t& message);

		/* Public: Find and return the corresponding string state for a CAN signal's
		* raw integer value.
		*
		* This is an implementation of the SignalDecoder type signature, and can be
		* used directly in the CanSignal.decoder field.
		*
		* signal  - The details of the signal that contains the state mapping.
		* signals - The list of all signals.
		* signalCount - the length of the signals array.
		* value - The numerical value that should map to a state.
		* send - An output argument that will be set to false if the value should
		*     not be sent for any reason.
		*
		* Returns a DynamicField with a string value if a matching state is found in
		* the signal. If an equivalent isn't found, send is sent to false and the
		* return value is undefined.
		*/
		static openxc_DynamicField stateDecoder(CanSignal& signal, const std::vector<CanSignal>& signals,
				float value, bool* send);

		/* Public: Coerces a numerical value to a boolean.
		*
		* This is an implementation of the SignalDecoder type signature, and can be
		* used directly in the CanSignal.decoder field.
		*
		* signal  - The details of the signal that contains the state mapping.
		* signals - The list of all signals
		* signalCount - The length of the signals array
		* value - The numerical value that will be converted to a boolean.
		* send - An output argument that will be set to false if the value should
		*     not be sent for any reason.
		*
		* Returns a DynamicField with a boolean value of false if the raw signal value
		* is 0.0, otherwise true. The 'send' argument will not be modified as this
		* decoder always succeeds.
		*/
		static openxc_DynamicField booleanDecoder(CanSignal& signal, const std::vector<CanSignal>& signals,
				float value, bool* send);

		/* Public: Update the metadata for a signal and the newly received value.
		*
		* This is an implementation of the SignalDecoder type signature, and can be
		* used directly in the CanSignal.decoder field.
		*
		* This function always flips 'send' to false.
		*
		* signal  - The details of the signal that contains the state mapping.
		* signals - The list of all signals.
		* signalCount - The length of the signals array.
		* value - The numerical value that will be converted to a boolean.
		* send - This output argument will always be set to false, so the caller will
		*      know not to publish this value to the pipeline.
		*
		* The return value is undefined.
		*/
		 openxc_DynamicField ignoreDecoder(CanSignal& signal, const std::vector<CanSignal>& signals,
				float value, bool* send);

		/* Public: Wrap a raw CAN signal value in a DynamicField without modification.
		*
		* This is an implementation of the SignalDecoder type signature, and can be
		* used directly in the CanSignal.decoder field.
		*
		* signal  - The details of the signal that contains the state mapping.
		* signals - The list of all signals
		* signalCount - The length of the signals array
		* value - The numerical value that will be wrapped in a DynamicField.
		* send - An output argument that will be set to false if the value should
		*     not be sent for any reason.
		*
		* Returns a DynamicField with the original, unmodified raw CAN signal value as
		* its numeric value. The 'send' argument will not be modified as this decoder
		* always succeeds.
		*/
		static openxc_DynamicField noopDecoder(CanSignal& signal, const std::vector<CanSignal>& signals,
				float value, bool* send);


		/* Public: Parse a signal from a CAN message, apply any required transforations
		 *      to get a human readable value and public the result to the pipeline.
		 *
		* If the CanSignal has a non-NULL 'decoder' field, the raw CAN signal value
		* will be passed to the decoder before publishing.
		*
		* signal - The details of the signal to decode and forward.
		* message   - The received CAN message that should contain this signal.
		* signals - an array of all active signals.
		*
		* The decoder returns an openxc_DynamicField, which may contain a number,
		* string or boolean.
		*/
		openxc_DynamicField translateSignal(CanSignal& signal, can_message_t& message,
			const std::vector<CanSignal>& signals);
		
		/* Public: Parse a signal from a CAN message and apply any required
		* transforations to get a human readable value.
		*
		* If the CanSignal has a non-NULL 'decoder' field, the raw CAN signal value
		* will be passed to the decoder before returning.
		*
		* signal - The details of the signal to decode and forward.
		* message   - The CAN message that contains the signal.
		* signals - an array of all active signals.
		* send - An output parameter that will be flipped to false if the value could
		*      not be decoded.
		*
		* The decoder returns an openxc_DynamicField, which may contain a number,
		* string or boolean. If 'send' is false, the return value is undefined.
		*/
		openxc_DynamicField decodeSignal(CanSignal& signal, const can_message_t& message,
				const std::vector<CanSignal>& signals, bool* send);

		/* Public: Decode a transformed, human readable value from an raw CAN signal
		* already parsed from a CAN message.
		*
		* This is the same as decodeSignal(const CanSignal&, CanMessage*, const CanSignal&, int,
		* bool*) but you must parse the bitfield value of the signal from the CAN
		* message yourself. This is useful if you need that raw value for something
		* else.
		*/
		openxc_DynamicField decodeSignal(CanSignal& signal, float value,
			const std::vector<CanSignal>& signals, bool* send);
};