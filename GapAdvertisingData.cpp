#include <stdio.h>
#include <string.h>

#include "GapAdvertisingData.h"

/**************************************************************************/
/*!
    @brief  Creates a new GapAdvertisingData instance

    @section EXAMPLE

    @code

    @endcode
*/
/**************************************************************************/
GapAdvertisingData::GapAdvertisingData(void)
{
    memset(_payload, 0, GAP_ADVERTISING_DATA_MAX_PAYLOAD);
    _payloadLen = 0;
}

/**************************************************************************/
/*!
    Destructor
*/
/**************************************************************************/
GapAdvertisingData::~GapAdvertisingData(void)
{
}

/**************************************************************************/
/*!
    @brief  Adds advertising data based on the specified AD type (see
            \ref DataType)

    @args[in]   advDataType The Advertising \ref DataType to add
    @args[in]   payload     Pointer to the payload contents
    @args[in]   len         Size of the payload in bytes
    
    @returns    ble_error_t
    
    @retval     BLE_ERROR_NONE
                Everything executed properly
                
    @retval     BLE_ERROR_BUFFER_OVERFLOW
                The specified data would cause the advertising buffer
                to overflow
    
    @section EXAMPLE

    @code

    @endcode
*/
/**************************************************************************/
ble_error_t GapAdvertisingData::addData(DataType advDataType, uint8_t * payload, uint8_t len)
{
    /* ToDo: Check if an AD type already exists and if the existing */
    /*       value is exclusive or not (flags, etc.) */
    
    /* Make sure we don't exceed the 31 byte payload limit */
    if (_payloadLen + len >= GAP_ADVERTISING_DATA_MAX_PAYLOAD)
        return BLE_ERROR_BUFFER_OVERFLOW;
    
    memcpy(&_payload[_payloadLen], payload, len);
    _payloadLen += len;
    
    return BLE_ERROR_NONE;
}

/**************************************************************************/
/*!
    @brief  Helper function to add \ref APPEARANCE data to the advertising
            payload

    @args[in]   appearance  The \ref APPEARANCE value to add
    
    @returns    ble_error_t
    
    @retval     BLE_ERROR_NONE
                Everything executed properly
                
    @retval     BLE_ERROR_BUFFER_OVERFLOW
                The specified data would cause the advertising buffer
                to overflow
    
    @section EXAMPLE

    @code

    @endcode
*/
/**************************************************************************/
ble_error_t GapAdvertisingData::addAppearance(Appearance appearance)
{
    return addData(GapAdvertisingData::APPEARANCE, (uint8_t*)&appearance, 2);
}

/**************************************************************************/
/*!
    @brief  Helper function to add \ref FLAGS data to the advertising
            payload

    @args[in]   flag  The \ref FLAGS value to add

                @para
                \ref LE_LIMITED_DISCOVERABLE - The peripheral is
                discoverable for a limited period of time

                @para
                \ref LE_GENERAL_DISCOVERABLE - The peripheral is
                permanently discoverable
                
                @para
                \ref BREDR_NOT_SUPPORTED - This peripheral is a
                Bluetooth Low Energy only device (no EDR support)

    @returns    ble_error_t
    
    @retval     BLE_ERROR_NONE
                Everything executed properly
                
    @retval     BLE_ERROR_BUFFER_OVERFLOW
                The specified data would cause the advertising buffer
                to overflow
    
    @section EXAMPLE

    @code

    @endcode
*/
/**************************************************************************/
ble_error_t GapAdvertisingData::addFlags(Flags flag)
{
    return addData(GapAdvertisingData::FLAGS, (uint8_t*)&flag, 1);
}

/**************************************************************************/
/*!
    @brief  Helper function to add \ref TX_POWER_LEVEL data to the
            advertising payload

    @args[in]   flag  The \ref TX_POWER_LEVEL value to add
    
    @returns    ble_error_t
    
    @retval     BLE_ERROR_NONE
                Everything executed properly
                
    @retval     BLE_ERROR_BUFFER_OVERFLOW
                The specified data would cause the advertising buffer
                to overflow
    
    @section EXAMPLE

    @code

    @endcode
*/
/**************************************************************************/
ble_error_t GapAdvertisingData::addTxPower(int8_t txPower)
{
    /* ToDo: Basic error checking to make sure txPower is in range */
    return addData(GapAdvertisingData::TX_POWER_LEVEL, (uint8_t*)&txPower, 1);
}

/**************************************************************************/
/*!
    @brief Clears the payload and resets the payload length counter
*/
/**************************************************************************/
void GapAdvertisingData::clear(void)
{
    memset(&_payload, 0, GAP_ADVERTISING_DATA_MAX_PAYLOAD);
    _payloadLen = 0;
}

/**************************************************************************/
/*!
    @brief Returns the current payload length (0..31 bytes)
*/
/**************************************************************************/
uint8_t GapAdvertisingData::getPayloadLen(void)
{
    return _payloadLen;
}
