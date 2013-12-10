#include <stdio.h>
#include <string.h>

#include "GapAdvertisingParams.h"

/**************************************************************************/
/*!
    @brief  Creates a new GapAdvertisingParams

    @section EXAMPLE

    @code

    @endcode
*/
/**************************************************************************/
GapAdvertisingParams::GapAdvertisingParams(uint8_t advType, uint16_t interval, uint16_t timeout)
{
    _advType = advType;
    _interval = interval;
    _timeout = timeout;
}

/**************************************************************************/
/*!
    Destructor
*/
/**************************************************************************/
GapAdvertisingParams::~GapAdvertisingParams(void)
{
}
