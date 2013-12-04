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
    
    @args   fptr[in]    Pointer to the callback function when any radio
                        event is raised by the radio HW.
*/
/**************************************************************************/
//nRF51822::nRF51822() : uart(P0_4, P0_0) /* LPC812 */
nRF51822::nRF51822() : uart(p9, p10)      /* LPC1768 using apps board */
{
    /* Setup the nRF UART interface */
    uart.baud(9600);
 
    /* Echo data on the debug CDC port */
    uart.attach(this, &nRF51822::uartCallback);
    
    /* Add flow control for UART (required by the nRF51822) */
    //uart.set_flow_control(Serial::RTSCTS, P0_6, P0_8);  /* LPC812 */
    uart.set_flow_control(Serial::RTSCTS, p30, p29);      /* LPC1768 */
    
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

*/
/**************************************************************************/
//ble_error_t nRF51822::attach(void (*fptr)(void))
//{
//    return BLE_ERROR_NONE;
//}

/**************************************************************************/
/*!

*/
/**************************************************************************/
ble_error_t nRF51822::addService(BLEService & service)
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
    service.index = serviceCount;
    serviceCount++;
    
    return BLE_ERROR_NONE;
}

/**************************************************************************/
/*!
    @brief  Updates the value of a characteristic, based on the service
            and characteristic index fields
            
    @param[in]  sIndex  
                The BLEService's index value (.index)
    @param[in]  cIndex
                The BLECharacteristic's index value (.index)
    @param[in]  buffer
                Data to use when updating the characteristic's value
                (raw byte array in LSB format)
    @param[in]  len
                The number of bytes in buffer
*/
/**************************************************************************/
ble_error_t nRF51822::updateValue(uint8_t sIndex, uint8_t cIndex, uint8_t buffer[], uint16_t len)
{
    /* Command ID = 0x0006, Payload = Service ID, Characteristic ID, Value */
    uart.printf("10 06 00 %02X %02X %02X", len + 2, cIndex, sIndex);
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
