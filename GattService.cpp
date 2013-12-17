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
    memset(&characteristics, 0, sizeof(serialisedChar_t) * BLE_SERVICE_MAX_CHARACTERISTICS);
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
    memset(&characteristics, 0, sizeof(serialisedChar_t) * BLE_SERVICE_MAX_CHARACTERISTICS);
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
    @brief  Adds a GattCharacterisic to the service, serialising the
            essential data for the characteristic.
            
    @note   The GattService does not store a reference to the source
            GattCharacteristic, only a serialised version of the key
            properties required to create the characteristic on the
            target radio board.
            
    @note   This function will update the .handle field in the
            GattCharacteristic to indicate where this characteristic was
            stored in the GattService's characteristic array.

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
    
    serialisedChar_t c;
    
    /* Serialise the source GattCharacteristic */
    memcpy(&c.id, &characteristic.uuid, 2);
    memcpy(&c.lenMin, &characteristic.lenMin, 2);
    memcpy(&c.lenMax, &characteristic.lenMax, 2);
    memcpy(&c.properties, &characteristic.properties, 2);
    memset(&c.reserved, 0, 1);
    
    /* Insert the serialised object into the buffer */
    memcpy(&characteristics[characteristicCount], &c, sizeof(serialisedChar_t));
    
    /* Update the handle value */
    characteristic.handle = characteristicCount;
    
    characteristicCount++;
    
    return BLE_ERROR_NONE;
}
