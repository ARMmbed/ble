#ifndef __GATT_CHARACTERISTIC_H__
#define __GATT_CHARACTERISTIC_H__

#include "blecommon.h"
#include "uuid.h"

class GattCharacteristic
{
private:

public:
    GattCharacteristic(uint16_t uuid=0, uint16_t minLen=1, uint16_t maxLen=1, uint8_t properties=0);
    virtual ~GattCharacteristic(void);

    uint16_t uuid;              /* Characteristic UUID */
    uint16_t lenMin;            /* Minimum length of the value */
    uint16_t lenMax;            /* Maximum length of the value */
    uint8_t  handle;
    uint8_t  properties;
};

#endif
