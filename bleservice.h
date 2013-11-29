#ifndef __BLE_SERVICE_H__
#define __BLE_SERVICE_H__

#include "blecommon.h"
#include "uuid.h"
#include "blecharacteristic.h"

#define BLE_SERVICE_MAX_CHARACTERISTICS (5)

class BLEService
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
    
    BLEService(uint8_t[16]);  /* 128-bit Base UUID */
    BLEService(uint16_t);     /* 16-bit BLE UUID */
    virtual ~BLEService(void);

    UUID                primaryServiceID;
    uint8_t             characteristicCount;
    serialisedChar_t    characteristics[BLE_SERVICE_MAX_CHARACTERISTICS];
    uint8_t             index;

    ble_error_t         addCharacteristic(BLECharacteristic &);
};

#endif
