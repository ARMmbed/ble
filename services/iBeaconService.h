/* mbed Microcontroller Library
 * Copyright (c) 2006-2015 ARM Limited
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
#ifndef __BLE_IBEACON_SERVICE_H__
#define __BLE_IBEACON_SERVICE_H__

#include "BLEDevice.h"

/**
* @class iBeaconService
* @brief iBeacon Service. This service sets up a device to broadcast advertising packets to mimic an iBeacon<br>
*/

class iBeaconService
{
public:
    iBeaconService(BLEDevice &_ble, uint8_t proxUUID[16],uint16_t majNum,uint16_t minNum,uint8_t txP=0xC8, uint16_t compID=0x004C):
        ble(_ble)
    {
        data.ID =  0x02;         // Optional ID field
        data.len = 0x15;         // Len of remaining stuff (16B UUID, 2B Maj, 2B Min, 1B TxP)
        data.majorNumber = ((majNum<<8) | (majNum >>8));
        data.minorNumber = ((minNum<<8) | (minNum >>8));
        data.txPower = txP;      // The user should calibrate this to ~1meter fromt he device
        data.companyID = compID; // Note: all iBeacons use the Apple ID of 0x004C

        // copy across proximity UUID
        for(int x=0; x<sizeof(data.proximityUUID); x++) {
            data.proximityUUID[x]=proxUUID[x];
        }

        // Set up iBeacon data
        // Generate the 0x020106 part of the iBeacon Prefix
        ble.accumulateAdvertisingPayload(GapAdvertisingData::BREDR_NOT_SUPPORTED | GapAdvertisingData::LE_GENERAL_DISCOVERABLE );
        // Generate the 0x1AFF part of the iBeacon Prefix
        ble.accumulateAdvertisingPayload(GapAdvertisingData::MANUFACTURER_SPECIFIC_DATA, data.raw, sizeof(data.raw));

        // Set advertising type
        ble.setAdvertisingType(GapAdvertisingParams::ADV_NON_CONNECTABLE_UNDIRECTED);
    }

public:
    union {
        uint8_t raw[25];
        struct {
            uint16_t companyID;
            uint8_t ID;
            uint8_t len;
            uint8_t proximityUUID[16];
            uint16_t majorNumber;
            uint16_t minorNumber;
            uint8_t txPower;
        };
    } data;

private:
    BLEDevice &ble;

};

#endif //__BLE_IBEACON_SERVICE_H__
