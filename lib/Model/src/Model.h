#ifndef MODEL_H
#define MODEL_H
#include <Arduino.h>
#include <vector>

enum AppState { RoutesVickyCommute, RoutesNorthSouth, RoutesEastWest, Weather, Sleeping };
// bool appStateIsRoutesType(AppState appState) {
//     return appState == RoutesVickyCommute || appState == RoutesNorthSouth || appState == RoutesEastWest;
// }

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

struct WeatherData {
    // std::vector<int> routeNumbers;
    // std::vector<std::string> routeLabels;
    // std::vector<int> scheduleArrivalTimes;   // Estimated or scheduled bus time of arrival.
    // std::vector<bool> scheduleIsEstimated;  // True  -> arrivalTime is estimated (Bus tracked via GPS).
                                            // False -> arrivalTime is scheduled.
};
enum WeatherType { Clear, Cloudy, Drizzle, Showers, Rain, FreezingRain, Snow, Thunderstorm, Fog, Hail, Mist, SmokeOrHaze, Invalid };


#endif