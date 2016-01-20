#include "FunctionPointerWithContext.h"
#include "GattClient.h"
#include "GattAttribute.h"
#include "DiscoveredCharacteristic.h"
#include "CharacteristicDescriptorDiscovery.h"

#ifndef __CCCDSETTER_H__
#define __CCCDSETTER_H__

class CCCDSetter {
public:
    static void launch(GattClient* client,
                       const DiscoveredCharacteristic &characteristic,
                       const GattClient::WriteCallback_t& callback,
                       uint16_t cccdData) {
        CCCDSetter* cccdSetterPtr = new CCCDSetter(client, characteristic, callback, cccdData);
        cccdSetterPtr->attach();
    }

private:
    CCCDSetter(GattClient* client,
               const DiscoveredCharacteristic &characteristic,
               const GattClient::WriteCallback_t& callback,
               uint16_t cccdData) :
        _client(client),
        _characteristic(characteristic),
        _callback(callback),
        _cccdData(cccdData),
        _cccdFound(false) { }

    void attach(void) {
        /* Launch descriptor discovery procedure */
        FunctionPointerWithContext<const CharacteristicDescriptorDiscovery::DiscoveryCallbackParams_t*> descriptorDiscoveredCallback(this, &CCCDSetter::descriptorDiscovered);
        FunctionPointerWithContext<const CharacteristicDescriptorDiscovery::TerminationCallbackParams_t*> descriptorDiscoveryTerminationCallback(this, &CCCDSetter::descriptorDiscoveryTermination);
        _characteristic.discoverDescriptors(descriptorDiscoveredCallback, descriptorDiscoveryTerminationCallback);
    }

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

    GattClient*                 _client;
    DiscoveredCharacteristic    _characteristic;
    GattClient::WriteCallback_t _callback;
    uint16_t                    _cccdData;
    bool                        _cccdFound;
};

#endif /* __CCCDSETTER_H__ */
