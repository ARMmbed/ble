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

#include "BLE.h"
#include "GattClient.h"
#include "Gap.h"
#include "GattAttribute.h"

#ifndef __ONESHOTWRITECALLBACK_H__
#define __ONESHOTWRITECALLBACK_H__

/**
 * This class encapsulates the data needed to keep track a write procedure to
 * a GATT Attribute in the peer's ATT table. The object can be instanciated by
 * using the launch() function with the required arguments. The user is able to
 * register a write callback that will be executed at the end of the write
 * procedure with the final outcome of the operation.
 *
 * @note This object will automatically release any resources acquired during
 *       its lifetime.
 */
class OneShotWriteCallback {
public:
    /**
     * Create a new instance of the object and launch the write procedure with
     * the specified arguments.
     *
     * @param[in] client
     *              A pointer to the GattClient used to trigger the write
     *              procedure.
     * @param[in] connHandle
     *              The ID of the connection to execute the write procedure.
     * @param[in] handle
     *              The attribute handle of the value written in the peer's
     *              ATT table.
     * @param[in] cb
     *              The callback executed at the end of the write procedure.
     */
    static void launch(GattClient* client, Gap::Handle_t connHandle,
                       GattAttribute::Handle_t handle, const GattClient::WriteCallback_t& cb) {
        OneShotWriteCallback* oneShot = new OneShotWriteCallback(client, connHandle, handle, cb);
        oneShot->attach();
        /* Delete will be made when this callback is called */
    }

private:
    /**
     * Construct a new instance of this object.
     *
     * @param[in] client
     *              A pointer to the GattClient used to trigger the write
     *              procedure.
     * @param[in] connHandle
     *              The ID of the connection to execute the write procedure.
     * @param[in] handle
     *              The attribute handle of the value written in the peer's
     *              ATT table.
     * @param[in] cb
     *              The callback executed at the end of the write procedure.
     */
    OneShotWriteCallback(GattClient* client, Gap::Handle_t connHandle,
                        GattAttribute::Handle_t handle, const GattClient::WriteCallback_t& cb) :
        _client(client),
        _connHandle(connHandle),
        _handle(handle),
        _callback(cb) { }

    /**
     * Attach callbacks to the GattClient and Gap to receive shutdown, data
     * written and disconnection events.
     */
    void attach(void) {
        _client->onDataWritten(makeFunctionPointer(this, &OneShotWriteCallback::call));
        _client->onShutdown(this, &OneShotWriteCallback::shutdownCallback);
        BLE::Instance().gap().onDisconnection(this, &OneShotWriteCallback::disconnectionCallback);
    }

    /**
     * This callback will be executed in case the user calls BLE::shutdown()
     * before the write procedure completes. In this case, the call() callback
     * will never be executed, but the resources acquired by this object need
     * to be released.
     *
     * @param[in] client
     *              A pointer to the GattClient used to trigger the write
     *              procedure.
     */
    void shutdownCallback(const GattClient *client) {
        /* Avoid compiler warnings */
        (void) client;

        /*
         * Free resources allocated to this object. Pending write callbacks
         * will no longer be executed.
         */
        delete this;
    }

    /**
     * This callback will be executed when a disconnection even occurs before
     * the write procedure completes. In this case, the call() callback
     * will never be executed, but the resources acquired by this object need
     * to be released.
     *
     * @param[in] params
     *              The disconnection parameters.
     */
    void disconnectionCallback(const Gap::DisconnectionCallbackParams_t *params) {
        if (params->handle == _connHandle) {
            /* Tell the user that the operation failed */
            GattWriteCallbackParams callbackParams;
            callbackParams.connHandle = _connHandle;
            callbackParams.handle     = _handle;
            callbackParams.writeOp    = GattWriteCallbackParams::OP_INVALID;
            callbackParams.offset     = 0;
            callbackParams.len        = 0;
            callbackParams.data       = NULL;
            _callback(&callbackParams);

            /*
             * Free resources allocated to this object. Pending write callbacks
             * will no longer be executed.
             */
            _client->onDataWritten().detach(makeFunctionPointer(this, &OneShotWriteCallback::call));
            _client->onShutdown().detach(makeFunctionPointer(this, &OneShotWriteCallback::shutdownCallback));
            BLE::Instance().gap().onDisconnection().detach(makeFunctionPointer(this, &OneShotWriteCallback::disconnectionCallback));
            delete this;
        }
    }

    /**
     * Registered callback that will be executed at the end of the write
     * procedure. If this callback is called with the correct connection and
     * attribute handles, then it will do the following:
     * - Call the user-registered write callback.
     * - Detach its callbacks registered for disconnection, shutdown and
     *   data written events.
     * - Release its resources.
     *
     * @param[in] param
     *              Parameters specifying the result of the write operation.
     */
    void call(const GattWriteCallbackParams* params) {
        /* Verifiy that it is the right characteristic on the right connection */
        if (params->connHandle == _connHandle && params->handle == _handle) {
            _callback(params);
            _client->onDataWritten().detach(makeFunctionPointer(this, &OneShotWriteCallback::call));
            _client->onShutdown().detach(makeFunctionPointer(this, &OneShotWriteCallback::shutdownCallback));
            BLE::Instance().gap().onDisconnection().detach(makeFunctionPointer(this, &OneShotWriteCallback::disconnectionCallback));
            delete this;
        }
    }

    /**
     * The GattClient instance that created this object.
     */
    GattClient* _client;
    /**
     * The connection ID that used to execute the write procedure.
     */
    Gap::Handle_t _connHandle;
    /**
     * The attribute handle of the value written in the peer's ATT table.
     */
    GattAttribute::Handle_t _handle;
    /**
     * The registered callback that will be executed when the write procedure
     * terminates.
     */
    GattClient::WriteCallback_t _callback;
};

#endif /* __ONESHOTWRITECALLBACK_H__ */
