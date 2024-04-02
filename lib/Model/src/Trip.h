#ifndef TRIP_H
#define TRIP_H
class Trip {
public:
    int arrivalTime;            // Estimated or scheduled arrival time of bus.
    bool arrivalIsEstimated;    // True  -> arrivalTime is estimated (Bus tracked via GPS).
                                // False -> arrivalTime is scheduled.

    Trip(int arrivalTime, bool arrivalIsEstimated) {
        this->arrivalTime = arrivalTime;
        this->arrivalIsEstimated = arrivalIsEstimated;
    }
};
#endif // TRIP_H