#ifndef __GAP_ADVERTISING_PARAMS_H__
#define __GAP_ADVERTISING_PARAMS_H__

#include "blecommon.h"

#define GAP_ADV_PARAMS_INTERVAL_MIN     (0x0020)
#define GAP_ADV_PARAMS_INTERVAL_MAX     (0x4000)
#define GAP_ADV_PARAMS_TIMEOUT_MAX      (0x3FFF)

class GapAdvertisingParams
{
  public:
    /* See Bluetooth Core Specification 4.0 (Vol. 3), Part C, Section 9.3 */
    enum ConnectionMode
    {
      NON_CONNECTABLE,          /**< Section 9.3.2 */
      DIRECTED_CONNECTABLE,     /**< Section 9.3.3 */
      UNDIRECTED_CONNECTABLE    /**< Section 9.3.4 */
    };
  
    GapAdvertisingParams(ConnectionMode connectionMode = GapAdvertisingParams::UNDIRECTED_CONNECTABLE,
                         uint16_t interval = GAP_ADV_PARAMS_INTERVAL_MIN, 
                         uint16_t timeout = 0);
    virtual ~GapAdvertisingParams(void);

  private:
    ConnectionMode  _connectionMode;
    uint16_t _interval;
    uint16_t _timeout;
};

#endif
