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

#ifndef __BLE_PERIPHERAL__
#define __BLE_PERIPHERAL__

#include "mbed.h"
#include "blecommon.h"
#include "hw/Gap.h"
#include "hw/GattServer.h"

class BLEPeripheralInstanceBase; /* forward declaration */

/**
 * BLEPeripheral uses composition to hide an interface object encapsulating the
 * backend transport.
 *
 * The following API is used to create the singleton interface object. An
 * implementation for this function must be provided by the device-specific
 * library, otherwise there will be a linker error.
 */
extern BLEPeripheralInstanceBase *createBLEPeripheralInstance(void);

/**
 * The base class used to abstract away BLE capable radio transceivers or SOCs,
 * to enable this BLE API to work with any radio transparently.
 */
class BLEPeripheral
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

    void        clearAdvertisingPayload(void);
    ble_error_t accumulateAdvertisingPayload(GapAdvertisingData::Flags flags);
    ble_error_t accumulateAdvertisingPayload(GapAdvertisingData::Appearance app);
    ble_error_t accumulateAdvertisingPayloadTxPower(int8_t power);
    ble_error_t accumulateAdvertisingPayload(GapAdvertisingData::DataType type, const uint8_t *data, uint8_t len);

    ble_error_t startAdvertising(void);
    ble_error_t stopAdvertising(void);

    ble_error_t disconnect(void);

    /* APIs to set GAP callbacks. */
    void onTimeout(Gap::EventCallback_t       timeoutCallback);
    void onConnection(Gap::EventCallback_t    connectionCallback);
    void onDisconnection(Gap::EventCallback_t disconnectionCallback);

    /* APIs to set GATT server callbacks */
    void onDataSent(GattServer::EventCallback_t callback);
    void onDataWritten(GattServer::EventCallback_t callback);
    void onUpdatesEnabled(GattServer::EventCallback_t callback);
    void onUpdatesDisabled(GattServer::EventCallback_t callback);
    void onConfirmationReceived(GattServer::EventCallback_t callback);

    ble_error_t addService(GattService &service);

    Gap::GapState_t getGapState(void) const;
    ble_error_t     updateCharacteristicValue(uint16_t handle, const uint8_t* value, uint16_t size, bool localOnly = false);

    /**
     * Yield control to the BLE stack or to other tasks waiting for events. This
     * is a sleep function which will return when there is an application
     * specific interrupt, but the MCU might wake up several times before
     * returning (to service the stack). This is not always interchangeable with
     * WFE().
     */
    void waitForEvent(void);

private:
    /**
     * Internal helper to udpate the transport backend with advertising data
     * before starting to advertise.
     */
    ble_error_t setAdvertisingDataForTransport(void);

public:
    BLEPeripheral() : transport(createBLEPeripheralInstance()), advParams(), advPayload(), scanResponse(), needToUpdateAdvData(true) {
        advPayload.clear();
        scanResponse.clear();
    }

private:
    BLEPeripheralInstanceBase *const transport; /* the device specific backend */

    GapAdvertisingParams advParams;
    GapAdvertisingData   advPayload;
    GapAdvertisingData   scanResponse;

    /* Accumulation of AD structures in the advertisement payload should
     * eventually result in a call to the target's setAdvertisingData() before
     * the server begins advertising. This flag marks the status of the pending
     * update.*/
    bool                 needToUpdateAdvData;

    /**
     * DEPRECATED
     */
public:
    ble_error_t setAdvertisingData(const GapAdvertisingData &ADStructures, const GapAdvertisingData &scanResponse);
    ble_error_t setAdvertisingData(const GapAdvertisingData &ADStructures);

    ble_error_t startAdvertising(const GapAdvertisingParams &advParams);
};

/**
 *  The interface for the transport object to be created by the target library's
 *  createBLEPeripheralInstance().
 */
class BLEPeripheralInstanceBase
{
public:
    virtual Gap&        getGap()           = 0;
    virtual GattServer& getGattServer()    = 0;
    virtual ble_error_t init(void)         = 0;
    virtual ble_error_t reset(void)        = 0;
    virtual void        waitForEvent(void) = 0;
};


/* BLEPeripheral methods. Most of these simply forward the calls to the underlying
 * transport.*/

inline ble_error_t
BLEPeripheral::init()
{
    return transport->init();
}

inline ble_error_t
BLEPeripheral::reset(void)
{
    return transport->reset();
}

inline ble_error_t
BLEPeripheral::setAddress(Gap::addr_type_t type, const uint8_t address[6])
{
    return transport->getGap().setAddress(type, address);
}

inline void
BLEPeripheral::setAdvertisingType(GapAdvertisingParams::AdvertisingType advType)
{
    advParams.setAdvertisingType(advType);
}

inline void
BLEPeripheral::setAdvertisingInterval(uint16_t interval)
{
    advParams.setInterval(interval);
}

inline void
BLEPeripheral::setAdvertisingTimeout(uint16_t timeout)
{
    advParams.setTimeout(timeout);
}

inline void
BLEPeripheral::setAdvertisingParams(const GapAdvertisingParams &newAdvParams)
{
    advParams = newAdvParams;
}

inline void
BLEPeripheral::clearAdvertisingPayload(void)
{
    needToUpdateAdvData = true;
    advPayload.clear();
}

inline ble_error_t
BLEPeripheral::accumulateAdvertisingPayload(GapAdvertisingData::Flags flags)
{
    needToUpdateAdvData = true;
    return advPayload.addFlags(flags);
}

inline ble_error_t
BLEPeripheral::accumulateAdvertisingPayload(GapAdvertisingData::Appearance app)
{
    needToUpdateAdvData = true;
    return advPayload.addAppearance(app);
}

inline ble_error_t
BLEPeripheral::accumulateAdvertisingPayloadTxPower(int8_t txPower)
{
    needToUpdateAdvData = true;
    return advPayload.addTxPower(txPower);
}

inline ble_error_t
BLEPeripheral::accumulateAdvertisingPayload(GapAdvertisingData::DataType  type, const uint8_t *data, uint8_t len)
{
    needToUpdateAdvData = true;
    return advPayload.addData(type, data, len);
}

inline ble_error_t
BLEPeripheral::startAdvertising(void)
{
    if (needToUpdateAdvData) {
        setAdvertisingDataForTransport();
        needToUpdateAdvData = false;
    }

    return transport->getGap().startAdvertising(advParams);
}

inline ble_error_t
BLEPeripheral::stopAdvertising(void)
{
    return transport->getGap().stopAdvertising();
}

inline ble_error_t
BLEPeripheral::disconnect(void)
{
    return transport->getGap().disconnect();
}

inline ble_error_t
BLEPeripheral::setAdvertisingDataForTransport(void)
{
    return transport->getGap().setAdvertisingData(advPayload, scanResponse);
}

inline void
BLEPeripheral::onTimeout(Gap::EventCallback_t timeoutCallback)
{
    transport->getGap().setOnTimeout(timeoutCallback);
}

inline void
BLEPeripheral::onConnection(Gap::EventCallback_t connectionCallback)
{
    transport->getGap().setOnConnection(connectionCallback);
}

inline void
BLEPeripheral::onDisconnection(Gap::EventCallback_t disconnectionCallback)
{
    transport->getGap().setOnDisconnection(disconnectionCallback);
}

inline void
BLEPeripheral::onDataSent(GattServer::EventCallback_t callback)
{
    transport->getGattServer().setOnDataSent(callback);
}

inline void
BLEPeripheral::onDataWritten(GattServer::EventCallback_t callback)
{
    transport->getGattServer().setOnDataWritten(callback);
}

inline void
BLEPeripheral::onUpdatesEnabled(GattServer::EventCallback_t callback)
{
    transport->getGattServer().setOnUpdatesEnabled(callback);
}

inline void
BLEPeripheral::onUpdatesDisabled(GattServer::EventCallback_t callback)
{
    transport->getGattServer().setOnUpdatesDisabled(callback);
}

inline void
BLEPeripheral::onConfirmationReceived(GattServer::EventCallback_t callback)
{
    transport->getGattServer().setOnConfirmationReceived(callback);
}

inline ble_error_t
BLEPeripheral::addService(GattService &service)
{
    return transport->getGattServer().addService(service);
}

inline Gap::GapState_t
BLEPeripheral::getGapState(void) const
{
    return transport->getGap().getState();
}

inline ble_error_t
BLEPeripheral::updateCharacteristicValue(uint16_t handle, const uint8_t* value, uint16_t size, bool localOnly)
{
    return transport->getGattServer().updateValue(handle, const_cast<uint8_t *>(value), size, localOnly);
}

inline void
BLEPeripheral::waitForEvent(void)
{
    transport->waitForEvent();
}

/*
 * ALL OF THE FOLLOWING METHODS ARE DEPRECATED
 */

inline ble_error_t
BLEPeripheral::setAdvertisingData(const GapAdvertisingData &ADStructures, const GapAdvertisingData &scanResponse)
{
    return transport->getGap().setAdvertisingData(ADStructures, scanResponse);
}

inline ble_error_t
BLEPeripheral::setAdvertisingData(const GapAdvertisingData &ADStructures)
{
    GapAdvertisingData scanResponse;
    return transport->getGap().setAdvertisingData(ADStructures, scanResponse);
}

inline ble_error_t
BLEPeripheral::startAdvertising(const GapAdvertisingParams &_advParams)
{
    return transport->getGap().startAdvertising(_advParams);
}

#endif // ifndef __BLE_PERIPHERAL__
