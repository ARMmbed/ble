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

class OneShotWriteCallback {
public:
    static void launch(GattClient* client, Gap::Handle_t connHandle,
                       GattAttribute::Handle_t handle, const GattClient::WriteCallback_t& cb) {
        OneShotWriteCallback* oneShot = new OneShotWriteCallback(client, connHandle, handle, cb);
        oneShot->attach();
        // delete will be made when this callback is called
    }

private:
    OneShotWriteCallback(GattClient* client, Gap::Handle_t connHandle,
                        GattAttribute::Handle_t handle, const GattClient::WriteCallback_t& cb) :
        _client(client),
        _connHandle(connHandle),
        _handle(handle),
        _callback(cb) { }

    void attach() {
        _client->onDataWritten(makeFunctionPointer(this, &OneShotWriteCallback::call));
        _client->onShutdown(this, &OneShotWriteCallback::shutdownCallback);
        BLE::Instance().gap().onDisconnection(this, &OneShotWriteCallback::disconnectionCallback);
    }

    void shutdownCallback(const GattClient *client) {
        /* Avoid compiler warnings */
        (void) client;
        /*
         * Free resources allocated to this object. Pending write callbacks
         * will no longer be executed.
         */
        delete this;
    }

    void disconnectionCallback(const Gap::DisconnectionCallbackParams_t *params) {
        /* Avoid compiler warnings */
        (void) params;
        /*
         * Free resources allocated to this object. Pending write callbacks
         * will no longer be executed.
         */
        _client->onDataWritten().detach(makeFunctionPointer(this, &OneShotWriteCallback::call));
        _client->onShutdown().detach(makeFunctionPointer(this, &OneShotWriteCallback::shutdownCallback));
        BLE::Instance().gap().onDisconnection().detach(makeFunctionPointer(this, &OneShotWriteCallback::disconnectionCallback));
        delete this;
    }

    void call(const GattWriteCallbackParams* params) {
        // verifiy that it is the right characteristic on the right connection
        if (params->connHandle == _connHandle && params->handle == _handle) {
            _callback(params);
            _client->onDataWritten().detach(makeFunctionPointer(this, &OneShotWriteCallback::call));
            _client->onShutdown().detach(makeFunctionPointer(this, &OneShotWriteCallback::shutdownCallback));
            BLE::Instance().gap().onDisconnection().detach(makeFunctionPointer(this, &OneShotWriteCallback::disconnectionCallback));
            delete this;
        }
    }

    GattClient* _client;
    Gap::Handle_t _connHandle;
    GattAttribute::Handle_t _handle;
    GattClient::WriteCallback_t _callback;
};

#endif /* __ONESHOTWRITECALLBACK_H__ */
