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

#ifndef __BLE_DEVICE_H__
#define __BLE_DEVICE_H__

#include "mbed.h"
#include "blecommon.h"
#include "hw/Gap.h"
#include "hw/GattServer.h"

/**************************************************************************/
/*!
    \brief
    The base class used to abstract away BLE capable radio transceivers
    or SOCs, to enable this BLE API to work with any radio transparently.
*/
/**************************************************************************/
class BLEDevice
{
public:
    virtual ble_error_t init()        = 0;
    virtual ble_error_t reset(void)   = 0;

    /* GAP specific APIs */
public:
    /**
     * Set the BTLE MAC address and type.
     * @return
     */
    ble_error_t setAddress(Gap::addr_type_t type, uint8_t address[6]);

    ble_error_t setAdvertisingData(GapAdvertisingData &ADStructures,
                                   GapAdvertisingData &scanResponse);
    ble_error_t setAdvertisingData(GapAdvertisingData &ADStructures);
    ble_error_t startAdvertising(GapAdvertisingParams &advParams);
    ble_error_t stopAdvertising(void);
    ble_error_t disconnect(void);

public: /* TODO: to be made private soon */
    virtual Gap&        getGap()        = 0;
    virtual GattServer& getGattServer() = 0;
};

inline ble_error_t BLEDevice::setAddress(Gap::addr_type_t type, uint8_t address[6]) {
    return getGap().setAddress(type, address);
}

inline ble_error_t BLEDevice::setAdvertisingData(GapAdvertisingData &ADStructures,
                               GapAdvertisingData &scanResponse) {
    return getGap().setAdvertisingData(ADStructures, scanResponse);
}

inline ble_error_t BLEDevice::setAdvertisingData(GapAdvertisingData &ADStructures) {
    GapAdvertisingData scanResponse;
    return getGap().setAdvertisingData(ADStructures, scanResponse);
}

inline ble_error_t BLEDevice::startAdvertising(GapAdvertisingParams &advParams) {
    return getGap().startAdvertising(advParams);
}

inline ble_error_t BLEDevice::stopAdvertising(void) {
    return getGap().stopAdvertising();
}

inline ble_error_t BLEDevice::disconnect(void) {
    return getGap().disconnect();
}

#endif // ifndef __BLE_DEVICE_H__
