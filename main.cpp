#include "mbed.h"
#include "uuid.h"
#include "hw/nrf51822.h"

DigitalOut        myled ( LED1 );

/* Radio HW */
nRF51822 radio;

void startBeacon(void)
{
    ble_error_t          error;
    GapAdvertisingParams advParams ( GapAdvertisingParams::ADV_NON_CONNECTABLE_UNDIRECTED );
    GapAdvertisingData   advData;
    GapAdvertisingData   scanResponse;
    
    uint8_t iBeaconPayload[25] = { 0x4C, 0x00, 0x02, 0x15, 0xE2, 0x0A, 0x39, 0xF4, 0x73, 0xF5, 0x4B, 0xC4, 0xA1, 0x2F, 0x17, 0xD1, 0xAD, 0x07, 0xA9, 0x61, 0x00, 0x00, 0x00, 0x00, 0xC8 };
    
    
    /* iBeacon includes the FLAG and MSD fields */
    error = advData.addFlags(GapAdvertisingData::BREDR_NOT_SUPPORTED);
    error = advData.addData(GapAdvertisingData::MANUFACTURER_SPECIFIC_DATA, iBeaconPayload, 25);

    error = radio.reset();
    error = radio.setAdvertising(advParams, advData, scanResponse);
    error = radio.start();
    
    /* Hang around here for a while */    
    while(1)
    {
    }
}

void startBatteryService(void)
{
    ble_error_t        error;
    GattService        battService ( 0x180F );
    GattCharacteristic battLevel   ( 0x2A19, 1, 1, 0x10 | 0x02);
    
    error = radio.reset();
    error = battService.addCharacteristic(battLevel);
    error = radio.addService(battService);
    error = radio.start();

    uint8_t batt = 72;
    error = radio.writeCharacteristic(battLevel.handle, (uint8_t*)&batt, sizeof(batt));

    /* Hang around here for a while */    
    while(1)
    {
    }
}

int main()
{
    /* Give the radio some time to boot up and settle */
    wait(2);

    // startBeacon();
    startBatteryService();
    
    while(1);
}
