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
#include "hw/BLEDevice.h"
#include "hw/nRF51822s/nRF51Gap.h"
#include "hw/nRF51822s/nRF51GattServer.h"

/**************************************************************************/
/*!
    \brief

*/
/**************************************************************************/
class nRF51822s : public BLEDevice
{
    protected:
        RawSerial       uart;
        nRF51Gap        gap;
        nRF51GattServer gattServer;
    
    public:
        nRF51822s(PinName, PinName, PinName, PinName);
        virtual ~nRF51822s(void);

        virtual Gap&        getGap()        { return gap; };
        virtual GattServer& getGattServer() { return gattServer; };
        virtual ble_error_t reset(void);

    private:
        void uartCallback(void);
};

#endif
