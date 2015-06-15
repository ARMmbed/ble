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

#ifndef __GATT_CLIENT_H__
#define __GATT_CLIENT_H__

#include "Gap.h"
#include "GattAttribute.h"
#include "ServiceDiscovery.h"

#include "GattCallbackParamTypes.h"

class GattClient {
public:
    typedef void (*ReadCallback_t)(const GattReadCallbackParams *params);

    enum WriteOp_t {
        GATT_OP_WRITE_REQ      = 0x01,  /**< Write Request. */
        GATT_OP_WRITE_CMD      = 0x02,  /**< Write Command. */
    };

    typedef void (*WriteCallback_t)(const GattWriteCallbackParams *params);

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
     *           This is the application callback for matching service.
     * @param  cc
     *           This is the application callback for matching characteristic.
     * @param  matchingServiceUUID
     *           UUID based filter for specifying a service in which the application is
     *           interested.
     * @param  matchingCharacteristicUUIDIn
     *           UUID based filter for specifying characteristic in which the application
     *           is interested.
     *
     * @Note     Using wildcard values for both service-UUID and characteristic-
     *           UUID will result in complete service discovery--callbacks being
     *           called for every service and characteristic.
     *
     * @return
     *           BLE_ERROR_NONE if service discovery is launched successfully; else an appropriate error.
     */
    virtual ble_error_t launchServiceDiscovery(Gap::Handle_t                               connectionHandle,
                                               ServiceDiscovery::ServiceCallback_t         sc                           = NULL,
                                               ServiceDiscovery::CharacteristicCallback_t  cc                           = NULL,
                                               const UUID                                 &matchingServiceUUID          = UUID::ShortUUIDBytes_t(BLE_UUID_UNKNOWN),
                                               const UUID                                 &matchingCharacteristicUUIDIn = UUID::ShortUUIDBytes_t(BLE_UUID_UNKNOWN)) = 0;

    virtual void        onServiceDiscoveryTermination(ServiceDiscovery::TerminationCallback_t callback) = 0;

    /**
     * Is service-discovery currently active?
     */
    virtual bool isServiceDiscoveryActive(void) const = 0;

    /**
     * Terminate an ongoing service-discovery. This should result in an
     * invocation of the TerminationCallback if service-discovery is active.
     */
    virtual void terminateServiceDiscovery(void) = 0;

    /* Initiate a Gatt Client read procedure by attribute-handle.*/
    virtual ble_error_t read(Gap::Handle_t connHandle, GattAttribute::Handle_t attributeHandle, uint16_t offset) const = 0;

    virtual ble_error_t write(GattClient::WriteOp_t cmd, Gap::Handle_t connHandle, GattAttribute::Handle_t attributeHandle, size_t length, const uint8_t *value) const = 0;

protected:
    GattClient() {
        /* empty */
    }

private:
    /* disallow copy and assignment */
    GattClient(const GattClient &);
    GattClient& operator=(const GattClient &);
};

#endif // ifndef __GATT_CLIENT_H__
