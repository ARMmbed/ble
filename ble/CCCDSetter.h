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

#include "FunctionPointerWithContext.h"
#include "GattClient.h"
#include "GattAttribute.h"
#include "DiscoveredCharacteristic.h"
#include "CharacteristicDescriptorDiscovery.h"

#ifndef __CCCDSETTER_H__
#define __CCCDSETTER_H__

/**
 * This class encapsulates the data needed to:
 * - Discover Characteristic Descriptors
 * - Identify the Client Configuration Characteristic Descriptor (CCCD) handle.
 * - Intitiate the write procedure to the CCCD.
 *
 * The object can be instanciated by using the launch() function with the
 * required arguments. The user is able to register a write callback that will
 * be executed at the end of the CCCD write procedure with the final outcome of
 * the operation.
 *
 * @note This object will automatically release any resources acquired during
 *       its lifetime.
 */
class CCCDSetter {
public:
    /**
     * Create a new instance of the object and launch the Characteristic
     * Descriptor Discovery procedure to find the CCCD handle.
     *
     * @param[in] client
     *              A pointer to the GattClient used for the operation.
     * @param[in] characteristic
     *              The characteristic that contains the CCCD to write.
     * @param[in] callback
     *              The callback executed at the end of the CCCD write procedure.
     * @param[in] cccdData
     *              The new data to write to the CCCD.
     */
    static void launch(GattClient* client,
                       const DiscoveredCharacteristic &characteristic,
                       const GattClient::WriteCallback_t& callback,
                       uint16_t cccdData) {
        CCCDSetter* cccdSetterPtr = new CCCDSetter(client, characteristic, callback, cccdData);
        cccdSetterPtr->attach();
    }

private:
    /**
     * Construct a new instance of this object.
     *
     * @param[in] client
     *              A pointer to the GattClient used for the operation.
     * @param[in] characteristic
     *              The characteristic that contains the CCCD to write.
     * @param[in] callback
     *              The callback executed at the end of the CCCD write procedure.
     * @param[in] cccdData
     *              The new data to write to the CCCD.
     */
    CCCDSetter(GattClient* client,
               const DiscoveredCharacteristic &characteristic,
               const GattClient::WriteCallback_t& callback,
               uint16_t cccdData) :
        _client(client),
        _characteristic(characteristic),
        _callback(callback),
        _cccdData(cccdData),
        _cccdFound(false) { }

    /**
     * Start the Characteristic Descriptor Discovery procedure to find the CCCD
     * handle and register callbacks for descriptor discovered and descriptor
     * discovery termination events.
     */
    void attach(void) {
        /* Launch descriptor discovery procedure */
        FunctionPointerWithContext<const CharacteristicDescriptorDiscovery::DiscoveryCallbackParams_t*> descriptorDiscoveredCallback(this, &CCCDSetter::descriptorDiscovered);
        FunctionPointerWithContext<const CharacteristicDescriptorDiscovery::TerminationCallbackParams_t*> descriptorDiscoveryTerminationCallback(this, &CCCDSetter::descriptorDiscoveryTermination);
        _characteristic.discoverDescriptors(descriptorDiscoveredCallback, descriptorDiscoveryTerminationCallback);
    }

    /**
     * Callback that will be executed when a characteristic descriptor discovery
     * termination event occurs. If the CCCD was not previously found, then this
     * callback will call the user-registered write callback with failure values.
     *
     * @note This callback release all resources previously acquired by this
     *       object.
     */
    void descriptorDiscoveryTermination(
        const CharacteristicDescriptorDiscovery::TerminationCallbackParams_t *) {
        /*
         * We do not check params->status because we really only care that the
         * CCCD is found and that the write procedure succeeded/failed and not
         * the discovery procedure
         */
        if (_cccdFound) {
            /* The CCCD was found and write was triggered */
            delete this;
            return;
        }

        /* The CCCD was not found post a callback to the user to report failure */
        GattWriteCallbackParams callbackParams;
        callbackParams.connHandle = _characteristic.getConnectionHandle();
        callbackParams.handle     = GattAttribute::INVALID_HANDLE;
        callbackParams.writeOp    = GattWriteCallbackParams::OP_INVALID;
        callbackParams.offset     = 0;
        callbackParams.len        = 0;
        callbackParams.data       = NULL;
        _callback.call(&callbackParams);

        delete this;
    }

    /**
     * Callback that will be executed when a characteristic descriptor is
     * discovered. If the descriptor is the CCCD, then the characteristic
     * descriptor discovery procedure is terminated and a write procedure is
     * started to update the CCCD.
     *
     * @param[in] params
     *              Information about the discovered characteristric descriptor.
     */
    void descriptorDiscovered(
        const CharacteristicDescriptorDiscovery::DiscoveryCallbackParams_t *params) {
        UUID cccdUuid(BLE_UUID_DESCRIPTOR_CLIENT_CHAR_CONFIG);
        if (params->descriptor.getUUID() == cccdUuid) {
            /* This is the CCCD that we need to write */
            /* Update setter config information */
            _cccdFound = true;
            /* Terminate the characteristic descriptor discovery */
            _client->terminateCharacteristicDescriptorDiscovery(params->characteristic);
            /*
             * Write the descriptor with the new value, this will trigger a
             * callback to user code that will report the final status of the write
             */
            ble_error_t error = _client->write(
                GattClient::GATT_OP_WRITE_REQ,
                _characteristic.getConnectionHandle(),
                params->descriptor.getAttributeHandle(),
                sizeof(CCCDSetter::_cccdData),
                (const uint8_t*) &_cccdData
            );
            if (error != BLE_ERROR_NONE) {
                /* Report the error and terminate */
                GattWriteCallbackParams callbackParams;
                callbackParams.connHandle = _characteristic.getConnectionHandle();
                callbackParams.handle     = params->descriptor.getAttributeHandle();
                callbackParams.writeOp    = GattWriteCallbackParams::OP_WRITE_REQ;
                callbackParams.offset     = 0;
                callbackParams.len        = 0;
                callbackParams.data       = NULL;
                _callback.call(&callbackParams);
            }
        }
    }

    /**
     * The GattClient instance that created this object.
     */
    GattClient*                 _client;
    /**
     * The discovered characteristic that contains the CCCD to write.
     */
    DiscoveredCharacteristic    _characteristic;
    /**
     * The registered callback that will be executed when the write procedure
     * terminates.
     */
    GattClient::WriteCallback_t _callback;
    /**
     * The data to write to the CCCD.
     */
    uint16_t                    _cccdData;
    /**
     * Flag when the CCCD has been found.
     */
    bool                        _cccdFound;
};

#endif /* __CCCDSETTER_H__ */
