#ifndef ROUTES_H
#define ROUTES_H
#include <Arduino.h>
#include <vector>
#include "octranspo/Route.h"
#include "octranspo/Trip.h"
#include "octranspo/UITrip.h"

class Routes {
public:
    Routes() {
        routes.push_back(Route::route_88_Hurdman());
        routes.push_back(Route::route_88_TerryFox());
        routes.push_back(Route::route_80_TunneysPasture());
        routes.push_back(Route::route_80_BarrhavenCentre());
        routes.push_back(Route::route_81_TunneysPasture());
        routes.push_back(Route::route_81_Clyde());
    }
    void replaceTripsForRoute(const String& routeNumber, const String& routeDestination, 
        std::vector<std::tuple<String, int, bool, TripArrivalLocation>> flatTrips) {
        xSemaphoreTake(mutex, portMAX_DELAY);
        Route* foundRoute = findRoute(routeNumber, routeDestination);
        if(foundRoute != NULL)
            foundRoute->replaceTrips(flatTrips);
        xSemaphoreGive(mutex);
    }

    std::vector<UITrip> getSortedUITripsByDirection(RouteDirection direction) {
        xSemaphoreTake(mutex, portMAX_DELAY);
        std::vector<UITrip> uiTrips;
        for (const auto& route: routes)
            if(route.direction == direction)
                for (const auto& trip: route.trips)
                    uiTrips.push_back(UITrip(route.number, route.type, trip.actualDestination, trip.arrivals));
        sortTrips(uiTrips);
        xSemaphoreGive(mutex);
        
        return uiTrips;
    }

    std::vector<UITrip> getSortedUITripsForCommute() {
        xSemaphoreTake(mutex, portMAX_DELAY);
        std::vector<UITrip> uiTrips;
        for (const auto& route: routes) {
            if(route.number == "88" && route.destination == "Hurdman") {
                for (const auto& trip: route.trips)
                    uiTrips.push_back(UITrip(route.number, route.type, trip.actualDestination, trip.arrivals));
                break;
            }
        }
        sortTrips(uiTrips);
        xSemaphoreGive(mutex);

        return uiTrips;
    }
    
    void printRoutes() {
        xSemaphoreTake(mutex, portMAX_DELAY);
        Serial.println(" ROUTES: Printing Routes:");
        for(const auto &route: routes) {
            String frequent = route.type == FrequentRoute ? "Yes" : "No";

            for(const auto &trip: route.trips) {
            Serial.print(" ROUTES:   > " + route.number 
                + " " + trip.actualDestination
                    + " (Frequent=" + frequent + ") in: ");

                for(const auto &tripArrival: trip.arrivals) {
                    String estimated = tripArrival.IsEstimated ? "*" : "";
                    String farAwayEnough = tripArrival.location == FarAwayEnough ? "!" : "";
                    Serial.print(farAwayEnough + estimated + String(tripArrival.time) + "min, ");
                }
                Serial.println("");
            }
        }
        xSemaphoreGive(mutex);
    }

    friend class OCTranspoAPI;

protected:
    SemaphoreHandle_t mutex = xSemaphoreCreateMutex();
    std::vector<Route> routes;

private:
    Route* findRoute(const String& number, const String& destination) {
        Route* foundRoute = NULL;
        for(auto& route: routes)
            if(route.number == number && route.destination == destination)
                foundRoute = &route;
        return foundRoute;
    }

    void sortTrips(std::vector<UITrip>& trips) {
        uint8_t minArrival;
        uint8_t right;
        uint8_t size = trips.size();
        for(uint8_t left = 0; left < size-1; left++) {
            minArrival = 255;
            uint8_t right;
            for(uint8_t i = left; i < size; i++) {
                uint8_t currentArrival = trips[i].arrivals[0].time;
                if(currentArrival < minArrival) {
                    minArrival = currentArrival;
                    right = i;
                }
            }

            if(right != left) {
                std::swap(trips[left], trips[right]);
            }
        }
    }

};
#endif // ROUTES_H