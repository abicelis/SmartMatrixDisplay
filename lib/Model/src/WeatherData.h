#ifndef WEATHERDATA_H
#define WEATHERDATA_H
#include <Arduino.h>
#include <Preferences.h>
#include <vector>
#include "Model.h"

enum WeatherDataType { VickyCommuteForecast, ThreeHourForecast };
enum UVIndex { Low, Moderate, High, VeryHigh, Extreme };
// Based on https://open-meteo.com/en/docs/ (scroll way down)
enum WeatherType { Clear, MainlyClear, PartlyCloudy, Overcast, Fog,
                   Drizzle, FreezingDrizzle,
                   SlightRain, ModerateRain, HeavyRain,
                   LightFreezingRain, HeavyFreezingRain,
                   LightSnow, ModerateSnow, HeavySnow, SnowGrains,
                   SlightRainShowers, ModerateRainShowers, ViolentRainShowers,
                   SlightSnowShowers, HeavySnowShowers,
                   Thunderstorm, Unknown };

class WeatherData {
public:
    WeatherDataType weatherDataType;
    uint8_t UVICurrent;
    uint16_t AQICurrent;
    std::vector<uint8_t> times;                          // hours of the below vectors, ascending. e.g. [4, 5, 6]
    std::vector<bool> isDaytime;
    std::vector<WeatherType> weatherType;
    std::vector<String> temperatureCelcius;
    std::vector<String> apparentTemperatureCelcius;     // "Feels like" temp.
    std::vector<String> relativeHumidity;
    std::vector<String> windSpeed;
    std::vector<String> precipitationProbability;
    std::vector<String> precipitation;
    
    void clear();
    bool isStaleOrInvalid(WeatherDataType requiredWeatherDataType);
    
    void tryLoadWeatherDataFromFlash(Preferences& preferences);
    void saveWeatherDataToFlash(Preferences& preferences);

    String weatherDataTypeToString(WeatherDataType weatherDataType);
    WeatherDataType stringToWeatherDataType(String weatherDataTypeStr);

    String weatherTypeToString(WeatherType weatherType);
    WeatherType stringToWeatherType(String weatherTypeStr);
};

#endif // WEATHERDATA_H


