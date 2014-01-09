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
  
#ifndef __NRF51822_H__
#define __NRF51822_H__

#include "mbed.h"
#include "blecommon.h"
#include "BLERadio.h"
#include "GattService.h"

/**************************************************************************/
/*!
    \brief
    Driver for the nRF51822 in connectivity mode using custom serialization
    firmware.
*/
/**************************************************************************/
class nRF51822 : public BLERadio
{
    public:
        nRF51822(PinName tx, PinName rx, PinName rts, PinName cts);
        virtual ~nRF51822(void);

        /* Functions that mus be implemented from BLERadio */
        virtual ble_error_t setAdvertising(GapAdvertisingParams &, GapAdvertisingData &, GapAdvertisingData &);
        virtual ble_error_t addService(GattService &);
        virtual ble_error_t readCharacteristic(uint8_t, uint8_t[], uint16_t);
        virtual ble_error_t writeCharacteristic(uint8_t, uint8_t[], uint16_t);
        virtual ble_error_t start(void);
        virtual ble_error_t stop(void);
        virtual ble_error_t reset(void);
        
    private:
        RawSerial uart;

        /* nRF51 Functions */
        void uartCallback(void);
};

#endif
