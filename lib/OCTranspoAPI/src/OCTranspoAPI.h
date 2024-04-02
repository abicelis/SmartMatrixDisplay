#ifndef OCTranspoAPI_h
#define OCTranspoAPI_h
#include <HTTPClient.h>
#include <WiFiClientSecure.h>
#include <RouteGroupData.h>

class OCTranspoAPI {
    public:
        OCTranspoAPI(WiFiClientSecure* wifiClient, HTTPClient* httpClient);
        RouteGroupData fetchTrips(RouteGroupType type);
    private:
        WiFiClientSecure* _wifiClient;
        HTTPClient* _httpClient;
        void fetchTripsFor(RouteGroupData& data, const String& stopNo, const String& routeNo);
        void addTrip(RouteGroupData& data, String& routeNumber, String& routeDestination, uint8_t arrivalTime, bool arrivalIsEstimated);
        void sortTrips(RouteGroupData& data);
        RouteType getRouteType(String& routeNumber);
};
#endif