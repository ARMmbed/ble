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

/**@brief Structure for holding information about the service and the characteristics found during
 *        the discovery process.
 */
class DiscoveredCharacteristic {
public:
    struct Properties_t {
        static const uint8_t BROADCAST_PROPERTY_MASK         = 0x01;
        static const uint8_t READ_PROPERTY_MASK              = 0x02;
        static const uint8_t WRITE_WO_RESPONSE_PROPERTY_MASK = 0x04;
        static const uint8_t WRITE_PROPERTY_MASK             = 0x08;
        static const uint8_t NOTIFY_PROPERTY_MASK            = 0x10;
        static const uint8_t INDICATE_PROPERTY_MASK          = 0x20;
        static const uint8_t AUTH_SIGNED_PROPERTY_MASK       = 0x40;

        Properties_t() : broadcast(0), read(0), write_wo_resp(0), write(0), notify(0), indicate(0), auth_signed_wr(0) {
            /* empty */
        }

        Properties_t(uint8_t props) :
            broadcast(props & BROADCAST_PROPERTY_MASK),
            read(props & READ_PROPERTY_MASK),
            write_wo_resp(props & WRITE_WO_RESPONSE_PROPERTY_MASK),
            write(props & WRITE_PROPERTY_MASK),
            notify(props & NOTIFY_PROPERTY_MASK),
            indicate(props & INDICATE_PROPERTY_MASK),
            auth_signed_wr(props & AUTH_SIGNED_PROPERTY_MASK) {
            /* empty*/
        }

        uint8_t broadcast      :1; /**< Broadcasting of the value permitted. */
        uint8_t read           :1; /**< Reading the value permitted. */
        uint8_t write_wo_resp  :1; /**< Writing the value with Write Command permitted. */
        uint8_t write          :1; /**< Writing the value with Write Request permitted. */
        uint8_t notify         :1; /**< Notications of the value permitted. */
        uint8_t indicate       :1; /**< Indications of the value permitted. */
        uint8_t auth_signed_wr :1; /**< Writing the value with Signed Write Command permitted. */
    };

    struct ReadResponse_t {
        GattAttribute::Handle_t  handle; /**< Attribute Handle. */
        uint16_t                 offset; /**< Offset of the attribute data. */
        uint16_t                 len;    /**< Attribute data length. */
        const uint8_t           *data;   /**< Attribute data, variable length. */
    };
    typedef void (*ReadCallback_t)(const ReadResponse_t *params);

    static void setupOnDataRead(ReadCallback_t callback) {
        onDataReadCallback = callback;
    }

    /**
     * Initiate (or continue) a read for the value attribute, optionally at a
     * given offset. If the Characteristic or Descriptor to be read is longer
     * than ATT_MTU - 1, this function must be called multiple times with
     * appropriate offset to read the complete value.
     *
     * @return BLE_ERROR_NONE if a read has been initiated, else
     *         BLE_ERROR_INVALID_STATE if some internal state about the connection is invalid, or
     *         BLE_STACK_BUSY if some client procedure already in progress.
     */
    virtual ble_error_t read(uint16_t offset = 0) = 0;

public:
    void setup(Properties_t            propsIn,
               GattAttribute::Handle_t declHandleIn,
               GattAttribute::Handle_t valueHandleIn) {
        props       = propsIn;
        declHandle  = declHandleIn;
        valueHandle = valueHandleIn;
    }

    void setup(UUID::ShortUUIDBytes_t  uuidIn,
               Properties_t            propsIn,
               GattAttribute::Handle_t declHandleIn,
               GattAttribute::Handle_t valueHandleIn) {
        uuid        = uuidIn;
        props       = propsIn;
        declHandle  = declHandleIn;
        valueHandle = valueHandleIn;
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
    DiscoveredCharacteristic() : uuid(UUID::ShortUUIDBytes_t(0)),
                                 props(),
                                 declHandle(GattAttribute::INVALID_HANDLE),
                                 valueHandle(GattAttribute::INVALID_HANDLE) {
        /* empty */
    }

private:
    UUID                    uuid;
    Properties_t            props;
    GattAttribute::Handle_t declHandle;
    GattAttribute::Handle_t valueHandle;

    static ReadCallback_t   onDataReadCallback;
};

#endif /*__DISCOVERED_CHARACTERISTIC_H__*/
