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

class GapScanningParams {
public:
    static const unsigned SCAN_INTERVAL_MIN = 0x0004; /**< Minimum Scan interval in 625us units - 2.5ms. */
    static const unsigned SCAN_INTERVAL_MAX = 0x4000; /**< Maximum Scan interval in 625us units - 10.24s. */
    static const unsigned SCAN_WINDOW_MIN   = 0x0004; /**< Minimum Scan window in 625us units - 2.5ms. */
    static const unsigned SCAN_WINDOW_MAX   = 0x4000; /**< Maximum Scan window in 625us units - 10.24s. */
    static const unsigned SCAN_TIMEOUT_MIN  = 0x0001; /**< Minimum Scan timeout in seconds. */
    static const unsigned SCAN_TIMEOUT_MAX  = 0xFFFF; /**< Maximum Scan timeout in seconds. */

public:
    GapScanningParams(uint16_t interval       = SCAN_INTERVAL_MAX,
                      uint16_t window         = SCAN_WINDOW_MAX,
                      uint16_t timeout        = 0,
                      bool     activeScanning = false);

    static const uint16_t UNIT_0_625_MS = 625;  /**< Number of microseconds in 0.625 milliseconds. */
    static uint16_t MSEC_TO_SCAN_DURATION_UNITS(uint32_t durationInMillis) {
        return (durationInMillis * 1000) / UNIT_0_625_MS;
    }

    ble_error_t setInterval(uint16_t newIntervalInMS);

    ble_error_t setWindow(uint16_t newWindowInMS);

    ble_error_t setTimeout(uint16_t newTimeout);

    void        setActiveScanning(bool activeScanning);

public:
    /* @Note: The following return durations in units of 0.625ms */
    uint16_t getInterval(void)       const {return _interval;      }
    uint16_t getWindow(void)         const {return _window;        }

    uint16_t getTimeout(void)        const {return _timeout;       }
    bool     getActiveScanning(void) const {return _activeScanning;}

    /**
     * Accessor for whiteList.
     */
    const BLEProtocol::Whitelist_t *getWhitelist(void) const {
        return _whiteList;
    }

    /**
     * Set the whiteList to be used for scanning. See @ref
     * BLEProtocol::Whitelist_t. When used for a Central, advertisements are
     * filtered by the white-list.
     *
     * @param [in] whiteList The white-list to be used for connections
     *                 advertisements. If passed in as NULL, the existing
     *                 whitelist (if any) remains in effect.
     *
     * @note The contents of the structure holding the white-list should remain
     *     valid at least until scanning or a connection request is issued.
     *
     * @note It is not possible to change the contents of the whitelist while it
     *     is being used. For example, if the device is waiting for a connection
     *     and using the whitelist to filter the devices to and from which it
     *     will respond, the whitelist cannot be changed until the
     *     connection/scan request times out. The whitelist can then be changed
     *     and scanning re-enabled.
     *
     * @note this API is experimental.
     */
    void setWhitelist(const BLEProtocol::Whitelist_t *whiteList) {
        _whiteList = whiteList;
    }

private:
    uint16_t                        _interval;       /**< Scan interval in units of 625us (between 2.5ms and 10.24s). */
    uint16_t                        _window;         /**< Scan window in units of 625us (between 2.5ms and 10.24s). */
    uint16_t                        _timeout;        /**< Scan timeout between 0x0001 and 0xFFFF in seconds; 0x0000 disables timeout. */
    bool                            _activeScanning; /**< Obtain the peer device's advertising data and (if possible) scanResponse. */
    const BLEProtocol::Whitelist_t *_whiteList;      /**< white list associated with scanning. \ref BLEProtocol::Whitelist_t */

private:
    /* Disallow copy constructor. */
    GapScanningParams(const GapScanningParams &);
    GapScanningParams& operator =(const GapScanningParams &in);
};

#endif // ifndef __GAP_SCANNING_PARAMS_H__
