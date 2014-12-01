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

#ifndef __BLE_URI_BEACON_CONFIG_SERVICE_H__
#define __BLE_URI_BEACON_CONFIG_SERVICE_H__

#include "BLEDevice.h"

#define UUID_INITIALIZER_LIST(FIRST, SECOND) {             \
        0xee, 0x0c, FIRST, SECOND, 0x87, 0x86, 0x40, 0xba, \
        0xab, 0x96, 0x99, 0xb9, 0x1a, 0xc9, 0x81, 0xd8,    \
}
const uint8_t URIBeacon2ControlServiceUUID[] = UUID_INITIALIZER_LIST(0x20, 0x80);
const uint8_t lockedStateCharUUID[]          = UUID_INITIALIZER_LIST(0x20, 0x81);
const uint8_t uriDataCharUUID[]              = UUID_INITIALIZER_LIST(0x20, 0x84);
const uint8_t flagsCharUUID[]                = UUID_INITIALIZER_LIST(0x20, 0x85);
const uint8_t txPowerLevelsCharUUID[]        = UUID_INITIALIZER_LIST(0x20, 0x86);
const uint8_t beaconPeriodCharUUID[]         = UUID_INITIALIZER_LIST(0x20, 0x88);
const uint8_t resetCharUUID[]                = UUID_INITIALIZER_LIST(0x20, 0x89);

class URIBeaconConfigService {
public:
    enum TXPowerModes_t {
        TX_POWER_MODE_LOWEST = 0,
        TX_POWER_MODE_LOW    = 1,
        TX_POWER_MODE_MEDIUM = 2,
        TX_POWER_MODE_HIGH   = 3,
        NUM_POWER_MODES
    };

    /**
     * @param[ref] ble
     *                 BLEDevice object for the underlying controller.
     * @param[in]  uridata
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
    URIBeaconConfigService(BLEDevice &bleIn, const char *uriDataIn, uint8_t flagsIn = 0, int8_t effectiveTxPowerIn = 0, uint16_t beaconPeriodIn = 1000) :
        ble(bleIn),
        payloadIndex(0),
        serviceDataPayload(),
        initSucceeded(false),
        lockedState(false),
        uriDataLength(0),
        uriData(),
        flags(flagsIn),
        effectiveTxPower(effectiveTxPowerIn),
        powerLevels(),
        beaconPeriod(beaconPeriodIn),
        lockedStateChar(lockedStateCharUUID, reinterpret_cast<uint8_t *>(&lockedState), 1, 1, GattCharacteristic::BLE_GATT_CHAR_PROPERTIES_READ),
        uriDataChar(uriDataCharUUID,
                    uriData,
                    MAX_SIZE_URI_DATA_CHAR_VALUE,
                    MAX_SIZE_URI_DATA_CHAR_VALUE,
                    GattCharacteristic::BLE_GATT_CHAR_PROPERTIES_WRITE | GattCharacteristic::BLE_GATT_CHAR_PROPERTIES_WRITE_WITHOUT_RESPONSE),
        flagsChar(flagsCharUUID, &flags, 1, 1, GattCharacteristic::BLE_GATT_CHAR_PROPERTIES_READ | GattCharacteristic::BLE_GATT_CHAR_PROPERTIES_WRITE),
        txPowerLevelsChar(txPowerLevelsCharUUID,
                          reinterpret_cast<uint8_t *>(powerLevels),
                          NUM_POWER_MODES * sizeof(int8_t),
                          NUM_POWER_MODES * sizeof(int8_t),
                          GattCharacteristic::BLE_GATT_CHAR_PROPERTIES_READ | GattCharacteristic::BLE_GATT_CHAR_PROPERTIES_WRITE),
        beaconPeriodChar(beaconPeriodCharUUID, reinterpret_cast<uint8_t *>(&beaconPeriod), 2, 2,
                         GattCharacteristic::BLE_GATT_CHAR_PROPERTIES_READ | GattCharacteristic::BLE_GATT_CHAR_PROPERTIES_WRITE),
        resetChar(resetCharUUID, reinterpret_cast<uint8_t *>(&resetFlag), 1, 1,
                  GattCharacteristic::BLE_GATT_CHAR_PROPERTIES_WRITE | GattCharacteristic::BLE_GATT_CHAR_PROPERTIES_WRITE_WITHOUT_RESPONSE)
    {
        if ((uriDataIn == NULL) || ((uriDataLength = strlen(uriDataIn)) == 0) || (uriDataLength > MAX_SIZE_URI_DATA_CHAR_VALUE)) {
            return;
        }
        strcpy(reinterpret_cast<char *>(uriData), uriDataIn);

        configure();
        if (initSucceeded) {
            saveDefaults();
        }

        GattCharacteristic *charTable[] = {&lockedStateChar, &uriDataChar, &flagsChar, &txPowerLevelsChar, &beaconPeriodChar, &resetChar};
        GattService         beaconControlService(URIBeacon2ControlServiceUUID, charTable, sizeof(charTable) / sizeof(GattCharacteristic *));

        ble.addService(beaconControlService);
        ble.onDataWritten(this, &URIBeaconConfigService::onDataWritten);
    }

    bool configuredSuccessfully(void) const {
        return initSucceeded;
    }

    /**
     * Please note that the following public APIs are offered to allow modifying
     * the service programmatically. It is also possible to do so over BLE GATT
     * transactions.
     */
public:
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
        configure();
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
    void useTxPowerMode(TXPowerModes_t mode) {
        effectiveTxPower = powerLevels[mode];
        configure();
    }

    /**
     * The period in milliseconds that a UriBeacon packet is transmitted.
     *
     * @Note: A value of zero disables UriBeacon transmissions.
     */
    void setBeaconPeriod(uint16_t beaconPeriodIn) {
        beaconPeriod = beaconPeriodIn;
        configure();
    }

private:
    /**
     * Setup the advertisement payload and GAP settings.
     */
    void configure(void) {
        const uint8_t BEACON_UUID[] = {0xD8, 0xFE};

        payloadIndex                       = 0;
        serviceDataPayload[payloadIndex++] = BEACON_UUID[0];
        serviceDataPayload[payloadIndex++] = BEACON_UUID[1];
        serviceDataPayload[payloadIndex++] = flags;
        serviceDataPayload[payloadIndex++] = effectiveTxPower;

        const char *urlData       = reinterpret_cast<char *>(uriData);
        size_t      sizeofURLData = uriDataLength;
        size_t      encodedBytes  = encodeURISchemePrefix(urlData, sizeofURLData) + encodeURI(urlData, sizeofURLData);

        ble.clearAdvertisingPayload();
        ble.accumulateAdvertisingPayload(GapAdvertisingData::COMPLETE_LIST_16BIT_SERVICE_IDS, BEACON_UUID, sizeof(BEACON_UUID));
        ble.accumulateAdvertisingPayload(GapAdvertisingData::SERVICE_DATA, serviceDataPayload, encodedBytes + 4);

        ble.setAdvertisingInterval(Gap::MSEC_TO_ADVERTISEMENT_DURATION_UNITS(beaconPeriod));
        ble.setTxPower(effectiveTxPower);
    }

    size_t encodeURISchemePrefix(const char *&urldata, size_t &sizeofURLData) {
        const char *prefixes[] = {
            "http://www.",
            "https://www.",
            "http://",
            "https://",
            "urn:uuid:"
        };

        size_t       encodedBytes = 0;
        const size_t NUM_PREFIXES = sizeof(prefixes) / sizeof(char *);
        for (unsigned i = 0; i < NUM_PREFIXES; i++) {
            size_t prefixLen = strlen(prefixes[i]);
            if (strncmp(urldata, prefixes[i], prefixLen) == 0) {
                serviceDataPayload[payloadIndex++] = i;
                encodedBytes                       = 1;

                urldata       += prefixLen;
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
        if (sizeofURLData == 0) {
            initSucceeded = true;
        }

        return encodedBytes;
    }

    void onDataWritten(const GattCharacteristicWriteCBParams *params) {
        if (params->charHandle == uriDataChar.getValueAttribute().getHandle()) {
            if (lockedState) { /* When locked, the device isn't allowed to update the uriData characteristic. */
                /* Restore GATT database with previous value. */
                ble.updateCharacteristicValue(uriDataChar.getValueAttribute().getHandle(), uriData, uriDataLength);
                return;
            }

            /* We don't handle very large writes at the moment. */
            if ((params->offset != 0) || (params->len > MAX_SIZE_URI_DATA_CHAR_VALUE)) {
                return;
            }

            uriDataLength = params->len;
            memcpy(uriData, params->data, uriDataLength);
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
                ble.updateCharacteristicValue(beaconPeriodChar.getValueAttribute().getHandle(), reinterpret_cast<uint8_t *>(&beaconPeriod), sizeof(uint16_t));
                return;
            } else {
                beaconPeriod = *((uint16_t *)(params->data));
            }
        } else if (params->charHandle == resetChar.getValueAttribute().getHandle()) {
            resetDefaults();
        }
        configure();
        ble.setAdvertisingPayload();
    }

    /* Preserve the defaults to be able to reset() upon request. */
    void saveDefaults(void) {
        memcpy(defaultURIData, uriData, MAX_SIZE_URI_DATA_CHAR_VALUE);
        defaultFlags            = flags;
        defaultEffectiveTxPower = effectiveTxPower;
        defaultBeaconPeriod     = beaconPeriod;
    }

    void resetDefaults(void) {
        memcpy(uriData, defaultURIData, MAX_SIZE_URI_DATA_CHAR_VALUE);
        memset(powerLevels, 0, sizeof(powerLevels));
        flags            = defaultFlags;
        effectiveTxPower = defaultEffectiveTxPower;
        beaconPeriod     = defaultBeaconPeriod;

        ble.updateCharacteristicValue(uriDataChar.getValueAttribute().getHandle(), uriData, uriDataLength);
        ble.updateCharacteristicValue(flagsChar.getValueAttribute().getHandle(), &flags, 1 /* size */);
        ble.updateCharacteristicValue(beaconPeriodChar.getValueAttribute().getHandle(), reinterpret_cast<uint8_t *>(&beaconPeriod), sizeof(uint16_t));

        configure();
    }

private:
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
    static const size_t MAX_SIZEOF_SERVICE_DATA_PAYLOAD = 18; /* Uri Data must be between 0 and 18 bytes in length. */
    static const size_t MAX_SIZE_URI_DATA_CHAR_VALUE    = 48; /* This is chosen arbitrarily. It should be large enough
                                                               * to hold any reasonable uncompressed URI. */

private:
    BLEDevice          &ble;

    size_t              payloadIndex;
    uint8_t             serviceDataPayload[MAX_SIZEOF_SERVICE_DATA_PAYLOAD];
    bool                initSucceeded;

    bool                lockedState;
    uint16_t            uriDataLength;
    uint8_t             uriData[MAX_SIZE_URI_DATA_CHAR_VALUE];
    uint8_t             flags;
    int8_t              effectiveTxPower;
    int8_t              powerLevels[NUM_POWER_MODES];
    uint16_t            beaconPeriod;
    bool                resetFlag;

    uint8_t             defaultURIData[MAX_SIZE_URI_DATA_CHAR_VALUE];
    uint8_t             defaultFlags;
    int8_t              defaultEffectiveTxPower;
    uint16_t            defaultBeaconPeriod;

    GattCharacteristic  lockedStateChar;
    GattCharacteristic  uriDataChar;
    GattCharacteristic  flagsChar;
    GattCharacteristic  txPowerLevelsChar;
    GattCharacteristic  beaconPeriodChar;
    GattCharacteristic  resetChar;
};

#endif /* #ifndef __BLE_URI_BEACON_CONFIG_SERVICE_H__*/
