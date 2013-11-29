#ifndef __UUID_H__
#define __UUID_H__

#include "blecommon.h"

class UUID
{
private:
    
public:
    enum
    {
        UUID_TYPE_SHORT = 0,    // Short BLE UUID
        UUID_TYPE_LONG  = 1     // Full 128-bit UUID
    };
    
    UUID(void);
    UUID(uint8_t const[16]);
    UUID(uint16_t const);
    virtual ~UUID(void);
    
    uint8_t   type;        // UUID_TYPE_SHORT or UUID_TYPE_LONG
    uint8_t   base[16];    // in case of custom
    uint16_t  value;       // 16 bit uuid (byte 2-3 using with base)
    
    ble_error_t update(uint8_t const[16]);
    ble_error_t update(uint16_t const);
};

#endif
