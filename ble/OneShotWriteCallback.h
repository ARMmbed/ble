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
    }

    void call(const GattWriteCallbackParams* params) {
        // verifiy that it is the right characteristic on the right connection
        if (params->connHandle == _connHandle && params->handle == _handle) {
            _callback(params);
            _client->onDataWritten().detach(makeFunctionPointer(this, &OneShotWriteCallback::call));
            delete this;
        }
    }

    GattClient* _client;
    Gap::Handle_t _connHandle;
    GattAttribute::Handle_t _handle;
    GattClient::WriteCallback_t _callback;
};

#endif /* __ONESHOTWRITECALLBACK_H__ */
