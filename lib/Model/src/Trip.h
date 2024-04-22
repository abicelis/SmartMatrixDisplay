#ifndef TRIP_H
#define TRIP_H
#include "Model.h"
class Trip {
public:
    int arrivalTime;            // Estimated or scheduled arrival time of bus.
    bool arrivalIsEstimated;    // True  -> arrivalTime is estimated (Bus tracked via GPS).
                                // False -> arrivalTime is scheduled.
    BusLocation busLocation;

    Trip(int arrivalTime, bool arrivalIsEstimated, BusLocation busLocation) {
        this->arrivalTime = arrivalTime;
        this->arrivalIsEstimated = arrivalIsEstimated;
        this->busLocation = busLocation;
    }
};
#endif // TRIP_H