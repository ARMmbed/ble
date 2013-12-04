#ifndef __NRF51822_H__
#define __NRF51822_H__

#include "mbed.h"
#include "blecommon.h"
#include "bleservice.h"
#include "bleradio.h"

class nRF51822 : public BLERadio
{
    public:
        nRF51822();
        virtual ~nRF51822(void);

        /* Functions that mus be implemented from NRFRadio */
        //virtual ble_error_t attach(void (*fptr)(void));
        virtual ble_error_t addService(BLEService &);
        virtual ble_error_t updateValue(uint8_t, uint8_t, uint8_t[], uint16_t);
        virtual ble_error_t start(void);
        virtual ble_error_t stop(void);
        virtual ble_error_t reset(void);
        
        /* nRF51 Functions */
        void uartCallback(void);

    private:
        Serial uart;        
};

#endif
