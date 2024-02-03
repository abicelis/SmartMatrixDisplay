#ifndef OCTranspoAPI_h
#define OCTranspoAPI_h
#include <HTTPClient.h>
#include <WiFiClientSecure.h>
#include <Model.h>
class OCTranspoAPI {
    public:
        OCTranspoAPI(WiFiClientSecure* wifiClient, HTTPClient* httpClient);
        TripsData fetchNextTripsFor(const String& stopNo, const String& routeNo);
        TripsData fetchNextTrips(TripsType tripsType);
    private:
        WiFiClientSecure* _wifiClient;
        HTTPClient* _httpClient;
        void mergeAndSortTrips(TripsData& result, TripsData& A, TripsData& B);
        void appendTrip(TripsData& appendee, TripsData& appended, const int& pos);
        boolean routeIsFrequent(int routeNumber);
};
#endif