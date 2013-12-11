#ifndef __GAP_ADVERTISING_DATA_H__
#define __GAP_ADVERTISING_DATA_H__

#include "blecommon.h"

#define GAP_ADVERTISING_DATA_MAX_PAYLOAD        (31)

/**************************************************************************/
/*!
    @brief
    This class provides several helper functions to generate properly
    formatted GAP Advertising and Scan Response data payloads
    
    @section Advertising and Scan Response Payloads
    
    @para
    Advertising data and Scan Response data are organized around a set of
    data types called 'AD types' in the Bluetooth 4.0 specification.
            
    @para
    Each AD type has it's own standardized 'assigned number', as defined
    by the Bluetooth SIG:
    https://www.bluetooth.org/en-us/specification/assigned-numbers/generic-access-profile
    
    @para
    For convenience sake, all appropriate AD types have been encapsulated
    into an enum at \ref ble_gap_adv_datatype_t.
    
    @para
    Raw Advertising or Scan Response payloads are formatted as follows:
    
    - Record length (1 byte)
    - AD Type (1 byte)
    - AD payload (optional, only present if record length > 1)
    
    @para
    When multiple AD types are present, the individual records are simply
    appended one after the other, up to the maximum payload length of 31
    bytes.
    
    @note See Bluetooth Specification 4.0 (Vol. 3) Section 11, 18 for
    further information on Advertising and Scan Response data.
    
    @section Sample Advertising Payload
    
    // Two record payload containing BLE_GAP_ADV_DATATYPE_FLAGS (0x01) and 
    // BLE_GAP_ADV_DATATYPE_COMPLETE_LOCAL_NAME (0x09) fields
    02 01 01 0A 09 50 65 64 6F 6D 65 74 65 72

    // Record 1 (FLAGS)
    02 01 01
    02 = record length (2 bytes)
    01 = BLE_GAP_ADV_DATATYPE_FLAGS
    01 = BLE_GAP_ADV_FLAGS_LE_LIMITED_DISCOVERABLE
    
    // Record 2 (COMPLETE LOCAL NAME)
    0A 09 50 65 64 6F 6D 65 74 65 72
    0A = record length (10 bytes)
    09 = BLE_GAP_ADV_DATATYPE_COMPLETE_LOCAL_NAME
    50 65 64 6F 6D 65 74 65 72 = "Pedometer"
*/
/**************************************************************************/
class GapAdvertisingData
{
public:    
    GapAdvertisingData(void);
    virtual ~GapAdvertisingData(void);

    ble_error_t addData(ble_gap_adv_datatype_t, uint8_t *, uint8_t);
    uint8_t     getPayloadLen(void);
    
private:
    uint8_t _payload[GAP_ADVERTISING_DATA_MAX_PAYLOAD];
    uint8_t _payloadLen;
};

#endif
