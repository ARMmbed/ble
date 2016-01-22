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

#include "GattClient.h"
#include "Gap.h"
#include "GattAttribute.h"

#ifndef __ONESHOTREADCALLBACK_H__
#define __ONESHOTREADCALLBACK_H__

class OneShotReadCallback {
public:
    static void launch(GattClient* client, Gap::Handle_t connHandle,
                       GattAttribute::Handle_t handle, const GattClient::ReadCallback_t& cb) {
        OneShotReadCallback* oneShot = new OneShotReadCallback(client, connHandle, handle, cb);
        oneShot->attach();
        // delete will be made when this callback is called
    }

private:
    OneShotReadCallback(GattClient* client, Gap::Handle_t connHandle,
                        GattAttribute::Handle_t handle, const GattClient::ReadCallback_t& cb) :
        _client(client),
        _connHandle(connHandle),
        _handle(handle),
        _callback(cb) { }

    void attach() {
        _client->onDataRead(makeFunctionPointer(this, &OneShotReadCallback::call));
    }

    void call(const GattReadCallbackParams* params) {
        // verifiy that it is the right characteristic on the right connection
        if (params->connHandle == _connHandle && params->handle == _handle) {
            _callback(params);
            _client->onDataRead().detach(makeFunctionPointer(this, &OneShotReadCallback::call));
            delete this;
        }
    }

    GattClient* _client;
    Gap::Handle_t _connHandle;
    GattAttribute::Handle_t _handle;
    GattClient::ReadCallback_t _callback;
};

#endif /* __ONESHOTREADCALLBACK_H__ */
