#include "mbed.h"

#include "uuid.h"
#include "bleservice.h"
#include "blecharacteristic.h"
#include "hw/nrf51822.h"

DigitalOut        myled ( LED1 );

/* Radio HW abstraction layer */
nRF51822 radio;

/* Battery Level Service */
/* See --> https://developer.bluetooth.org/gatt/services/Pages/ServiceViewer.aspx?u=org.bluetooth.service.battery_service.xml */
BLEService        battService   ( 0x180F );
BLECharacteristic battLevel     ( 0x2A19, 1, 1, 0x10 | 0x02);   /* Read + Notify */

/* Heart Rate Monitor Service */
/* See --> https://developer.bluetooth.org/gatt/services/Pages/ServiceViewer.aspx?u=org.bluetooth.service.heart_rate.xml */
BLEService        hrmService    ( 0x180D );
BLECharacteristic hrmRate       ( 0x2A37, 2, 3, 0x10 );         /* Notify */
BLECharacteristic hrmLocation   ( 0x2A39, 1, 1, 0x02 );         /* Read */

/* Health Thermometer Service */
/* See --> https://developer.bluetooth.org/gatt/services/Pages/ServiceViewer.aspx?u=org.bluetooth.service.health_thermometer.xml */
BLEService        thermService  ( 0x1809 );
BLECharacteristic thermTemp     ( 0x2A1C, 5, 13, 0x20 );         /* Indicate */
BLECharacteristic thermType     ( 0x2A1D, 1, 1, 0x02 );          /* Read */
BLECharacteristic thermInterval ( 0x2A21, 2, 2, 0x02 );          /* Read */

/* Notify   = device (server) sends data when it changes */
/* Indicate = device (server) sends data when it changes and client confirms reception */ 
 
int main()
{
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
    radio.updateValue(hrmService.index, hrmLocation.index, (uint8_t*)&location, sizeof(location));

    /* Update the battery level */
    /* See --> https://developer.bluetooth.org/gatt/characteristics/Pages/CharacteristicViewer.aspx?u=org.bluetooth.characteristic.battery_level.xml */
    uint8_t batt = 72; /* Percentage (0..100) */
    radio.updateValue(battService.index, battLevel.index, (uint8_t*)&batt, sizeof(batt));

    /* Update the fixed health thermometer characteristics */
    /* See --> https://developer.bluetooth.org/gatt/characteristics/Pages/CharacteristicViewer.aspx?u=org.bluetooth.characteristic.temperature_type.xml */
    uint8_t thermLocation = 6; /* Location = mouth */
    radio.updateValue(thermService.index, thermType.index, (uint8_t*)&thermLocation, sizeof(thermLocation));
    /* See --> https://developer.bluetooth.org/gatt/characteristics/Pages/CharacteristicViewer.aspx?u=org.bluetooth.characteristic.measurement_interval.xml */
    uint16_t thermDelay = 5;
    radio.updateValue(thermService.index, thermInterval.index, (uint8_t*)&thermDelay, sizeof(thermDelay));
    
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
        radio.updateValue(hrmService.index, hrmRate.index, bpm, 2);
        
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
        radio.updateValue(thermService.index, thermTemp.index, temperature, 5);
    }
}
