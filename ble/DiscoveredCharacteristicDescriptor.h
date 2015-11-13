/* mbed Microcontroller Library
 * Copyright (c) 2006-2013 ARM Limited
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

#ifndef __DISCOVERED_CHARACTERISTIC_DESCRIPTOR_H__
#define __DISCOVERED_CHARACTERISTIC_DESCRIPTOR_H__

#include "UUID.h"
#include "Gap.h"
#include "GattAttribute.h"
#include "GattClient.h"
#include "CharacteristicDescriptorDiscovery.h"


/**
 * 
 */
class DiscoveredCharacteristicDescriptor {

public:
    DiscoveredCharacteristicDescriptor(
        GattClient* client, Gap::Handle_t connectionHandle,  GattAttribute::Handle_t gattHandle, const UUID& uuid) : 
        _client(client), _connectionHandle(connectionHandle), _uuid(uuid), _gattHandle(gattHandle) {

    }

    GattClient* client() { 
        return _client;
    }

    const GattClient* client() const { 
        return _client;
    }

    Gap::Handle_t connectionHandle() const {
        return _connectionHandle;
    }

    const UUID& uuid(void) const {
        return _uuid;
    }

    GattAttribute::Handle_t gattHandle() const {
        return _gattHandle;
    }

private:
    GattClient  *_client;
    Gap::Handle_t _connectionHandle;
    UUID _uuid;
    GattAttribute::Handle_t _gattHandle;
};

#endif /*__DISCOVERED_CHARACTERISTIC_DESCRIPTOR_H__*/
