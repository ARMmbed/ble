#ifndef __BLE_CHARACTERISTIC_H__
#define __BLE_CHARACTERISTIC_H__

#include "blecommon.h"
#include "uuid.h"

class BLECharacteristic
{
private:

public:
    BLECharacteristic(uint16_t uuid, uint16_t minLen, uint16_t maxLen, uint8_t properties);
    virtual ~BLECharacteristic(void);

    uint16_t uuid;              /* Characteristic UUID */
    uint16_t lenMin;            /* Minimum length of the value */
    uint16_t lenMax;            /* Maximum length of the value */
    uint8_t  index;
    
    struct Properties
    {
        /* Standard properties */
        uint8_t broadcast       :1;         /**< Broadcasting of value permitted. */
        uint8_t read            :1;         /**< Reading value permitted. */
        uint8_t write_wo_resp   :1;         /**< Writing value with Write Command permitted. */
        uint8_t write           :1;         /**< Writing value with Write Request permitted. */
        uint8_t notify          :1;         /**< Notications of value permitted. */ // https://developer.bluetooth.org/gatt/descriptors/Pages/DescriptorViewer.aspx?u=org.bluetooth.descriptor.gatt.client_characteristic_configuration.xml
        uint8_t indicate        :1;         /**< Indications of value permitted. */ // https://developer.bluetooth.org/gatt/descriptors/Pages/DescriptorViewer.aspx?u=org.bluetooth.descriptor.gatt.client_characteristic_configuration.xml
        uint8_t auth_signed_wr  :1;         /**< Writing value with Signed Write Command permitted. */
    } properties;
};

#endif
