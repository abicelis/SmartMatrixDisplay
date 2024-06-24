#ifndef WEATHERDATA_H
#define WEATHERDATA_H
#include <Arduino.h>
#include <Preferences.h>
#include <vector>
#include "Model.h"

enum WeatherDataType { VickyCommuteForecast, ThreeHourForecast };
enum UVIndex { Low, Moderate, High, VeryHigh, Extreme };

class WeatherData {
public:
    WeatherDataType weatherDataType;
    uint8_t UVICurrent;
    uint16_t AQICurrent;
    std::vector<uint8_t> times;                     // Hours of day of the below vectors, ascending. e.g. [4, 5, 6]
    std::vector<uint8_t> weatherIcons;               // https://developer.accuweather.com/weather-icons
    std::vector<String> temperaturesCelcius;
    std::vector<String> apparentTemperaturesCelcius; // Realfeel
    std::vector<String> relativeHumidities;
    std::vector<String> windSpeeds;
    std::vector<String> precipitationProbabilities;
    std::vector<String> precipitationAmounts;
    
    void clear();
    bool isStaleOrInvalid(WeatherDataType requiredWeatherDataType);
    
    void tryLoadWeatherDataFromFlash(Preferences& preferences);
    void saveWeatherDataToFlash(Preferences& preferences);

    String weatherDataTypeToString(WeatherDataType weatherDataType);
    WeatherDataType stringToWeatherDataType(String weatherDataTypeStr);
};

#endif // WEATHERDATA_H

