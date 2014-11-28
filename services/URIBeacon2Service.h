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
const uint8_t flagsCharUUID[]                = UUID_INITIALIZER_LIST(0x20, 0x85);
const uint8_t txPowerLevelsCharUUID[]        = UUID_INITIALIZER_LIST(0x20, 0x86);
const uint8_t beaconPeriodCharUUID[]         = UUID_INITIALIZER_LIST(0x20, 0x88);

class URIBeacon2Service {
    enum TXPowerModes_t {
        TX_POWER_MODE_LOWEST = 0,
        TX_POWER_MODE_LOW    = 1,
        TX_POWER_MODE_MEDIUM = 2,
        TX_POWER_MODE_HIGH   = 3,
        NUM_POWER_MODES
    };

public:
    /**
     * @param[ref] ble
     *                 BLEDevice object for the underlying controller.
     * @param[in]  urldata
     *                 URI as a null-terminated string.
     * @param[in]  flagsIn
     *                 UriBeacon Flags.
     * @param[in]  effectiveTxPowerIn
     *                 UriBeacon Tx Power Level in dBm.
     * @param[in]  beaconPeriodIn
     *                 The period in milliseconds that a UriBeacon packet is
     *                 transmitted. A value of zero disables UriBeacon
     *                 transmissions.
     */
    URIBeacon2Service(BLEDevice &ble_, const char *urldata, uint8_t flagsIn = 0, int8_t effectiveTxPowerIn = 0, uint16_t beaconPeriodIn = 1000) :
        ble(ble_),
        payloadIndex(0),
        serviceDataPayload(),
        lockedState(false),
        uriDataLength(0),
        uriDataValue(),
        flags(flagsIn),
        effectivePower(effectiveTxPowerIn),
        powerLevels(),
        beaconPeriod(Gap::MSEC_TO_ADVERTISEMENT_DURATION_UNITS(beaconPeriodIn)),
        lockedStateChar(lockedStateCharUUID, (uint8_t *)&lockedState, 1, 1, GattCharacteristic::BLE_GATT_CHAR_PROPERTIES_READ),
        uriDataChar(uriDataCharUUID,
                    uriDataValue,
                    MAX_SIZE_URI_DATA_CHAR_VALUE,
                    MAX_SIZE_URI_DATA_CHAR_VALUE,
                    GattCharacteristic::BLE_GATT_CHAR_PROPERTIES_WRITE | GattCharacteristic::BLE_GATT_CHAR_PROPERTIES_WRITE_WITHOUT_RESPONSE),
        flagsChar(flagsCharUUID, &flags, 1, 1, GattCharacteristic::BLE_GATT_CHAR_PROPERTIES_READ | GattCharacteristic::BLE_GATT_CHAR_PROPERTIES_WRITE),
        txPowerLevelsChar(txPowerLevelsCharUUID,
                          reinterpret_cast<uint8_t *>(powerLevels),
                          NUM_POWER_MODES * sizeof(int8_t),
                          NUM_POWER_MODES * sizeof(int8_t),
                          GattCharacteristic::BLE_GATT_CHAR_PROPERTIES_READ | GattCharacteristic::BLE_GATT_CHAR_PROPERTIES_WRITE),
        beaconPeriodChar(beaconPeriodCharUUID, (uint8_t *)&beaconPeriod, 2, 2,
                         GattCharacteristic::BLE_GATT_CHAR_PROPERTIES_READ | GattCharacteristic::BLE_GATT_CHAR_PROPERTIES_WRITE)
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

        GattCharacteristic *charTable[] = {&lockedStateChar, &uriDataChar, &flagsChar, &txPowerLevelsChar, &beaconPeriodChar};
        GattService         beaconControlService(URIBeacon2ControlServiceUUID, charTable, sizeof(charTable) / sizeof(GattCharacteristic *));

        ble.addService(beaconControlService);
        serviceAdded = true;

        ble.onDataWritten(this, &URIBeacon2Service::onDataWritten);
    }

    /**
     * Update flags of the URIBeacon dynamically.
     *
     * @param[in] flagsIn
     *
     *     ### UriBeacon Flags
     *     Bit   | Description
     *     :---- | :----------
     *     0     | Invisible Hint
     *     1..7  | Reserved for future use. Must be zero.
     *
     *     The `Invisible Hint` flag is a command for the user-agent that tells
     *     it not to access or display the UriBeacon. This is a guideline only,
     *     and is not a blocking method. User agents may, with user approval,
     *     display invisible beacons.
     */
    void setFlags(uint8_t flagsIn) {
        flags = flagsIn;
        setup();
    }

    /**
     * Update the txPower for a particular mode in the powerLevels table.
     */
    void setTxPowerLevel(TXPowerModes_t mode, int8_t txPowerIn) {
        powerLevels[mode] = txPowerIn;
    }

    /**
     * Set the effective power mode from one of the values in the powerLevels tables.
     */
    void setPowerMode(TXPowerModes_t mode) {
        effectivePower = powerLevels[mode];
        setup();
    }

    void setBeaconPeriod(uint16_t beaconPeriodIn) {
        beaconPeriod = beaconPeriodIn;
        setup();
    }

protected:
    /**
     * This callback allows the DFU service to receive the initial trigger to
     * handover control to the bootloader; but first the application is given a
     * chance to clean up.
     */
    virtual void onDataWritten(const GattCharacteristicWriteCBParams *params) {
        if (params->charHandle == uriDataChar.getValueAttribute().getHandle()) {
            if (lockedState) { /* When locked, the device isn't allowed to update the uriData characteristic. */
                /* Restore GATT database with previous value. */
                ble.updateCharacteristicValue(uriDataChar.getValueAttribute().getHandle(), uriDataValue, uriDataLength);
                return;
            }

            /* We don't handle very large writes at the moment. */
            if ((params->offset != 0) || (params->len > MAX_SIZE_URI_DATA_CHAR_VALUE)) {
                return;
            }

            uriDataLength = params->len;
            memcpy(uriDataValue, params->data, uriDataLength);
        } else if (params->charHandle == flagsChar.getValueAttribute().getHandle()) {
            if (lockedState) { /* When locked, the device isn't allowed to update the characteristic. */
                /* Restore GATT database with previous value. */
                ble.updateCharacteristicValue(flagsChar.getValueAttribute().getHandle(), &flags, 1 /* size */);
                return;
            } else {
                flags = *(params->data);
            }
        } else if (params->charHandle == txPowerLevelsChar.getValueAttribute().getHandle()) {
            if (lockedState) { /* When locked, the device isn't allowed to update the characteristic. */
                /* Restore GATT database with previous value. */
                ble.updateCharacteristicValue(txPowerLevelsChar.getValueAttribute().getHandle(), reinterpret_cast<uint8_t *>(powerLevels), NUM_POWER_MODES * sizeof(int8_t));
                return;
            } else {
                memcpy(powerLevels, params->data, NUM_POWER_MODES * sizeof(int8_t));
            }
        } else if (params->charHandle == beaconPeriodChar.getValueAttribute().getHandle()) {
            if (lockedState) { /* When locked, the device isn't allowed to update the characteristic. */
                /* Restore GATT database with previous value. */
                ble.updateCharacteristicValue(beaconPeriodChar.getValueAttribute().getHandle(), (uint8_t *)&beaconPeriod, 2 /* size */);
                return;
            } else {
                beaconPeriod = *((uint16_t *)(params->data));
            }
        }
        setup();
        ble.setAdvertisingPayload();
    }

    /**
     * For debugging only.
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
        serviceDataPayload[payloadIndex++] = effectivePower;

        const char *urlData = reinterpret_cast<char *>(uriDataValue);
        size_t sizeofURLData = uriDataLength;
        size_t encodedBytes = encodeURISchemePrefix(urlData, sizeofURLData) + encodeURI(urlData, sizeofURLData);

        ble.clearAdvertisingPayload();
        ble.accumulateAdvertisingPayload(GapAdvertisingData::COMPLETE_LIST_16BIT_SERVICE_IDS, BEACON_UUID, sizeof(BEACON_UUID));
        ble.accumulateAdvertisingPayload(GapAdvertisingData::SERVICE_DATA, serviceDataPayload, encodedBytes + 4);

        ble.setAdvertisingInterval(beaconPeriod);
        ble.setTxPower(effectivePower);
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
    bool     lockedState;
    uint16_t uriDataLength;
    uint8_t  uriDataValue[MAX_SIZE_URI_DATA_CHAR_VALUE];
    uint8_t  flags;
    int8_t   effectivePower;
    int8_t   powerLevels[NUM_POWER_MODES];
    uint16_t beaconPeriod;

    GattCharacteristic lockedStateChar;
    GattCharacteristic uriDataChar;
    GattCharacteristic flagsChar;
    GattCharacteristic txPowerLevelsChar;
    GattCharacteristic beaconPeriodChar;
};

#endif /* #ifndef __BLE_URI_BEACON_2_SERVICE_H__*/
