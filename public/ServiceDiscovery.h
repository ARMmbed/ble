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

#ifndef __SERVICE_DISOVERY_H__
#define __SERVICE_DISOVERY_H__

#include "UUID.h"
#include "Gap.h"
#include "GattAttribute.h"
#include "DiscoveredService.h"
#include "DiscoveredCharacteristic.h"

class ServiceDiscovery {
public:
    typedef void (*ServiceCallback_t)(const DiscoveredService &);
    typedef void (*CharacteristicCallback_t)(const DiscoveredCharacteristic &);
    typedef void (*TerminationCallback_t)(Gap::Handle_t connectionHandle);

public:
    static ble_error_t launch(Gap::Handle_t             connectionHandle,
                              ServiceCallback_t         sc = NULL,
                              CharacteristicCallback_t  cc = NULL,
                              const UUID               &matchingServiceUUID = UUID::ShortUUIDBytes_t(BLE_UUID_UNKNOWN),
                              const UUID               &matchingCharacteristicUUIDIn = UUID::ShortUUIDBytes_t(BLE_UUID_UNKNOWN));

    /**
     * Terminate an ongoing service-discovery. This should result in an
     * invocation of the TerminationCallback if service-discovery is active.
     */
    static void        terminate(void);

    /**
     * Is service-discovery currently active?
     */
    static bool        isActive(void);

    /**
     * Setup callback to be invoked when service discovery is terminated.
     */
    static void        onTermination(TerminationCallback_t callback);

protected:
    Gap::Handle_t            connHandle; /**< Connection handle as provided by the SoftDevice. */
    UUID                     matchingServiceUUID;
    ServiceCallback_t        serviceCallback;
    UUID                     matchingCharacteristicUUID;
    CharacteristicCallback_t characteristicCallback;
};

#endif // ifndef __SERVICE_DISOVERY_H__
