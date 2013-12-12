#include <stdio.h>
#include <string.h>

#include "blecommon.h"
#include "GapAdvertisingParams.h"

/**************************************************************************/
/*!
    @brief  Instantiates a new GapAdvertisingParams instance

    @param[in]  connectionMode
                The GAP connection mode to use for this device. Valid
                values are defined in \ref ConnectionMode

                @para
                NON_CONNECTABLE - All connections to the peripheral device
                will be refused.
                
                @para
                DIRECTED_CONNECTABLE - Only connections from a pre-defined
                central device will be accepted.
                
                @para
                UNDIRECTED_CONNECTABLE - Any central device can connect to
                this peripheral.
                
                @note See Bluetooth Core Specification 4.0 (Vol. 3),
                Part C, Section 9.3 for further information on GAP
                connection modes

    @param[in]  interval
                Advertising interval between 0x20 and 0x4000 (32 and 16384)
                in 0.625ms intervals (20ms to 10.24s).

                @para
                Increasing this value will allow central devices to detect
                your peripheral faster at the expense of more power being
                used by the radio due to the higher data transmit rate.
                
                @note This field must be set to 0 if connectionMode is equal
                to \ref DIRECTED_CONNECTABLE

    @param[in]  timeout
                Advertising timeout between 0x1 and 0x3FFF (1 and 16383)
                in seconds.  Enter 0 to disable the advertising timeout.
                
    @section EXAMPLE

    @code

    @endcode
*/
/**************************************************************************/
GapAdvertisingParams::GapAdvertisingParams(ConnectionMode connectionMode, uint16_t interval, uint16_t timeout)
{
    _connectionMode = connectionMode;
    _interval = interval;
    _timeout = timeout;

    /* Interval checks */
    if (_connectionMode == DIRECTED_CONNECTABLE)
    {
        /* Interval must be 0 in directed connectable mode */
        _interval = 0;
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
