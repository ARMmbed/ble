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

#ifndef __GATT_SERVER_H__
#define __GATT_SERVER_H__

#include "Gap.h"
#include "GattService.h"
#include "GattAttribute.h"
#include "GattServerEvents.h"
#include "GattCallbackParamTypes.h"
#include "CallChainOfFunctionPointersWithContext.h"

class GattServer {
public:
    /* Event callback handlers. */
    typedef void (*EventCallback_t)(GattAttribute::Handle_t attributeHandle);
    typedef void (*ServerEventCallback_t)(void);                    /**< independent of any particular attribute */

protected:
    GattServer() :
        serviceCount(0),
        characteristicCount(0),
        dataSentCallChain(),
        dataWrittenCallChain(),
        onDataRead(),
        updatesEnabledCallback(NULL),
        onUpdatesDisabled(NULL),
        onConfirmationReceived(NULL) {
        /* empty */
    }

public:
    /* These functions must be defined in the sub-class */

    /**
     * Add a service declaration to the local server ATT table. Also add the
     * characteristics contained within.
     */
    virtual ble_error_t addService(GattService &) = 0;

    /**
     * Read the value of a characteristic from the local GattServer
     * @param[in]     attributeHandle
     *                  Attribute handle for the value attribute of the characteristic.
     * @param[out]    buffer
     *                  A buffer to hold the value being read.
     * @param[in/out] lengthP
     *                  Length of the buffer being supplied. If the attribute
     *                  value is longer than the size of the supplied buffer,
     *                  this variable will hold upon return the total attribute value length
     *                  (excluding offset). The application may use this
     *                  information to allocate a suitable buffer size.
     *
     * @return BLE_ERROR_NONE if a value was read successfully into the buffer.
     */
    virtual ble_error_t read(GattAttribute::Handle_t attributeHandle, uint8_t buffer[], uint16_t *lengthP) = 0;

    /**
     * Read the value of a characteristic from the local GattServer
     * @param[in]     connectionHandle
     *                  Connection Handle.
     * @param[in]     attributeHandle
     *                  Attribute handle for the value attribute of the characteristic.
     * @param[out]    buffer
     *                  A buffer to hold the value being read.
     * @param[in/out] lengthP
     *                  Length of the buffer being supplied. If the attribute
     *                  value is longer than the size of the supplied buffer,
     *                  this variable will hold upon return the total attribute value length
     *                  (excluding offset). The application may use this
     *                  information to allocate a suitable buffer size.
     *
     * @return BLE_ERROR_NONE if a value was read successfully into the buffer.
     *
     * @note This API is a version of above with an additional connection handle
     *     parameter to allow fetches for connection-specific multivalued
     *     attribtues (such as the CCCDs).
     */
    virtual ble_error_t read(Gap::Handle_t connectionHandle, GattAttribute::Handle_t attributeHandle, uint8_t *buffer, uint16_t *lengthP) = 0;

    /**
     * Update the value of a characteristic on the local GattServer.
     *
     * @param[in] attributeHandle
     *              Handle for the value attribute of the Characteristic.
     * @param[in] value
     *              A pointer to a buffer holding the new value
     * @param[in] size
     *              Size of the new value (in bytes).
     * @param[in] localOnly
     *              Should this update be kept on the local
     *              GattServer regardless of the state of the
     *              notify/indicate flag in the CCCD for this
     *              Characteristic? If set to true, no notification
     *              or indication is generated.
     *
     * @return BLE_ERROR_NONE if we have successfully set the value of the attribute.
     */
    virtual ble_error_t write(GattAttribute::Handle_t, const uint8_t *, uint16_t, bool localOnly = false) = 0;

    /**
     * Update the value of a characteristic on the local GattServer. A version
     * of the same as above with connection handle parameter to allow updates
     * for connection-specific multivalued attribtues (such as the CCCDs).
     *
     * @param[in] connectionHandle
     *              Connection Handle.
     * @param[in] attributeHandle
     *              Handle for the value attribute of the Characteristic.
     * @param[in] value
     *              A pointer to a buffer holding the new value
     * @param[in] size
     *              Size of the new value (in bytes).
     * @param[in] localOnly
     *              Should this update be kept on the local
     *              GattServer regardless of the state of the
     *              notify/indicate flag in the CCCD for this
     *              Characteristic? If set to true, no notification
     *              or indication is generated.
     *
     * @return BLE_ERROR_NONE if we have successfully set the value of the attribute.
     */
    virtual ble_error_t write(Gap::Handle_t connectionHandle, GattAttribute::Handle_t, const uint8_t *, uint16_t, bool localOnly = false) = 0;

    virtual ble_error_t initializeGATTDatabase(void) = 0;

    // ToDo: For updateValue, check the CCCD to see if the value we are
    // updating has the notify or indicate bits sent, and if BOTH are set
    // be sure to call sd_ble_gatts_hvx() twice with notify then indicate!
    // Strange use case, but valid and must be covered!

    /**
     * Add a callback for the GATT event DATA_SENT (which is triggered when
     * updates are sent out by GATT in the form of notifications).
     *
     * @Note: it is possible to chain together multiple onDataSent callbacks
     * (potentially from different modules of an application) to receive updates
     * to characteristics.
     *
     * @Note: it is also possible to setup a callback into a member function of
     * some object.
     */
    void onDataSent(void (*callback)(unsigned count)) {dataSentCallChain.add(callback);}
    template <typename T>
    void onDataSent(T *objPtr, void (T::*memberPtr)(unsigned count)) {
        dataSentCallChain.add(objPtr, memberPtr);
    }

    /**
     * Setup a callback for when an attribute has its value updated by or at the
     * connected peer. For a peripheral, this callback triggered when the local
     * GATT server has an attribute updated by a write command from the peer.
     * For a Central, this callback is triggered when a response is received for
     * a write request.
     *
     * @Note: it is possible to chain together multiple onDataWritten callbacks
     * (potentially from different modules of an application) to receive updates
     * to characteristics. Many services, such as DFU and UART add their own
     * onDataWritten callbacks behind the scenes to trap interesting events.
     *
     * @Note: it is also possible to setup a callback into a member function of
     * some object.
     */
    void onDataWritten(void (*callback)(const GattWriteCallbackParams *eventDataP)) {dataWrittenCallChain.add(callback);}
    template <typename T>
    void onDataWritten(T *objPtr, void (T::*memberPtr)(const GattWriteCallbackParams *context)) {
        dataWrittenCallChain.add(objPtr, memberPtr);
    }

    /**
     * A virtual function to allow underlying stacks to indicate if they support
     * onDataRead(). It should be overridden to return true as applicable.
     */
    virtual bool isOnDataReadAvailable() const {
        return false;
    }
    ble_error_t setOnDataRead(void (*callback)(const GattReadCallbackParams *eventDataP)) {
        if (!isOnDataReadAvailable()) {
            return BLE_ERROR_NOT_IMPLEMENTED;
        }

        onDataRead.add(callback);
        return BLE_ERROR_NONE;
    }
    template <typename T>
    ble_error_t setOnDataRead(T *objPtr, void (T::*memberPtr)(const GattReadCallbackParams *context)) {
        if (!isOnDataReadAvailable()) {
            return BLE_ERROR_NOT_IMPLEMENTED;
        }

        onDataRead.add(objPtr, memberPtr);
        return BLE_ERROR_NONE;
    }

    /**
     * Setup a callback for when notifications/indications are enabled for a
     * characteristic on the local GattServer.
     */
    void onUpdatesEnabled(EventCallback_t callback)          {updatesEnabledCallback = callback;}

    void setOnUpdatesDisabled(EventCallback_t callback)      {onUpdatesDisabled      = callback;}
    void setOnConfirmationReceived(EventCallback_t callback) {onConfirmationReceived = callback;}

protected:
    void handleDataWrittenEvent(const GattWriteCallbackParams *params) {
        if (dataWrittenCallChain.hasCallbacksAttached()) {
            dataWrittenCallChain.call(params);
        }
    }

    void handleDataReadEvent(const GattReadCallbackParams *params) {
        if (onDataRead.hasCallbacksAttached()) {
            onDataRead.call(params);
        }
    }

    void handleEvent(GattServerEvents::gattEvent_e type, GattAttribute::Handle_t charHandle) {
        switch (type) {
            case GattServerEvents::GATT_EVENT_UPDATES_ENABLED:
                if (updatesEnabledCallback) {
                    updatesEnabledCallback(charHandle);
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
        if (dataSentCallChain.hasCallbacksAttached()) {
            dataSentCallChain.call(count);
        }
    }

protected:
    uint8_t serviceCount;
    uint8_t characteristicCount;

private:
    CallChainOfFunctionPointersWithContext<unsigned>                        dataSentCallChain;
    CallChainOfFunctionPointersWithContext<const GattWriteCallbackParams *> dataWrittenCallChain;
    CallChainOfFunctionPointersWithContext<const GattReadCallbackParams *>  onDataRead;
    EventCallback_t                                                         updatesEnabledCallback;
    EventCallback_t                                                         onUpdatesDisabled;
    EventCallback_t                                                         onConfirmationReceived;

private:
    /* disallow copy and assignment */
    GattServer(const GattServer &);
    GattServer& operator=(const GattServer &);
};

#endif // ifndef __GATT_SERVER_H__
