/* mbed Microcontroller Library
 * Copyright (c) 2006-2015 ARM Limited
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

#ifndef SERVICES_EDDYSTONEBEACON_H_
#define SERVICES_EDDYSTONEBEACON_H_

#include "ble/BLE.h"
#include "mbed.h"

static const uint8_t BEACON_EDDYSTONE[] = {0xAA, 0xFE};

//Debug is disabled by default
#if 0
#define DBG(x, ...)  printf("[EddyStone: DBG]"x" \t[%s,%d]\r\n", ##__VA_ARGS__,__FILE__,__LINE__);
#define WARN(x, ...) printf("[EddyStone: WARN]"x" \t[%s,%d]\r\n", ##__VA_ARGS__,__FILE__,__LINE__);
#define ERR(x, ...)  printf("[EddyStone: ERR]"x" \t[%s,%d]\r\n", ##__VA_ARGS__,__FILE__,__LINE__);
#else
#define DBG(x, ...) //wait_us(10);
#define WARN(x, ...) //wait_us(10);
#define ERR(x, ...)
#endif

#if 0
#define INFO(x, ...)  printf("[EddyStone: INFO]"x" \t[%s,%d]\r\n", ##__VA_ARGS__,__FILE__,__LINE__);
#else
#define INFO(x, ...)
#endif

/**
* @class Eddystone
* @brief Eddystone Configuration Service. Can be used to set URL, adjust power levels, and set flags.
* See https://github.com/google/eddystone
*
*/
class EddystoneService
{
public:
    /**
     * @brief Transmission Power Modes for UriBeacon
     */

    static const int ADVERTISING_INTERVAL_MSEC = 1000;  // Advertising interval for config service.
    static const int SERVICE_DATA_MAX = 31;             // Maximum size of service data in ADV packets

    // There are currently 3 subframes defined, URI, UID, and TLM
#define EDDYSTONE_MAX_FRAMETYPE 3
    void (*frames[EDDYSTONE_MAX_FRAMETYPE])(uint8_t *, uint32_t);
    uint8_t frameIndex;
    static const int URI_DATA_MAX = 18;
    typedef uint8_t  UriData_t[URI_DATA_MAX];

    // UID Frame Type subfields
    static const int UID_NAMESPACEID_SIZE = 10;
    typedef uint8_t  UIDNamespaceID_t[UID_NAMESPACEID_SIZE];
    static const int UID_INSTANCEID_SIZE = 6;
    typedef uint8_t  UIDInstanceID_t[UID_INSTANCEID_SIZE];

    // Eddystone Frame Type ID
    static const uint8_t FRAME_TYPE_UID = 0x00;
    static const uint8_t FRAME_TYPE_URL = 0x10;
    static const uint8_t FRAME_TYPE_TLM = 0x20;

    static const uint8_t FRAME_SIZE_TLM = 14; // TLM frame is a constant 14Bytes
    static const uint8_t FRAME_SIZE_UID = 20; // includes RFU bytes

    /*
    *  Set Eddystone UID Frame information.
    *  @param[in] power   TX Power in dB measured at 0 meters from the device. Range of -100 to +20 dB.
    *  @param namespaceID 10B namespace ID
    *  @param instanceID  6B instance ID
    *  @param RFU         2B of RFU, initialized to 0x0000 and not broadcast, included for future reference.
    *
    */
    void setUIDFrameData(int8_t power, UIDNamespaceID_t namespaceID, UIDInstanceID_t instanceID, uint16_t RFU = 0x0000) {
        if(power > 20) {
            power = 20;
        }
        if(power < -100) {
            power = -100;
        }
        defaultUidPower = power;
        memcpy(defaultUidNamespaceID, namespaceID, UID_NAMESPACEID_SIZE);
        memcpy(defaultUidInstanceID,  instanceID,  UID_INSTANCEID_SIZE);
        uidRFU = (uint16_t)RFU; // this is probably bad form, but it doesnt really matter yet.
        return;
    }

    /*
    *  Construct UID frame from private variables
    *  @param[in/out] Data pointer to array to store constructed frame in
    *  @param[in] maxSize number of bytes left in array, effectively how much emtpy space is available to write to
    *  @return number of bytes used. negative number indicates error message.
    */
    int constructUIDFrame(uint8_t * Data, uint8_t maxSize) {

        int index = 0;
        Data[index++] = FRAME_TYPE_UID;                     // 1B  Type
        if(defaultUidPower > 20) {
            defaultUidPower = 20;   // enforce range of vaild values.
        }
        if(defaultUidPower < -100) {
            defaultUidPower = -100;
        }
        Data[index++] = defaultUidPower;                    // 1B  Power @ 0meter
        for(int x = 0; x < UID_NAMESPACEID_SIZE; x++) {     // 10B Namespce ID
            Data[index++] = defaultUidNamespaceID[x];
        }
        for(int x = 0; x< UID_INSTANCEID_SIZE; x++) {       // 6B  Instance ID
            Data[index++] = defaultUidInstanceID[x];
        }
        if(0 != uidRFU) {                                // 2B RFU, include if non-zero, otherwise ignore
            Data[index++] = (uint8_t)(uidRFU >> 0);
            Data[index++] = (uint8_t)(uidRFU >> 8);
        }
        DBG("construcUIDFrame %d, %d",maxSize,index);
        return index;
    }

    /*
    *  Set Eddystone URL Frame information.
    *  @param[in] power   TX Power in dB measured at 0 meters from the device.
    *  @param url         URL to encode
    *  @return            false on success, true on failure.
    */
    bool setURLFrameData(int8_t power, const char * url) {
        defaultUrlPower = power;
        encodeURL(url, defaultUriData, defaultUriDataLength); // encode URL to URL Formatting
        if (defaultUriDataLength > URI_DATA_MAX) {
            return true; // error, URL is too big
        }
        return false;
    }

    /*
    *  Construct URL frame from private variables
    *  @param[in/out] Data pointer to array to store constructed frame in
    *  @param[in] maxSize number of bytes left in array, effectively how much emtpy space is available to write to
    *  @return number of bytes used. negative number indicates error message.
    */
    int constructURLFrame(uint8_t * Data, uint8_t maxSize) {
        int index = 0;
        Data[index++] = FRAME_TYPE_URL;                     // 1B  Type
        Data[index++] = defaultUrlPower;                    // 1B  TX Power
        for(int x = 0; x < defaultUriDataLength; x++) {     // 18B of URL Prefix + encoded URL
            Data[index++] = defaultUriData[x];
        }
        DBG("constructURLFrame: %d, %d",maxSize,index);
        return index;
    }

    /*
    *  Set Eddystone TLM Frame information.
    *  @param[in] Version    of the TLM beacon data format
    *  @param batteryVoltage in milivolts
    *  @param beaconTemp     in 8.8 floating point notation
    *
    */
    void setTLMFrameData(uint8_t version, uint16_t batteryVoltage, uint16_t beaconTemp, uint32_t pduCount = 0, uint32_t timeSinceBoot = 0) {
        TlmVersion = version;
        TlmBatteryVoltage = batteryVoltage;
        TlmBeaconTemp = beaconTemp;
        TlmPduCount = pduCount; // reset
        TlmTimeSinceBoot = timeSinceBoot; // reset
        return;
    }

    /*
    *  Construct TLM frame from private variables
    *  @param[in/out] Data pointer to array to store constructed frame in
    *  @param[in] maxSize number of bytes left in array, effectively how much emtpy space is available to write to
    *  @return number of bytes used. negative number indicates error message.
    */
    int constructTLMFrame(uint8_t * Data, uint8_t maxSize) {
        int index = 0;
        Data[index++] = FRAME_TYPE_TLM;                    // Eddystone frame type = Telemetry
        Data[index++] = TlmVersion;                        // TLM Version Number
        Data[index++] = (uint8_t)(TlmBatteryVoltage>>8);   // Battery Voltage[0]
        Data[index++] = (uint8_t)(TlmBatteryVoltage>>0);   // Battery Voltage[1]
        Data[index++] = (uint8_t)(TlmBeaconTemp>>8);       // Beacon Temp[0]
        Data[index++] = (uint8_t)(TlmBeaconTemp>>0);       // Beacon Temp[1]
        Data[index++] = (uint8_t)(TlmPduCount>>24);         // PDU Count [0]
        Data[index++] = (uint8_t)(TlmPduCount>>16);         // PDU Count [1]
        Data[index++] = (uint8_t)(TlmPduCount>>8);        // PDU Count [2]
        Data[index++] = (uint8_t)(TlmPduCount>>0);        // PDU Count [3]
        Data[index++] = (uint8_t)(TlmTimeSinceBoot>>24);    // Time Since Boot [0]
        Data[index++] = (uint8_t)(TlmTimeSinceBoot>>16);    // Time Since Boot [1]
        Data[index++] = (uint8_t)(TlmTimeSinceBoot>>8);   // Time Since Boot [2]
        Data[index++] = (uint8_t)(TlmTimeSinceBoot>>0);   // Time Since Boot [3]
        DBG("constructURLFrame: %d, %d",maxSize,index);
        return index;
    }

    /*
    *  Update the TLM frame battery voltage value
    *  @param[in] voltagemv Voltage to update the TLM field battery voltage with (in mV)
    *  @return nothing
    */
    void updateTlmBatteryVoltage(uint16_t voltagemv) {
        TlmBatteryVoltage = voltagemv;
        return;
    }

    /*
    *  Update the TLM frame beacon temperature
    *  @param[in] temp Temperature of beacon (in 8.8fpn)
    *  @return nothing
    */
    void updateTlmBeaconTemp(uint16_t temp) {
        TlmBeaconTemp = temp;
        return;
    }

    /*
    *  Update the TLM frame PDU Count field
    *  @param[in] pduCount Number of Advertisiting frames sent since powerup
    *  @return nothing
    */
    void updateTlmPduCount(uint32_t pduCount) {
        TlmPduCount = pduCount;
        return;
    }

    /*
    *  Update the TLM frame Time since boot in 0.1s incriments
    *  @param[in] timeSinceBoot Time since boot in 0.1s incriments
    *  @return nothing
    */
    void updateTlmTimeSinceBoot(uint32_t timeSinceBoot) {
        TlmTimeSinceBoot = timeSinceBoot;
        return;
    }

    /*
    *  callback function, called every 0.1s, incriments the TimeSinceBoot field in the TLM frame
    *  @return nothing
    */
    void tsbCallback(void) {
        TlmTimeSinceBoot++;
    }

    /*
    * Update advertising data
    * @return true on success, false on failure
    */
    bool updateAdvPacket(uint8_t serviceData[], unsigned serviceDataLen) {
        // Fields from the Service
        DBG("Updating AdvFrame: %d", serviceDataLen);
//        printf("\r\n");
//        for(int x = 0; x<serviceDataLen; x++) {
//            printf("%2.2x:",serviceData[x]);
//        }
//        printf("\r\n");
        ble.clearAdvertisingPayload();
        ble.accumulateAdvertisingPayload(GapAdvertisingData::BREDR_NOT_SUPPORTED | GapAdvertisingData::LE_GENERAL_DISCOVERABLE);
        ble.accumulateAdvertisingPayload(GapAdvertisingData::COMPLETE_LIST_16BIT_SERVICE_IDS, BEACON_EDDYSTONE, sizeof(BEACON_EDDYSTONE));
        ble.accumulateAdvertisingPayload(GapAdvertisingData::SERVICE_DATA, serviceData, serviceDataLen);

        return true;
    }

    /*
    *   State machine for switching out frames.
    *   This function is called by the radioNotificationCallback when a frame needs to get swapped out.
    *   This function exists because of time constraints in the radioNotificationCallback, so it is effectively
    *   broken up into two functions.
    */
    void swapOutFrames(void) {
        uint8_t serviceData[SERVICE_DATA_MAX];
        unsigned serviceDataLen = 0;
        //hard code in the eddystone UUID
        serviceData[serviceDataLen++] = BEACON_EDDYSTONE[0];
        serviceData[serviceDataLen++] = BEACON_EDDYSTONE[1];

        // if certain frames are not enabled, then skip them. Worst case TLM is always enabled
        switch(frameIndex) {
            case 1:
                // URL Frame
                if(urlIsSet) {
                    INFO("Swapping in URL Frame: Power: %d",defaultUrlPower);
                    serviceDataLen += constructURLFrame(serviceData+serviceDataLen,20);
                    DBG("\t Swapping in URL Frame: len=%d ",serviceDataLen);
                    updateAdvPacket(serviceData,serviceDataLen);
                    switchFlag = false;
                    frameIndex++;
                    break;
                }
            case 2:
                // UID Frame
                if(uidIsSet) {
                    INFO("Swapping in UID Frame: Power: %d",defaultUidPower);
                    serviceDataLen += constructUIDFrame(serviceData+serviceDataLen,20);
                    DBG("\t Swapping in UID Frame: len=%d",serviceDataLen);
                    updateAdvPacket(serviceData,serviceDataLen);
                    switchFlag = false;
                    frameIndex++;
                    break;
                }
            default:
                // TLM frame
                INFO("Swapping in TLM Frame: version=%x, Batt=%d, Temp = %d, PDUCnt = %d, TimeSinceBoot=%d",TlmVersion, TlmBatteryVoltage, TlmBeaconTemp, TlmPduCount, TlmTimeSinceBoot);
                serviceDataLen += constructTLMFrame(serviceData+serviceDataLen,20);
                DBG("\t Swapping in TLM Frame: len=%d",serviceDataLen);
                updateAdvPacket(serviceData,serviceDataLen);
                frameIndex++;
                break;
        }
    }

    /*
    *  Callback from onRadioNotification(), used to update the PDUCounter and process next state.
    */
#define EDDYSTONE_SWAPFRAME_DELAYMS 1
    void radioNotificationCallback(bool radioActive) {
        //DBG("RadioNotificationCallback : %d, %d, %d, %d",radioActive,frameIndex,TlmPduCount,TlmTimeSinceBoot);
        // Update PDUCount
        TlmPduCount++;
        frameIndex = frameIndex % EDDYSTONE_MAX_FRAMETYPE;


        // True just before an frame is sent, fale just after a frame is sent
        if(radioActive) {
            // Do Nothing
        } else {
            // state machine to control which packet is being sent
            switch(frameIndex) {
                case 0: // TLM Frame
                    switchFrame.attach_us(this, &EddystoneService::swapOutFrames, EDDYSTONE_SWAPFRAME_DELAYMS);
                    switchFlag = true;
                    break;
                case 1: // URL Frame
                    // switch out packets
                    if(switchFlag) {
                        switchFrame.attach_us(this, &EddystoneService::swapOutFrames, EDDYSTONE_SWAPFRAME_DELAYMS);
                        switchFlag = false;
                    } else {
                        if((TlmPduCount % 10) == 0) { // every 10 adv packets switch the frame
                            switchFlag = true;
                        }
                    }
                    break;
                case 2: // UIDFrame
                    // switch out packets
                    if(switchFlag ) {
                        switchFrame.attach_us(this, &EddystoneService::swapOutFrames, EDDYSTONE_SWAPFRAME_DELAYMS);
                        switchFlag = false;
                    } else {
                        if((TlmPduCount % 10) == 0) { // every 10 adv packets switch the frame
                            switchFlag = true;
                        }
                    }
                    break;
            }
        }

        return;
    }

    /*
    *   This function explicityly sets the parameters used by the Eddystone beacon.
    *   this function should be used in leu of the config service.
    *
    *   @param bleIn ble object used to broadcast eddystone information
    *   @oaram beaconPeriodus is how often ble broadcasts are mde, in mili seconds
    *   @param txPowerLevel sets the broadcasting power level.
    *   @param uidNamespaceID 10Byte Namespace UUID
    *   @param uidInstanceID  6Byte  Instance UUID
    *   @param url shortened URL to broadcast (pass in as a string)
    *   @param urlLen length of shortened url
    *   @param tlmVersion version of telemetry data field to use (default to 0x00)
    *
    */
    EddystoneService(BLEDevice       &bleIn,
              uint16_t        beaconPeriodus = 100,
              uint8_t         txPowerLevel = 0,
              uint8_t *       uidNamespaceID = NULL,
              uint8_t *       uidInstanceID = NULL,
              const char *    url = NULL,
              uint8_t         urlLen = 0,
              uint8_t         tlmVersion = 0) :
              ble(bleIn)
    { 
        uint8_t serviceData[SERVICE_DATA_MAX];
        unsigned serviceDataLen = 0;
        ERR("This function is not fully implemented yet, dont use it!!");
        // Check optional frames, set their 'isSet' flags appropriately
        if((uidNamespaceID != NULL) & (uidInstanceID != NULL)) {
            uidIsSet = true;
            setUIDFrameData(txPowerLevel,uidNamespaceID, uidInstanceID);
        } else {
            uidIsSet = false;
        }
        if(url != NULL) {
            urlIsSet = true;
            setURLFrameData(txPowerLevel,url);
        } else {
            uidIsSet = false;
        }
        // Default TLM frame to version 0x00, start all values at zero to be spec compliant.
        setTLMFrameData(tlmVersion, 0x00,0x00);

        // Initialize Frame transition
        frameIndex = 0;
        uidRFU = 0;
        switchFlag = true;

        /* Reinitialize the BLE stack. This will clear away the existing services and advertising state. */
        ble.shutdown();
        ble.init();
        ble.setTxPower(txPowerLevel);
        ble.setAdvertisingType(GapAdvertisingParams::ADV_NON_CONNECTABLE_UNDIRECTED);
        ble.setAdvertisingInterval(beaconPeriodus);

        // Make double sure the PDUCount and TimeSinceBoot fields are set to zero at reset
        updateTlmPduCount(0);
        updateTlmTimeSinceBoot(0);

        // Construct TLM Frame in initial advertising.
        serviceData[serviceDataLen++] = BEACON_EDDYSTONE[0];
        serviceData[serviceDataLen++] = BEACON_EDDYSTONE[1];
        serviceDataLen += constructTLMFrame(serviceData+serviceDataLen,SERVICE_DATA_MAX);

        updateAdvPacket(serviceData, serviceDataLen);
        ble.gap().startAdvertising();
        ble.gap().onRadioNotification(this,&EddystoneService::radioNotificationCallback);
        timeSinceBootTick.attach(this,&EddystoneService::tsbCallback,0.1); // incriment the TimeSinceBoot ticker every 0.1s

    }

private:


    BLEDevice           &ble;
    Ticker              timeSinceBootTick;
    Timeout             switchFrame;
// Default value that is restored on reset
    size_t              defaultUriDataLength;
    UriData_t           defaultUriData;
    UIDNamespaceID_t    defaultUidNamespaceID;
    UIDInstanceID_t     defaultUidInstanceID;
    int8_t              defaultUidPower;
    int8_t              defaultUrlPower;
    uint16_t            uidRFU;
    bool                uidIsSet;
    bool                urlIsSet;
    bool switchFlag;

// Private Variables for Telemetry Data
    uint8_t                      TlmVersion;
    volatile uint16_t            TlmBatteryVoltage;
    volatile uint16_t            TlmBeaconTemp;
    volatile uint32_t            TlmPduCount;
    volatile uint32_t            TlmTimeSinceBoot;

public:
    /*
     *  Encode a human-readable URI into the binary format defined by URIBeacon spec (https://github.com/google/uribeacon/tree/master/specification).
     */
    static void encodeURL(const char *uriDataIn, UriData_t uriDataOut, size_t &sizeofURIDataOut) {
        const char *prefixes[] = {
            "http://www.",
            "https://www.",
            "http://",
            "https://",
        };
        const size_t NUM_PREFIXES = sizeof(prefixes) / sizeof(char *);
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

        sizeofURIDataOut = 0;
        memset(uriDataOut, 0, sizeof(UriData_t));

        if ((uriDataIn == NULL) || (strlen(uriDataIn) == 0)) {
            return;
        }

        /*
         * handle prefix
         */
        for (unsigned i = 0; i < NUM_PREFIXES; i++) {
            size_t prefixLen = strlen(prefixes[i]);
            if (strncmp(uriDataIn, prefixes[i], prefixLen) == 0) {
                uriDataOut[sizeofURIDataOut++]  = i;
                uriDataIn                      += prefixLen;
                break;
            }
        }

        /*
         * handle suffixes
         */
        while (*uriDataIn && (sizeofURIDataOut < URI_DATA_MAX)) {
            /* check for suffix match */
            unsigned i;
            for (i = 0; i < NUM_SUFFIXES; i++) {
                size_t suffixLen = strlen(suffixes[i]);
                if (strncmp(uriDataIn, suffixes[i], suffixLen) == 0) {
                    uriDataOut[sizeofURIDataOut++]  = i;
                    uriDataIn                      += suffixLen;
                    break; /* from the for loop for checking against suffixes */
                }
            }
            /* This is the default case where we've got an ordinary character which doesn't match a suffix. */
            if (i == NUM_SUFFIXES) {
                uriDataOut[sizeofURIDataOut++] = *uriDataIn;
                ++uriDataIn;
            }
        }
    }
};

#endif  // SERVICES_EDDYSTONEBEACON_H_
