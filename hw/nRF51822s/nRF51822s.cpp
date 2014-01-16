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
  
#include "mbed.h"
#include "nRF51822s.h"

/**************************************************************************/
/*!
    @brief  UART callback function
*/
/**************************************************************************/
void nRF51822s::uartCallback(void)
{
    /* ToDo: Check responses and set a flag for success/error/etc. */
    
    /* Read serial to clear the RX interrupt */
    uart.getc();
}

/**************************************************************************/
/*!
    @brief  Constructor
*/
/**************************************************************************/
nRF51822s::nRF51822s(PinName tx, PinName rx, PinName rts, PinName cts) : uart(tx, rx), gap(uart), gattServer(uart)
{
    /* Setup the nRF UART interface */
    uart.baud(9600);

    /* Echo data on the debug CDC port */
    uart.attach(this, &nRF51822s::uartCallback);
    
    /* Add flow control for UART (required by the nRF51822) */
    uart.set_flow_control(RawSerial::RTSCTS, rts, cts);
}

/**************************************************************************/
/*!
    @brief  Destructor
*/
/**************************************************************************/
nRF51822s::~nRF51822s(void)
{
}

/**************************************************************************/
/*!
    @brief  Resets the BLE HW, removing any existing services and
            characteristics
            
    @returns    ble_error_t
    
    @retval     BLE_ERROR_NONE
                Everything executed properly
                
    @section EXAMPLE

    @code

    @endcode
*/
/**************************************************************************/
ble_error_t nRF51822s::reset(void)
{
    wait(0.5);
    
    /* Command ID = 0x0005, No payload */
    uart.printf("10 05 00 00\r\n");

    /* Reset the service and characteristic counters */
    //serviceCount = 0;
    //characteristicCount = 0;

    /* Wait for the radio to come back up */    
    wait(1);
    
    return BLE_ERROR_NONE;
}
