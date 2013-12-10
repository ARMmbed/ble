#ifndef __GATT_SERVICE_H__
#define __GATT_SERVICE_H__

#include "blecommon.h"
#include "uuid.h"
#include "GattCharacteristic.h"

#define BLE_SERVICE_MAX_CHARACTERISTICS (5)

class GattService
{
private:

public:
    typedef struct
    {
        uint16_t    id;
        uint16_t    lenMin;
        uint16_t    lenMax;
        uint8_t     properties;
        uint8_t     reserved;
    } serialisedChar_t;
    
    GattService(uint8_t[16]);  /* 128-bit Base UUID */
    GattService(uint16_t);     /* 16-bit BLE UUID */
    virtual ~GattService(void);

    UUID                primaryServiceID;
    uint8_t             characteristicCount;
    serialisedChar_t    characteristics[BLE_SERVICE_MAX_CHARACTERISTICS];
    uint8_t             index;

    ble_error_t         addCharacteristic(GattCharacteristic &);
};

#endif
