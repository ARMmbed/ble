/* mbed Microcontroller Library
 * Copyright (c) 2006-2015 ARM Limited
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef __CHARACTERISTIC_DESCRIPTOR_DISCOVERY_H__
#define __CHARACTERISTIC_DESCRIPTOR_DISCOVERY_H__

#include "FunctionPointerWithContext.h"

class DiscoveredCharacteristic;
class DiscoveredCharacteristicDescriptor;

class CharacteristicDescriptorDiscovery {
public:
    /*
     * Exposed application callback types.
     */
    struct DiscoveryCallbackParams_t {
        const DiscoveredCharacteristic& characteristic;
        const DiscoveredCharacteristicDescriptor& descriptor;
    };

    struct TerminationCallbackParams_t { 
        const DiscoveredCharacteristic& characteristic;
        ble_error_t status;
    };

    /**
     * Callback type for when a matching characteristic descriptor is found during 
     * characteristic descriptor discovery. The receiving function is passed in a 
     * pointer to a DiscoveryCallbackParams_t object which will remain 
     * valid for the lifetime of the callback. Memory for this object is owned by 
     * the BLE_API eventing framework. The application can safely make a persistent 
     * shallow-copy of this object in order to work with the service beyond the 
     * callback.
     */
    typedef FunctionPointerWithContext<const DiscoveryCallbackParams_t*> DiscoveryCallback_t;

    /**
     * Callback type for when characteristic descriptor discovery terminates.
     */
    typedef FunctionPointerWithContext<const TerminationCallbackParams_t*> TerminationCallback_t;
};

#endif // ifndef __CHARACTERISTIC_DESCRIPTOR_DISCOVERY_H__
