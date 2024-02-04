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
enum UVIndex { Low, Moderate, High, VeryHigh, Extreme };
struct WeatherData {
    WeatherType currentWeatherType;
    const char* currentTemperatureCelcius;
    const char* currentApparentTemperatureCelcius;                    // "Feels like" temp.
    const char* currentRelativeHumidityPercent;
    const char* currenWindSpeedKmh;
    const char* currenWindGustsKmh;
    const char* dailyTemperatureMinCelcius;
    const char* dailyTemperatureMaxCelcius;
    const char* dailyPrecipitationSumMillimeter;
    UVIndex dailyMaxUVIndex;
};

#endif