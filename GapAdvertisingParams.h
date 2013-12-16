#ifndef __GAP_ADVERTISING_PARAMS_H__
#define __GAP_ADVERTISING_PARAMS_H__

#include "blecommon.h"

#define GAP_ADV_PARAMS_INTERVAL_MIN     (0x0020)
#define GAP_ADV_PARAMS_INTERVAL_MAX     (0x4000)
#define GAP_ADV_PARAMS_TIMEOUT_MAX      (0x3FFF)

class GapAdvertisingParams
{
  public:
    /* See Bluetooth Core Specification 4.0 (Vol. 6), Part B, Section 2.3.1 */
    /* See Bluetooth Core Specification 4.0 (Vol. 3), Part C, Section 9.3 */
    enum AdvertisingType
    {
      ADV_CONNECTABLE_UNDIRECTED,       /**< Vol 3, Part C, Section 9.3.4 and Vol 6, Part B, Section 2.3.1.1 */
      ADV_CONNECTABLE_DIRECTED,         /**< Vol 3, Part C, Section 9.3.3 and Vol 6, Part B, Section 2.3.1.2 */
      ADV_NON_CONNECTABLE_UNDIRECTED,   /**< Vol 3, Part C, Section 9.3.2 and Vol 6, Part B, Section 2.3.1.3 */
      ADV_SCANNABLE_UNDIRECTED          /**< Include support for Scan Response payloads, see Vol 6, Part B, Section 2.3.1.4 */
    };
  
    GapAdvertisingParams(AdvertisingType advType = GapAdvertisingParams::ADV_CONNECTABLE_UNDIRECTED,
                         uint16_t interval = GAP_ADV_PARAMS_INTERVAL_MIN,
                         uint16_t timeout = 0);
    virtual ~GapAdvertisingParams(void);
    
    virtual AdvertisingType getAdvertisingType(void);

  private:
    AdvertisingType  _advType;
    uint16_t _interval;
    uint16_t _timeout;
};

#endif
