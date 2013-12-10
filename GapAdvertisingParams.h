#ifndef __GAP_ADVERTISING_PARAMS_H__
#define __GAP_ADVERTISING_PARAMS_H__

#include "blecommon.h"

class GapAdvertisingParams
{
public:
    GapAdvertisingParams(uint8_t advType, uint16_t interval, uint16_t timeout);
    virtual ~GapAdvertisingParams(void);

private:
    uint8_t  _advType;
    uint16_t _interval;
    uint16_t _timeout;
};

#endif
