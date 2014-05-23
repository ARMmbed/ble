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

class BLEDeviceInstanceBase; /* forward declaration */

/**
 * BLEDevice uses composition to hide an interface object encapsulating the
 * backend transport.
 *
 * The following API is used to create the singleton interface object. An
 * implementation for this function must be provided by the device-specific
 * library, otherwise there will be a linker error.
 */
extern BLEDeviceInstanceBase *createBLEDeviceInstance(void);

/**
 * The base class used to abstract away BLE capable radio transceivers or SOCs,
 * to enable this BLE API to work with any radio transparently.
 */
class BLEDevice
{
public:
    ble_error_t init();
    ble_error_t reset(void);

    /* GAP specific APIs */
public:
    /**
     * Set the BTLE MAC address and type.
     * @return
     */
    ble_error_t setAddress(Gap::addr_type_t type, const uint8_t address[6]);

    ble_error_t setAdvertisingData(const GapAdvertisingData &ADStructures,
                                   const GapAdvertisingData &scanResponse);
    ble_error_t setAdvertisingData(const GapAdvertisingData &ADStructures);
    ble_error_t startAdvertising(const GapAdvertisingParams &advParams);
    ble_error_t stopAdvertising(void);
    ble_error_t disconnect(void);

public:
    BLEDevice() : transport(createBLEDeviceInstance()) {
        /* empty */
    }

private:
    BLEDeviceInstanceBase *transport;
};

/**
 *  The interface for the transport object to be created by the target library's
 *  createBLEDeviceInstance().
 */
class BLEDeviceInstanceBase
{
public:
    virtual Gap&        getGap()        = 0;
    virtual GattServer& getGattServer() = 0;
    virtual ble_error_t init(void)      = 0;
    virtual ble_error_t reset(void)     = 0;
};


/* BLEDevice methods. Most of these simply forward the calls to the underlying
 * transport.*/

inline ble_error_t
BLEDevice::init() {
    return transport->init();
}

inline ble_error_t
BLEDevice::reset(void) {
    return transport->reset();
}

inline ble_error_t
BLEDevice::setAddress(Gap::addr_type_t type, const uint8_t address[6]) {
    return transport->getGap().setAddress(type, address);
}

inline ble_error_t
BLEDevice::setAdvertisingData(const GapAdvertisingData &ADStructures,
                              const GapAdvertisingData &scanResponse) {
    return transport->getGap().setAdvertisingData(ADStructures, scanResponse);
}

inline ble_error_t
BLEDevice::setAdvertisingData(const GapAdvertisingData &ADStructures) {
    GapAdvertisingData scanResponse;
    return transport->getGap().setAdvertisingData(ADStructures, scanResponse);
}

inline ble_error_t
BLEDevice::startAdvertising(const GapAdvertisingParams &advParams) {
    return transport->getGap().startAdvertising(advParams);
}

inline ble_error_t
BLEDevice::stopAdvertising(void) {
    return transport->getGap().stopAdvertising();
}

inline ble_error_t
BLEDevice::disconnect(void) {
    return transport->getGap().disconnect();
}

#endif // ifndef __BLE_DEVICE_H__
