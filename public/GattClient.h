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

#include "GattCharacteristicCallbackParams.h"

class GattClient {
public:
    typedef void (*ReadCallback_t)(const GattCharacteristicReadCBParams *params);

    enum WriteOp_t {
        GATT_OP_INVALID        = 0x00,  /**< Invalid Operation. */
        GATT_OP_WRITE_REQ      = 0x01,  /**< Write Request. */
        GATT_OP_WRITE_CMD      = 0x02,  /**< Write Command. */
        GATT_OP_SIGN_WRITE_CMD = 0x03,  /**< Signed Write Command. */
        GATT_OP_PREP_WRITE_REQ = 0x04,  /**< Prepare Write Request. */
        GATT_OP_EXEC_WRITE_REQ = 0x05,  /**< Execute Write Request. */
    };

    typedef void (*WriteCallback_t)(const GattCharacteristicWriteCBParams *params);

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

    virtual ble_error_t write(GattClient::WriteOp_t cmd, Gap::Handle_t connHandle, size_t length, const uint8_t *value) const = 0;

#if 0
public:
    /* Event callback handlers. */
    typedef void (*EventCallback_t)(GattAttribute::Handle_t attributeHandle);
    typedef void (*ServerEventCallback_t)(void);                    /**< independent of any particular attribute */

    friend class BLEDevice;
private:
    /* These functions must be defined in the sub-class */
    virtual ble_error_t addService(GattService &)                                                               = 0;
    virtual ble_error_t readValue(GattAttribute::Handle_t attributeHandle, uint8_t buffer[], uint16_t *lengthP) = 0;
    virtual ble_error_t readValue(Gap::Handle_t connectionHandle, GattAttribute::Handle_t attributeHandle, uint8_t buffer[], uint16_t *lengthP) = 0;
    virtual ble_error_t updateValue(GattAttribute::Handle_t, const uint8_t[], uint16_t, bool localOnly = false) = 0;
    virtual ble_error_t updateValue(Gap::Handle_t connectionHandle, GattAttribute::Handle_t, const uint8_t[], uint16_t, bool localOnly = false) = 0;
    virtual ble_error_t initializeGATTDatabase(void)                                                            = 0;

    // ToDo: For updateValue, check the CCCD to see if the value we are
    // updating has the notify or indicate bits sent, and if BOTH are set
    // be sure to call sd_ble_gatts_hvx() twice with notify then indicate!
    // Strange use case, but valid and must be covered!

    void setOnDataSent(void (*callback)(unsigned count)) {onDataSent.add(callback);}
    template <typename T>
    void setOnDataSent(T *objPtr, void (T::*memberPtr)(unsigned count)) {
        onDataSent.add(objPtr, memberPtr);
    }
    void setOnDataWritten(void (*callback)(const GattCharacteristicWriteCBParams *eventDataP)) {onDataWritten.add(callback);}
    template <typename T>
    void setOnDataWritten(T *objPtr, void (T::*memberPtr)(const GattCharacteristicWriteCBParams *context)) {
        onDataWritten.add(objPtr, memberPtr);
    }

    /**
     * A virtual function to allow underlying stacks to indicate if they support
     * onDataRead(). It should be overridden to return true as applicable.
     */
    virtual bool isOnDataReadAvailable() const {
        return false;
    }
    ble_error_t setOnDataRead(void (*callback)(const GattCharacteristicReadCBParams *eventDataP)) {
        if (!isOnDataReadAvailable()) {
            return BLE_ERROR_NOT_IMPLEMENTED;
        }

        onDataRead.add(callback);
        return BLE_ERROR_NONE;
    }
    template <typename T>
    ble_error_t setOnDataRead(T *objPtr, void (T::*memberPtr)(const GattCharacteristicReadCBParams *context)) {
        if (!isOnDataReadAvailable()) {
            return BLE_ERROR_NOT_IMPLEMENTED;
        }

        onDataRead.add(objPtr, memberPtr);
        return BLE_ERROR_NONE;
    }
    void setOnUpdatesEnabled(EventCallback_t callback)       {onUpdatesEnabled       = callback;}
    void setOnUpdatesDisabled(EventCallback_t callback)      {onUpdatesDisabled      = callback;}
    void setOnConfirmationReceived(EventCallback_t callback) {onConfirmationReceived = callback;}

protected:
    void handleDataWrittenEvent(const GattCharacteristicWriteCBParams *params) {
        if (onDataWritten.hasCallbacksAttached()) {
            onDataWritten.call(params);
        }
    }

    void handleDataReadEvent(const GattCharacteristicReadCBParams *params) {
        if (onDataRead.hasCallbacksAttached()) {
            onDataRead.call(params);
        }
    }

    void handleEvent(GattServerEvents::gattEvent_e type, GattAttribute::Handle_t charHandle) {
        switch (type) {
            case GattServerEvents::GATT_EVENT_UPDATES_ENABLED:
                if (onUpdatesEnabled) {
                    onUpdatesEnabled(charHandle);
                }
                break;
            case GattServerEvents::GATT_EVENT_UPDATES_DISABLED:
                if (onUpdatesDisabled) {
                    onUpdatesDisabled(charHandle);
                }
                break;
            case GattServerEvents::GATT_EVENT_CONFIRMATION_RECEIVED:
                if (onConfirmationReceived) {
                    onConfirmationReceived(charHandle);
                }
                break;
            default:
                break;
        }
    }

    void handleDataSentEvent(unsigned count) {
        if (onDataSent.hasCallbacksAttached()) {
            onDataSent.call(count);
        }
    }

protected:
    uint8_t serviceCount;
    uint8_t characteristicCount;

private:
    CallChainOfFunctionPointersWithContext<unsigned>                                onDataSent;
    CallChainOfFunctionPointersWithContext<const GattCharacteristicWriteCBParams *> onDataWritten;
    CallChainOfFunctionPointersWithContext<const GattCharacteristicReadCBParams *>  onDataRead;
    EventCallback_t                                                                 onUpdatesEnabled;
    EventCallback_t                                                                 onUpdatesDisabled;
    EventCallback_t                                                                 onConfirmationReceived;
#endif

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
