#ifndef MODEL_H
#define MODEL_H
#include <Arduino.h>
#include <vector>

enum AppState { Initializing, NextPageLoading, NextPageLoaded, NextPageErrorLoading, Sleeping, DeepSleeping };
enum AppPage { NoPage, VickyCommutePage, NorthSouthPage, EastWestPage, WeatherPage};

enum BusLocation { None, TooClose, FarAwayEnough };
enum RouteType { FrequentRoute, LocalRoute };
enum RouteGroupType { VickyCommute, NorthSouth, EastWest };

// Based on https://open-meteo.com/en/docs/ (scroll way down)
enum WeatherType { Clear, MainlyClear, PartlyCloudy, Overcast, Fog,
                   Drizzle, FreezingDrizzle,
                   SlightRain, ModerateRain, HeavyRain,
                   LightFreezingRain, HeavyFreezingRain,
                   LightSnow, ModerateSnow, HeavySnow, SnowGrains,
                   SlightRainShowers, ModerateRainShowers, ViolentRainShowers,
                   SlightSnowShowers, HeavySnowShowers,
                   Thunderstorm, Unknown };
inline String weatherTypeToString(WeatherType weatherType) {
    switch (weatherType) {
        case Clear: return "Clear";
        case MainlyClear: return "MainlyClear";
        case PartlyCloudy: return "PartlyCloudy";
        case Overcast: return "Overcast";
        case Fog: return "Fog";
        
        case Drizzle: return "Drizzle";
        case FreezingDrizzle: return "FreezingDrizzle";
        
        case SlightRain: return "SlightRain";
        case ModerateRain: return "ModerateRain";
        case HeavyRain: return "HeavyRain";

        case LightFreezingRain: return "LightFreezingRain";
        case HeavyFreezingRain: return "HeavyFreezingRain";

        case LightSnow: return "LightSnow";
        case ModerateSnow: return "ModerateSnow";
        case HeavySnow: return "HeavySnow";
        case SnowGrains: return "SnowGrains";

        case SlightRainShowers: return "SlightShowers";
        case ModerateRainShowers: return "ModerateShowers";
        case ViolentRainShowers: return "ViolentShowers";

        case SlightSnowShowers: return "SlightSnowShowers";
        case HeavySnowShowers: return "HeavySnowShowers";

        case Thunderstorm: return "Thunderstorm";
        
        case Unknown: 
        default:
            return "Unknown";
    }
}

enum UVIndex { Low, Moderate, High, VeryHigh, Extreme };
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
};

#endif