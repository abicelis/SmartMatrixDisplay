#ifndef TRIP_H
#define TRIP_H
#include <Arduino.h>
#include <vector>
#include "octranspo/TripArrival.h"
class Trip {
public:
    String actualDestination;           // e.g. "Bells Corners" instead of "Barrhaven Centre" or could actually be "Barrhaven Centre"
    std::vector<TripArrival> arrivals;  // e.g. [12m estimated, 34m scheduled]

    Trip(String actualDestination) {
        this->actualDestination = actualDestination;
    }
};
#endif // TRIP_H