#ifndef UITRIP_H
#define UITRIP_H
#include <Arduino.h>
#include <vector>
#include "octranspo/Trip.h"
#include "Model.h"

class UITrip: public Trip {
public:
    String              routeNumber;                 // e.g.: 80
    RouteType           routeType;                   // e.g.: FrequentRoute, LocalRoute

    UITrip(String routeNumber, RouteType routeType, 
        String actualDestination, std::vector<TripArrival> inputArrivals) : Trip(actualDestination) {
        this->routeNumber = routeNumber;
        this->routeType = routeType;
        this->arrivals = inputArrivals;
    }
};
#endif // TRIP_H