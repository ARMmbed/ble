#ifndef __GAP_ADVERTISING_DATA_H__
#define __GAP_ADVERTISING_DATA_H__

#include "blecommon.h"

#define GAP_ADVERTISING_DATA_MAX_PAYLOAD        (31)

/**************************************************************************/
/*!
    \brief
    This class provides several helper functions to generate properly
    formatted GAP Advertising and Scan Response data payloads

    \note 
    See Bluetooth Specification 4.0 (Vol. 3), Part C, Section 11 and 18
    for further information on Advertising and Scan Response data.
        
    \par Advertising and Scan Response Payloads
    Advertising data and Scan Response data are organized around a set of
    data types called 'AD types' in Bluetooth 4.0 (see the Bluetooth Core
    Specification v4.0, Vol. 3, Part C, Sections 11 and 18).

    \par
    Each AD type has it's own standardized 'assigned number', as defined
    by the Bluetooth SIG:
    https://www.bluetooth.org/en-us/specification/assigned-numbers/generic-access-profile
    
    \par
    For convenience sake, all appropriate AD types have been encapsulated
    into GapAdvertisingData::DataType.
    
    \par
    Before the AD Types and their payload (if any) can be inserted into
    the Advertising or Scan Response frames, they need to be formatted as
    follows:
    
    \li \c Record length (1 byte)
    \li \c AD Type (1 byte)
    \li \c AD payload (optional, only present if record length > 1)
    
    \par
    This class takes care of properly formatting the payload, performs
    some basic checks on the payload length, and tries to avoid common
    errors like adding an exclusive AD field twice in the Advertising
    or Scan Response payload.
    
    \par EXAMPLE
    
    \code
    
    // ToDo
    
    \endcode
*/
/**************************************************************************/
class GapAdvertisingData
{
  public:
    /**********************************************************************/
    /*!
        \brief
        A list of Advertising Data types commonly used by peripherals.
        These AD types are used to describe the capabilities of the
        peripheral, and get inserted inside the advertising or scan
        response payloads.
        
        \par Source
        \li \c Bluetooth Core Specification 4.0 (Vol. 3), Part C, Section 11, 18
        \li \c https://www.bluetooth.org/en-us/specification/assigned-numbers/generic-access-profile
    */
    /**********************************************************************/
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
    
    /**********************************************************************/
    /*!
        \brief
        A list of values for the FLAGS AD Type
        
        \note
        You can use more than one value in the FLAGS AD Type (ex.
        LE_GENERAL_DISCOVERABLE and BREDR_NOT_SUPPORTED).
        
        \par Source
        \li \c Bluetooth Core Specification 4.0 (Vol. 3), Part C, Section 18.1
    */
    /**********************************************************************/
    enum Flags
    {
      LE_LIMITED_DISCOVERABLE                           = 0x01, /**< Peripheral device is discoverable for a limited period of time */
      LE_GENERAL_DISCOVERABLE                           = 0x02, /**< Peripheral device is discoverable at any moment */
      BREDR_NOT_SUPPORTED                               = 0x04, /**< Peripheral device is LE only */
      SIMULTANEOUS_LE_BREDR_C                           = 0x08, /**< Not relevant - central mode only */
      SIMULTANEOUS_LE_BREDR_H                           = 0x10  /**< Not relevant - central mode only */
    };

    /**********************************************************************/
    /*!
        \brief
        A list of values for the APPEARANCE AD Type, which describes the
        physical shape or appearance of the device
        
        \par Source
        \li \c Bluetooth Core Specification Supplement, Part A, Section 1.12
        \li \c Bluetooth Core Specification 4.0 (Vol. 3), Part C, Section 12.2
        \li \c https://developer.bluetooth.org/gatt/characteristics/Pages/CharacteristicViewer.aspx?u=org.bluetooth.characteristic.gap.appearance.xml
    */
    /**********************************************************************/
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
    uint8_t *   getPayload(void);
    uint8_t     getPayloadLen(void);

  private:
    uint8_t _payload[GAP_ADVERTISING_DATA_MAX_PAYLOAD];
    uint8_t _payloadLen;
};

#endif
