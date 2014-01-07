#include <stdio.h>
#include <string.h>

#include "GattService.h"

/**************************************************************************/
/*!
    @brief  Creates a new GattService using the specified 128-bit UUID
            
    @note   The UUID value must be unique on the device

    @param[in]  uuid
                The 16 byte (128-bit) UUID to use for this characteristic

    @section EXAMPLE

    @code
   
    @endcode
*/
/**************************************************************************/
GattService::GattService(uint8_t base_uuid[16])
{
    primaryServiceID.update(base_uuid);
    characteristicCount = 0;
    handle = 0;
}

/**************************************************************************/
/*!
    @brief  Creates a new GattService using the specified 16-bit BLE UUID
            
    @param[in]  ble_uuid
                The standardised 16-bit (2 byte) BLE UUID to use for this
                characteristic

    @section EXAMPLE

    @code
   
    @endcode
*/
/**************************************************************************/
GattService::GattService(uint16_t ble_uuid)
{
    primaryServiceID.update( ble_uuid );
    characteristicCount = 0;
    handle = 0;
}

/**************************************************************************/
/*!
    @brief  Destructor
*/
/**************************************************************************/
GattService::~GattService(void)
{
}

/**************************************************************************/
/*!
    @brief  Adds a GattCharacterisic to the service.
            
    @note   This function will not update the .handle field in the
            GattCharacteristic. This value is updated when the parent
            service is added via the radio driver. 

    @param[in]  characteristic
                The GattCharacteristic object describing the characteristic
                to add to this service

    @returns    BLE_ERROR_NONE (0) if everything executed correctly, or an
                error code if there was a problem
    @retval     BLE_ERROR_NONE
                Everything executed correctly

    @section EXAMPLE

    @code
   
    @endcode
*/
/**************************************************************************/
ble_error_t GattService::addCharacteristic(GattCharacteristic & characteristic)
{
    /* ToDo: Make sure we don't overflow the array, etc. */
    /* ToDo: Make sure this characteristic UUID doesn't already exist */
    /* ToDo: Basic validation */

    characteristics[characteristicCount] = characteristic;
    characteristicCount++;
    
    return BLE_ERROR_NONE;
}
