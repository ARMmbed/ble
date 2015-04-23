/* mbed Microcontroller Library
 * Copyright (c) 2006-2013 ARM Limited
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
*/

#ifndef __GAP_SCANNING_PARAMS_H__
#define __GAP_SCANNING_PARAMS_H__

#include "Gap.h"

class GapScanningParams {
public:
    static const unsigned SCAN_INTERVAL_MIN = 0x0004; /**< Minimum Scan interval in 625 us units, i.e. 2.5 ms. */
    static const unsigned SCAN_INTERVAL_MAX = 0x4000; /**< Maximum Scan interval in 625 us units, i.e. 10.24 s. */
    static const unsigned SCAN_WINDOW_MIN   = 0x0004; /**< Minimum Scan window in 625 us units, i.e. 2.5 ms. */
    static const unsigned SCAN_WINDOW_MAX   = 0x4000; /**< Maximum Scan window in 625 us units, i.e. 10.24 s. */
    static const unsigned SCAN_TIMEOUT_MIN  = 0x0001; /**< Minimum Scan timeout in seconds. */
    static const unsigned SCAN_TIMEOUT_MAX  = 0xFFFF; /**< Maximum Scan timeout in seconds. */

public:
    GapScanningParams(uint16_t interval = SCAN_INTERVAL_MAX,
                      uint16_t window   = SCAN_WINDOW_MAX,
                      uint16_t timeout  = 0) : _interval(interval), _window(window), _timeout(timeout) {
        /* stay within limits */
        if (_interval < SCAN_INTERVAL_MIN) {
            _interval = SCAN_INTERVAL_MIN;
        }
        if (_interval > SCAN_INTERVAL_MAX) {
            _interval = SCAN_INTERVAL_MAX;
        }
        if (_timeout > SCAN_TIMEOUT_MAX) {
            _timeout = SCAN_TIMEOUT_MAX;
        }
    }

    void setInterval(uint16_t newInterval) {_interval = newInterval;}
    void setWindow(uint16_t newInterval)   {_window   = newWindow;  }
    void setTimeout(uint16_t newTimeout)   {_timeout  = newTimeout; }

    uint16_t getInterval(void) const {return _interval;}
    uint16_t getWindow(void)   const {return _window;  }
    uint16_t getTimeout(void)  const {return _timeout; }

private:
    uint16_t _interval; /**< Scan interval (between 2.5ms to 10.24s). */
    uint16_t _window;   /**< Scan window (between 2.5ms to 10.24s). */
    uint16_t _timeout;  /**< Scan timeout between 0x0001 and 0xFFFF in seconds, 0x0000 disables timeout. */

private:
    /* disallow copy constructor */
    GapScanningParams(const GapScanningParams &);
    GapScanningParams& operator =(const GapScanningParams &in);
};

#endif // ifndef __GAP_SCANNING_PARAMS_H__
