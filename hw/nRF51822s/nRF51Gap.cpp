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
  
#include "nRF51Gap.h"
#include "mbed.h"

/**************************************************************************/
/*!
    @brief  Constructor
*/
/**************************************************************************/
nRF51Gap::nRF51Gap(RawSerial &serial) : Gap(), uart(serial)
{
    /* ToDo: Reset the service and characteristic counters */
    /* ToDo: Set state variable to an appropriate value */
}

/**************************************************************************/
/*!
    @brief  Destructor
*/
/**************************************************************************/
nRF51Gap::~nRF51Gap(void)
{
}

/**************************************************************************/
/*!
    @brief  Sets the advertising parameters and payload for the device

    @param[in]  params
                Basic advertising details, including the advertising
                delay, timeout and how the device should be advertised
    @params[in] advData
                The primary advertising data payload
    @params[in] scanResponse
                The optional Scan Response payload if the advertising
                type is set to \ref GapAdvertisingParams::ADV_SCANNABLE_UNDIRECTED
                in \ref GapAdveritinngParams
            
    @returns    \ref ble_error_t
    
    @retval     BLE_ERROR_NONE
                Everything executed properly

    @retval     BLE_ERROR_BUFFER_OVERFLOW
                The proposed action would cause a buffer overflow.  All
                advertising payloads must be <= 31 bytes, for example.
                
    @retval     BLE_ERROR_NOT_IMPLEMENTED
                A feature was requested that is not yet supported in the
                nRF51 firmware or hardware.

    @retval     BLE_ERROR_PARAM_OUT_OF_RANGE
                One of the proposed values is outside the valid range.

    @section EXAMPLE

    @code

    @endcode
*/
/**************************************************************************/
ble_error_t nRF51Gap::setAdvertising(GapAdvertisingParams & params, GapAdvertisingData & advData, GapAdvertisingData & scanResponse)
{
    uint8_t len = 0;
    uint8_t *buffer;
    
    /* Make sure we support the advertising type */
    if (params.getAdvertisingType() == GapAdvertisingParams::ADV_CONNECTABLE_DIRECTED)
    {
        /* ToDo: This requires a propery security implementation, etc. */
        return BLE_ERROR_NOT_IMPLEMENTED;
    }
    
    /* Check interval range */
    if (params.getAdvertisingType() == GapAdvertisingParams::ADV_NON_CONNECTABLE_UNDIRECTED)
    {
        /* Min delay is slightly longer for unconnectable devices */
        if ((params.getInterval() < GAP_ADV_PARAMS_INTERVAL_MIN_NONCON) ||
            (params.getInterval() > GAP_ADV_PARAMS_INTERVAL_MAX))
        {
            return BLE_ERROR_PARAM_OUT_OF_RANGE;
        }
    }
    else
    {
        if ((params.getInterval() < GAP_ADV_PARAMS_INTERVAL_MIN) ||
            (params.getInterval() > GAP_ADV_PARAMS_INTERVAL_MAX))
        {
            return BLE_ERROR_PARAM_OUT_OF_RANGE;
        }
    }
    
    /* Check timeout is zero for Connectable Directed */
    if ((params.getAdvertisingType() == GapAdvertisingParams::ADV_CONNECTABLE_DIRECTED) &&
        (params.getTimeout() != 0))
    {
        /* Timeout must be 0 with this type, although we'll never get here */
        /* since this isn't implemented yet anyway */
        return BLE_ERROR_PARAM_OUT_OF_RANGE;
    }
    
    /* Check timeout for other advertising types */
    if ((params.getAdvertisingType() != GapAdvertisingParams::ADV_CONNECTABLE_DIRECTED) &&
        (params.getTimeout() > GAP_ADV_PARAMS_TIMEOUT_MAX))
    {
        return BLE_ERROR_PARAM_OUT_OF_RANGE;
    }

    /* Make sure we don't exceed the advertising payload length */
    if (advData.getPayloadLen() > GAP_ADVERTISING_DATA_MAX_PAYLOAD)
    {
        return BLE_ERROR_BUFFER_OVERFLOW;
    }
    
    /* Check the scan response payload limits */
    if ((params.getAdvertisingType() == GapAdvertisingParams::ADV_SCANNABLE_UNDIRECTED))
    {
        /* Check if we're within the upper limit */
        if (advData.getPayloadLen() > GAP_ADVERTISING_DATA_MAX_PAYLOAD)
        {
            return BLE_ERROR_BUFFER_OVERFLOW;
        }
        /* Make sure we have a payload! */
        if (advData.getPayloadLen() == 0)
        {
            return BLE_ERROR_PARAM_OUT_OF_RANGE;
        }
    }
    
    /* ToDo: Perform some checks on the payload, for example the Scan Response can't */
    /* contains a flags AD type, etc. */

    /* ToDo: Refactor these actions into separate private functions */

    /* 1.) Send advertising params, Command IDs = 0x000C, 0x000D, 0x000E */
    /* A.) Command ID = 0x000C, Advertising Interval, uint16_t */
    uart.printf("10 0C 00 02 %02X %02X\r\n", (uint8_t)(params.getInterval() & 0xFF),
                                             (uint8_t)(params.getInterval() >> 8));
    /* ToDo: Check response */
    wait(0.5);
    
    /* B.) Command ID = 0x000D, Advertising Timeout, uint16_t */
    uart.printf("10 0D 00 02 %02X %02X\r\n", (uint8_t)(params.getTimeout() & 0xFF),
                                             (uint8_t)(params.getTimeout() >> 8));
    /* ToDo: Check response */
    wait(0.5);
    
    /* C.) Command ID = 0x000E, Advertising Type, uint8_t */
    uart.printf("10 0E 00 01 %02X\r\n", (uint8_t)(params.getAdvertisingType()));
    /* ToDo: Check response */
    wait(0.5);    

    /* 2.) Send advertising data, Command ID = 0x000A */
    len = advData.getPayloadLen();
    buffer = advData.getPayload();
    uart.printf("10 0A 00 %02X", len);
    for (uint16_t i = 0; i < len; i++)
    {
        uart.printf(" %02X", buffer[i]);
    }
    uart.printf("\r\n");
    
    /* ToDo: Check response */
    wait(0.1);

    /* 3.) Send scan response data, Command ID = 0x000B */
    if ((params.getAdvertisingType() == GapAdvertisingParams::ADV_SCANNABLE_UNDIRECTED))
    {
        len = scanResponse.getPayloadLen();
        buffer = scanResponse.getPayload();
        uart.printf("10 0B 00 %02X", len);
        for (uint16_t i = 0; i < len; i++)
        {
            uart.printf(" %02X", buffer[i]);
        }
        uart.printf("\r\n");

        /* ToDo: Check response */
        wait(0.1);
    }
    
    return BLE_ERROR_NONE;
}

/**************************************************************************/
/*!
    @brief  Starts the BLE HW, initialising any services that were
            added before this function was called.
            
    @note   All services must be added before calling this function!
            
    @returns    ble_error_t
    
    @retval     BLE_ERROR_NONE
                Everything executed properly
                
    @section EXAMPLE

    @code

    @endcode
*/
/**************************************************************************/
ble_error_t nRF51Gap::startAdvertising(void)
{
    /* Command ID = 0x0003, No payload */
    uart.printf("10 03 00 00\r\n");

    /* ToDo: Check response */
    wait(0.5);

    return BLE_ERROR_NONE;
}

/**************************************************************************/
/*!
    @brief  Stops the BLE HW and disconnects from any devices
            
    @returns    ble_error_t
    
    @retval     BLE_ERROR_NONE
                Everything executed properly
                
    @section EXAMPLE

    @code

    @endcode
*/
/**************************************************************************/
ble_error_t nRF51Gap::stopAdvertising(void)
{
    /* Command ID = 0x0004, No payload */
    uart.printf("10 04 00 00\r\n");

    /* ToDo: Check response */
    wait(0.1);

    return BLE_ERROR_NONE;
}
