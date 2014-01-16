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
  
#include "nRF51GattServer.h"
#include "mbed.h"

/* ToDo: Convert to Singleton! */

/**************************************************************************/
/*!
    @brief  Constructor
*/
/**************************************************************************/
nRF51GattServer::nRF51GattServer(RawSerial &serial): GattServer(), uart(serial)
{
    /* Reset the service and characteristic counters */
    serviceCount = 0;
    characteristicCount = 0;
}

/**************************************************************************/
/*!
    @brief  Destructor
*/
/**************************************************************************/
nRF51GattServer::~nRF51GattServer(void)
{
}

/**************************************************************************/
/*!
    @brief  Adds a new service to the GATT table on the peripheral
            
    @returns    ble_error_t
    
    @retval     BLE_ERROR_NONE
                Everything executed properly
                
    @section EXAMPLE

    @code

    @endcode
*/
/**************************************************************************/
ble_error_t nRF51GattServer::addService(GattService & service)
{
    /* ToDo: Make sure we don't overflow the array, etc. */
    /* ToDo: Make sure this service UUID doesn't already exist (?) */
    /* ToDo: Basic validation */
    
    /* Add the service to the nRF51 */
    if (service.primaryServiceID.type == UUID::UUID_TYPE_SHORT)
    {
        /* 16-bit BLE UUID */
        uart.printf("10 01 00 04 01 02 %02X %02X\r\n",
                    service.primaryServiceID.value & 0xFF,
                    service.primaryServiceID.value >> 8);
    }
    else
    {
        /* 128-bit Custom UUID */
        uart.printf("10 01 00 12 01 10 %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X\r\n",
            service.primaryServiceID.base[0], 
            service.primaryServiceID.base[1], 
            service.primaryServiceID.base[2], 
            service.primaryServiceID.base[3], 
            service.primaryServiceID.base[4], 
            service.primaryServiceID.base[5], 
            service.primaryServiceID.base[6], 
            service.primaryServiceID.base[7], 
            service.primaryServiceID.base[8], 
            service.primaryServiceID.base[9], 
            service.primaryServiceID.base[10], 
            service.primaryServiceID.base[11], 
            service.primaryServiceID.base[12], 
            service.primaryServiceID.base[13], 
            service.primaryServiceID.base[14], 
            service.primaryServiceID.base[15]);
    }
    
    /* ToDo: Check response */
    wait(0.1);
    
    /* Add characteristics to the service */
    for (uint8_t i = 0; i < service.characteristicCount; i++)
    {
        /* Command ID = 0x0002 */
        uart.printf("10 02 00 0F 01 02 %02X %02X 04 02 %02X %02X 05 02 %02X %02X 03 01 %02X\r\n",
                    service.characteristics[i].uuid & 0xFF, 
                    service.characteristics[i].uuid >> 8,
                    service.characteristics[i].lenMin & 0xFF,
                    service.characteristics[i].lenMin >> 8,
                    service.characteristics[i].lenMax & 0xFF,
                    service.characteristics[i].lenMax >> 8,
                    service.characteristics[i].properties);
                    
        /* ToDo: Check response */
        wait(0.1);
        
        /* Update the characteristic handle */
        service.characteristics[i].handle = characteristicCount;
        characteristicCount++;
    }

    /* Update the service handle */
    service.handle = serviceCount;
    serviceCount++;
    
    return BLE_ERROR_NONE;
}

/**************************************************************************/
/*!
    @brief  Reads the value of a characteristic, based on the service
            and characteristic index fields

    @param[in]  charHandle
                The handle of the GattCharacteristic to read from
    @param[in]  buffer
                Buffer to hold the the characteristic's value
                (raw byte array in LSB format)
    @param[in]  len
                The number of bytes read into the buffer
            
    @returns    ble_error_t
    
    @retval     BLE_ERROR_NONE
                Everything executed properly
                
    @section EXAMPLE

    @code

    @endcode
*/
/**************************************************************************/
ble_error_t nRF51GattServer::readValue(uint8_t charHandle, uint8_t buffer[], uint16_t len)
{
    /* ToDo */
    
    return BLE_ERROR_NONE;
}

/**************************************************************************/
/*!
    @brief  Updates the value of a characteristic, based on the service
            and characteristic index fields

    @param[in]  charHandle
                The handle of the GattCharacteristic to write to
    @param[in]  buffer
                Data to use when updating the characteristic's value
                (raw byte array in LSB format)
    @param[in]  len
                The number of bytes in buffer
            
    @returns    ble_error_t
    
    @retval     BLE_ERROR_NONE
                Everything executed properly
                
    @section EXAMPLE

    @code

    @endcode
*/
/**************************************************************************/
ble_error_t nRF51GattServer::updateValue(uint8_t charHandle, uint8_t buffer[], uint16_t len)
{
    /* Command ID = 0x0006, Payload = Characteristic ID, Value */
    uart.printf("10 06 00 %02X %02X", len + 1, charHandle);
    for (uint16_t i = 0; i<len; i++)
    {
        uart.printf(" %02X", buffer[i]);
    }
    uart.printf("\r\n");

    /* ToDo: Check response */
    wait(0.1);
    
    return BLE_ERROR_NONE;
}
