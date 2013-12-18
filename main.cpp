#include "mbed.h"

#include "uuid.h"
#include "GattService.h"
#include "GattCharacteristic.h"
#include "hw/nrf51822.h"

DigitalOut        myled ( LED1 );

/* Radio HW abstraction layer */
nRF51822 radio;

/* Heart Rate Monitor Service */
/* See --> https://developer.bluetooth.org/gatt/services/Pages/ServiceViewer.aspx?u=org.bluetooth.service.heart_rate.xml */
GattService        hrmService    ( 0x180D );
GattCharacteristic hrmRate       ( 0x2A37, 2, 3, BLE_GATT_CHAR_PROPERTIES_NOTIFY );
GattCharacteristic hrmLocation   ( 0x2A39, 1, 1, BLE_GATT_CHAR_PROPERTIES_READ );

/* GAP Advertising Example (iBeacon) */
GapAdvertisingParams advParams ( GapAdvertisingParams::ADV_NON_CONNECTABLE_UNDIRECTED );
GapAdvertisingData   advData;
GapAdvertisingData   scanResponse;

uint8_t iBeaconPayload[25] = { 0x4C, 0x00, 0x02, 0x15, 0xE2, 0x0A, 0x39, 0xF4, 0x73, 0xF5, 0x4B, 0xC4, 0xA1, 0x2F, 0x17, 0xD1, 0xAD, 0x07, 0xA9, 0x61, 0x00, 0x00, 0x00, 0x00, 0xC8 };

void startBeacon(void)
{
    ble_error_t error;
    
    /* iBeacon includes the FLAG and MSD fields */
    advData.addFlags(GapAdvertisingData::BREDR_NOT_SUPPORTED);
    advData.addData(GapAdvertisingData::MANUFACTURER_SPECIFIC_DATA, iBeaconPayload, 25);
    
    error = radio.reset();
    error = radio.setAdvertising(advParams, advData, scanResponse);
    error = radio.start();
}

void startHRM(void)
{
    uint8_t hrmCounter = 100;
    
    /* Add the heart rate and sensor location chars to the HRM service      */
    hrmService.addCharacteristic(hrmRate);
    hrmService.addCharacteristic(hrmLocation);

    /* Reset the BLE hardware to make sure we get a clean start             */
    wait(2);
    radio.reset();
    
    /* Add the services to the radio HAL */
    radio.addService(hrmService);
    
    /* Start the services so that we can start pushing data out */
    radio.start();
    
    /* Set the heart rate monitor location (one time only) */
    /* See --> https://developer.bluetooth.org/gatt/characteristics/Pages/CharacteristicViewer.aspx?u=org.bluetooth.characteristic.body_sensor_location.xml */
    uint8_t location = 0x01; /* Chest */
    radio.writeCharacteristic(hrmService, hrmLocation, (uint8_t*)&location, sizeof(location));

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
    }
}

int main()
{
    /* Give the radio some time to boot up and settle */
    wait(2);

    /* Only use one of these two options! */
    startBeacon();
    // startHRM();
    
    while(1);
}
