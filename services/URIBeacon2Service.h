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

#ifndef __BLE_URI_BEACON_2_SERVICE_H__
#define __BLE_URI_BEACON_2_SERVICE_H__

#include "BLEDevice.h"

class URIBeacon2Service {
public:

    // ee0c2080-8786-40ba-ab96-99b91ac981d8

    URIBeacon2Service(BLEDevice &ble_, const uint8_t urldata_[], size_t sizeofURLData) : ble(ble_), payload() {
        if ((sizeofURLData + 4) > 24) {
            return;
        }

        const uint8_t BEACON_UUID[] = {0xD8, 0xFE};

        payload[0] = BEACON_UUID[0];
        payload[1] = BEACON_UUID[1];
        payload[2] = 0x00; /* flags */
        payload[3] = 0x20; /* power */
        encodeURIData(urldata_, sizeofURLData);

        ble.accumulateAdvertisingPayload(GapAdvertisingData::COMPLETE_LIST_16BIT_SERVICE_IDS, BEACON_UUID, sizeof(BEACON_UUID));
        ble.accumulateAdvertisingPayload(GapAdvertisingData::SERVICE_DATA, payload, sizeofURLData + 4);
    }

private:
    void encodeURIData(const uint8_t urldata_[], size_t sizeofURLData) {
        memcpy(&payload[4], urldata_, sizeofURLData);
// |Decimal  | Hex        | Expansion
// |:------- | :--------- | :--------
// |0        | 0x00       | http://www.
// |1        | 0x01       | https://www.
// |2        | 0x02       | http://
// |3        | 0x03       | https://
// |4        | 0x04       | urn:uuid:
    }

    // URIBeacon2Service(BLEDevice &_ble, uint8_t level = 100) :
    //     ble(_ble),
    //     batteryLevel(level),
    //     batteryLevelCharacteristic(GattCharacteristic::UUID_BATTERY_LEVEL_CHAR, &batteryLevel, sizeof(batteryLevel), sizeof(batteryLevel),
    //                                GattCharacteristic::BLE_GATT_CHAR_PROPERTIES_READ | GattCharacteristic::BLE_GATT_CHAR_PROPERTIES_NOTIFY) {

    //     static bool serviceAdded = false; /* We should only ever need to add the heart rate service once. */
    //     if (serviceAdded) {
    //         return;
    //     }

    //     GattCharacteristic *charTable[] = {&batteryLevelCharacteristic};
    //     GattService         batteryService(GattService::UUID_BATTERY_SERVICE, charTable, sizeof(charTable) / sizeof(GattCharacteristic *));

    //     ble.addService(batteryService);
    //     serviceAdded = true;
    // }
    //

private:
    static const size_t MAX_SIZEOF_PAYLOAD = 32; /* TODO */

private:
    BLEDevice &ble;
    uint8_t    payload[MAX_SIZEOF_PAYLOAD];
    // uint8_t              batteryLevel;
    // GattCharacteristic   batteryLevelCharacteristic;
};

#endif /* #ifndef __BLE_URI_BEACON_2_SERVICE_H__*/
