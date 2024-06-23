#ifndef MODEL_H
#define MODEL_H
#include <Arduino.h>
#include <vector>

enum AppState { Initializing, NextPageLoading, NextPageLoaded, NextPageErrorLoading, Sleeping, DeepSleeping };
enum AppPage { NoPage, VickyCommutePage, NorthSouthPage, EastWestPage, WeatherPage};

enum BusLocation { None, TooClose, FarAwayEnough };
enum RouteType { FrequentRoute, LocalRoute };
enum RouteGroupType { VickyCommute, NorthSouth, EastWest };

#endif