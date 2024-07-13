#ifndef TRIPARRIVAL_H
#define TRIPARRIVAL_H
enum TripArrivalLocation { None, TooClose, FarAwayEnough };
class TripArrival {
public:
    int         time;           // Estimated or scheduled arrival time.
    bool        IsEstimated;    // True  -> arrivalTime is estimated (Tracked via GPS).
                                // False -> arrivalTime is scheduled.
    TripArrivalLocation location;

    TripArrival(int arrivalTime, bool arrivalIsEstimated, TripArrivalLocation location) {
        this->time = arrivalTime;
        this->IsEstimated = arrivalIsEstimated;
        this->location = location;
    }
};
#endif // TRIPARRIVAL_H