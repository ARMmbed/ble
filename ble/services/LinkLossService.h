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

#ifndef __BLE_LINK_LOSS_SERVICE_H__
#define __BLE_LINK_LOSS_SERVICE_H__

#include "ble/Gap.h"

/**
* @class LinkLossService
* @brief This service defines behavior when a link is lost between two devices.
* Service:  https://developer.bluetooth.org/gatt/services/Pages/ServiceViewer.aspx?u=org.bluetooth.service.link_loss.xml
* Alertness Level Char: https://developer.bluetooth.org/gatt/characteristics/Pages/CharacteristicViewer.aspx?u=org.bluetooth.characteristic.alert_level.xml
*/
class LinkLossService {
public:
    enum AlertLevel_t {
        NO_ALERT   = 0,
        MILD_ALERT = 1,
        HIGH_ALERT = 2
    };

    typedef void (* callback_t)(AlertLevel_t level);

    /**
     * Constructor for handlig a callback function. 
     *
     * @param ble BLE object for the underlying controller.
     * @param callbackIn pointer to the function to be called after the link is loss.
     * @param levelIn default Alert Level to be sent to the callback function.
     */
    LinkLossService(BLE &bleIn, callback_t callbackIn, AlertLevel_t levelIn = NO_ALERT) :
        ble(bleIn),
        alertLevel(levelIn),
        callback(callbackIn),
        alertLevelChar(GattCharacteristic::UUID_ALERT_LEVEL_CHAR, reinterpret_cast<uint8_t *>(&alertLevel)) {
        static bool serviceAdded = false; /* We should only ever add one LinkLoss service. */
        if (serviceAdded) {
            return;
        }

        GattCharacteristic *charTable[] = {&alertLevelChar};
        GattService         linkLossService(GattService::UUID_LINK_LOSS_SERVICE, charTable, sizeof(charTable) / sizeof(GattCharacteristic *));

        ble.gattServer().addService(linkLossService);
        serviceAdded = true;

        ble.gap().onDisconnection(this, &LinkLossService::onDisconnectionFilter);
        ble.gattServer().onDataWritten(this, &LinkLossService::onDataWritten);
    }

    /**
     * Constructor for handling a callback inside a C++ object.
     *
     * @param ble BLE object for the underlying controller.
     * @param objPtr pointer to the object that contains the callback function.
     * @param onLinkLossCallback pointer to the function to be called after the link is loss.
     * @param levelIn default Alert Level to be sent to the callback function.
     */
    template <typename T> 
    LinkLossService(BLE &bleIn, T *objPtr, void (T::*onLinkLossCallback)(void), AlertLevel_t levelIn = NO_ALERT) :
        ble(bleIn),
        alertLevel(levelIn),
        alertLevelChar(GattCharacteristic::UUID_ALERT_LEVEL_CHAR, reinterpret_cast<uint8_t *>(&alertLevel)) {
        static bool serviceAdded = false; /* We should only ever add one LinkLoss service. */
        if (serviceAdded) {
            return;
        }

        GattCharacteristic *charTable[] = {&alertLevelChar};
        GattService         linkLossService(GattService::UUID_LINK_LOSS_SERVICE, charTable, sizeof(charTable) / sizeof(GattCharacteristic *));

        ble.gattServer().addService(linkLossService);
        serviceAdded = true;

        ble.gap().onDisconnection(this, &LinkLossService::onDisconnectionFilter);
        ble.gattServer().onDataWritten(this, &LinkLossService::onDataWritten);

        onLinkLoss.attach(objPtr, onLinkLossCallback);
    }

    /**
     * Update alertness level.
     *
     * @param   new_level New alert level.
     */
    void setAlertLevel(AlertLevel_t newLevel) {
        alertLevel = newLevel;
        ble.gattServer().write(alertLevelChar.getValueHandle(), (uint8_t *) &alertLevel, 1);
    }

    /**
     * @brief   Get current alert level. This is used because we can't pass arguments to callbacks.
     */    
    int8_t getAlertLevel()
    {
        return alertLevel;
    }

    /**
     * Update the callback with an object's method.
     *
     * @param objPtr pointer to the object that contains the callback function.
     * @param onLinkLossCallback pointer to the function to be called after the link is loss.     
     */
    template <typename T> 
    void setCallback(T *objPtr, void (T::*onLinkLossCallback)(void)) {
        if (callback != NULL) callback = NULL;
        onLinkLoss.attach(objPtr, onLinkLossCallback);
    }

    /**
     * Update the callback function.
     *
     * @param callbackIn pointer to the function to be called after the link is loss.
     */
    void setCallback(callback_t newCallback) {
        if (onLinkLoss != NULL) onLinkLoss = NULL;
        callback = newCallback;
    }

protected:
    /**
     * This callback allows receiving updates to the AlertLevel characteristic.
     *
     * @param[in] params
     *     Information about the characterisitc being updated.
     */
    virtual void onDataWritten(const GattWriteCallbackParams *params) {
        if (params->handle == alertLevelChar.getValueHandle()) {
            alertLevel = *reinterpret_cast<const AlertLevel_t *>(params->data);
        }
    }

    void onDisconnectionFilter(const Gap::DisconnectionCallbackParams_t *params) {
        if (alertLevel != NO_ALERT) {
            if(onLinkLoss != NULL) onLinkLoss.call();
            else callback(alertLevel);
        }
    }

protected:
    BLE          &ble;
    AlertLevel_t  alertLevel;

    FunctionPointer onLinkLoss;
    callback_t    callback;

    ReadWriteGattCharacteristic<uint8_t> alertLevelChar;
};

#endif /* __BLE_LINK_LOSS_SERVICE_H__ */
