#ifndef __BLE_RADIO_H__
#define __BLE_RADIO_H__

#include "blecommon.h"
#include "GattService.h"
#include "GapAdvertisingData.h"
#include "GapAdvertisingParams.h"

class BLERadio
{
    protected:
        FunctionPointer _callback_event;
        
    public:
        typedef enum radio_event_e
        {
            RADIO_EVENT_CONNECT     = 0x01,
            RADIO_EVENT_DISCONNECT  = 0x02,
            RADIO_EVENT_WRITE       = 0x03,
            RADIO_EVENT_RADIOERROR  = 0x80
        } radioEvent_t;
        
        uint8_t serviceCount;
        
        /* ToDo: Force constructor with event handler callback */

        /* These functions must be defined in the sub-class */
        virtual ble_error_t setAdvertising(GapAdvertisingParams &, GapAdvertisingData &, GapAdvertisingData &) = 0;
        virtual ble_error_t addService(GattService &) = 0;
        virtual ble_error_t readCharacteristic(GattService &, GattCharacteristic &, uint8_t[], uint16_t) = 0;
        virtual ble_error_t writeCharacteristic(GattService &, GattCharacteristic &, uint8_t[], uint16_t) = 0;
        virtual ble_error_t start(void) = 0;
        virtual ble_error_t stop(void) = 0;
        virtual ble_error_t reset(void) = 0;

        /* BLE event callback (connect, disconnect, etc.) */
        void attach(void (*function)(void)) { 
            _callback_event.attach( function ); 
        }
 
        template<typename T>
        void attach(T *object, void (T::*member)(void)) { 
            _callback_event.attach( object, member );
        }
};

#endif
