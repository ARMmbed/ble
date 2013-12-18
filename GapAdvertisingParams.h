#ifndef __GAP_ADVERTISING_PARAMS_H__
#define __GAP_ADVERTISING_PARAMS_H__

#include "blecommon.h"

#define GAP_ADV_PARAMS_INTERVAL_MIN        (0x0020)
#define GAP_ADV_PARAMS_INTERVAL_MIN_NONCON (0x00A0)
#define GAP_ADV_PARAMS_INTERVAL_MAX        (0x1000)
#define GAP_ADV_PARAMS_TIMEOUT_MAX         (0x3FFF)

/**************************************************************************/
/*!
    \brief
    This class provides a wrapper for the core advertising parameters,
    including the advertising type (Connectable Undirected,
    Non Connectable Undirected, etc.), as well as the advertising and
    timeout intervals.

    \par
    See the following for more information on advertising types:
    
    \li \c Bluetooth Core Specification 4.0 (Vol. 6), Part B, Section 2.3.1
    \li \c Bluetooth Core Specification 4.0 (Vol. 3), Part C, Section 9.3

    \par EXAMPLE
    
    \code
    
    // ToDo
    
    \endcode
*/
/**************************************************************************/
class GapAdvertisingParams
{
  public:
    /* See Bluetooth Core Specification 4.0 (Vol. 6), Part B, Section 2.3.1 */
    /* See Bluetooth Core Specification 4.0 (Vol. 3), Part C, Section 9.3 */
    enum AdvertisingType
    {
      ADV_CONNECTABLE_UNDIRECTED,       /**< Vol 3, Part C, Section 9.3.4 and Vol 6, Part B, Section 2.3.1.1 */
      ADV_CONNECTABLE_DIRECTED,         /**< Vol 3, Part C, Section 9.3.3 and Vol 6, Part B, Section 2.3.1.2 */
      ADV_SCANNABLE_UNDIRECTED,         /**< Include support for Scan Response payloads, see Vol 6, Part B, Section 2.3.1.4 */
      ADV_NON_CONNECTABLE_UNDIRECTED    /**< Vol 3, Part C, Section 9.3.2 and Vol 6, Part B, Section 2.3.1.3 */
    };
  
    GapAdvertisingParams(AdvertisingType advType = GapAdvertisingParams::ADV_CONNECTABLE_UNDIRECTED,
                         uint16_t interval = GAP_ADV_PARAMS_INTERVAL_MIN_NONCON,
                         uint16_t timeout = 0);
    virtual ~GapAdvertisingParams(void);
    
    virtual AdvertisingType getAdvertisingType(void);
    virtual uint16_t        getInterval(void);
    virtual uint16_t        getTimeout(void);

  private:
    AdvertisingType  _advType;
    uint16_t _interval;
    uint16_t _timeout;
};

#endif
