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

#ifndef __BLE_IMMEDATE_ALERT_SERVICE_H__
#define __BLE_IMMEDATE_ALERT_SERVICE_H__

#include "ble/Gap.h"

/**
* @class ImmediateAlertService
* @brief This service defines behavior an alert is sent from the central device.
* Service:  https://developer.bluetooth.org/gatt/services/Pages/ServiceViewer.aspx?u=org.bluetooth.service.immediate_alert.xml
* Alertness Level Char: https://developer.bluetooth.org/gatt/characteristics/Pages/CharacteristicViewer.aspx?u=org.bluetooth.characteristic.alert_level.xml
*/
class ImmediateAlertService {
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
     * @param callbackIn pointer to the function to be called after the alert is received.
     * @param levelIn default Alert Level to be sent to the callback function.
     */
    ImmediateAlertService(BLE &bleIn, callback_t callbackIn, AlertLevel_t levelIn = NO_ALERT) :
        ble(bleIn),
        alertLevel(levelIn),
        callback(callbackIn),
        alertLevelChar(GattCharacteristic::UUID_ALERT_LEVEL_CHAR, reinterpret_cast<uint8_t *>(&alertLevel)) {
        static bool serviceAdded = false; /* We should only ever add one ImmediateAlert service. */
        if (serviceAdded) return;

        GattCharacteristic *charTable[] = {&alertLevelChar};
        GattService         immediateAlertService(GattService::UUID_IMMEDIATE_ALERT_SERVICE, charTable, sizeof(charTable) / sizeof(GattCharacteristic *));

        ble.gattServer().addService(immediateAlertService);
        serviceAdded = true;

        ble.gattServer().onDataWritten(this, &ImmediateAlertService::onDataWritten);
    }

    /**
     * Constructor for handling a callback inside a C++ object.
     *
     * @param ble BLE object for the underlying controller.
     * @param objPtr pointer to the object that contains the callback function.
     * @param onAlertCallback pointer to the function to be called after the alert is received.
     * @param levelIn default Alert Level to be sent to the callback function.
     */
    template <typename T> 
    ImmediateAlertService(BLE &_ble, T *objPtr, void (T::*onAlertCallback)(void), AlertLevel_t levelIn = NO_ALERT) :
        ble(_ble),
        alertLevel(levelIn),
        alertLevelChar(GattCharacteristic::UUID_ALERT_LEVEL_CHAR, reinterpret_cast<uint8_t *>(&alertLevel)) {
        static bool serviceAdded = false; /* We should only ever need to add the information service once. */
        if (serviceAdded) return;

        GattCharacteristic *charTable[] = {&alertLevelChar};
        GattService         immediateAlertService(GattService::UUID_IMMEDIATE_ALERT_SERVICE, charTable, sizeof(charTable) / sizeof(GattCharacteristic *));

        ble.gattServer().addService(immediateAlertService);
        serviceAdded = true;

        ble.gattServer().onDataWritten(this, &ImmediateAlertService::onDataWritten);
        
        onAlert.attach(objPtr, onAlertCallback);
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
     * @param onAlertCallback pointer to the function to be called after the alert is received.     
     */
    template <typename T> 
    void setCallback(T *objPtr, void (T::*onAlertCallback)(void)) {
        if (callback != NULL) callback = NULL;
        onAlert.attach(objPtr, onAlertCallback);
    }

    /**
     * Update the callback function.
     *
     * @param callbackIn pointer to the function to be called after the alert is received.
     */
    void setCallback(callback_t newCallback) {
        if (onAlert != NULL) onAlert = NULL;
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
            if(onAlert != NULL) onAlert.call();
            else callback(alertLevel);
        }
    }

private:
    BLE          &ble;
    AlertLevel_t  alertLevel;

    FunctionPointer onAlert;
    callback_t    callback;

    ReadWriteGattCharacteristic<uint8_t> alertLevelChar;
};

#endif /* __BLE_IMMEDATE_ALERT_SERVICE_H__ */
