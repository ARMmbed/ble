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

#ifndef __BLE_H__
#define __BLE_H__

#include "blecommon.h"
#include "Gap.h"
#include "GattServer.h"
#include "GattClient.h"
#include "BLEInstanceBase.h"

/**
 * The base class used to abstract away BLE capable radio transceivers or SOCs,
 * to enable this BLE API to work with any radio transparently.
 */
class BLE
{
public:
    /**
     * Initialize the BLE controller. This should be called before using
     * anything else in the BLE_API.
     *
     * init() hands control to the underlying BLE module to accomplish
     * initialization. This initialization may tacitly depend on other hardware
     * setup (such as clocks or power-modes) which happens early on during
     * system startup. It may not be safe to call init() from global static
     * context where ordering is compiler specific and can't be guaranteed--it
     * is safe to call BLE::init() from within main().
     */
    ble_error_t init();

    ble_error_t reset(void);

    /**
     * Purge the BLE stack of GATT and GAP state. init() must be called afterwards to re-instate services and GAP state.
     */
    ble_error_t shutdown(void);

    /* Accessors to GAP. Please refer to Gap.h for GAP related functionality. */
    const Gap &gap() const {
        return transport->getGap();
    }
    Gap &gap() {
        return transport->getGap();
    }

public:
    /* GAP specific APIs */
    /**
     * Set the BTLE MAC address and type.
     * @return BLE_ERROR_NONE on success.
     */
    ble_error_t setAddress(Gap::AddressType_t type, const Gap::Address_t address);

    /**
     * Fetch the BTLE MAC address and type.
     * @return BLE_ERROR_NONE on success.
     */
    ble_error_t getAddress(Gap::AddressType_t *typeP, Gap::Address_t address);

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
     *              Include support for Scan Response payloads.
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
     *              Advertising interval in units of milliseconds. Advertising
     *              is disabled if interval is 0. If interval is smaller than
     *              the minimum supported value, then the minimum supported
     *              value is used instead.
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
     *
     * @Note: [WARNING] This API previously used 0.625ms as the unit for its
     * 'interval' argument. That required an explicit conversion from
     * milliseconds using Gap::MSEC_TO_GAP_DURATION_UNITS(). This conversion is
     * no longer required as the new units are milliseconds. Any application
     * code depending on the old semantics would need to be updated accordingly.
     */
    void        setAdvertisingInterval(uint16_t interval);

    /**
     * @return Minimum Advertising interval in milliseconds.
     */
    uint16_t    getMinAdvertisingInterval(void) const;
    /**
     * @return Minimum Advertising interval in milliseconds for non connectible mode.
     */
    uint16_t    getMinNonConnectableAdvertisingInterval(void) const;
    /**
     * @return Maximum Advertising interval in milliseconds.
     */
    uint16_t    getMaxAdvertisingInterval(void) const;

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

    /**
     * @return  Read back advertising parameters. Useful for storing and
     *          restoring parameters rapidly.
     */
    const GapAdvertisingParams &getAdvertisingParams(void) const;

    /**
     * This API is typically used as an internal helper to udpate the transport
     * backend with advertising data before starting to advertise. It may also
     * be explicity used to dynamically reset the accumulated advertising
     * payload and scanResponse; to do this, the application can clear and re-
     * accumulate a new advertising payload (and scanResponse) before using this
     * API.
     */
    ble_error_t setAdvertisingPayload(void);

    /**
     * Set advertising data using object.
     */
    ble_error_t setAdvertisingData(const GapAdvertisingData &advData);

    /**
     * @return  Read back advertising data. Useful for storing and
     *          restoring payload.
     */
    const GapAdvertisingData &getAdvertisingData(void) const;

    /**
     * Reset any advertising payload prepared from prior calls to
     * accumulateAdvertisingPayload().
     *
     * Note: This should be followed by a call to setAdvertisingPayload() or
     * startAdvertising() before the update takes effect.
     */
    void        clearAdvertisingPayload(void);

    /**
     * Accumulate an AD structure in the advertising payload. Please note that
     * the payload is limited to 31 bytes. The SCAN_RESPONSE message may be used
     * as an additional 31 bytes if the advertising payload proves to be too
     * small.
     *
     * @param  flags
     *         The flags to be added. Multiple flags may be specified in
     *         combination.
     */
    ble_error_t accumulateAdvertisingPayload(uint8_t flags);

    /**
     * Accumulate an AD structure in the advertising payload. Please note that
     * the payload is limited to 31 bytes. The SCAN_RESPONSE message may be used
     * as an additional 31 bytes if the advertising payload proves to be too
     * small.
     *
     * @param  app
     *         The appearance of the peripheral.
     */
    ble_error_t accumulateAdvertisingPayload(GapAdvertisingData::Appearance app);

    /**
     * Accumulate an AD structure in the advertising payload. Please note that
     * the payload is limited to 31 bytes. The SCAN_RESPONSE message may be used
     * as an additional 31 bytes if the advertising payload proves to be too
     * small.
     *
     * @param  app
     *         The max transmit power to be used by the controller. This is
     *         only a hint.
     */
    ble_error_t accumulateAdvertisingPayloadTxPower(int8_t power);

    /**
     * Accumulate a variable length byte-stream as an AD structure in the
     * advertising payload. Please note that the payload is limited to 31 bytes.
     * The SCAN_RESPONSE message may be used as an additional 31 bytes if the
     * advertising payload proves to be too small.
     *
     * @param  type The type which describes the variable length data.
     * @param  data data bytes.
     * @param  len  length of data.
     */
    ble_error_t accumulateAdvertisingPayload(GapAdvertisingData::DataType type, const uint8_t *data, uint8_t len);

    /**
     * Accumulate a variable length byte-stream as an AD structure in the
     * scanResponse payload.
     *
     * @param  type The type which describes the variable length data.
     * @param  data data bytes.
     * @param  len  length of data.
     */
    ble_error_t accumulateScanResponse(GapAdvertisingData::DataType type, const uint8_t *data, uint8_t len);

    /**
     * Reset any scan response prepared from prior calls to
     * accumulateScanResponse().
     *
     * Note: This should be followed by a call to setAdvertisingPayload() or
     * startAdvertising() before the update takes effect.
     */
    void        clearScanResponse(void);

    /**
     * Start advertising (GAP Discoverable, Connectable modes, Broadcast
     * Procedure).
     */
    ble_error_t startAdvertising(void);

    /**
     * Stop advertising (GAP Discoverable, Connectable modes, Broadcast
     * Procedure).
     */
    ble_error_t stopAdvertising(void);

    /**
     * Setup parameters for GAP scanning--i.e. observer mode.
     * @param  interval Scan interval (in milliseconds) [valid values lie between 2.5ms and 10.24s].
     * @param  window   Scan Window (in milliseconds) [valid values lie between 2.5ms and 10.24s].
     * @param  timeout  Scan timeout (in seconds) between 0x0001 and 0xFFFF, 0x0000 disables timeout.
     * @param  activeScanning Set to True if active-scanning is required. This is used to fetch the
     *                        scan response from a peer if possible.
     *
     * The scanning window divided by the interval determines the duty cycle for
     * scanning. For example, if the interval is 100ms and the window is 10ms,
     * then the controller will scan for 10 percent of the time. It is possible
     * to have the interval and window set to the same value. In this case,
     * scanning is continuous, with a change of scanning frequency once every
     * interval.
     *
     * Once the scanning parameters have been configured, scanning can be
     * enabled by using startScan().
     *
     * @Note: The scan interval and window are recommendations to the BLE stack.
     */
    ble_error_t setScanParams(uint16_t interval       = GapScanningParams::SCAN_INTERVAL_MAX,
                              uint16_t window         = GapScanningParams::SCAN_WINDOW_MAX,
                              uint16_t timeout        = 0,
                              bool     activeScanning = false);
    ble_error_t setScanInterval(uint16_t interval);
    ble_error_t setScanWindow  (uint16_t window);
    ble_error_t setScanTimeout (uint16_t timeout);
    void        setActiveScan  (bool     activeScanning);

    /**
     * Start scanning (Observer Procedure) based on the scan-params currently
     * in effect.
     *
     * @param  callback The application callback to be invoked upon receiving
     *     every advertisement report. Can be passed in as NULL, in which case
     *     scanning may not be enabled at all.
     */
    ble_error_t startScan(void (*callback)(const Gap::AdvertisementCallbackParams_t *params));

    /**
     * Start scanning (Observer Procedure) based on the scan-params currently
     * in effect.
     *
     * @param[in] object
     * @param[in] callbackMember
     *                The above pair of parameters define the callback object
     *                and member function to receive the advertisement params.
     */
    template<typename T>
    ble_error_t startScan(T *object, void (T::*memberCallback)(const Gap::AdvertisementCallbackParams_t *params));

    /**
     * Stop scanning. The current scanning parameters remain in effect.
     *
     * @retval BLE_ERROR_NONE if successfully stopped scanning procedure.
     */
    ble_error_t stopScan(void);

    /**
     * Create a connection (GAP Link Establishment).
     * @param peerAddr
     *          48-bit address, LSB format.
     * @param peerAddrType
     *          Address type of the peer.
     * @param connectionParams
     *         Connection parameters.
     * @param scanParams
     *          Paramters to be used while scanning for the peer.
     * @return  BLE_ERROR_NONE if connection establishment procedure is started
     *     successfully. The onConnection callback (if set) will be invoked upon
     *     a connection event.
     */
    ble_error_t connect(const Gap::Address_t           peerAddr,
                        Gap::AddressType_t             peerAddrType = Gap::ADDR_TYPE_RANDOM_STATIC,
                        const Gap::ConnectionParams_t *connectionParams = NULL,
                        const GapScanningParams       *scanParams = NULL);

    /**
     * This call initiates the disconnection procedure, and its completion will
     * be communicated to the application with an invocation of the
     * onDisconnection callback.
     *
     * @param  reason
     *           The reason for disconnection to be sent back to the peer.
     */
    ble_error_t disconnect(Gap::DisconnectionReason_t reason);

    /* APIs to set GAP callbacks. */
    void onTimeout(Gap::EventCallback_t timeoutCallback);

    void onConnection(Gap::ConnectionEventCallback_t connectionCallback);
    /**
     * Used to setup a callback for GAP disconnection.
     */
    void onDisconnection(Gap::DisconnectionEventCallback_t disconnectionCallback);

    /**
     * Append to a chain of callbacks to be invoked upon disconnection; these
     * callbacks receive no context and are therefore different from the
     * onDisconnection callback.
     */
    template<typename T>
    void addToDisconnectionCallChain(T *tptr, void (T::*mptr)(void));

    /**
     * Add a callback for the GATT event DATA_SENT (which is triggered when
     * updates are sent out by GATT in the form of notifications).
     *
     * @Note: it is possible to chain together multiple onDataSent callbacks
     * (potentially from different modules of an application) to receive updates
     * to characteristics.
     *
     * @Note: it is also possible to setup a callback into a member function of
     * some object.
     */
    void onDataSent(void (*callback)(unsigned count));
    template <typename T> void onDataSent(T * objPtr, void (T::*memberPtr)(unsigned count));

    /**
     * Setup a callback for when a characteristic has its value updated by a
     * client.
     *
     * @Note: it is possible to chain together multiple onDataWritten callbacks
     * (potentially from different modules of an application) to receive updates
     * to characteristics. Many services, such as DFU and UART add their own
     * onDataWritten callbacks behind the scenes to trap interesting events.
     *
     * @Note: it is also possible to setup a callback into a member function of
     * some object.
     */
    void onDataWritten(void (*callback)(const GattWriteCallbackParams *eventDataP));
    template <typename T> void onDataWritten(T * objPtr, void (T::*memberPtr)(const GattWriteCallbackParams *context));

    /**
     * Setup a callback for when a characteristic is being read by a client.
     *
     * @Note: this functionality may not be available on all underlying stacks.
     * You could use GattCharacteristic::setReadAuthorizationCallback() as an
     * alternative.
     *
     * @Note: it is possible to chain together multiple onDataRead callbacks
     * (potentially from different modules of an application) to receive updates
     * to characteristics. Services may add their own onDataRead callbacks
     * behind the scenes to trap interesting events.
     *
     * @Note: it is also possible to setup a callback into a member function of
     * some object.
     *
     * @return BLE_ERROR_NOT_IMPLEMENTED if this functionality isn't available;
     *         else BLE_ERROR_NONE.
     */
    ble_error_t onDataRead(void (*callback)(const GattReadCallbackParams *eventDataP));
    template <typename T> ble_error_t onDataRead(T * objPtr, void (T::*memberPtr)(const GattReadCallbackParams *context));

    void onUpdatesEnabled(GattServer::EventCallback_t callback);
    void onUpdatesDisabled(GattServer::EventCallback_t callback);
    void onConfirmationReceived(GattServer::EventCallback_t callback);

    /**
     * Radio Notification is a feature that enables ACTIVE and INACTIVE
     * (nACTIVE) signals from the stack that notify the application when the
     * radio is in use. The signal is sent using software interrupt.
     *
     * The ACTIVE signal is sent before the Radio Event starts. The nACTIVE
     * signal is sent at the end of the Radio Event. These signals can be used
     * by the application programmer to synchronize application logic with radio
     * activity. For example, the ACTIVE signal can be used to shut off external
     * devices to manage peak current drawn during periods when the radio is on,
     * or to trigger sensor data collection for transmission in the Radio Event.
     *
     * @param callback
     *          The application handler to be invoked in response to a radio
     *          ACTIVE/INACTIVE event.
     */
    void onRadioNotification(Gap::RadioNotificationEventCallback_t callback);

    /**
     * Add a service declaration to the local server ATT table. Also add the
     * characteristics contained within.
     */
    ble_error_t addService(GattService &service);

    /**
     * Returns the current GAP state of the device using a bitmask which
     * describes whether the device is advertising and/or connected.
     */
    Gap::GapState_t getGapState(void) const;

    /**
     * @param[in/out]  lengthP
     *     input:  Length in bytes to be read,
     *     output: Total length of attribute value upon successful return.
     */
    ble_error_t readCharacteristicValue(GattAttribute::Handle_t attributeHandle, uint8_t *buffer, uint16_t *lengthP);
    /**
     * A version of the same as above with connection handle parameter to allow fetches for connection-specific multivalued attribtues (such as the CCCDs).
     */
    ble_error_t readCharacteristicValue(Gap::Handle_t connectionHandle, GattAttribute::Handle_t attributeHandle, uint8_t *buffer, uint16_t *lengthP);

    /**
     * @param  localOnly
     *         Only update the characteristic locally regardless of notify/indicate flags in the CCCD.
     */
    ble_error_t updateCharacteristicValue(GattAttribute::Handle_t attributeHandle, const uint8_t *value, uint16_t size, bool localOnly = false);
    /**
     * A version of the same as above with connection handle parameter to allow updates for connection-specific multivalued attribtues (such as the CCCDs).
     */
    ble_error_t updateCharacteristicValue(Gap::Handle_t            connectionHandle,
                                          GattAttribute::Handle_t  attributeHandle,
                                          const uint8_t           *value,
                                          uint16_t                 size,
                                          bool                     localOnly = false);

    /**
     * Yield control to the BLE stack or to other tasks waiting for events. This
     * is a sleep function which will return when there is an application
     * specific interrupt, but the MCU might wake up several times before
     * returning (to service the stack). This is not always interchangeable with
     * WFE().
     */
    void waitForEvent(void);

    ble_error_t getPreferredConnectionParams(Gap::ConnectionParams_t *params);
    ble_error_t setPreferredConnectionParams(const Gap::ConnectionParams_t *params);
    ble_error_t updateConnectionParams(Gap::Handle_t handle, const Gap::ConnectionParams_t *params);

    /**
     * This call allows the application to get the BLE stack version information.
     *
     * @return  A pointer to a const string representing the version.
     *          Note: The string is owned by the BLE_API.
     */
    const char *getVersion(void);

    /**
     * Set the device name characteristic in the GAP service.
     * @param  deviceName The new value for the device-name. This is a UTF-8 encoded, <b>NULL-terminated</b> string.
     */
    ble_error_t setDeviceName(const uint8_t *deviceName);

    /**
     * Get the value of the device name characteristic in the GAP service.
     * @param[out]    deviceName Pointer to an empty buffer where the UTF-8 *non NULL-
     *                           terminated* string will be placed. Set this
     *                           value to NULL in order to obtain the deviceName-length
     *                           from the 'length' parameter.
     *
     * @param[in/out] lengthP    (on input) Length of the buffer pointed to by deviceName;
     *                           (on output) the complete device name length (without the
     *                           null terminator).
     *
     * @note          If the device name is longer than the size of the supplied buffer,
     *                length will return the complete device name length,
     *                and not the number of bytes actually returned in deviceName.
     *                The application may use this information to retry with a suitable buffer size.
     *
     * Sample use:
     *     uint8_t deviceName[20];
     *     unsigned length = sizeof(deviceName);
     *     ble.getDeviceName(deviceName, &length);
     *     if (length < sizeof(deviceName)) {
     *         deviceName[length] = 0;
     *     }
     *     DEBUG("length: %u, deviceName: %s\r\n", length, deviceName);
     */
    ble_error_t getDeviceName(uint8_t *deviceName, unsigned *lengthP);

    /**
     * Set the appearance characteristic in the GAP service.
     * @param[in]  appearance The new value for the device-appearance.
     */
    ble_error_t setAppearance(uint16_t appearance);

    /**
     * Set the appearance characteristic in the GAP service.
     * @param[out]  appearance The new value for the device-appearance.
     */
    ble_error_t getAppearance(uint16_t *appearanceP);

    /**
     * Set the radio's transmit power.
     * @param[in] txPower Radio transmit power in dBm.
     */
    ble_error_t setTxPower(int8_t txPower);

    /**
     * Query the underlying stack for permitted arguments for setTxPower().
     *
     * @param[out] valueArrayPP
     *                 Out parameter to receive the immutable array of Tx values.
     * @param[out] countP
     *                 Out parameter to receive the array's size.
     */
    void getPermittedTxPowerValues(const int8_t **valueArrayPP, size_t *countP);

    /**
     * Enable the BLE stack's Security Manager. The Security Manager implements
     * the actual cryptographic algorithms and protocol exchanges that allow two
     * devices to securely exchange data and privately detect each other.
     * Calling this API is a prerequisite for encryption and pairing (bonding).
     *
     * @param[in]  enableBonding Allow for bonding.
     * @param[in]  requireMITM   Require protection for man-in-the-middle attacks.
     * @param[in]  iocaps        To specify IO capabilities of this peripheral,
     *                           such as availability of a display or keyboard to
     *                           support out-of-band exchanges of security data.
     * @param[in]  passkey       To specify a static passkey.
     *
     * @return BLE_ERROR_NONE on success.
     */
    ble_error_t initializeSecurity(bool                          enableBonding = true,
                                   bool                          requireMITM   = true,
                                   Gap::SecurityIOCapabilities_t iocaps        = Gap::IO_CAPS_NONE,
                                   const Gap::Passkey_t          passkey       = NULL);

    /**
     * Setup a callback for when the security setup procedure (key generation
     * and exchange) for a link has started. This will be skipped for bonded
     * devices. The callback is passed in parameters received from the peer's
     * security request: bool allowBonding, bool requireMITM, and
     * SecurityIOCapabilities_t.
     */
    void onSecuritySetupInitiated(Gap::SecuritySetupInitiatedCallback_t callback);

    /**
     * Setup a callback for when the security setup procedure (key generation
     * and exchange) for a link has completed. This will be skipped for bonded
     * devices. The callback is passed in the success/failure status of the
     * security setup procedure.
     */
    void onSecuritySetupCompleted(Gap::SecuritySetupCompletedCallback_t callback);

    /**
     * Setup a callback for when a link with the peer is secured. For bonded
     * devices, subsequent reconnections with bonded peer will result only in
     * this callback when the link is secured and setup procedures will not
     * occur unless the bonding information is either lost or deleted on either
     * or both sides. The callback is passed in a Gap::SecurityMode_t according
     * to the level of security in effect for the secured link.
     */
    void onLinkSecured(Gap::LinkSecuredCallback_t callback);

    /**
     * Setup a callback for successful bonding; i.e. that link-specific security
     * context is stored persistently for a peer device.
     */
    void onSecurityContextStored(Gap::HandleSpecificEvent_t callback);

    /**
     * Setup a callback for when the passkey needs to be displayed on a
     * peripheral with DISPLAY capability. This happens when security is
     * configured to prevent Man-In-The-Middle attacks, and a PIN (or passkey)
     * needs to be exchanged between the peers to authenticate the connection
     * attempt.
     */
    void onPasskeyDisplay(Gap::PasskeyDisplayCallback_t callback);

    /**
     * Get the security status of a connection.
     *
     * @param[in]  connectionHandle   Handle to identify the connection.
     * @param[out] securityStatusP    security status.
     *
     * @return BLE_SUCCESS Or appropriate error code indicating reason for failure.
     */
    ble_error_t getLinkSecurity(Gap::Handle_t connectionHandle, Gap::LinkSecurityStatus_t *securityStatusP);

    /**
     * Delete all peer device context and all related bonding information from
     * the database within the security manager.
     *
     * @retval BLE_ERROR_NONE             On success, else an error code indicating reason for failure.
     * @retval BLE_ERROR_INVALID_STATE    If the API is called without module initialization and/or
     *                                    application registration.
     */
    ble_error_t purgeAllBondingState(void);

    /**
     * Launch service discovery. Once launched, service discovery will remain
     * active with callbacks being issued back into the application for matching
     * services/characteristics. isServiceDiscoveryActive() can be used to
     * determine status; and a termination callback (if setup) will be invoked
     * at the end. Service discovery can be terminated prematurely if needed
     * using terminateServiceDiscovery().
     *
     * @param  connectionHandle
     *           Handle for the connection with the peer.
     * @param  sc
     *           This is the application callback for matching service. Taken as
     *           NULL by default. Note: service discovery may still be active
     *           when this callback is issued; calling asynchronous BLE-stack
     *           APIs from within this application callback might cause the
     *           stack to abort service discovery. If this becomes an issue, it
     *           may be better to make local copy of the discoveredService and
     *           wait for service discovery to terminate before operating on the
     *           service.
     * @param  cc
     *           This is the application callback for matching characteristic.
     *           Taken as NULL by default. Note: service discovery may still be
     *           active when this callback is issued; calling asynchronous
     *           BLE-stack APIs from within this application callback might cause
     *           the stack to abort service discovery. If this becomes an issue,
     *           it may be better to make local copy of the discoveredCharacteristic
     *           and wait for service discovery to terminate before operating on the
     *           characteristic.
     * @param  matchingServiceUUID
     *           UUID based filter for specifying a service in which the application is
     *           interested. By default it is set as the wildcard UUID_UNKNOWN,
     *           in which case it matches all services. If characteristic-UUID
     *           filter (below) is set to the wildcard value, then a service
     *           callback will be invoked for the matching service (or for every
     *           service if the service filter is a wildcard).
     * @param  matchingCharacteristicUUIDIn
     *           UUID based filter for specifying characteristic in which the application
     *           is interested. By default it is set as the wildcard UUID_UKNOWN
     *           to match against any characteristic. If both service-UUID
     *           filter and characteristic-UUID filter are used with non- wildcard
     *           values, then only a single characteristic callback is
     *           invoked for the matching characteristic.
     *
     * @Note     Using wildcard values for both service-UUID and characteristic-
     *           UUID will result in complete service discovery--callbacks being
     *           called for every service and characteristic.
     *
     * @return
     *           BLE_ERROR_NONE if service discovery is launched successfully; else an appropriate error.
     */
    ble_error_t launchServiceDiscovery(Gap::Handle_t                               connectionHandle,
                                       ServiceDiscovery::ServiceCallback_t         sc = NULL,
                                       ServiceDiscovery::CharacteristicCallback_t  cc = NULL,
                                       const UUID                                 &matchingServiceUUID = UUID::ShortUUIDBytes_t(BLE_UUID_UNKNOWN),
                                       const UUID                                 &matchingCharacteristicUUIDIn = UUID::ShortUUIDBytes_t(BLE_UUID_UNKNOWN));

    /**
     * Setup callback for when serviceDiscovery terminates.
     */
    void onServiceDiscoveryTermination(ServiceDiscovery::TerminationCallback_t callback);

    /**
     * Is service-discovery currently active?
     */
    bool isServiceDiscoveryActive(void);

    /**
     * Terminate an ongoing service-discovery. This should result in an
     * invocation of the TerminationCallback if service-discovery is active.
     */
    void terminateServiceDiscovery(void);

public:
    BLE() : transport(createBLEInstance()) {
        /* empty */
    }

private:
    BLEInstanceBase *const transport; /* the device specific backend */
};

typedef BLE BLEDevice; /* DEPRECATED. This type alias is retained for the sake of compatibilty with older
                        * code. Will be dropped at some point soon.*/

/* BLE methods. Most of these simply forward the calls to the underlying
 * transport.*/

inline ble_error_t
BLE::reset(void)
{
    return transport->reset();
}

inline ble_error_t
BLE::shutdown(void)
{
    gap().advPayload().clear();
    return transport->shutdown();
}

inline ble_error_t
BLE::setAddress(Gap::AddressType_t type, const Gap::Address_t address)
{
    return gap().setAddress(type, address);
}

inline ble_error_t
BLE::getAddress(Gap::AddressType_t *typeP, Gap::Address_t address)
{
    return gap().getAddress(typeP, address);
}

inline void
BLE::setAdvertisingType(GapAdvertisingParams::AdvertisingType advType)
{
    gap().advParams().setAdvertisingType(advType);
}

inline void
BLE::setAdvertisingInterval(uint16_t interval)
{
    if (interval == 0) {
        stopAdvertising();
    } else if (interval < getMinAdvertisingInterval()) {
        interval = getMinAdvertisingInterval();
    }
    gap().advParams().setInterval(Gap::MSEC_TO_ADVERTISEMENT_DURATION_UNITS(interval));
}

inline uint16_t
BLE::getMinAdvertisingInterval(void) const {
    return gap().getMinAdvertisingInterval();
}

inline uint16_t
BLE::getMinNonConnectableAdvertisingInterval(void) const {
    return gap().getMinNonConnectableAdvertisingInterval();
}

inline uint16_t
BLE::getMaxAdvertisingInterval(void) const {
    return gap().getMaxAdvertisingInterval();
}

inline void
BLE::setAdvertisingTimeout(uint16_t timeout)
{
    gap().advParams().setTimeout(timeout);
}

inline void
BLE::setAdvertisingParams(const GapAdvertisingParams &newAdvParams)
{
    gap().setAdvParams(newAdvParams);
}

inline const GapAdvertisingParams &
BLE::getAdvertisingParams(void) const
{
    return gap().advParams();
}

inline void
BLE::clearAdvertisingPayload(void)
{
    gap().advPayload().clear();
}

inline ble_error_t
BLE::accumulateAdvertisingPayload(uint8_t flags)
{
    return gap().advPayload().addFlags(flags);
}

inline ble_error_t
BLE::accumulateAdvertisingPayload(GapAdvertisingData::Appearance app)
{
    gap().setAppearance(app);
    return gap().advPayload().addAppearance(app);
}

inline ble_error_t
BLE::accumulateAdvertisingPayloadTxPower(int8_t txPower)
{
    return gap().advPayload().addTxPower(txPower);
}

inline ble_error_t
BLE::accumulateAdvertisingPayload(GapAdvertisingData::DataType type, const uint8_t *data, uint8_t len)
{
    if (type == GapAdvertisingData::COMPLETE_LOCAL_NAME) {
        gap().setDeviceName(data);
    }
    return gap().advPayload().addData(type, data, len);
}

inline ble_error_t
BLE::accumulateScanResponse(GapAdvertisingData::DataType type, const uint8_t *data, uint8_t len)
{
    return gap().scanResponse().addData(type, data, len);
}

inline void
BLE::clearScanResponse(void)
{
    gap().scanResponse().clear();
}

inline ble_error_t
BLE::setAdvertisingPayload(void) {
    return gap().setAdvertisingData();
}

inline ble_error_t
BLE::setAdvertisingData(const GapAdvertisingData& newPayload)
{
    gap().advPayload() = newPayload;
    return setAdvertisingPayload();
}

inline const GapAdvertisingData &
BLE::getAdvertisingData(void) const {
    return gap().advPayload();
}

inline ble_error_t
BLE::startAdvertising(void)
{
    ble_error_t rc;
    if ((rc = transport->getGattServer().initializeGATTDatabase()) != BLE_ERROR_NONE) {
        return rc;
    }
    if ((rc = setAdvertisingPayload()) != BLE_ERROR_NONE) {
        return rc;
    }

    return gap().startAdvertising();
}

inline ble_error_t
BLE::stopAdvertising(void)
{
    return gap().stopAdvertising();
}

inline ble_error_t
BLE::setScanParams(uint16_t interval, uint16_t window, uint16_t timeout, bool activeScanning) {
    ble_error_t rc;
    if (((rc = gap().scanningParams().setInterval(interval)) == BLE_ERROR_NONE) &&
        ((rc = gap().scanningParams().setWindow(window))     == BLE_ERROR_NONE) &&
        ((rc = gap().scanningParams().setTimeout(timeout))   == BLE_ERROR_NONE)) {
        gap().scanningParams().setActiveScanning(activeScanning);
        return BLE_ERROR_NONE;
    }

    return rc;
}

inline ble_error_t
BLE::setScanInterval(uint16_t interval) {
    return gap().scanningParams().setInterval(interval);
}

inline ble_error_t
BLE::setScanWindow(uint16_t window) {
    return gap().scanningParams().setWindow(window);
}

inline ble_error_t
BLE::setScanTimeout(uint16_t timeout) {
    return gap().scanningParams().setTimeout(timeout);
}

inline void
BLE::setActiveScan(bool activeScanning) {
    return gap().scanningParams().setActiveScanning(activeScanning);
}

inline ble_error_t
BLE::startScan(void (*callback)(const Gap::AdvertisementCallbackParams_t *params)) {
    return gap().startScan(callback);
}

template<typename T>
inline ble_error_t
BLE::startScan(T *object, void (T::*memberCallback)(const Gap::AdvertisementCallbackParams_t *params)) {
    return gap().startScan(scanningParams, object, memberCallback);
}

inline ble_error_t
BLE::stopScan(void) {
    return gap().stopScan();
}

inline ble_error_t
BLE::connect(const Gap::Address_t           peerAddr,
             Gap::AddressType_t             peerAddrType,
             const Gap::ConnectionParams_t *connectionParams,
             const GapScanningParams       *scanParams) {
    return gap().connect(peerAddr, peerAddrType, connectionParams, scanParams);
}

inline ble_error_t
BLE::disconnect(Gap::DisconnectionReason_t reason)
{
    return gap().disconnect(reason);
}

inline void
BLE::onTimeout(Gap::EventCallback_t timeoutCallback)
{
    gap().setOnTimeout(timeoutCallback);
}

inline void
BLE::onConnection(Gap::ConnectionEventCallback_t connectionCallback)
{
    gap().setOnConnection(connectionCallback);
}

inline void
BLE::onDisconnection(Gap::DisconnectionEventCallback_t disconnectionCallback)
{
    gap().setOnDisconnection(disconnectionCallback);
}

template<typename T>
inline void
BLE::addToDisconnectionCallChain(T *tptr, void (T::*mptr)(void)) {
    gap().addToDisconnectionCallChain(tptr, mptr);
}

inline void
BLE::onDataSent(void (*callback)(unsigned count)) {
    transport->getGattServer().setOnDataSent(callback);
}

template <typename T> inline void
BLE::onDataSent(T *objPtr, void (T::*memberPtr)(unsigned count)) {
    transport->getGattServer().setOnDataSent(objPtr, memberPtr);
}

inline void
BLE::onDataWritten(void (*callback)(const GattWriteCallbackParams *eventDataP)) {
    transport->getGattServer().setOnDataWritten(callback);
}

template <typename T> inline void
BLE::onDataWritten(T *objPtr, void (T::*memberPtr)(const GattWriteCallbackParams *context)) {
    transport->getGattServer().setOnDataWritten(objPtr, memberPtr);
}

inline ble_error_t
BLE::onDataRead(void (*callback)(const GattReadCallbackParams *eventDataP)) {
    return transport->getGattServer().setOnDataRead(callback);
}

template <typename T> inline ble_error_t
BLE::onDataRead(T *objPtr, void (T::*memberPtr)(const GattReadCallbackParams *context)) {
    return transport->getGattServer().setOnDataRead(objPtr, memberPtr);
}

inline void
BLE::onUpdatesEnabled(GattServer::EventCallback_t callback)
{
    transport->getGattServer().setOnUpdatesEnabled(callback);
}

inline void
BLE::onUpdatesDisabled(GattServer::EventCallback_t callback)
{
    transport->getGattServer().setOnUpdatesDisabled(callback);
}

inline void
BLE::onConfirmationReceived(GattServer::EventCallback_t callback)
{
    transport->getGattServer().setOnConfirmationReceived(callback);
}

inline void
BLE::onRadioNotification(Gap::RadioNotificationEventCallback_t callback)
{
    gap().setOnRadioNotification(callback);
}

inline ble_error_t
BLE::addService(GattService &service)
{
    return transport->getGattServer().addService(service);
}

inline Gap::GapState_t
BLE::getGapState(void) const
{
    return gap().getState();
}

inline ble_error_t
BLE::readCharacteristicValue(GattAttribute::Handle_t attributeHandle, uint8_t *buffer, uint16_t *lengthP)
{
    return transport->getGattServer().readValue(attributeHandle, buffer, lengthP);
}

inline ble_error_t
BLE::readCharacteristicValue(Gap::Handle_t            connectionHandle,
                             GattAttribute::Handle_t  attributeHandle,
                             uint8_t                 *buffer,
                             uint16_t                *lengthP)
{
    return transport->getGattServer().readValue(connectionHandle, attributeHandle, buffer, lengthP);
}

inline ble_error_t
BLE::updateCharacteristicValue(GattAttribute::Handle_t attributeHandle, const uint8_t *value, uint16_t size, bool localOnly)
{
    return transport->getGattServer().updateValue(attributeHandle, const_cast<uint8_t *>(value), size, localOnly);
}

inline ble_error_t
BLE::updateCharacteristicValue(Gap::Handle_t            connectionHandle,
                               GattAttribute::Handle_t  attributeHandle,
                               const uint8_t           *value,
                               uint16_t                 size,
                               bool                     localOnly)
{
    return transport->getGattServer().updateValue(connectionHandle, attributeHandle, const_cast<uint8_t *>(value), size, localOnly);
}

inline void
BLE::waitForEvent(void)
{
    transport->waitForEvent();
}

inline ble_error_t
BLE::getPreferredConnectionParams(Gap::ConnectionParams_t *params)
{
    return gap().getPreferredConnectionParams(params);
}

inline ble_error_t
BLE::setPreferredConnectionParams(const Gap::ConnectionParams_t *params)
{
    return gap().setPreferredConnectionParams(params);
}

inline ble_error_t
BLE::updateConnectionParams(Gap::Handle_t handle, const Gap::ConnectionParams_t *params) {
    return gap().updateConnectionParams(handle, params);
}

inline const char *
BLE::getVersion(void)
{
    return transport->getVersion();
}

inline ble_error_t
BLE::setDeviceName(const uint8_t *deviceName)
{
    return gap().setDeviceName(deviceName);
}

inline ble_error_t
BLE::getDeviceName(uint8_t *deviceName, unsigned *lengthP)
{
    return gap().getDeviceName(deviceName, lengthP);
}

inline ble_error_t
BLE::setAppearance(uint16_t appearance)
{
    return gap().setAppearance(appearance);
}

inline ble_error_t
BLE::getAppearance(uint16_t *appearanceP)
{
    return gap().getAppearance(appearanceP);
}

inline ble_error_t
BLE::setTxPower(int8_t txPower)
{
    return gap().setTxPower(txPower);
}

inline void
BLE::getPermittedTxPowerValues(const int8_t **valueArrayPP, size_t *countP)
{
    gap().getPermittedTxPowerValues(valueArrayPP, countP);
}

inline ble_error_t
BLE::initializeSecurity(bool                          enableBonding,
                        bool                          requireMITM,
                        Gap::SecurityIOCapabilities_t iocaps,
                        const Gap::Passkey_t          passkey)
{
    return transport->initializeSecurity(enableBonding, requireMITM, iocaps, passkey);
}

inline void
BLE::onSecuritySetupInitiated(Gap::SecuritySetupInitiatedCallback_t callback)
{
    gap().setOnSecuritySetupInitiated(callback);
}

inline void
BLE::onSecuritySetupCompleted(Gap::SecuritySetupCompletedCallback_t callback)
{
    gap().setOnSecuritySetupCompleted(callback);
}

inline void
BLE::onLinkSecured(Gap::LinkSecuredCallback_t callback)
{
    gap().setOnLinkSecured(callback);
}

inline void
BLE::onSecurityContextStored(Gap::HandleSpecificEvent_t callback)
{
    gap().setOnSecurityContextStored(callback);
}

inline void
BLE::onPasskeyDisplay(Gap::PasskeyDisplayCallback_t callback)
{
    return gap().setOnPasskeyDisplay(callback);
}

inline ble_error_t
BLE::getLinkSecurity(Gap::Handle_t connectionHandle, Gap::LinkSecurityStatus_t *securityStatusP)
{
    return gap().getLinkSecurity(connectionHandle, securityStatusP);
}

inline ble_error_t
BLE::purgeAllBondingState(void)
{
    return gap().purgeAllBondingState();
}

inline ble_error_t
BLE::launchServiceDiscovery(Gap::Handle_t                               connectionHandle,
                            ServiceDiscovery::ServiceCallback_t         sc,
                            ServiceDiscovery::CharacteristicCallback_t  cc,
                            const UUID                                 &matchingServiceUUID,
                            const UUID                                 &matchingCharacteristicUUID)
{
    return transport->getGattClient().launchServiceDiscovery(connectionHandle, sc, cc, matchingServiceUUID, matchingCharacteristicUUID);
}

inline void
BLE::onServiceDiscoveryTermination(ServiceDiscovery::TerminationCallback_t callback)
{
    transport->getGattClient().onServiceDiscoveryTermination(callback);
}

/**
 * Is service-discovery currently active?
 */
inline bool
BLE::isServiceDiscoveryActive(void)
{
    return transport->getGattClient().isServiceDiscoveryActive();
}

/**
 * Terminate an ongoing service-discovery. This should result in an
 * invocation of the TerminationCallback if service-discovery is active.
 */
inline void
BLE::terminateServiceDiscovery(void)
{
    transport->getGattClient().terminateServiceDiscovery();
}

#endif // ifndef __BLE_H__
