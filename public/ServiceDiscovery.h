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
    /*
     * Exposed application callback types.
     */
    typedef void (*ServiceCallback_t)(const DiscoveredService *);
    typedef void (*CharacteristicCallback_t)(const DiscoveredCharacteristic *);
    typedef void (*TerminationCallback_t)(Gap::Handle_t connectionHandle);

public:
    /**
     * Launch service discovery. Once launched, service discovery will remain
     * active with callbacks being issued back into the application for matching
     * services/characteristics. isActive() can be used to determine status; and
     * a termination callback (if setup) will be invoked at the end. Service
     * discovery can be terminated prematurely if needed using terminate().
     *
     * @param  connectionHandle
     *           Handle for the connection with the peer.
     * @param  sc
     *           This is the application callback for matching service. Taken as
     *           NULL by default. Note: service discovery may still be active
     *           when this callback is issued; calling asynchronous BLE-stack
     *           APIs from within this application callback might cause the
     *           stack to abort service discovery. If this becomes an issue, it
     *           may be better to make local copy of the discoveredService and
     *           wait for service discovery to terminate before operating on the
     *           service.
     * @param  cc
     *           This is the application callback for matching characteristic.
     *           Taken as NULL by default. Note: service discovery may still be
     *           active when this callback is issued; calling asynchronous
     *           BLE-stack APIs from within this application callback might cause
     *           the stack to abort service discovery. If this becomes an issue,
     *           it may be better to make local copy of the discoveredCharacteristic
     *           and wait for service discovery to terminate before operating on the
     *           characteristic.
     * @param  matchingServiceUUID
     *           UUID based filter for specifying a service in which the application is
     *           interested. By default it is set as the wildcard UUID_UNKNOWN,
     *           in which case it matches all services. If characteristic-UUID
     *           filter (below) is set to the wildcard value, then a service
     *           callback will be invoked for the matching service (or for every
     *           service if the service filter is a wildcard).
     * @param  matchingCharacteristicUUIDIn
     *           UUID based filter for specifying characteristic in which the application
     *           is interested. By default it is set as the wildcard UUID_UKNOWN
     *           to match against any characteristic. If both service-UUID
     *           filter and characteristic-UUID filter are used with non- wildcard
     *           values, then only a single characteristic callback is
     *           invoked for the matching characteristic.
     *
     * @Note     Using wildcard values for both service-UUID and characteristic-
     *           UUID will result in complete service discovery--callbacks being
     *           called for every service and characteristic.
     *
     * @return
     *           BLE_ERROR_NONE if service discovery is launched successfully; else an appropriate error.
     */
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
