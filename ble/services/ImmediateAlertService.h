/*
 * ImmediateAlertService.h
 *
 *  Created on: 2016-02-02
 *      Author: KUANG Qi
 */

#ifndef __BLE_IMMEDIATE_ALERT_SERVICE_H__
#define __BLE_IMMEDIATE_ALERT_SERVICE_H__

#include "ble/BLE.h"

class ImmediateAlertService
{
public:
    enum AlertLevel_t
    {
        NO_ALERT = 0,
        MILD_ALERT = 1,
        HIGH_ALERT = 2
    };

    typedef void (*callback_t)(AlertLevel_t level);

    /**
     * @param[ref] ble
     *               BLE object for the underlying controller.
     */
    ImmediateAlertService(BLE &bleIn, callback_t callbackIn,
            AlertLevel_t levelIn = NO_ALERT) :
            ble(bleIn), alertLevel(levelIn), callback(callbackIn), alertLevelChar(
                    GattCharacteristic::UUID_ALERT_LEVEL_CHAR,
                    reinterpret_cast<uint8_t *>(&alertLevel), 1, 1,
                    GattCharacteristic::BLE_GATT_CHAR_PROPERTIES_WRITE_WITHOUT_RESPONSE)
    {
        static bool serviceAdded = false; /* We should only ever add one Immdiate Alert service. */
        if (serviceAdded)
        {
            return;
        }

        GattCharacteristic *charTable[] =
        { &alertLevelChar };
        GattService immediateAlertService(
                GattService::UUID_IMMEDIATE_ALERT_SERVICE, charTable,
                sizeof(charTable) / sizeof(GattCharacteristic *));
        ble.gattServer().addService(immediateAlertService);
        serviceAdded = true;

        ble.gattServer().onDataWritten(this,
                &ImmediateAlertService::onDataWritten);
    }

    /**
     * Update the callback.
     */
    void setCallback(callback_t newCallback)
    {
        callback = newCallback;
    }

    /**
     * Update alertness level.
     */
    void setAlertLevel(AlertLevel_t newLevel)
    {
        alertLevel = newLevel;
    }

protected:
    /**
     * This callback allows receiving updates to the AlertLevel characteristic.
     *
     * @param[in] params
     *     Information about the characterisitc being updated.
     */
    virtual void onDataWritten(const GattWriteCallbackParams *params)
    {
        if (params->handle == alertLevelChar.getValueHandle())
        {
            alertLevel = *reinterpret_cast<const AlertLevel_t *>(params->data);
            callback(alertLevel);
        }
    }

protected:
    BLE &ble;
    AlertLevel_t alertLevel;
    callback_t callback;
    GattCharacteristic alertLevelChar;

};

#endif /* __BLE_IMMEDIATE_ALERT_SERVICE_H__ */
