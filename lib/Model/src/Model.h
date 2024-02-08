#ifndef MODEL_H
#define MODEL_H
#include <Arduino.h>
#include <vector>

enum AppState { RoutesVickyCommute, RoutesNorthSouth, RoutesEastWest, Weather, Sleeping };

enum BusType { FrequentBus, LocalBus };
enum TripsType { VickyCommute, NorthSouth, EastWest };

struct TripsData {
    std::vector<int> routeNumbers;
    std::vector<String> routeLabels;
    std::vector<int> busArrivalTimes;        // Estimated or scheduled bus time of arrival.
    std::vector<bool> arrivalIsEstimated;    // True  -> busArrivalTime is estimated (Bus tracked via GPS).
                                             // False -> busArrivalTime is scheduled.
    std::vector<bool> routeIsFrequent;       // True -> Route is Frequent. False -> Route is Local
                                             // https://www.octranspo.com/en/our-services/bus-o-train-network/service-types/
};

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
    WeatherType currentWeatherType;
    String currentTemperatureCelcius;
    String currentApparentTemperatureCelcius;                    // "Feels like" temp.
    String dailyTemperatureMinCelcius;
    String dailyTemperatureMaxCelcius;
    bool isDaytime;
    std::vector<std::pair<ExtraWeatherDataType, String>> extraWeatherData;
};

#endif