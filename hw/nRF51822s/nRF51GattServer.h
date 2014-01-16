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
  
#ifndef __NRF51822_GATT_SERVER_H__
#define __NRF51822_GATT_SERVER_H__

#include "mbed.h"
#include "blecommon.h"
#include "GattService.h"
#include "hw/GattServer.h"

/**************************************************************************/
/*!
    \brief

*/
/**************************************************************************/
class nRF51GattServer : public GattServer
{        
    public:
        nRF51GattServer(RawSerial &);
        virtual ~nRF51GattServer(void);

        /* Functions that must be implemented from GattServer */
        virtual ble_error_t addService(GattService &);
        virtual ble_error_t readValue(uint8_t, uint8_t[], uint16_t);
        virtual ble_error_t updateValue(uint8_t, uint8_t[], uint16_t);
        
        /* nRF51 Functions */
        void uartCallback(void);
        
    private:
        RawSerial& uart;
};

#endif
