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
