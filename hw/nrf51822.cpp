#include "nrf51822.h"
#include "mbed.h"

/**************************************************************************/
/*!
    @brief  UART callback function
*/
/**************************************************************************/
void nRF51822::uartCallback(void)
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
nRF51822::nRF51822() : uart(p9, p10)
{
    /* Setup the nRF UART interface */
    uart.baud(9600);

    /* Echo data on the debug CDC port */
    uart.attach(this, &nRF51822::uartCallback);
    
    /* Add flow control for UART (required by the nRF51822) */
    uart.set_flow_control(RawSerial::RTSCTS, p30, p29);

    /* Reset the service counter */
    serviceCount = 0;
}

/**************************************************************************/
/*!
    @brief  Destructor
*/
/**************************************************************************/
nRF51822::~nRF51822(void)
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
ble_error_t nRF51822::setAdvertising(GapAdvertisingParams & params, GapAdvertisingData & advData, GapAdvertisingData & scanResponse)
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
    @brief  Adds a new service to the GATT table on the peripheral
            
    @returns    ble_error_t
    
    @retval     BLE_ERROR_NONE
                Everything executed properly
                
    @section EXAMPLE

    @code

    @endcode
*/
/**************************************************************************/
ble_error_t nRF51822::addService(GattService & service)
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
                    service.characteristics[i].id & 0xFF, 
                    service.characteristics[i].id >> 8,
                    service.characteristics[i].lenMin & 0xFF,
                    service.characteristics[i].lenMin >> 8,
                    service.characteristics[i].lenMax & 0xFF,
                    service.characteristics[i].lenMax >> 8,
                    service.characteristics[i].properties);
                    
        /* ToDo: Check response */
        wait(0.1);
    }    

    /* Update the service index value */
    service.handle = serviceCount;
    serviceCount++;
    
    return BLE_ERROR_NONE;
}

/**************************************************************************/
/*!
    @brief  Reads the value of a characteristic, based on the service
            and characteristic index fields

    @param[in]  service
                The GattService to read from
    @param[in]  characteristic
                The GattCharacteristic to read from
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
ble_error_t nRF51822::readCharacteristic(GattService &service, GattCharacteristic &characteristic, uint8_t buffer[], uint16_t len)
{
    /* ToDo */
    
    return BLE_ERROR_NONE;
}

/**************************************************************************/
/*!
    @brief  Updates the value of a characteristic, based on the service
            and characteristic index fields
            
    @param[in]  service
                The GattService to write to
    @param[in]  characteristic
                The GattCharacteristic to write to
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
ble_error_t nRF51822::writeCharacteristic(GattService &service, GattCharacteristic &characteristic, uint8_t buffer[], uint16_t len)
{
    /* Command ID = 0x0006, Payload = Service ID, Characteristic ID, Value */
    uart.printf("10 06 00 %02X %02X %02X", len + 2, characteristic.handle, service.handle);
    for (uint16_t i = 0; i<len; i++)
    {
        uart.printf(" %02X", buffer[i]);
    }
    uart.printf("\r\n");

    /* ToDo: Check response */
    wait(0.1);
    
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
ble_error_t nRF51822::start(void)
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
ble_error_t nRF51822::stop(void)
{
    /* Command ID = 0x0004, No payload */
    uart.printf("10 04 00 00\r\n");

    /* ToDo: Check response */
    wait(0.1);

    return BLE_ERROR_NONE;
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
ble_error_t nRF51822::reset(void)
{
    /* Command ID = 0x0005, No payload */
    uart.printf("10 05 00 00\r\n");

    /* Reset the service counter */
    serviceCount = 0;

    /* Wait for the radio to come back up */    
    wait(1);
    
    return BLE_ERROR_NONE;
}
