#ifndef ROUTE_H
#define ROUTE_H
#include <Arduino.h>
#include <vector>
#include "Model.h"
#include "octranspo/Trip.h"


enum RouteDirection { NorthSouth, EastWest };

class Route {
public:
    String              number;                      // e.g.: 80
    String              destination;                 // e.g.: Barhaven Centre
    String              stopNumber;                  // The Physical Bus Stop #
    RouteType           type;                        // e.g.: FrequentRoute, LocalRoute
    RouteDirection      direction;                   // e.g.: North, East
    unsigned long       lastUpdateMillis;
    std::vector<Trip>   trips;                       //  "Bells Corners" -> [12min , 35min]
    
    Route(const String& number, const String& destination, const String& stopNumber, RouteType type, RouteDirection direction) {
        this->number = number;
        this->destination = destination;
        this->stopNumber = stopNumber;
        this->type = type;
        this->direction = direction;
        this->lastUpdateMillis = millis();   // We cheat a little
    }

    static Route route_88_Hurdman() { return Route("88", "Hurdman", "4483", FrequentRoute, EastWest); }
    static Route route_88_TerryFox() { return Route("88", "Terry Fox", "4491", FrequentRoute, EastWest); }
    
    static Route route_80_TunneysPasture() { return Route("80", "Tunney's Pasture", "1267", FrequentRoute, NorthSouth); }
    static Route route_80_BarrhavenCentre() { return Route("80", "Barrhaven Centre", "2491", FrequentRoute, NorthSouth); }
    
    static Route route_81_TunneysPasture() { return Route("81", "Tunney's Pasture", "1251", LocalRoute, NorthSouth); }
    static Route route_81_Clyde() { return Route("81", "Clyde", "2491", LocalRoute, NorthSouth); }

    friend class Routes;

protected:
    void replaceTrips(std::vector<std::tuple<String, int, bool, TripArrivalLocation>> flatTrips) {
        trips.clear();
        lastUpdateMillis = millis();

        for (const auto &flatTrip : flatTrips) {
            TripArrival tripArrival = TripArrival(std::get<1>(flatTrip), std::get<2>(flatTrip), std::get<3>(flatTrip));

            bool inserted = false;
            for (auto &trip : trips) {
                if(trip.actualDestination == std::get<0>(flatTrip)) {
                    inserted = true;
                    trip.arrivals.push_back(tripArrival);
                }
            }
            if(inserted == false) {
                Trip newTrip = Trip(std::get<0>(flatTrip));
                newTrip.arrivals.push_back(tripArrival);
                trips.push_back(newTrip);
            }
        }
    }
};
#endif // ROUTE_H