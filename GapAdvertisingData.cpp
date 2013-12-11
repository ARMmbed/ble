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
    @brief Returns the current payload length (0..31 bytes)
*/
/**************************************************************************/
uint8_t GapAdvertisingData::getPayloadLen(void)
{
    return _payloadLen;
}

/**************************************************************************/
/*!
    @brief Adds advertising data based on the specified AD types

    @args[in]   adType      The advertising data type to add
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
ble_error_t GapAdvertisingData::addData(ble_gap_adv_datatype_t adType, uint8_t * payload, uint8_t len)
{
    /* ToDo: Make sure we don't exceed the 31 byte payload limit */
    if (_payloadLen + len >= GAP_ADVERTISING_DATA_MAX_PAYLOAD)
        return BLE_ERROR_BUFFER_OVERFLOW;
    
    memcpy(&_payload[_payloadLen], payload, len);
    _payloadLen += len;
    
    return BLE_ERROR_NONE;
}
