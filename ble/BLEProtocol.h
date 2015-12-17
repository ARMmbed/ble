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
                          * would allow the use of AliasedType_t::PUBLIC in code. */
        enum Type {
            PUBLIC = 0,
            RANDOM_STATIC,
            RANDOM_PRIVATE_RESOLVABLE,
            RANDOM_PRIVATE_NON_RESOLVABLE
        };
    };
    typedef AddressType::Type AddressType_t; /**< Alias for AddressType::Type */

    static const size_t ADDR_LEN = 6;        /**< Length (in octets) of the BLE MAC address. */
    typedef uint8_t Address_t[ADDR_LEN];     /**< 48-bit address, in LSB format. */

    /**
     *  A list of values for the APPEARANCE AD Type, which describes the
     *  physical shape or appearance of the device. Retrieved from:
     *      - Bluetooth Core Specification Supplement, Part A, Section 1.12
     *      - Bluetooth Core Specification 4.0 (Vol. 3), Part C, Section 12.2
     *      - https://developer.bluetooth.org/gatt/characteristics/Pages/CharacteristicViewer.aspx?u=org.bluetooth.characteristic.gap.appearance.xml
     */
    struct AppearanceType {
        enum Type {
            UNKNOWN                                        = 0,     /**< Unknown or unspecified appearance type. */
            GENERIC_PHONE                                  = 64,    /**< Generic Phone. */
            GENERIC_COMPUTER                               = 128,   /**< Generic Computer. */
            GENERIC_WATCH                                  = 192,   /**< Generic Watch. */
            WATCH_SPORTS_WATCH                             = 193,   /**< Sports Watch. */
            GENERIC_CLOCK                                  = 256,   /**< Generic Clock. */
            GENERIC_DISPLAY                                = 320,   /**< Generic Display. */
            GENERIC_REMOTE_CONTROL                         = 384,   /**< Generic Remote Control. */
            GENERIC_EYE_GLASSES                            = 448,   /**< Generic Eye Glasses. */
            GENERIC_TAG                                    = 512,   /**< Generic Tag. */
            GENERIC_KEYRING                                = 576,   /**< Generic Keyring. */
            GENERIC_MEDIA_PLAYER                           = 640,   /**< Generic Media Player. */
            GENERIC_BARCODE_SCANNER                        = 704,   /**< Generic Barcode Scanner. */
            GENERIC_THERMOMETER                            = 768,   /**< Generic Thermometer. */
            THERMOMETER_EAR                                = 769,   /**< Ear Thermometer. */
            GENERIC_HEART_RATE_SENSOR                      = 832,   /**< Generic Heart Rate Sensor. */
            HEART_RATE_SENSOR_HEART_RATE_BELT              = 833,   /**< Belt Heart Rate Sensor. */
            GENERIC_BLOOD_PRESSURE                         = 896,   /**< Generic Blood Pressure. */
            BLOOD_PRESSURE_ARM                             = 897,   /**< Arm Blood Pressure. */
            BLOOD_PRESSURE_WRIST                           = 898,   /**< Wrist Blood Pressure. */
            HUMAN_INTERFACE_DEVICE_HID                     = 960,   /**< Human Interface Device (HID). */
            KEYBOARD                                       = 961,   /**< Keyboard. */
            MOUSE                                          = 962,   /**< Mouse. */
            JOYSTICK                                       = 963,   /**< Joystick. */
            GAMEPAD                                        = 964,   /**< Gamepad. */
            DIGITIZER_TABLET                               = 965,   /**< Digitizer Tablet. */
            CARD_READER                                    = 966,   /**< Card Reader. */
            DIGITAL_PEN                                    = 967,   /**< Digital Pen. */
            BARCODE_SCANNER                                = 968,   /**< Barcode Scanner. */
            GENERIC_GLUCOSE_METER                          = 1024,  /**< Generic Glucose Meter. */
            GENERIC_RUNNING_WALKING_SENSOR                 = 1088,  /**< Generic Running/Walking Sensor. */
            RUNNING_WALKING_SENSOR_IN_SHOE                 = 1089,  /**< In Shoe Running/Walking Sensor. */
            RUNNING_WALKING_SENSOR_ON_SHOE                 = 1090,  /**< On Shoe Running/Walking Sensor. */
            RUNNING_WALKING_SENSOR_ON_HIP                  = 1091,  /**< On Hip Running/Walking Sensor. */
            GENERIC_CYCLING                                = 1152,  /**< Generic Cycling. */
            CYCLING_CYCLING_COMPUTER                       = 1153,  /**< Cycling Computer. */
            CYCLING_SPEED_SENSOR                           = 1154,  /**< Cycling Speed Sensor. */
            CYCLING_CADENCE_SENSOR                         = 1155,  /**< Cycling Cadence Sensor. */
            CYCLING_POWER_SENSOR                           = 1156,  /**< Cycling Power Sensor. */
            CYCLING_SPEED_AND_CADENCE_SENSOR               = 1157,  /**< Cycling Speed and Cadence Sensor. */
            PULSE_OXIMETER_GENERIC                         = 3136,  /**< Generic Pulse Oximeter. */
            PULSE_OXIMETER_FINGERTIP                       = 3137,  /**< Fingertip Pulse Oximeter. */
            PULSE_OXIMETER_WRIST_WORN                      = 3138,  /**< Wrist Worn Pulse Oximeter. */
            GENERIC_WEIGHT_SCALE                           = 3200,  /**< Generic Weight Scale */
            OUTDOOR_GENERIC                                = 5184,  /**< Generic Outdoor. */
            OUTDOOR_LOCATION_DISPLAY_DEVICE                = 5185,  /**< Outdoor Location Display Device. */
            OUTDOOR_LOCATION_AND_NAVIGATION_DISPLAY_DEVICE = 5186,  /**< Outdoor Location and Navigation Display Device. */
            OUTDOOR_LOCATION_POD                           = 5187,  /**< Outdoor Location Pod. */
            OUTDOOR_LOCATION_AND_NAVIGATION_POD            = 5188   /**< Outdoor Location and Navigation Pod. */
        };
    };
    typedef AppearanceType::Type AppearanceType_t; /**< Alias for @ref AppearanceType::Type */
};

#endif /* __BLE_PROTOCOL_H__ */
