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

#define URI_BEACON_CONFIG_UUID_INITIALIZER_LIST(FIRST, SECOND) { \
        0xee, 0x0c, FIRST, SECOND, 0x87, 0x86, 0x40, 0xba,       \
        0xab, 0x96, 0x99, 0xb9, 0x1a, 0xc9, 0x81, 0xd8,          \
}
static const uint8_t URIBeacon2ControlServiceUUID[] = URI_BEACON_CONFIG_UUID_INITIALIZER_LIST(0x20, 0x80);
static const uint8_t lockedStateCharUUID[]          = URI_BEACON_CONFIG_UUID_INITIALIZER_LIST(0x20, 0x81);
static const uint8_t lockCharUUID[]                 = URI_BEACON_CONFIG_UUID_INITIALIZER_LIST(0x20, 0x82);
static const uint8_t unlockCharUUID[]               = URI_BEACON_CONFIG_UUID_INITIALIZER_LIST(0x20, 0x83);
static const uint8_t uriDataCharUUID[]              = URI_BEACON_CONFIG_UUID_INITIALIZER_LIST(0x20, 0x84);
static const uint8_t flagsCharUUID[]                = URI_BEACON_CONFIG_UUID_INITIALIZER_LIST(0x20, 0x85);
static const uint8_t txPowerLevelsCharUUID[]        = URI_BEACON_CONFIG_UUID_INITIALIZER_LIST(0x20, 0x86);
static const uint8_t txPowerModeCharUUID[]          = URI_BEACON_CONFIG_UUID_INITIALIZER_LIST(0x20, 0x87);
static const uint8_t beaconPeriodCharUUID[]         = URI_BEACON_CONFIG_UUID_INITIALIZER_LIST(0x20, 0x88);
static const uint8_t resetCharUUID[]                = URI_BEACON_CONFIG_UUID_INITIALIZER_LIST(0x20, 0x89);

/**
* @class URIBeaconConfigService
* @brief UriBeacon Configuration Service. Can be used to set URL, adjust power levels, and set flags.
*/
class URIBeaconConfigService {
public:
    /**
     * @enum TXPowerModes_t
     * @brief Transmission Power Modes for UriBeacon
     */
    enum TXPowerModes_t {
        TX_POWER_MODE_LOWEST = 0, /*!< Lowest TX power mode */
        TX_POWER_MODE_LOW    = 1, /*!< Low TX power mode */
        TX_POWER_MODE_MEDIUM = 2, /*!< Medium TX power mode */
        TX_POWER_MODE_HIGH   = 3, /*!< High TX power mode */
        NUM_POWER_MODES           /*!< Number of Power Modes defined */
    };

    static const size_t SIZEOF_LOCK_BITS = 16;                /* uint128 */
    typedef uint8_t     LockBits_t[SIZEOF_LOCK_BITS];

    /**
     * @param[ref] ble
     *                 BLEDevice object for the underlying controller.
     * @param[in]  uridata
     *                 URI as a null-terminated string.
     * @param[in]  flagsIn
     *                 UriBeacon Flags.
     * @param[in]  powerLevels[]
     *                 Table of UriBeacon Tx Power Levels in dBm.
     * @param[in]  powerMode
     *                 Currently effective power mode.
     * @param[in]  beaconPeriodIn
     *                 The period in milliseconds that a UriBeacon packet is
     *                 transmitted. A value of zero disables UriBeacon
     *                 transmissions.
     */
    URIBeaconConfigService(BLEDevice      &bleIn,
                           const char     *uriDataIn,
                           uint8_t         flagsIn                        = 0,
                           const int8_t    powerLevelsIn[NUM_POWER_MODES] = NULL,
                           TXPowerModes_t  powerModeIn                    = TX_POWER_MODE_LOW,
                           uint16_t        beaconPeriodIn                 = 1000) :
        ble(bleIn),
        payloadIndex(0),
        serviceDataPayload(),
        initSucceeded(false),
        lockedState(false),
        lockBits(),
        uriDataLength(0),
        uriData(),
        flags(flagsIn),
        powerLevels(),
        beaconPeriod(beaconPeriodIn),
        lockedStateChar(lockedStateCharUUID, reinterpret_cast<uint8_t *>(&lockedState), 1, 1, GattCharacteristic::BLE_GATT_CHAR_PROPERTIES_READ),
        lockChar(lockCharUUID, lockBits, SIZEOF_LOCK_BITS, SIZEOF_LOCK_BITS, GattCharacteristic::BLE_GATT_CHAR_PROPERTIES_WRITE),
        unlockChar(unlockCharUUID, lockBits, SIZEOF_LOCK_BITS, SIZEOF_LOCK_BITS, GattCharacteristic::BLE_GATT_CHAR_PROPERTIES_WRITE),
        uriDataChar(uriDataCharUUID, uriData, MAX_SIZE_URI_DATA_CHAR_VALUE, MAX_SIZE_URI_DATA_CHAR_VALUE,
                    GattCharacteristic::BLE_GATT_CHAR_PROPERTIES_READ | GattCharacteristic::BLE_GATT_CHAR_PROPERTIES_WRITE),
        flagsChar(flagsCharUUID, &flags, 1, 1, GattCharacteristic::BLE_GATT_CHAR_PROPERTIES_READ | GattCharacteristic::BLE_GATT_CHAR_PROPERTIES_WRITE),
        txPowerLevelsChar(txPowerLevelsCharUUID, reinterpret_cast<uint8_t *>(powerLevels), sizeof(powerLevels), sizeof(powerLevels),
                          GattCharacteristic::BLE_GATT_CHAR_PROPERTIES_READ | GattCharacteristic::BLE_GATT_CHAR_PROPERTIES_WRITE),
        txPowerModeChar(txPowerModeCharUUID, reinterpret_cast<uint8_t *>(&txPowerMode), sizeof(uint8_t), sizeof(uint8_t),
                        GattCharacteristic::BLE_GATT_CHAR_PROPERTIES_READ | GattCharacteristic::BLE_GATT_CHAR_PROPERTIES_WRITE),
        beaconPeriodChar(beaconPeriodCharUUID, reinterpret_cast<uint8_t *>(&beaconPeriod), 2, 2,
                         GattCharacteristic::BLE_GATT_CHAR_PROPERTIES_READ | GattCharacteristic::BLE_GATT_CHAR_PROPERTIES_WRITE),
        resetChar(resetCharUUID, reinterpret_cast<uint8_t *>(&resetFlag), 1, 1, GattCharacteristic::BLE_GATT_CHAR_PROPERTIES_WRITE)
    {
        if ((uriDataIn == NULL) || ((uriDataLength = strlen(uriDataIn)) == 0) || (uriDataLength > MAX_SIZE_URI_DATA_CHAR_VALUE)) {
            return;
        }
        strcpy(reinterpret_cast<char *>(uriData), uriDataIn);

        if (powerLevelsIn != NULL) {
            memcpy(powerLevels, powerLevelsIn, sizeof(powerLevels));
            updateTxPowerLevelsCharacteristic();
        }

        configureGAP();

        lockChar.setWriteAuthorizationCallback(this, &URIBeaconConfigService::lockAuthorizationCallback);
        unlockChar.setWriteAuthorizationCallback(this, &URIBeaconConfigService::unlockAuthorizationCallback);
        uriDataChar.setWriteAuthorizationCallback(this, &URIBeaconConfigService::uriDataWriteAuthorizationCallback);
        flagsChar.setWriteAuthorizationCallback(this, &URIBeaconConfigService::flagsAuthorizationCallback);
        txPowerLevelsChar.setWriteAuthorizationCallback(this, &URIBeaconConfigService::denyGATTWritesIfLocked);
        txPowerModeChar.setWriteAuthorizationCallback(this, &URIBeaconConfigService::powerModeAuthorizationCallback);
        beaconPeriodChar.setWriteAuthorizationCallback(this, &URIBeaconConfigService::denyGATTWritesIfLocked);
        resetChar.setWriteAuthorizationCallback(this, &URIBeaconConfigService::denyGATTWritesIfLocked);

        GattCharacteristic *charTable[] = {&lockedStateChar,
                                           &lockChar,
                                           &unlockChar,
                                           &uriDataChar,
                                           &flagsChar,
                                           &txPowerLevelsChar,
                                           &beaconPeriodChar,
                                           &resetChar};
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
     * @verbatim
     *     ### UriBeacon Flags
     *     Bit   | Description
     *     :---- | :----------
     *     0     | Invisible Hint
     *     1..7  | Reserved for future use. Must be zero.
     * @endverbatim
     *     The `Invisible Hint` flag is a command for the user-agent that tells
     *     it not to access or display the UriBeacon. This is a guideline only,
     *     and is not a blocking method. User agents may, with user approval,
     *     display invisible beacons.
     */
    void setFlags(uint8_t flagsIn) {
        flags = flagsIn;
        configureGAP();
        updateFlagsCharacteristic();
    }

    /**
     * @brief Update the txPowerLevels table.
     *
     * @param[in] powerLevelsIn
     *              Array of power levels
     */
    void setTxPowerLevels(const int8_t powerLevelsIn[NUM_POWER_MODES]) {
        memcpy(powerLevels, powerLevelsIn, sizeof(powerLevels));
        configureGAP();
        updateTxPowerLevelsCharacteristic();
    }

    /**
     * @brief Set the effective power mode from one of the values in the powerLevels tables.
     *
     * @param[in] mode
     *              Set the TX Power Mode.
     */
    void setTxPowerMode(TXPowerModes_t mode) {
        txPowerMode = mode;
        configureGAP();
        updateTxPowerModeCharacteristic();
    }

    /**
     * The period in milliseconds that a UriBeacon packet is transmitted.
     *
     * @note A value of zero disables UriBeacon transmissions.
     *
     * @param beaconPeriodIn
     *              Beacon advertising period in milliseconds
     */
    void setBeaconPeriod(uint16_t beaconPeriodIn) {
        beaconPeriod = beaconPeriodIn;
        configureGAP();
        updateBeaconPeriodCharacteristic();
    }

protected:
    void updateLockBits(const LockBits_t lockBitsIn) {
        static const uint8_t allZeroes[SIZEOF_LOCK_BITS] = {0, 0, 0, 0, 0, 0, 0, 0,
                                                            0, 0, 0, 0, 0, 0, 0, 0};

        memcpy(lockBits, lockBitsIn, SIZEOF_LOCK_BITS);
        if (memcmp(lockBits, allZeroes, SIZEOF_LOCK_BITS)) {
            lockedState = true;
        }
    }

    void copyLockBitsInto(LockBits_t lockBitsOut) const {
        memcpy(lockBitsOut, lockBits, SIZEOF_LOCK_BITS);
    }

    /**
     * APIs around making lockBits persistent.
     */
private:
    /**
     * Have we previously saved lockedBits? Once set, this state is expected to persist.
     * @return true if we've previously saved locked bits.
     */
    virtual bool storage_haveSavedLockBits() const {
        /* Expecting to be overridden. Left empty to allow the default URIBeacon to be instantiated if persistence isn't required. */
        return false;
    }

    /**
     * Save the current value of lockBits into persistent storage; this value is then retrievable by lockLockBits() until a subsequent call to saveLockBits().
     */
    virtual void storage_saveLockBits() {
        /* Expecting to be overridden. Left empty to allow the default URIBeacon to be instantiated if persistence isn't required. */
    }

    /**
     * Retrieve the saved lockBits from persistent storage and update the class member 'lockBits'.
     */
    virtual void storage_loadLockBits() {
        /* Expecting to be overridden. Left empty to allow the default URIBeacon to be instantiated if persistence isn't required. */
    }

private:
    /*
     * Setup the advertisement payload and GAP settings.
     */
    void configureGAP(void) {
        const uint8_t BEACON_UUID[] = {0xD8, 0xFE};

        payloadIndex                       = 0;
        serviceDataPayload[payloadIndex++] = BEACON_UUID[0];
        serviceDataPayload[payloadIndex++] = BEACON_UUID[1];
        serviceDataPayload[payloadIndex++] = flags;
        serviceDataPayload[payloadIndex++] = powerLevels[txPowerMode];

        const char *urlData       = reinterpret_cast<char *>(uriData);
        size_t      sizeofURLData = uriDataLength;
        size_t      encodedBytes  = encodeURISchemePrefix(urlData, sizeofURLData) + encodeURI(urlData, sizeofURLData);

        ble.clearAdvertisingPayload();
        ble.accumulateAdvertisingPayload(GapAdvertisingData::COMPLETE_LIST_16BIT_SERVICE_IDS, BEACON_UUID, sizeof(BEACON_UUID));
        ble.accumulateAdvertisingPayload(GapAdvertisingData::SERVICE_DATA, serviceDataPayload, encodedBytes + 4);

        ble.setAdvertisingInterval(Gap::MSEC_TO_ADVERTISEMENT_DURATION_UNITS(beaconPeriod));
        ble.setTxPower(powerLevels[txPowerMode]);
    }

    /*
     *  Encode the URI Prefix to a single byte if possible.
     */
    size_t encodeURISchemePrefix(const char *&urldata, size_t &sizeofURLData) {
        if (!sizeofURLData) {
            return 0;
        }

        /* These are the URI Prefixes that can be abbreviated.*/
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

    /*
     *  Encode the URI Suffix to a single byte if possible.
     */
    size_t encodeURI(const char *urldata, size_t sizeofURLData) {
        /* These are the URI suffixes that can be abbreviated. */
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

    /*
     * This callback is invoked when a GATT client attempts to modify any of the
     * characteristics of this service. Attempts to do so are also applied to
     * the internal state of this service object.
     */
    void onDataWritten(const GattCharacteristicWriteCBParams *params) {
        uint16_t handle = params->charHandle;
        if (handle == lockChar.getValueHandle()) {
            updateLockBits(params->data);
            storage_saveLockBits();
        } else if (handle == unlockChar.getValueHandle()) {
            memset(lockBits, 0, SIZEOF_LOCK_BITS);
            lockedState = false;

            storage_saveLockBits();
        } else if (handle == uriDataChar.getValueHandle()) {
            uriDataLength = params->len;
            memcpy(uriData, params->data, uriDataLength);
        } else if (handle == flagsChar.getValueHandle()) {
            flags = *(params->data);
        } else if (handle == txPowerLevelsChar.getValueHandle()) {
            memcpy(powerLevels, params->data, NUM_POWER_MODES * sizeof(int8_t));
        } else if (handle == txPowerModeChar.getValueHandle()) {
            txPowerMode = *reinterpret_cast<const TXPowerModes_t *>(params->data);
        } else if (handle == beaconPeriodChar.getValueHandle()) {
            beaconPeriod = *((uint16_t *)(params->data));
        } else if (handle == resetChar.getValueHandle()) {
            resetDefaults();
        }
        configureGAP();
        ble.setAdvertisingPayload();
    }

    /*
     * Reset the default values.
     */
    void resetDefaults(void) {
        lockedState      = false;
        memset(lockBits, 0, SIZEOF_LOCK_BITS);
        uriDataLength    = 0;
        memset(uriData, 0, MAX_SIZE_URI_DATA_CHAR_VALUE);
        flags            = 0;
        memset(powerLevels, 0, sizeof(powerLevels));
        txPowerMode      = TX_POWER_MODE_LOW;
        beaconPeriod     = 0;

        if (storage_haveSavedLockBits()) {
            storage_saveLockBits();
        }

        updateGATT();
    }

    /*
     * Internal helper function used to update the GATT database following any
     * change to the internal state of the service object.
     */
    void updateGATT(void) {
        updateLockedStateCharacteristic();
        updateURIDataCharacteristic();
        updateFlagsCharacteristic();
        updateBeaconPeriodCharacteristic();
        updateTxPowerLevelsCharacteristic();
        updateTxPowerModeCharacteristic();
    }

    void updateLockedStateCharacteristic(void) {
        ble.updateCharacteristicValue(lockedStateChar.getValueHandle(), reinterpret_cast<uint8_t *>(&lockedState), sizeof(lockedState));
    }

    void updateURIDataCharacteristic(void) {
        ble.updateCharacteristicValue(uriDataChar.getValueHandle(), uriData, uriDataLength);
    }

    void updateFlagsCharacteristic(void) {
        ble.updateCharacteristicValue(flagsChar.getValueHandle(), &flags, 1 /* size */);
    }

    void updateBeaconPeriodCharacteristic(void) {
        ble.updateCharacteristicValue(beaconPeriodChar.getValueHandle(), reinterpret_cast<uint8_t *>(&beaconPeriod), sizeof(uint16_t));
    }

    void updateTxPowerModeCharacteristic(void) {
        ble.updateCharacteristicValue(txPowerModeChar.getValueHandle(), reinterpret_cast<uint8_t *>(&txPowerMode), sizeof(uint8_t));
    }

    void updateTxPowerLevelsCharacteristic(void) {
        ble.updateCharacteristicValue(txPowerLevelsChar.getValueHandle(), reinterpret_cast<uint8_t *>(powerLevels), NUM_POWER_MODES * sizeof(int8_t));
    }

private:
    void lockedStateAuthorizationCallback(GattCharacteristicReadAuthCBParams *params) {
        printf("read authorization callback: lockedState is %u\r\n", lockedState);
        params->authorizationReply = true;
    }

    void lockAuthorizationCallback(GattCharacteristicWriteAuthCBParams *params) {
        params->authorizationReply = !lockedState;
    }

    void unlockAuthorizationCallback(GattCharacteristicWriteAuthCBParams *params) {
        if (lockedState && (memcmp(params->data, lockBits, SIZEOF_LOCK_BITS) == 0)) {
            params->authorizationReply = true;
        } else {
            params->authorizationReply = false;
        }
    }

    void uriDataWriteAuthorizationCallback(GattCharacteristicWriteAuthCBParams *params) {
        if (lockedState || (params->offset != 0) || (params->len > MAX_SIZE_URI_DATA_CHAR_VALUE)) {
            params->authorizationReply = false;
        }
    }

    void flagsAuthorizationCallback(GattCharacteristicWriteAuthCBParams *params) {
        if (lockedState || ((*(params->data) & 0xFE) != 0)) {
            params->authorizationReply = false;
        }
    }

    void powerModeAuthorizationCallback(GattCharacteristicWriteAuthCBParams *params) {
        if (lockedState || (*(params->data) >= NUM_POWER_MODES)) {
            params->authorizationReply = false;
        }
    }

    void denyGATTWritesIfLocked(GattCharacteristicWriteAuthCBParams *params) {
        if (lockedState) {
            params->authorizationReply = false;
        }
    }

private:
    /**
     * For debugging only. Print Hex representation of ServiceDataPayload to the console.
     */
    void dumpEncodedSeviceData() const {
        printf("encoded: '");
        for (unsigned i = 0; i < payloadIndex; i++) {
            printf(" %02x", serviceDataPayload[i]);
        }
        printf("'\r\n");
    }

private:
    static const size_t MAX_SIZEOF_SERVICE_DATA_PAYLOAD = 22; /* Uri Data must be between 0 and 18 bytes in length; and
                                                               * together with the 4-byte header, the service data must
                                                               * fit within 22 bytes. */
    static const size_t MAX_SIZE_URI_DATA_CHAR_VALUE    = 48; /* This is chosen arbitrarily. It should be large enough
                                                               * to hold any reasonable uncompressed URI. */
private:
    BLEDevice          &ble;

    size_t              payloadIndex;
    uint8_t             serviceDataPayload[MAX_SIZEOF_SERVICE_DATA_PAYLOAD];
    bool                initSucceeded;

    bool                lockedState;
    uint8_t             lockBits[SIZEOF_LOCK_BITS];

    uint16_t            uriDataLength;
    uint8_t             uriData[MAX_SIZE_URI_DATA_CHAR_VALUE];
    uint8_t             flags;
    int8_t              powerLevels[NUM_POWER_MODES];
    TXPowerModes_t      txPowerMode;
    uint16_t            beaconPeriod;
    bool                resetFlag;

    GattCharacteristic  lockedStateChar;
    GattCharacteristic  lockChar;
    GattCharacteristic  unlockChar;
    GattCharacteristic  uriDataChar;
    GattCharacteristic  flagsChar;
    GattCharacteristic  txPowerLevelsChar;
    GattCharacteristic  txPowerModeChar;
    GattCharacteristic  beaconPeriodChar;
    GattCharacteristic  resetChar;
};

#endif /* #ifndef __BLE_URI_BEACON_CONFIG_SERVICE_H__*/
