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

/**
 * A common namespace for types and constants used everywhere in BLE API.
 */
namespace BLEProtocol {
    /**< Address-type for Protocol addresses. */
    struct AddressType { /* Adding a struct to encapsulate the contained enumeration
                            * prevents polluting the BLEProtocol namespace with the
                            * enumerated values. It also allows type-aliases for the
                            * enumeration while retaining the enumerated values. i.e.
                            *
                            * doing:
                            *       typedef AddressType_t AliasedType_t;
                            * would allow the use of AliasedType_t::PUBLIC in code.
                            */
        enum Type {
            PUBLIC = 0,
            RANDOM_STATIC,
            RANDOM_PRIVATE_RESOLVABLE,
            RANDOM_PRIVATE_NON_RESOLVABLE
        };
    };

    static const size_t ADDR_LEN = 6;    /**< Length (in octets) of the BLE MAC address. */
    typedef uint8_t Address_t[ADDR_LEN]; /**< 48-bit address, in LSB format. */
};

#endif /* __BLE_PROTOCOL_H__ */
