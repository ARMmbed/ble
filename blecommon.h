#ifndef __BLE_COMMON_H__
#define __BLE_COMMON_H__

#include <stdint.h>

typedef enum ble_error_e
{
  BLE_ERROR_NONE = 0
} ble_error_t;

// https://developer.bluetooth.org/gatt/characteristics/Pages/CharacteristicViewer.aspx?u=org.bluetooth.characteristic.gap.appearance.xml
typedef enum ble_gap_char_appearance_e
{
  BLE_GAP_CHAR_APPEARANCE_UNKNOWN                                         = 0,
  BLE_GAP_CHAR_APPEARANCE_GENERIC_PHONE                                   = 64,
  BLE_GAP_CHAR_APPEARANCE_GENERIC_COMPUTER                                = 128,
  BLE_GAP_CHAR_APPEARANCE_GENERIC_WATCH                                   = 192,
  BLE_GAP_CHAR_APPEARANCE_WATCH_SPORTS_WATCH                              = 193,
  BLE_GAP_CHAR_APPEARANCE_GENERIC_CLOCK                                   = 256,
  BLE_GAP_CHAR_APPEARANCE_GENERIC_DISPLAY                                 = 320,
  BLE_GAP_CHAR_APPEARANCE_GENERIC_REMOTE_CONTROL                          = 384,
  BLE_GAP_CHAR_APPEARANCE_GENERIC_EYE_GLASSES                             = 448,
  BLE_GAP_CHAR_APPEARANCE_GENERIC_TAG                                     = 512,
  BLE_GAP_CHAR_APPEARANCE_GENERIC_KEYRING                                 = 576,
  BLE_GAP_CHAR_APPEARANCE_GENERIC_MEDIA_PLAYER                            = 640,
  BLE_GAP_CHAR_APPEARANCE_GENERIC_BARCODE_SCANNER                         = 704,
  BLE_GAP_CHAR_APPEARANCE_GENERIC_THERMOMETER                             = 768,
  BLE_GAP_CHAR_APPEARANCE_THERMOMETER_EAR                                 = 769,
  BLE_GAP_CHAR_APPEARANCE_GENERIC_HEART_RATE_SENSOR                       = 832,
  BLE_GAP_CHAR_APPEARANCE_HEART_RATE_SENSOR_HEART_RATE_BELT               = 833,
  BLE_GAP_CHAR_APPEARANCE_GENERIC_BLOOD_PRESSURE                          = 896,
  BLE_GAP_CHAR_APPEARANCE_BLOOD_PRESSURE_ARM                              = 897,
  BLE_GAP_CHAR_APPEARANCE_BLOD_PRESSURE_WRIST                             = 898,
  BLE_GAP_CHAR_APPEARANCE_HUMAN_INTERFACE_DEVICE_HID                      = 960,
  BLE_GAP_CHAR_APPEARANCE_KEYBOARD                                        = 961,
  BLE_GAP_CHAR_APPEARANCE_MOUSE                                           = 962,
  BLE_GAP_CHAR_APPEARANCE_JOYSTICK                                        = 963,
  BLE_GAP_CHAR_APPEARANCE_GAMEPAD                                         = 964,
  BLE_GAP_CHAR_APPEARANCE_DIGITIZER_TABLET                                = 965,
  BLE_GAP_CHAR_APPEARANCE_CARD_READER                                     = 966,
  BLE_GAP_CHAR_APPEARANCE_DIGITAL_PEN                                     = 967,
  BLE_GAP_CHAR_APPEARANCE_BARCODE_SCANNER                                 = 968,
  BLE_GAP_CHAR_APPEARANCE_GENERIC_GLUCOSE_METER                           = 1024,
  BLE_GAP_CHAR_APPEARANCE_GENERIC_RUNNING_WALKING_SENSOR                  = 1088,
  BLE_GAP_CHAR_APPEARANCE_RUNNING_WALKING_SENSOR_IN_SHOE                  = 1089,
  BLE_GAP_CHAR_APPEARANCE_RUNNING_WALKING_SENSOR_ON_SHOE                  = 1090,
  BLE_GAP_CHAR_APPEARANCE_RUNNING_WALKING_SENSOR_ON_HIP                   = 1091,
  BLE_GAP_CHAR_APPEARANCE_GENERIC_CYCLING                                 = 1152,
  BLE_GAP_CHAR_APPEARANCE_CYCLING_CYCLING_COMPUTER                        = 1153,
  BLE_GAP_CHAR_APPEARANCE_CYCLING_SPEED_SENSOR                            = 1154,
  BLE_GAP_CHAR_APPEARANCE_CYCLING_CADENCE_SENSOR                          = 1155,
  BLE_GAP_CHAR_APPEARANCE_CYCLING_POWER_SENSOR                            = 1156,
  BLE_GAP_CHAR_APPEARANCE_CYCLING_SPEED_AND_CADENCE_SENSOR                = 1157,
  BLE_GAP_CHAR_APPEARANCE_PULSE_OXIMETER_GENERIC                          = 3136,
  BLE_GAP_CHAR_APPEARANCE_PULSE_OXIMETERFINGERTIP                         = 3137,
  BLE_GAP_CHAR_APPEARANCE_PULSE_OXIMETERWRIST_WORN                        = 3138,
  BLE_GAP_CHAR_APPEARANCE_OUTDOOR_GENERIC                                 = 5184,
  BLE_GAP_CHAR_APPEARANCE_OUTDOOR_LOCATION_DISPLAY_DEVICE                 = 5185,
  BLE_GAP_CHAR_APPEARANCE_OUTDOOR_LOCATION_AND_NAVIGATION_DISPLAY_DEVICE  = 5186,
  BLE_GAP_CHAR_APPEARANCE_OUTDOOR_LOCATION_POD                            = 5187,
  BLE_GAP_CHAR_APPEARANCE_OUTDOOR_LOCATION_AND_NAVIGATION_POD             = 5188
} ble_gap_char_appearance_t;

// https://developer.bluetooth.org/gatt/units/Pages/default.aspx
typedef enum ble_gatt_unit_e
{
  BLE_GATT_UNIT_NONE                                                   = 0x2700,
  BLE_GATT_UNIT_LENGTH_METRE                                           = 0x2701,
  BLE_GATT_UNIT_MASS_KILOGRAM                                          = 0x2702,
  BLE_GATT_UNIT_TIME_SECOND                                            = 0x2703,
  BLE_GATT_UNIT_ELECTRIC_CURRENT_AMPERE                                = 0x2704,
  BLE_GATT_UNIT_THERMODYNAMIC_TEMPERATURE_KELVIN                       = 0x2705,
  BLE_GATT_UNIT_AMOUNT_OF_SUBSTANCE_MOLE                               = 0x2706,
  BLE_GATT_UNIT_LUMINOUS_INTENSITY_CANDELA                             = 0x2707,
  BLE_GATT_UNIT_AREA_SQUARE_METRES                                     = 0x2710,
  BLE_GATT_UNIT_VOLUME_CUBIC_METRES                                    = 0x2711,
  BLE_GATT_UNIT_VELOCITY_METRES_PER_SECOND                             = 0x2712,
  BLE_GATT_UNIT_ACCELERATION_METRES_PER_SECOND_SQUARED                 = 0x2713,
  BLE_GATT_UNIT_WAVENUMBER_RECIPROCAL_METRE                            = 0x2714,
  BLE_GATT_UNIT_DENSITY_KILOGRAM_PER_CUBIC_METRE                       = 0x2715,
  BLE_GATT_UNIT_SURFACE_DENSITY_KILOGRAM_PER_SQUARE_METRE              = 0x2716,
  BLE_GATT_UNIT_SPECIFIC_VOLUME_CUBIC_METRE_PER_KILOGRAM               = 0x2717,
  BLE_GATT_UNIT_CURRENT_DENSITY_AMPERE_PER_SQUARE_METRE                = 0x2718,
  BLE_GATT_UNIT_MAGNETIC_FIELD_STRENGTH_AMPERE_PER_METRE               = 0x2719,
  BLE_GATT_UNIT_AMOUNT_CONCENTRATION_MOLE_PER_CUBIC_METRE              = 0x271A,
  BLE_GATT_UNIT_MASS_CONCENTRATION_KILOGRAM_PER_CUBIC_METRE            = 0x271B,
  BLE_GATT_UNIT_LUMINANCE_CANDELA_PER_SQUARE_METRE                     = 0x271C,
  BLE_GATT_UNIT_REFRACTIVE_INDEX                                       = 0x271D,
  BLE_GATT_UNIT_RELATIVE_PERMEABILITY                                  = 0x271E,
  BLE_GATT_UNIT_PLANE_ANGLE_RADIAN                                     = 0x2720,
  BLE_GATT_UNIT_SOLID_ANGLE_STERADIAN                                  = 0x2721,
  BLE_GATT_UNIT_FREQUENCY_HERTZ                                        = 0x2722,
  BLE_GATT_UNIT_FORCE_NEWTON                                           = 0x2723,
  BLE_GATT_UNIT_PRESSURE_PASCAL                                        = 0x2724,
  BLE_GATT_UNIT_ENERGY_JOULE                                           = 0x2725,
  BLE_GATT_UNIT_POWER_WATT                                             = 0x2726,
  BLE_GATT_UNIT_ELECTRIC_CHARGE_COULOMB                                = 0x2727,
  BLE_GATT_UNIT_ELECTRIC_POTENTIAL_DIFFERENCE_VOLT                     = 0x2728,
  BLE_GATT_UNIT_CAPACITANCE_FARAD                                      = 0x2729,
  BLE_GATT_UNIT_ELECTRIC_RESISTANCE_OHM                                = 0x272A,
  BLE_GATT_UNIT_ELECTRIC_CONDUCTANCE_SIEMENS                           = 0x272B,
  BLE_GATT_UNIT_MAGNETIC_FLEX_WEBER                                    = 0x272C,
  BLE_GATT_UNIT_MAGNETIC_FLEX_DENSITY_TESLA                            = 0x272D,
  BLE_GATT_UNIT_INDUCTANCE_HENRY                                       = 0x272E,
  BLE_GATT_UNIT_THERMODYNAMIC_TEMPERATURE_DEGREE_CELSIUS               = 0x272F,
  BLE_GATT_UNIT_LUMINOUS_FLUX_LUMEN                                    = 0x2730,
  BLE_GATT_UNIT_ILLUMINANCE_LUX                                        = 0x2731,
  BLE_GATT_UNIT_ACTIVITY_REFERRED_TO_A_RADIONUCLIDE_BECQUEREL          = 0x2732,
  BLE_GATT_UNIT_ABSORBED_DOSE_GRAY                                     = 0x2733,
  BLE_GATT_UNIT_DOSE_EQUIVALENT_SIEVERT                                = 0x2734,
  BLE_GATT_UNIT_CATALYTIC_ACTIVITY_KATAL                               = 0x2735,
  BLE_GATT_UNIT_DYNAMIC_VISCOSITY_PASCAL_SECOND                        = 0x2740,
  BLE_GATT_UNIT_MOMENT_OF_FORCE_NEWTON_METRE                           = 0x2741,
  BLE_GATT_UNIT_SURFACE_TENSION_NEWTON_PER_METRE                       = 0x2742,
  BLE_GATT_UNIT_ANGULAR_VELOCITY_RADIAN_PER_SECOND                     = 0x2743,
  BLE_GATT_UNIT_ANGULAR_ACCELERATION_RADIAN_PER_SECOND_SQUARED         = 0x2744,
  BLE_GATT_UNIT_HEAT_FLUX_DENSITY_WATT_PER_SQUARE_METRE                = 0x2745,
  BLE_GATT_UNIT_HEAT_CAPACITY_JOULE_PER_KELVIN                         = 0x2746,
  BLE_GATT_UNIT_SPECIFIC_HEAT_CAPACITY_JOULE_PER_KILOGRAM_KELVIN       = 0x2747,
  BLE_GATT_UNIT_SPECIFIC_ENERGY_JOULE_PER_KILOGRAM                     = 0x2748,
  BLE_GATT_UNIT_THERMAL_CONDUCTIVITY_WATT_PER_METRE_KELVIN             = 0x2749,
  BLE_GATT_UNIT_ENERGY_DENSITY_JOULE_PER_CUBIC_METRE                   = 0x274A,
  BLE_GATT_UNIT_ELECTRIC_FIELD_STRENGTH_VOLT_PER_METRE                 = 0x274B,
  BLE_GATT_UNIT_ELECTRIC_CHARGE_DENSITY_COULOMB_PER_CUBIC_METRE        = 0x274C,
  BLE_GATT_UNIT_SURFACE_CHARGE_DENSITY_COULOMB_PER_SQUARE_METRE        = 0x274D,
  BLE_GATT_UNIT_ELECTRIC_FLUX_DENSITY_COULOMB_PER_SQUARE_METRE         = 0x274E,
  BLE_GATT_UNIT_PERMITTIVITY_FARAD_PER_METRE                           = 0x274F,
  BLE_GATT_UNIT_PERMEABILITY_HENRY_PER_METRE                           = 0x2750,
  BLE_GATT_UNIT_MOLAR_ENERGY_JOULE_PER_MOLE                            = 0x2751,
  BLE_GATT_UNIT_MOLAR_ENTROPY_JOULE_PER_MOLE_KELVIN                    = 0x2752,
  BLE_GATT_UNIT_EXPOSURE_COULOMB_PER_KILOGRAM                          = 0x2753,
  BLE_GATT_UNIT_ABSORBED_DOSE_RATE_GRAY_PER_SECOND                     = 0x2754,
  BLE_GATT_UNIT_RADIANT_INTENSITY_WATT_PER_STERADIAN                   = 0x2755,
  BLE_GATT_UNIT_RADIANCE_WATT_PER_SQUARE_METRE_STERADIAN               = 0x2756,
  BLE_GATT_UNIT_CATALYTIC_ACTIVITY_CONCENTRATION_KATAL_PER_CUBIC_METRE = 0x2757,
  BLE_GATT_UNIT_TIME_MINUTE                                            = 0x2760,
  BLE_GATT_UNIT_TIME_HOUR                                              = 0x2761,
  BLE_GATT_UNIT_TIME_DAY                                               = 0x2762,
  BLE_GATT_UNIT_PLANE_ANGLE_DEGREE                                     = 0x2763,
  BLE_GATT_UNIT_PLANE_ANGLE_MINUTE                                     = 0x2764,
  BLE_GATT_UNIT_PLANE_ANGLE_SECOND                                     = 0x2765,
  BLE_GATT_UNIT_AREA_HECTARE                                           = 0x2766,
  BLE_GATT_UNIT_VOLUME_LITRE                                           = 0x2767,
  BLE_GATT_UNIT_MASS_TONNE                                             = 0x2768,
  BLE_GATT_UNIT_PRESSURE_BAR                                           = 0x2780,
  BLE_GATT_UNIT_PRESSURE_MILLIMETRE_OF_MERCURY                         = 0x2781,
  BLE_GATT_UNIT_LENGTH_ANGSTROM                                        = 0x2782,
  BLE_GATT_UNIT_LENGTH_NAUTICAL_MILE                                   = 0x2783,
  BLE_GATT_UNIT_AREA_BARN                                              = 0x2784,
  BLE_GATT_UNIT_VELOCITY_KNOT                                          = 0x2785,
  BLE_GATT_UNIT_LOGARITHMIC_RADIO_QUANTITY_NEPER                       = 0x2786,
  BLE_GATT_UNIT_LOGARITHMIC_RADIO_QUANTITY_BEL                         = 0x2787,
  BLE_GATT_UNIT_LENGTH_YARD                                            = 0x27A0,
  BLE_GATT_UNIT_LENGTH_PARSEC                                          = 0x27A1,
  BLE_GATT_UNIT_LENGTH_INCH                                            = 0x27A2,
  BLE_GATT_UNIT_LENGTH_FOOT                                            = 0x27A3,
  BLE_GATT_UNIT_LENGTH_MILE                                            = 0x27A4,
  BLE_GATT_UNIT_PRESSURE_POUND_FORCE_PER_SQUARE_INCH                   = 0x27A5,
  BLE_GATT_UNIT_VELOCITY_KILOMETRE_PER_HOUR                            = 0x27A6,
  BLE_GATT_UNIT_VELOCITY_MILE_PER_HOUR                                 = 0x27A7,
  BLE_GATT_UNIT_ANGULAR_VELOCITY_REVOLUTION_PER_MINUTE                 = 0x27A8,
  BLE_GATT_UNIT_ENERGY_GRAM_CALORIE                                    = 0x27A9,
  BLE_GATT_UNIT_ENERGY_KILOGRAM_CALORIE                                = 0x27AA,
  BLE_GATT_UNIT_ENERGY_KILOWATT_HOUR                                   = 0x27AB,
  BLE_GATT_UNIT_THERMODYNAMIC_TEMPERATURE_DEGREE_FAHRENHEIT            = 0x27AC,
  BLE_GATT_UNIT_PERCENTAGE                                             = 0x27AD,
  BLE_GATT_UNIT_PER_MILLE                                              = 0x27AE,
  BLE_GATT_UNIT_PERIOD_BEATS_PER_MINUTE                                = 0x27AF,
  BLE_GATT_UNIT_ELECTRIC_CHARGE_AMPERE_HOURS                           = 0x27B0,
  BLE_GATT_UNIT_MASS_DENSITY_MILLIGRAM_PER_DECILITRE                   = 0x27B1,
  BLE_GATT_UNIT_MASS_DENSITY_MILLIMOLE_PER_LITRE                       = 0x27B2,
  BLE_GATT_UNIT_TIME_YEAR                                              = 0x27B3,
  BLE_GATT_UNIT_TIME_MONTH                                             = 0x27B4,
  BLE_GATT_UNIT_CONCENTRATION_COUNT_PER_CUBIC_METRE                    = 0x27B5,
  BLE_GATT_UNIT_IRRADIANCE_WATT_PER_SQUARE_METRE                       = 0x27B6
} ble_gatt_unit_t;

// http://developer.bluetooth.org/gatt/descriptors/Pages/DescriptorViewer.aspx?u=org.bluetooth.descriptor.gatt.characteristic_presentation_format.xml
typedef enum ble_gatt_format_e
{
  BLE_GATT_FORMAT_RFU                 = 0x00, /**< Reserved For Future Use. */
  BLE_GATT_FORMAT_BOOLEAN             = 0x01, /**< Boolean. */
  BLE_GATT_FORMAT_2BIT                = 0x02, /**< Unsigned 2-bit integer. */
  BLE_GATT_FORMAT_NIBBLE              = 0x03, /**< Unsigned 4-bit integer. */
  BLE_GATT_FORMAT_UINT8               = 0x04, /**< Unsigned 8-bit integer. */
  BLE_GATT_FORMAT_UINT12              = 0x05, /**< Unsigned 12-bit integer. */
  BLE_GATT_FORMAT_UINT16              = 0x06, /**< Unsigned 16-bit integer. */
  BLE_GATT_FORMAT_UINT24              = 0x07, /**< Unsigned 24-bit integer. */
  BLE_GATT_FORMAT_UINT32              = 0x08, /**< Unsigned 32-bit integer. */
  BLE_GATT_FORMAT_UINT48              = 0x09, /**< Unsigned 48-bit integer. */
  BLE_GATT_FORMAT_UINT64              = 0x0A, /**< Unsigned 64-bit integer. */
  BLE_GATT_FORMAT_UINT128             = 0x0B, /**< Unsigned 128-bit integer. */
  BLE_GATT_FORMAT_SINT8               = 0x0C, /**< Signed 2-bit integer. */
  BLE_GATT_FORMAT_SINT12              = 0x0D, /**< Signed 12-bit integer. */
  BLE_GATT_FORMAT_SINT16              = 0x0E, /**< Signed 16-bit integer. */
  BLE_GATT_FORMAT_SINT24              = 0x0F, /**< Signed 24-bit integer. */
  BLE_GATT_FORMAT_SINT32              = 0x10, /**< Signed 32-bit integer. */
  BLE_GATT_FORMAT_SINT48              = 0x11, /**< Signed 48-bit integer. */
  BLE_GATT_FORMAT_SINT64              = 0x12, /**< Signed 64-bit integer. */
  BLE_GATT_FORMAT_SINT128             = 0x13, /**< Signed 128-bit integer. */
  BLE_GATT_FORMAT_FLOAT32             = 0x14, /**< IEEE-754 32-bit floating point. */
  BLE_GATT_FORMAT_FLOAT64             = 0x15, /**< IEEE-754 64-bit floating point. */
  BLE_GATT_FORMAT_SFLOAT              = 0x16, /**< IEEE-11073 16-bit SFLOAT. */
  BLE_GATT_FORMAT_FLOAT               = 0x17, /**< IEEE-11073 32-bit FLOAT. */
  BLE_GATT_FORMAT_DUINT16             = 0x18, /**< IEEE-20601 format. */
  BLE_GATT_FORMAT_UTF8S               = 0x19, /**< UTF-8 string. */
  BLE_GATT_FORMAT_UTF16S              = 0x1A, /**< UTF-16 string. */
  BLE_GATT_FORMAT_STRUCT              = 0x1B /**< Opaque Structure. */
} ble_gatt_format_t;

struct UTF8String
{
  uint16_t  length;         /**< String length. */
  uint8_t  str[32];         /**< String data. */
};

struct UTF16String
{
  uint16_t  length;         /**< String length. */
  uint16_t  str[32];        /**< String data. */
};

struct SecurityMode
{
  uint8_t  mode;            /**< Security Mode (1 or 2), 0 for no permissions at all. */
  uint8_t  level;           /**< Level (1, 2 or 3), 0 for no permissions at all. */
};

// See https://developer.bluetooth.org/gatt/descriptors/Pages/DescriptorViewer.aspx?u=org.bluetooth.descriptor.gatt.characteristic_presentation_format.xml
struct PresentationFormat
{
  uint8_t   format;         /**< Format of the value, see @ref ble_gatt_format_t. */
  int8_t    exponent;       /**< Exponent for integer data types. */
  uint16_t  unit;           /**< UUID from Bluetooth Assigned Numbers, see @ref ble_gatt_unit_t. */
  uint8_t   name_space;     /**< Namespace from Bluetooth Assigned Numbers, see @ref BLE_GATT_CPF_NAMESPACES. */
  uint16_t  desc;           /**< Namespace description from Bluetooth Assigned Numbers, see @ref BLE_GATT_CPF_NAMESPACES. */
};

#endif
