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

class URIBeacon2Service {
public:
    // ee0c2080-8786-40ba-ab96-99b91ac981d8
    URIBeacon2Service(BLEDevice &ble_, const char *urldata, uint8_t flags = 0, uint8_t power = 0) : ble(ble_), payloadIndex(0), serviceDataPayload() {
        const uint8_t BEACON_UUID[] = {0xD8, 0xFE};

        serviceDataPayload[payloadIndex++] = BEACON_UUID[0];
        serviceDataPayload[payloadIndex++] = BEACON_UUID[1];
        serviceDataPayload[payloadIndex++] = flags;
        serviceDataPayload[payloadIndex++] = power;

        size_t sizeofURLData = strlen(urldata);
        size_t encodedBytes = encodeServiceData(urldata, sizeofURLData);

        ble.setTxPower(power);
        ble.accumulateAdvertisingPayload(GapAdvertisingData::COMPLETE_LIST_16BIT_SERVICE_IDS, BEACON_UUID, sizeof(BEACON_UUID));
        ble.accumulateAdvertisingPayload(GapAdvertisingData::SERVICE_DATA, serviceDataPayload, encodedBytes + 4);
    }

    void dumpEncodedSeviceData() const {
        printf("encoded: '");
        for (unsigned i = 0; i < payloadIndex; i++) {
            printf(" %02x", serviceDataPayload[i]);
        }
        printf("'\r\n");
    }

private:
    size_t encodeServiceData(const char *urldata, size_t sizeofURLData) {
        if (sizeofURLData == 0) {
            return 0;
        }

        return encodeURISchemePrefix(urldata, sizeofURLData) + encodeURI(urldata, sizeofURLData);
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
        while (sizeofURLData && (payloadIndex < MAX_SIZEOF_PAYLOAD)) {
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

    // URIBeacon2Service(BLEDevice &_ble, uint8_t level = 100) :
    //     ble(_ble),
    //     batteryLevel(level),
    //     batteryLevelCharacteristic(GattCharacteristic::UUID_BATTERY_LEVEL_CHAR, &batteryLevel, sizeof(batteryLevel), sizeof(batteryLevel),
    //                                GattCharacteristic::BLE_GATT_CHAR_PROPERTIES_READ | GattCharacteristic::BLE_GATT_CHAR_PROPERTIES_NOTIFY) {

    //     static bool serviceAdded = false; /* We should only ever need to add the heart rate service once. */
    //     if (serviceAdded) {
    //         return;
    //     }

    //     GattCharacteristic *charTable[] = {&batteryLevelCharacteristic};
    //     GattService         batteryService(GattService::UUID_BATTERY_SERVICE, charTable, sizeof(charTable) / sizeof(GattCharacteristic *));

    //     ble.addService(batteryService);
    //     serviceAdded = true;
    // }
    //

private:
    static const size_t MAX_SIZEOF_PAYLOAD = 32; /* TODO */

private:
    BLEDevice &ble;

    size_t     payloadIndex;
    uint8_t    serviceDataPayload[MAX_SIZEOF_PAYLOAD];
    // uint8_t              batteryLevel;
    // GattCharacteristic   batteryLevelCharacteristic;
};

#endif /* #ifndef __BLE_URI_BEACON_2_SERVICE_H__*/
