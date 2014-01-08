#include "mbed.h"
#include "uuid.h"
#include "hw/nrf51822.h"

/* Radio HW */
nRF51822 radio;

void startBeacon(void)
{
    GapAdvertisingParams advParams ( GapAdvertisingParams::ADV_NON_CONNECTABLE_UNDIRECTED );
    GapAdvertisingData   advData;
    GapAdvertisingData   scanResponse;
    
    /* Define an iBeacon payload
       --------------------------------------------------------------
       128-Bit UUID = E2 0A 39 F4 73 F5 4B C4 A1 2F 17 D1 AD 07 A9 61
       Major/Minor  = 0000 / 0000
       Tx Power     = C8 
    */
    uint8_t iBeaconPayload[25] = { 0x4C, 0x00, 0x02, 0x15, 0xE2, 0x0A, 0x39, 0xF4, 0x73, 0xF5, 0x4B, 0xC4, 0xA1, 0x2F, 0x17, 0xD1, 0xAD, 0x07, 0xA9, 0x61, 0x00, 0x00, 0x00, 0x00, 0xC8 };
    
    /* iBeacon includes the FLAG and MSD fields */
    advData.addFlags(GapAdvertisingData::BREDR_NOT_SUPPORTED);
    advData.addData(GapAdvertisingData::MANUFACTURER_SPECIFIC_DATA, iBeaconPayload, sizeof(iBeaconPayload));

    radio.reset();
    radio.setAdvertising(advParams, advData, scanResponse);
    radio.start();
}

void startBatteryService(void)
{
    GattService        battService ( 0x180F );
    GattCharacteristic battLevel   ( 0x2A19, 1, 1, BLE_GATT_CHAR_PROPERTIES_NOTIFY | BLE_GATT_CHAR_PROPERTIES_READ);

    /* Make sure we get a clean start */    
    radio.reset();
    
    /* Add the characteristic to our service */
    battService.addCharacteristic(battLevel);
    
    /* Pass the service into the radio */
    radio.addService(battService);
    
    /* Configure the radio and start advertising with default values */
    /* Make sure you've added all of your services before calling this function! */
    radio.start();

    /* Now that we're live, update the battery level characteristic */
    uint8_t batt = 72;
    radio.writeCharacteristic(battLevel.handle, (uint8_t*)&batt, sizeof(batt));
}

int main()
{
    /* Give the radio some time to boot up and settle */
    wait(2);

    /* Select one of the options below depending on the demo you want to see */
    // startBeacon();
    startBatteryService();
    
    while(1);
}
