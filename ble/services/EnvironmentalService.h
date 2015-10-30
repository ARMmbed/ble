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
 
#ifndef __BLE_ENVIRONMENTAL_SERVICE_H__
#define __BLE_ENVIRONMENTAL_SERVICE_H__

#include "ble/BLE.h"

 /**
* @class EnvironmentalService
* @brief BLE Environmental Service. This service provides the location of the thermometer and the temperature.  <br>
* Service:  https://developer.bluetooth.org/gatt/services/Pages/ServiceViewer.aspx?u=org.bluetooth.service.environmental_sensing.xml <br>
* Temperature: https://developer.bluetooth.org/gatt/characteristics/Pages/CharacteristicViewer.aspx?u=org.bluetooth.characteristic.temperature.xml <br>
* Humidity: https://developer.bluetooth.org/gatt/characteristics/Pages/CharacteristicViewer.aspx?u=org.bluetooth.characteristic.humidity.xml <br>
* Pressure: https://developer.bluetooth.org/gatt/characteristics/Pages/CharacteristicViewer.aspx?u=org.bluetooth.characteristic.pressure.xml
*/
class EnvironmentalService {
public:
    /**
     * @brief   EnvironmentalService constructor.
     * @param   ble Reference to BLE device.
     * @param   temperature_en Enable this characteristic.
     * @param   humidity_en Enable this characteristic.
     * @param   pressure_en Enable this characteristic.
     */
    EnvironmentalService(BLE &_ble,
                         bool temperature_en = false,
                         bool humidity_en = false,
                         bool pressure_en = false) :
        ble(_ble),
        temperatureCharacteristic(GattCharacteristic::UUID_TEMPERATURE_CHAR,
                              (uint8_t *) &temperature,
                              (temperature_en) ? 2 : 0, // minLength
                              (temperature_en) ? 2 : 0, // maxLength
                              GattCharacteristic::BLE_GATT_CHAR_PROPERTIES_READ),
        humidityCharacteristic(GattCharacteristic::UUID_HUMIDITY_CHAR,
                              (uint8_t *) &humidity,
                              (humidity_en) ? 2 : 0, // minLength
                              (humidity_en) ? 2 : 0, // maxLength
                              GattCharacteristic::BLE_GATT_CHAR_PROPERTIES_READ),
        pressureCharacteristic(GattCharacteristic::UUID_PRESSURE_CHAR,
                              (uint8_t *) &pressure,
                              (pressure_en) ? 4 : 0, // minLength
                              (pressure_en) ? 4 : 0, // maxLength
                              GattCharacteristic::BLE_GATT_CHAR_PROPERTIES_READ)
        {
        static bool serviceAdded = false; /* We should only ever need to add the information service once. */
        if (serviceAdded) {
            return;
        }

        GattCharacteristic *charTable[] = { &humidityCharacteristic,
                                            &pressureCharacteristic,
                                            &temperatureCharacteristic };

        GattService environmentalService(GattService::UUID_ENVIRONMENTAL_SERVICE, charTable,
                                                     sizeof(charTable) / sizeof(GattCharacteristic *));
        
        ble.gattServer().addService(environmentalService);
        serviceAdded = true;
    }

    /**
     * @brief   Update humidity characteristic.
     * @param   newHumidityVal New humidity measurement.
     */
    void updateHumidity(uint16_t newHumidityVal)
    {
        humidity = (uint32_t) (newHumidityVal*100);                
        ble.gattServer().write(humidityCharacteristic.getValueHandle(), (uint8_t *) &humidity, 2);
    }

    /**
     * @brief   Update pressure characteristic.
     * @param   newPressureVal New pressure measurement.
     */
    void updatePressure(uint32_t newPressureVal)
    {
        pressure = (uint32_t) (newPressureVal*10);        
        ble.gattServer().write(pressureCharacteristic.getValueHandle(), (uint8_t *) &pressure, 4);
    }

    /**
     * @brief   Update temperature characteristic.
     * @param   newTemperatureVal New temperature measurement.
     */
    void updateTemperature(float newTemperatureVal)
    {
        temperature = (int16_t) (newTemperatureVal*100);
        ble.gattServer().write(temperatureCharacteristic.getValueHandle(), (uint8_t *) &temperature, 2);
    }

private:
    BLE &ble;

    int16_t temperature;
    GattCharacteristic temperatureCharacteristic;
    uint16_t humidity;
    GattCharacteristic humidityCharacteristic;
    uint32_t pressure;
    GattCharacteristic pressureCharacteristic;
};

#endif /* #ifndef __BLE_ENVIRONMENTAL_SERVICE_H__*/