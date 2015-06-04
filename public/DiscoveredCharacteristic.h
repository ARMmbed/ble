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

#ifndef __DISCOVERED_CHARACTERISTIC_H__
#define __DISCOVERED_CHARACTERISTIC_H__

#include "UUID.h"
#include "GattAttribute.h"
#include "GattClient.h"

/**
 * Structure for holding information about the service and the characteristics
 * found during the discovery process.
 */
class DiscoveredCharacteristic {
public:
    struct Properties_t {
        uint8_t _broadcast       :1; /**< Broadcasting of the value permitted. */
        uint8_t _read            :1; /**< Reading the value permitted. */
        uint8_t _writeWoResp     :1; /**< Writing the value with Write Command permitted. */
        uint8_t _write           :1; /**< Writing the value with Write Request permitted. */
        uint8_t _notify          :1; /**< Notications of the value permitted. */
        uint8_t _indicate        :1; /**< Indications of the value permitted. */
        uint8_t _authSignedWrite :1; /**< Writing the value with Signed Write Command permitted. */

    public:
        bool broadcast(void)       const {return _broadcast;      }
        bool read(void)            const {return _read;           }
        bool writeWoResp(void)     const {return _writeWoResp;    }
        bool write(void)           const {return _write;          }
        bool notify(void)          const {return _notify;         }
        bool indicate(void)        const {return _indicate;       }
        bool authSignedWrite(void) const {return _authSignedWrite;}

    private:
        operator uint8_t()  const; /* disallow implicit conversion into an integer */
        operator unsigned() const; /* disallow implicit conversion into an integer */
    };

    struct ReadResponse_t {
        GattAttribute::Handle_t  handle; /**< Attribute Handle. */
        uint16_t                 offset; /**< Offset of the attribute data. */
        uint16_t                 len;    /**< Attribute data length. */
        const uint8_t           *data;   /**< Attribute data, variable length. */
    };
    typedef void (*ReadCallback_t)(const ReadResponse_t *params);

    /**
     * Initiate (or continue) a read for the value attribute, optionally at a
     * given offset. If the Characteristic or Descriptor to be read is longer
     * than ATT_MTU - 1, this function must be called multiple times with
     * appropriate offset to read the complete value.
     *
     * @return BLE_ERROR_NONE if a read has been initiated, else
     *         BLE_ERROR_INVALID_STATE if some internal state about the connection is invalid, or
     *         BLE_STACK_BUSY if some client procedure already in progress, or
     *         BLE_ERROR_OPERATION_NOT_PERMITTED due to the characteristic's properties.
     */
    ble_error_t read(uint16_t offset = 0) const {
        if (!props.read()) {
            return BLE_ERROR_OPERATION_NOT_PERMITTED;
        }

        return gattc->read(connHandle, valueHandle, offset);
    }

    /**
     * Perform a write without response procedure.
     *
     * @param  length
     *           The amount of data being written.
     * @param  value
     *           The bytes being written.
     *
     * @note   It is important to note that a write without response will
     *         <b>consume an application buffer</b>, and will therefore
     *         generate a onDataSent() callback when the packet has been
     *         transmitted. The application should ensure that the buffer is
     *         valid until the callback.
     *
     * @retval BLE_ERROR_NONE Successfully started the Write procedure, else
     *         BLE_ERROR_INVALID_STATE if some internal state about the connection is invalid, or
     *         BLE_STACK_BUSY if some client procedure already in progress, or
     *         BLE_ERROR_NO_MEM if there are no available buffers left to process the request, or
     *         BLE_ERROR_OPERATION_NOT_PERMITTED due to the characteristic's properties.
     */
    ble_error_t writeWoResponse(uint16_t length, const uint8_t *value) const {
        if (!props.writeWoResp()) {
            return BLE_ERROR_OPERATION_NOT_PERMITTED;
        }

        return BLE_ERROR_NONE;
        // return (ble.getGattClient())->write(BLE_GATT_OP_WRITE_CMD, connHandle, length, value);
    }

    static void setupOnDataRead(ReadCallback_t callback) {
        onDataReadCallback = callback;
    }

    void setupLongUUID(UUID::LongUUIDBytes_t longUUID) {
        uuid.setupLong(longUUID);
    }

public:
    UUID::ShortUUIDBytes_t getShortUUID(void) const {
        return uuid.getShortUUID();
    }

    const Properties_t& getProperties(void) const {
        return props;
    }

    const GattAttribute::Handle_t& getDeclHandle(void) const {
        return declHandle;
    }
    const GattAttribute::Handle_t& getValueHandle(void) const {
        return valueHandle;
    }

public:
    DiscoveredCharacteristic() : gattc(NULL),
                                 uuid(UUID::ShortUUIDBytes_t(0)),
                                 props(),
                                 declHandle(GattAttribute::INVALID_HANDLE),
                                 valueHandle(GattAttribute::INVALID_HANDLE) {
        /* empty */
    }

protected:
    GattClient             *gattc;

protected:
    UUID                    uuid;
    Properties_t            props;
    GattAttribute::Handle_t declHandle;
    GattAttribute::Handle_t valueHandle;

    Gap::Handle_t           connHandle;

public:
    static ReadCallback_t   onDataReadCallback;
};

#endif /*__DISCOVERED_CHARACTERISTIC_H__*/
