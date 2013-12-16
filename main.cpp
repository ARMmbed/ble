#include "mbed.h"

#include "uuid.h"
#include "GattService.h"
#include "GattCharacteristic.h"
#include "hw/nrf51822.h"

DigitalOut        myled ( LED1 );

/* Radio HW abstraction layer */
nRF51822 radio;

/* Battery Level Service */
/* See --> https://developer.bluetooth.org/gatt/services/Pages/ServiceViewer.aspx?u=org.bluetooth.service.battery_service.xml */
GattService        battService   ( 0x180F );
GattCharacteristic battLevel     ( 0x2A19, 1, 1, BLE_GATT_CHAR_PROPERTIES_NOTIFY | BLE_GATT_CHAR_PROPERTIES_READ );

/* Heart Rate Monitor Service */
/* See --> https://developer.bluetooth.org/gatt/services/Pages/ServiceViewer.aspx?u=org.bluetooth.service.heart_rate.xml */
GattService        hrmService    ( 0x180D );
GattCharacteristic hrmRate       ( 0x2A37, 2, 3, BLE_GATT_CHAR_PROPERTIES_NOTIFY );
GattCharacteristic hrmLocation   ( 0x2A39, 1, 1, BLE_GATT_CHAR_PROPERTIES_READ );

/* Health Thermometer Service */
/* See --> https://developer.bluetooth.org/gatt/services/Pages/ServiceViewer.aspx?u=org.bluetooth.service.health_thermometer.xml */
GattService        thermService  ( 0x1809 );
GattCharacteristic thermTemp     ( 0x2A1C, 5, 13, BLE_GATT_CHAR_PROPERTIES_INDICATE );
GattCharacteristic thermType     ( 0x2A1D, 1, 1, BLE_GATT_CHAR_PROPERTIES_READ );
GattCharacteristic thermInterval ( 0x2A21, 2, 2, BLE_GATT_CHAR_PROPERTIES_READ );

/* Notify   = device (server) sends data when it changes */
/* Indicate = device (server) sends data when it changes and client confirms reception */

/* GAP Advertising Example (iBeacon) */
GapAdvertisingParams advParams ( GapAdvertisingParams::ADV_NON_CONNECTABLE_UNDIRECTED );
GapAdvertisingData   advData;
GapAdvertisingData   scanResponse;

uint8_t iBeaconPayload[25] = { 0x4C, 0x00, 0x02, 0x15, 0xE2, 0x0A, 0x39, 0xF4, 0x73, 0xF5, 0x4B, 0xC4, 0xA1, 0x2F, 0x17, 0xD1, 0xAD, 0x07, 0xA9, 0x61, 0x00, 0x00, 0x00, 0x00, 0xC8 };

void startBeacon(void)
{
    /* iBeacon includes the FLAG and MSD fields */
    advData.addFlags(GapAdvertisingData::BREDR_NOT_SUPPORTED);
    advData.addData(GapAdvertisingData::MANUFACTURER_SPECIFIC_DATA, iBeaconPayload, 25);
    
    wait(2);
    radio.reset();
    radio.setAdvertising(advParams, advData, scanResponse);
    radio.start();
}

int main()
{
    wait(2);
    // radio.test();
    startBeacon();
    while(1);

    /* Add the battery level characteristic to the battery service          */
    /* Note: This will also update the characteristic's .index field        */
    /* so that we know where it's stored in the BLEService.characteristics  */
    /* array.                                                               */
    battService.addCharacteristic(battLevel);
    
    /* Add the heart rate and sensor location chars to the HRM service      */
    hrmService.addCharacteristic(hrmRate);
    hrmService.addCharacteristic(hrmLocation);
    
    /* Add the Health Thermometer server characteristics                    */
    thermService.addCharacteristic(thermTemp);
    thermService.addCharacteristic(thermType);
    thermService.addCharacteristic(thermInterval);
    
    /* Reset the BLE hardware to make sure we get a clean start             */
    wait(2);
    radio.reset();
    
    /* Add the services to the radio HAL */
    radio.addService(battService);
    radio.addService(hrmService);
    radio.addService(thermService);
    
    /* Start the services so that we can start pushing data out */
    radio.start();

    /* Set the heart rate monitor location (one time only) */
    /* See --> https://developer.bluetooth.org/gatt/characteristics/Pages/CharacteristicViewer.aspx?u=org.bluetooth.characteristic.body_sensor_location.xml */
    uint8_t location = 0x01; /* Chest */
    radio.writeCharacteristic(hrmService, hrmLocation, (uint8_t*)&location, sizeof(location));

    /* Update the battery level */
    /* See --> https://developer.bluetooth.org/gatt/characteristics/Pages/CharacteristicViewer.aspx?u=org.bluetooth.characteristic.battery_level.xml */
    uint8_t batt = 72; /* Percentage (0..100) */
    radio.writeCharacteristic(battService, battLevel, (uint8_t*)&batt, sizeof(batt));

    /* Update the fixed health thermometer characteristics */
    /* See --> https://developer.bluetooth.org/gatt/characteristics/Pages/CharacteristicViewer.aspx?u=org.bluetooth.characteristic.temperature_type.xml */
    uint8_t thermLocation = 6; /* Location = mouth */
    radio.writeCharacteristic(thermService, thermType, (uint8_t*)&thermLocation, sizeof(thermLocation));
    /* See --> https://developer.bluetooth.org/gatt/characteristics/Pages/CharacteristicViewer.aspx?u=org.bluetooth.characteristic.measurement_interval.xml */
    uint16_t thermDelay = 5;
    radio.writeCharacteristic(thermService, thermInterval, (uint8_t*)&thermDelay, sizeof(thermDelay));
    
    /* Blinky + value updates */
    uint8_t hrmCounter = 100;
        
    while(1)
    {
        myled = 1;
        wait(0.1);
        myled = 0;
        wait(0.1);
                
        /* Update the HRM measurement */
        /* First byte = 8-bit values, no extra info, Second byte = uint8_t HRM value */
        /* See --> https://developer.bluetooth.org/gatt/characteristics/Pages/CharacteristicViewer.aspx?u=org.bluetooth.characteristic.heart_rate_measurement.xml */
        hrmCounter++;
        if (hrmCounter == 175) hrmCounter = 100;
        uint8_t bpm[2] = { 0x00, hrmCounter };
        radio.writeCharacteristic(hrmService, hrmRate, bpm, 2);
        
        /* Update the Health Thermometer measurement */
        
        // NOTE: This is an IEEE-11073 32-bit float NOT a IEEE-754 float (standard single precision float type) !!!
        // See: Section 2.2 of https://www.bluetooth.org/docman/handlers/downloaddoc.ashx?doc_id=242961
        // Example:
        // Consider a temperature measurement of 36.4 degrees Celsius with precision of 0.1 degrees Celsius.
        // The FLOAT-Type representation is a 32-bit value consisting of an exponent of an 8-bit signed integer
        // followed by a mantissa of a 24-bit signed integer; here, the exponent is -1 (0xFF) and the mantissa
        // is 364 (0x00016C). Therefore, the FLOAT-Type representation of 36.4 is 0xFF00016C.
        
        uint8_t temperature[5] = { 0x00, 0x00, 0x00, 0x00, 0xFF };
        // Use the hrm counter to provide a shifting temperature value (175 = 17.5C, etc.)
        memcpy (temperature+1, &hrmCounter, 1);
        radio.writeCharacteristic(thermService, thermTemp, temperature, 5);
    }
}
