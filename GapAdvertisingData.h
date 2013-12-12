#ifndef __GAP_ADVERTISING_DATA_H__
#define __GAP_ADVERTISING_DATA_H__

#include "blecommon.h"

#define GAP_ADVERTISING_DATA_MAX_PAYLOAD        (31)

/**************************************************************************/
/*!
    @brief
    This class provides several helper functions to generate properly
    formatted GAP Advertising and Scan Response data payloads

    @note See Bluetooth Specification 4.0 (Vol. 3), Part C, Section 11 and 18
    for further information on Advertising and Scan Response data.
        
    @section Advertising and Scan Response Payloads
    
    @para
    Advertising data and Scan Response data are organized around a set of
    data types called 'AD types' in Bluetooth 4.0 (see the Bluetooth Core
    Specification v4.0, Vol. 3, Part C, Sections 11 and 18).

    @para
    Each AD type has it's own standardized 'assigned number', as defined
    by the Bluetooth SIG:
    https://www.bluetooth.org/en-us/specification/assigned-numbers/generic-access-profile
    
    @para
    For convenience sake, all appropriate AD types have been encapsulated
    into an enum at \ref DataType.
    
    @para
    Before the AD Types and their payload (if any) can be inserted into
    the Advertising or Scan Response frames, they need to be formatted as
    follows:
    
    - Record length (1 byte)
    - AD Type (1 byte)
    - AD payload (optional, only present if record length > 1)

    @para
    When multiple AD records are present, the individual records are
    appended one after the other, up to the maximum payload length of 31
    bytes, as can be seen in the example payload below.
    
    @section Sample Raw Advertising Payload
    
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
    /* Bluetooth Core Specification 4.0 (Vol. 3), Part C, Section 11, 18 */
    /* https://www.bluetooth.org/en-us/specification/assigned-numbers/generic-access-profile */
    enum DataType
    {
      FLAGS                                             = 0x01,
      INCOMPLETE_LIST_16BIT_SERVICE_IDS                 = 0x02,
      COMPLETE_LIST_16BIT_SERVICE_IDS                   = 0x03,
      INCOMPLETE_LIST_32BIT_SERVICE_IDS                 = 0x04,
      COMPLETE_LIST_32BIT_SERVICE_IDS                   = 0x05,
      INCOMPLETE_LIST_128BIT_SERVICE_IDS                = 0x06,
      COMPLETE_LIST_128BIT_SERVICE_IDS                  = 0x07,
      SHORTENED_LOCAL_NAME                              = 0x08,
      COMPLETE_LOCAL_NAME                               = 0x09,
      TX_POWER_LEVEL                                    = 0x0A,
      DEVICE_ID                                         = 0x10,
      SLAVE_CONNECTION_INTERVAL_RANGE                   = 0x12,
      SERVICE_DATA                                      = 0x16,
      APPEARANCE                                        = 0x19,
      ADVERTISING_INTERVAL                              = 0x1A,
      MANUFACTURER_SPECIFIC_DATA                        = 0xFF
    };
    
    /* Bluetooth Core Specification 4.0 (Vol. 3), Part C, Section 18.1 */
    enum Flags
    {
      LE_LIMITED_DISCOVERABLE                           = 0x01,
      LE_GENERAL_DISCOVERABLE                           = 0x02,
      BREDR_NOT_SUPPORTED                               = 0x04,
      SIMULTANEOUS_LE_BREDR_C                           = 0x08,
      SIMULTANEOUS_LE_BREDR_H                           = 0x10
    };

    /* Bluetooth Core Specification Supplement, Part A, Section 1.12 */
    /* Bluetooth Core Specification 4.0 (Vol. 3), Part C, Section 12.2 */
    /* https://developer.bluetooth.org/gatt/characteristics/Pages/CharacteristicViewer.aspx?u=org.bluetooth.characteristic.gap.appearance.xml */
    enum Appearance
    {
      UNKNOWN                                           = 0,
      GENERIC_PHONE                                     = 64,
      GENERIC_COMPUTER                                  = 128,
      GENERIC_WATCH                                     = 192,
      WATCH_SPORTS_WATCH                                = 193,
      GENERIC_CLOCK                                     = 256,
      GENERIC_DISPLAY                                   = 320,
      GENERIC_REMOTE_CONTROL                            = 384,
      GENERIC_EYE_GLASSES                               = 448,
      GENERIC_TAG                                       = 512,
      GENERIC_KEYRING                                   = 576,
      GENERIC_MEDIA_PLAYER                              = 640,
      GENERIC_BARCODE_SCANNER                           = 704,
      GENERIC_THERMOMETER                               = 768,
      THERMOMETER_EAR                                   = 769,
      GENERIC_HEART_RATE_SENSOR                         = 832,
      HEART_RATE_SENSOR_HEART_RATE_BELT                 = 833,
      GENERIC_BLOOD_PRESSURE                            = 896,
      BLOOD_PRESSURE_ARM                                = 897,
      BLOOD_PRESSURE_WRIST                              = 898,
      HUMAN_INTERFACE_DEVICE_HID                        = 960,
      KEYBOARD                                          = 961,
      MOUSE                                             = 962,
      JOYSTICK                                          = 963,
      GAMEPAD                                           = 964,
      DIGITIZER_TABLET                                  = 965,
      CARD_READER                                       = 966,
      DIGITAL_PEN                                       = 967,
      BARCODE_SCANNER                                   = 968,
      GENERIC_GLUCOSE_METER                             = 1024,
      GENERIC_RUNNING_WALKING_SENSOR                    = 1088,
      RUNNING_WALKING_SENSOR_IN_SHOE                    = 1089,
      RUNNING_WALKING_SENSOR_ON_SHOE                    = 1090,
      RUNNING_WALKING_SENSOR_ON_HIP                     = 1091,
      GENERIC_CYCLING                                   = 1152,
      CYCLING_CYCLING_COMPUTER                          = 1153,
      CYCLING_SPEED_SENSOR                              = 1154,
      CYCLING_CADENCE_SENSOR                            = 1155,
      CYCLING_POWER_SENSOR                              = 1156,
      CYCLING_SPEED_AND_CADENCE_SENSOR                  = 1157,
      PULSE_OXIMETER_GENERIC                            = 3136,
      PULSE_OXIMETER_FINGERTIP                          = 3137,
      PULSE_OXIMETER_WRIST_WORN                         = 3138,
      OUTDOOR_GENERIC                                   = 5184,
      OUTDOOR_LOCATION_DISPLAY_DEVICE                   = 5185,
      OUTDOOR_LOCATION_AND_NAVIGATION_DISPLAY_DEVICE    = 5186,
      OUTDOOR_LOCATION_POD                              = 5187,
      OUTDOOR_LOCATION_AND_NAVIGATION_POD               = 5188
    };

    GapAdvertisingData(void);
    virtual ~GapAdvertisingData(void);

    ble_error_t addData(DataType, uint8_t *, uint8_t);
    ble_error_t addAppearance(Appearance appearance = UNKNOWN);
    ble_error_t addFlags(Flags flag = LE_GENERAL_DISCOVERABLE);
    ble_error_t addTxPower(int8_t txPower);
    void        clear(void);
    uint8_t     getPayloadLen(void);

  private:
    uint8_t _payload[GAP_ADVERTISING_DATA_MAX_PAYLOAD];
    uint8_t _payloadLen;
};

#endif
