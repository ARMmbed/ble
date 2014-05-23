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

    /**
     * @param[in] advType
     *              The GAP advertising mode to use for this device. Valid
     *              values are defined in AdvertisingType:
     *
     *              \par ADV_NON_CONNECTABLE_UNDIRECTED
     *              All connections to the peripheral device will be refused.
     *
     *              \par ADV_CONNECTABLE_DIRECTED
     *              Only connections from a pre-defined central device will be
     *              accepted.
     *
     *              \par ADV_CONNECTABLE_UNDIRECTED
     *              Any central device can connect to this peripheral.
     *
     *              \par ADV_SCANNABLE_UNDIRECTED
     *              Any central device can connect to this peripheral, and
     *              the secondary Scan Response payload will be included or
     *              available to central devices.
     *
     *              \par
     *              See Bluetooth Core Specification 4.0 (Vol. 3), Part C,
     *              Section 9.3 and Core Specification 4.0 (Vol. 6), Part B,
     *              Section 2.3.1 for further information on GAP connection
     *              modes
     */
    void        setAdvertisingType(GapAdvertisingParams::AdvertisingType);

    /**
     * @param[in] interval
     *              Advertising interval between 0x0020 and 0x4000 in 0.625ms
     *              units (20ms to 10.24s).  If using non-connectable mode
     *              (ADV_NON_CONNECTABLE_UNDIRECTED) this min value is
     *              0x00A0 (100ms). To reduce the likelihood of collisions, the
     *              link layer perturbs this interval by a pseudo-random delay
     *              with a range of 0 ms to 10 ms for each advertising event.
     *
     *              \par
     *              Decreasing this value will allow central devices to detect
     *              your peripheral faster at the expense of more power being
     *              used by the radio due to the higher data transmit rate.
     *
     *              \par
     *              This field must be set to 0 if connectionMode is equal
     *              to ADV_CONNECTABLE_DIRECTED
     *
     *              \par
     *              See Bluetooth Core Specification, Vol 3., Part C,
     *              Appendix A for suggested advertising intervals.
     */
    void        setAdvertisingInterval(uint16_t interval);

    /**
     * @param[in] timeout
     *              Advertising timeout between 0x1 and 0x3FFF (1 and 16383)
     *              in seconds.  Enter 0 to disable the advertising timeout.
     */
    void        setAdvertisingTimeout(uint16_t timeout);

    /**
     * Please refer to the APIs above.
     */
    void        setAdvertisingParams(const GapAdvertisingParams &advParams);

    ble_error_t startAdvertising(void);
    ble_error_t stopAdvertising(void);

    ble_error_t disconnect(void);

public:
    BLEDevice() : transport(createBLEDeviceInstance()), advParams() {
        /* empty */
    }

private:
    BLEDeviceInstanceBase *transport; /* handle to the device specific backend*/
    GapAdvertisingParams   advParams;


    /**
     * DEPRECATED
     */
public:
    ble_error_t setAdvertisingData(const GapAdvertisingData &ADStructures,
                                   const GapAdvertisingData &scanResponse);
    ble_error_t setAdvertisingData(const GapAdvertisingData &ADStructures);

    ble_error_t startAdvertising(const GapAdvertisingParams &advParams);
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

inline void
BLEDevice::setAdvertisingType(GapAdvertisingParams::AdvertisingType advType) {
    advParams.setAdvertisingType(advType);
}

inline void
BLEDevice::setAdvertisingInterval(uint16_t interval) {
    advParams.setInterval(interval);
}

inline void
BLEDevice::setAdvertisingTimeout(uint16_t timeout) {
    advParams.setTimeout(timeout);
}

inline void
BLEDevice::setAdvertisingParams(const GapAdvertisingParams &newAdvParams) {
    advParams = newAdvParams;
}

inline ble_error_t
BLEDevice::startAdvertising(void) {
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

/*
 * ALL OF THE FOLLOWING METHODS ARE DEPRECATED
 */

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
BLEDevice::startAdvertising(const GapAdvertisingParams &_advParams) {
    return transport->getGap().startAdvertising(_advParams);
}

#endif // ifndef __BLE_DEVICE_H__
