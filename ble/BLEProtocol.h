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

#ifndef __BLE_PROTOCOL_H__
#define __BLE_PROTOCOL_H__

#include <stddef.h>
#include <stdint.h>
#include <algorithm>

/**
 * A common namespace for types and constants used everywhere in BLE API.
 */
namespace BLEProtocol {
    /**<
     * A simple container for the enumeration of address-types for Protocol addresses.
     *
     * Adding a struct to encapsulate the contained enumeration prevents
     * polluting the BLEProtocol namespace with the enumerated values. It also
     * allows type-aliases for the enumeration while retaining the enumerated
     * values. i.e.
     *
     * doing:
     *       typedef AddressType AliasedType;
     *
     * would allow the use of AliasedType::PUBLIC in code.
     */
    struct AddressType {
        /**< Address-types for Protocol addresses. */
        enum Type {
            PUBLIC = 0,
            RANDOM_STATIC,
            RANDOM_PRIVATE_RESOLVABLE,
            RANDOM_PRIVATE_NON_RESOLVABLE
        };
    };
    typedef AddressType::Type AddressType_t;   /**< Alias for AddressType::Type */

    static const size_t ADDR_LEN = 6;         /**< Length (in octets) of the BLE MAC address. */
    typedef uint8_t AddressBytes_t[ADDR_LEN]; /**< 48-bit address bytes, in LSB format. */

    /**
     * BLE address. It contains an address-type (@ref AddressType_t) and bytes (@ref AddressBytes_t).
     */
    struct Address_t {
        AddressType_t  type;    /**< @ref AddressType_t */
        AddressBytes_t address; /**< @ref AddressBytes_t */

        Address_t(AddressType_t typeIn, const AddressBytes_t addressIn) : type(typeIn) {
            std::copy(addressIn, addressIn + ADDR_LEN, address);
        }
    };

    static const size_t SECURITY_KEY_LEN = 16; /**@brief GAP Security Key Length. */
    typedef uint8_t Irk_t[SECURITY_KEY_LEN];   /**@brief Identity Resolving Key. */

    /**
     * Whitelisting is an important feature available in BLE. White-lists allow
     * hosts to filter devices when advertising, scanning, and establishing
     * connections on both sides. White lists are simply arrays of Bluetooth
     * device addresses that are populated by the host (i.e. the processor where
     * the application is running) and stored and used in the controller (i.e.
     * Bluetooth controller).
     *
     * A device scanning or initiating a connection can use a white list to
     * limit the number of devices that will be detected or with which it can
     * connect, and the advertising device can use a white list to specify which
     * peers it will accept an incoming connection from. This is especially
     * useful when searching for a few known devices in a fog of advertising
     * packets in busy locations.
     *
     * Any advertising packets (in the case of a scanner) or connection request
     * packets (in the case of an advertiser) received from devices whose
     * Bluetooth Address is not present in the white list will simply be
     * dropped.
     *
     * @note A whitelist has a fixed size. There will be some upper limit
     *     imposed by the underlying BLE stack.
     *
     * @note It is not possible to change the contents of the whitelist while it
     *     is being used. For example, if the device is advertising and using
     *     the whitelist to filter the devices to and from which it will respond
     *     to scan requests or connection requests, the whitelist cannot be
     *     changed until advertising is disabled. The whitelist can then be
     *     changed and advertising re-enabled.
     */
    struct Whitelist_t {
        AddressBytes_t *addrs[];  /**< Pointer to an array of device address pointers, pointing to addresses to be used in whitelist.
                                       NULL if none are given. */
        uint8_t        addrCount; /**< Count of device addresses in the array `addrs`.
                                       @note There will be some upper limit to this size imposed by the underlying BLE stack. */
        Irk_t         *irks;      /**< Pointer to an array of Identity Resolving Key (IRK) pointers, each pointing
                                       to an IRK in the whitelist. NULL if none are given. */
        uint8_t        irkCount;  /**< Count of IRKs in array.
                                       @note There will be some upper limit to this size imposed by the underlying BLE stack. */
    };
};

#endif /* __BLE_PROTOCOL_H__ */
