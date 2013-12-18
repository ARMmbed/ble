#include <stdio.h>
#include <string.h>

#include "blecommon.h"
#include "GapAdvertisingParams.h"

/**************************************************************************/
/*!
    @brief  Instantiates a new GapAdvertisingParams instance

    @param[in]  advType
                The GAP advertising mode to use for this device. Valid
                values are defined in \ref AdvertisingType

                @para
                ADV_NON_CONNECTABLE_UNDIRECTED - All connections to the
                peripheral device will be refused.
                
                @para
                ADV_CONNECTABLE_DIRECTED - Only connections from a
                pre-defined central device will be accepted.
                
                @para
                ADV_CONNECTABLE_UNDIRECTED - Any central device can connect
                to this peripheral.

                @para
                ADV_SCANNABLE_UNDIRECTED - Any central device can connect
                to this peripheral, and the secondary Scan Response
                payload will be included or available to central devices.
                
                @note See Bluetooth Core Specification 4.0 (Vol. 3),
                Part C, Section 9.3 and Core Specification 4.0 (Vol. 6),
                Part B, Section 2.3.1 for further information on GAP
                connection modes

    @param[in]  interval
                Advertising interval between 0x0020 and 0x4000 in 0.625ms
                units (20ms to 10.24s).  If using non-connectable mode
                (\ref ADV_NON_CONNECTABLE_UNDIRECTED) this min value is
                0x00A0 (100ms).

                @para
                Increasing this value will allow central devices to detect
                your peripheral faster at the expense of more power being
                used by the radio due to the higher data transmit rate.
                
                @note This field must be set to 0 if connectionMode is equal
                to \ref ADV_CONNECTABLE_DIRECTED
                
                @note See Bluetooth Core Specification, Vol 3., Part C,
                Appendix A for suggested advertising intervals:
                
    @param[in]  timeout
                Advertising timeout between 0x1 and 0x3FFF (1 and 16383)
                in seconds.  Enter 0 to disable the advertising timeout.
                
    @section EXAMPLE

    @code

    @endcode
*/
/**************************************************************************/
GapAdvertisingParams::GapAdvertisingParams(AdvertisingType advType, uint16_t interval, uint16_t timeout)
{
    _advType = advType;
    _interval = interval;
    _timeout = timeout;

    /* Interval checks */
    if (_advType == ADV_CONNECTABLE_DIRECTED)
    {
        /* Interval must be 0 in directed connectable mode */
        _interval = 0;
    }
    else if (_advType == ADV_NON_CONNECTABLE_UNDIRECTED)
    {
        /* Min interval is slightly larger than in other modes */
        if (_interval < GAP_ADV_PARAMS_INTERVAL_MIN_NONCON)
        {
            _interval = GAP_ADV_PARAMS_INTERVAL_MIN_NONCON;
        }
        if (_interval > GAP_ADV_PARAMS_INTERVAL_MAX)
        {
            _interval = GAP_ADV_PARAMS_INTERVAL_MAX;
        }
    }
    else
    {
        /* Stay within interval limits */
        if (_interval < GAP_ADV_PARAMS_INTERVAL_MIN)
        {
            _interval = GAP_ADV_PARAMS_INTERVAL_MIN;
        }
        if (_interval > GAP_ADV_PARAMS_INTERVAL_MAX)
        {
            _interval = GAP_ADV_PARAMS_INTERVAL_MAX;
        }
    }

    /* Timeout checks */
    if (timeout)
    {
        /* Stay within timeout limits */
        if (_timeout > GAP_ADV_PARAMS_TIMEOUT_MAX)
        {
            _timeout = GAP_ADV_PARAMS_TIMEOUT_MAX;
        }
    }
}

/**************************************************************************/
/*!
    Destructor
*/
/**************************************************************************/
GapAdvertisingParams::~GapAdvertisingParams(void)
{
}

/**************************************************************************/
/*!
    @brief returns the current Advertising Type value
*/
/**************************************************************************/
GapAdvertisingParams::AdvertisingType GapAdvertisingParams::getAdvertisingType(void)
{
    return _advType;
}

/**************************************************************************/
/*!
    @brief returns the current Advertising Delay (in units of 0.625ms)
*/
/**************************************************************************/
uint16_t GapAdvertisingParams::getInterval(void)
{
    return _interval;
}

/**************************************************************************/
/*!
    @brief returns the current Advertising Timeout (in seconds)
*/
/**************************************************************************/
uint16_t GapAdvertisingParams::getTimeout(void)
{
    return _timeout;
}
