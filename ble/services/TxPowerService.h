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

#ifndef __BLE_TX_POWER_SERVICE_H__
#define __BLE_TX_POWER_SERVICE_H__

#include "ble/Gap.h"

/**
* @class TxPowerService
* @brief This service exposes a device’s current transmit power level when in a connection. <br>
* Service:  https://developer.bluetooth.org/gatt/services/Pages/ServiceViewer.aspx?u=org.bluetooth.service.tx_power.xml <br>
* Tx Power Char: https://developer.bluetooth.org/gatt/characteristics/Pages/CharacteristicViewer.aspx?u=org.bluetooth.characteristic.tx_power_level.xml <br>
*/
class TxPowerService {
public:

    /**
     * @param[ref] ble
     *               BLE object for the underlying controller.
     */
    TxPowerService(BLE &bleIn, int8_t txPower) :
        ble(bleIn),
        TxPowerChar(GattCharacteristic::UUID_TX_POWER_LEVEL_CHAR, reinterpret_cast<uint8_t *>(&txPower)) {
        static bool serviceAdded = false; /* We should only ever add one TxPower service. */
        if (serviceAdded) {
            return;
        }

        GattCharacteristic *charTable[] = {&TxPowerChar};
        GattService         TxPowerService(GattService::UUID_TX_POWER_SERVICE, charTable, sizeof(charTable) / sizeof(GattCharacteristic *));

        ble.gattServer().addService(TxPowerService);
        serviceAdded = true;

    }

    /**
     * Update Tx Power.
     */
    void updateTxPower(int8_t newTxPower) {
        txPower = newTxPower;
        ble.gattServer().write(TxPowerChar.getValueHandle(), (const uint8_t*) &txPower, sizeof(txPower));
    }

protected:
    BLE             &ble;
    int8_t          txPower;

    ReadWriteGattCharacteristic<uint8_t> TxPowerChar;
};

#endif /* __BLE_TX_POWER_SERVICE_H__ */
