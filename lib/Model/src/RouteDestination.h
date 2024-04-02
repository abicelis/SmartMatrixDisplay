#ifndef ROUTEDESTINATION_H
#define ROUTEDESTINATION_H
#include <Arduino.h>
#include <vector>
#include "Trip.h"
#include "Model.h"

class RouteDestination {
public:
    String              routeNumber;
    String              routeDestination;
    RouteType           routeType;                        // e.g.: FrequentRoute, LocalRoute
    std::vector<Trip>   trips;

    RouteDestination(const String& routeNumber, const String& routeDestination, RouteType routeType)
        : routeNumber(routeNumber), routeDestination(routeDestination), routeType(routeType) {}
};
#endif // ROUTEDESTINATION_H