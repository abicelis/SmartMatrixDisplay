#ifndef MODEL_H
#define MODEL_H
#include <Arduino.h>
#include <vector>

enum AppState { Initializing, NextPageLoading, NextPageLoaded, NextPageErrorLoading, Sleeping };
enum AppPage { NoPage, VickyCommutePage, NorthSouthPage, EastWestPage, WeatherPage};

enum RouteType { FrequentRoute, LocalRoute };
enum RouteGroupType { VickyCommute, NorthSouth, EastWest };

enum WeatherType { Clear, Cloudy, Drizzle, Showers, Rain, FreezingRain, Snow, Thunderstorm, Fog, Hail, Mist, SmokeOrHaze, Invalid };
inline String weatherTypeToString(WeatherType weatherType) {
    switch (weatherType) {
        case Clear: return "Clear";
        case Cloudy: return "Cloudy";
        case Drizzle: return "Drizzle";
        case Showers: return "Showers";
        case Rain: return "Rain";
        case FreezingRain: return "FrzRain";
        case Snow: return "Snow";
        case Thunderstorm: return "Thunder";
        case Fog: return "Fog";
        case Hail: return "Hail";
        case Mist: return "Mist";
        case SmokeOrHaze: return "Smoke";
        case Invalid: return "Invalid";
        default: return "";
    }
}

enum UVIndex { Low, Moderate, High, VeryHigh, Extreme };
enum ExtraWeatherDataType { CurrentRelativeHumidity, CurrentWindSpeed, DailyPrecipitation, Sunrise, Sunset, MaxUVIndex };
struct WeatherData {
    bool setCorrectly;
    std::vector<String> times;
    std::vector<bool> isDaytime;
    std::vector<WeatherType> weatherType;
    std::vector<String> temperatureCelcius;
    std::vector<String> apparentTemperatureCelcius;                    // "Feels like" temp.
    std::vector<String> relativeHumidity;
    std::vector<String> precipitationProbability;
    std::vector<String> precipitation;
    

    // std::vector<std::pair<ExtraWeatherDataType, String>> extraWeatherData;
};

#endif