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

#ifndef __BLE_URI_BEACON_2_SERVICE_H__
#define __BLE_URI_BEACON_2_SERVICE_H__

#include "BLEDevice.h"

#define UUID_INITIALIZER_LIST(FIRST, SECOND) {         \
    0xee, 0x0c, FIRST, SECOND, 0x87, 0x86, 0x40, 0xba, \
    0xab, 0x96, 0x99, 0xb9, 0x1a, 0xc9, 0x81, 0xd8,    \
}
const uint8_t URIBeacon2ControlServiceUUID[] = UUID_INITIALIZER_LIST(0x20, 0x80);
const uint8_t lockedStateCharUUID[]          = UUID_INITIALIZER_LIST(0x20, 0x81);
const uint8_t uriDataCharUUID[]              = UUID_INITIALIZER_LIST(0x20, 0x84);

class URIBeacon2Service {
public:
    URIBeacon2Service(BLEDevice &ble_, const char *urldata, uint8_t flags_ = 0, uint8_t power_ = 0) :
        ble(ble_), payloadIndex(0), serviceDataPayload(),
        lockedState(false),
        uriDataLength(0),
        uriDataValue(),
        flags(flags_),
        power(power_),
        lockedStateChar(lockedStateCharUUID, (uint8_t *)&lockedState, 1, 1, GattCharacteristic::BLE_GATT_CHAR_PROPERTIES_READ),
        uriDataChar(uriDataCharUUID,
                    uriDataValue,
                    MAX_SIZE_URI_DATA_CHAR_VALUE,
                    MAX_SIZE_URI_DATA_CHAR_VALUE,
                    GattCharacteristic::BLE_GATT_CHAR_PROPERTIES_WRITE | GattCharacteristic::BLE_GATT_CHAR_PROPERTIES_WRITE_WITHOUT_RESPONSE)
    {
        if ((urldata == NULL) || ((uriDataLength = strlen(urldata)) == 0)) {
            return;
        }
        strncpy(reinterpret_cast<char *>(uriDataValue), urldata, MAX_SIZE_URI_DATA_CHAR_VALUE);

        setup();

        static bool serviceAdded = false; /* We should only ever need to add the heart rate service once. */
        if (serviceAdded) {
            return;
        }

        GattCharacteristic *charTable[] = {&lockedStateChar, &uriDataChar};
        GattService         beaconControlService(URIBeacon2ControlServiceUUID, charTable, sizeof(charTable) / sizeof(GattCharacteristic *));

        ble.addService(beaconControlService);
        serviceAdded = true;

        ble.onDataWritten(this, &URIBeacon2Service::onDataWritten);
    }

    /**
     * This callback allows the DFU service to receive the initial trigger to
     * handover control to the bootloader; but first the application is given a
     * chance to clean up.
     */
    virtual void onDataWritten(const GattCharacteristicWriteCBParams *params) {
        if (params->charHandle == uriDataChar.getValueAttribute().getHandle()) {
            /* we don't handle very large writes at the moment. */
            if ((params->offset != 0) || (params->len > MAX_SIZE_URI_DATA_CHAR_VALUE)) {
                return;
            }

            uriDataLength = params->len;
            memcpy(uriDataValue, params->data, uriDataLength);
            setup();
            ble.setAdvertisingPayload();
        }
    }

    /**
     * for debugging only
     */
    void dumpEncodedSeviceData() const {
        printf("encoded: '");
        for (unsigned i = 0; i < payloadIndex; i++) {
            printf(" %02x", serviceDataPayload[i]);
        }
        printf("'\r\n");
    }

private:
    void setup(void) {
        const uint8_t BEACON_UUID[] = {0xD8, 0xFE};

        payloadIndex = 0;
        serviceDataPayload[payloadIndex++] = BEACON_UUID[0];
        serviceDataPayload[payloadIndex++] = BEACON_UUID[1];
        serviceDataPayload[payloadIndex++] = flags;
        serviceDataPayload[payloadIndex++] = power;

        const char *urlData = reinterpret_cast<char *>(uriDataValue);
        size_t sizeofURLData = uriDataLength;
        size_t encodedBytes = encodeURISchemePrefix(urlData, sizeofURLData) + encodeURI(urlData, sizeofURLData);

        ble.clearAdvertisingPayload();
        ble.accumulateAdvertisingPayload(GapAdvertisingData::COMPLETE_LIST_16BIT_SERVICE_IDS, BEACON_UUID, sizeof(BEACON_UUID));
        ble.accumulateAdvertisingPayload(GapAdvertisingData::SERVICE_DATA, serviceDataPayload, encodedBytes + 4);

        ble.setTxPower(power);
    }

    size_t encodeURISchemePrefix(const char *&urldata, size_t &sizeofURLData) {
        const char *prefixes[] = {
            "http://www.",
            "https://www.",
            "http://",
            "https://",
            "urn:uuid:"
        };

        size_t encodedBytes = 0;
        const size_t NUM_PREFIXES = sizeof(prefixes) / sizeof(char *);
        for (unsigned i = 0; i < NUM_PREFIXES; i++) {
            size_t prefixLen = strlen(prefixes[i]);
            if (strncmp(urldata, prefixes[i], prefixLen) == 0) {
                serviceDataPayload[payloadIndex++] = i;
                encodedBytes = 1;

                urldata      += prefixLen;
                sizeofURLData -= prefixLen;
                break;
            }
        }

        return encodedBytes;
    }

    size_t encodeURI(const char *urldata, size_t sizeofURLData) {
        const char *suffixes[] = {
            ".com/",
            ".org/",
            ".edu/",
            ".net/",
            ".info/",
            ".biz/",
            ".gov/",
            ".com",
            ".org",
            ".edu",
            ".net",
            ".info",
            ".biz",
            ".gov"
        };
        const size_t NUM_SUFFIXES = sizeof(suffixes) / sizeof(char *);

        size_t encodedBytes = 0;
        while (sizeofURLData && (payloadIndex < MAX_SIZEOF_SERVICE_DATA_PAYLOAD)) {
            /* check for suffix match */
            unsigned i;
            for (i = 0; i < NUM_SUFFIXES; i++) {
                size_t suffixLen = strlen(suffixes[i]);
                if ((suffixLen == 0) || (sizeofURLData < suffixLen)) {
                    continue;
                }

                if (strncmp(urldata, suffixes[i], suffixLen) == 0) {
                    serviceDataPayload[payloadIndex++] = i;
                    ++encodedBytes;
                    urldata       += suffixLen;
                    sizeofURLData -= suffixLen;
                    break; /* from the for loop for checking against suffixes */
                }
            }
            /* This is the default case where we've got an ordinary character which doesn't match a suffix. */
            if (i == NUM_SUFFIXES) {
                serviceDataPayload[payloadIndex++] = *urldata;
                ++encodedBytes;
                ++urldata;
                --sizeofURLData;
            }
        }

        return encodedBytes;
    }

private:
    static const size_t MAX_SIZEOF_SERVICE_DATA_PAYLOAD = 27;
    static const size_t MAX_SIZE_URI_DATA_CHAR_VALUE    = 48;

private:
    BLEDevice &ble;

    size_t   payloadIndex;
    uint8_t  serviceDataPayload[MAX_SIZEOF_SERVICE_DATA_PAYLOAD];
    bool               lockedState;
    uint16_t uriDataLength;
    uint8_t  uriDataValue[MAX_SIZE_URI_DATA_CHAR_VALUE];
    uint8_t  flags;
    uint8_t  power;

    GattCharacteristic lockedStateChar;
    GattCharacteristic uriDataChar;
};

#endif /* #ifndef __BLE_URI_BEACON_2_SERVICE_H__*/
