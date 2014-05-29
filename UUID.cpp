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


#include <stdio.h>
#include <string.h>

#include "UUID.h"

/**************************************************************************/
/*!
    @brief  Creates an empty 128-bit UUID

    @note   This UUID must be assigned a valid value via the 'update'
            function before it can be safely used!
*/
/**************************************************************************/
UUID::UUID(void) : type(UUID_TYPE_SHORT),
                   base(),
                   value(0)
{
    /* empty */
}

/**************************************************************************/
/*!
    @brief  Creates a new 128-bit UUID

    @note   The UUID is a unique 128-bit (16 byte) ID used to identify
            different service or characteristics on the BLE device.

    @note   When creating a UUID, the constructor will check if all bytes
            except bytes 2/3 are equal to 0.  If only bytes 2/3 have a
            value, the UUID will be treated as a short/BLE UUID, and the
            .type field will be set to UUID::UUID_TYPE_SHORT.  If any
            of the bytes outside byte 2/3 have a non-zero value, the UUID
            will be considered a 128-bit ID, and .type will be assigned
            as UUID::UUID_TYPE_LONG.

    @param[in]  uuid_base
                The 128-bit (16-byte) UUID value.  For 128-bit values,
                assign all 16 bytes.  For 16-bit values, assign the
                16-bits to byte 2 and 3, and leave the rest of the bytes
                as 0.

    @section EXAMPLE

    @code

    // Create a short UUID (0x180F)
    uint8_t shortID[16] = { 0, 0, 0x0F, 0x18, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
    UUID ble_uuid = UUID(shortID);
    // ble_uuid.type  = UUID_TYPE_SHORT
    // ble_uuid.value = 0x180F

    // Creeate a long UUID
    uint8_t longID[16] = { 0x00, 0x11, 0x22, 0x33,
                           0x44, 0x55, 0x66, 0x77,
                           0x88, 0x99, 0xAA, 0xBB,
                           0xCC, 0xDD, 0xEE, 0xFF };
    UUID custom_uuid = UUID(longID);
    // custom_uuid.type  = UUID_TYPE_LONG
    // custom_uuid.value = 0x3322
    // custom_uuid.base  = 00 11 22 33 44 55 66 77 88 99 AA BB CC DD EE FF

    @endcode
*/
/**************************************************************************/
UUID::UUID(const uint8_t uuid_base[LENGTH_OF_LONG_UUID]) :
                                        type(UUID_TYPE_SHORT),
                                        base(),
                                        value(0)
{
    memcpy(base, uuid_base, LENGTH_OF_LONG_UUID);
    value = (uint16_t)((uuid_base[3] << 8) | (uuid_base[2]));

    /* Check if this is a short of a long UUID */
    if (uuid_base[0] + uuid_base[1] +
        uuid_base[4] + uuid_base[5] + uuid_base[6] + uuid_base[7] +
        uuid_base[8] + uuid_base[9] + uuid_base[10] + uuid_base[11] +
        uuid_base[12] + uuid_base[13] + uuid_base[14] + uuid_base[15] == 0) {
        type = UUID_TYPE_SHORT;
    } else {
        type = UUID_TYPE_LONG;
    }
}

/**************************************************************************/
/*!
    @brief  Creates a short (16-bit) UUID

    @param[in]  ble_uuid
                The 16-bit BLE UUID value.
*/
/**************************************************************************/
UUID::UUID(const uint16_t ble_uuid) : type(UUID_TYPE_SHORT),
                                      base(),
                                      value(ble_uuid)
{
    memcpy(base + 2, (uint8_t *)&ble_uuid, 2);
}

/**************************************************************************/
/*!
    @brief  UUID destructor
*/
/**************************************************************************/
UUID::~UUID(void)
{
}
