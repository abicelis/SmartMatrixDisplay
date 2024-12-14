#ifndef OCTranspoAPI_h
#define OCTranspoAPI_h
#include <HTTPClient.h>
#include <WiFiClientSecure.h>
#include <ArduinoJson.h>
#include "octranspo/Routes.h"

class OCTranspoAPI {
    public:
        OCTranspoAPI(WiFiClientSecure* wifiClient, HTTPClient* httpClient, Routes* routes);
        void fetchRoutes(bool doItQuickly);
    private:
        WiFiClientSecure* _wifiClient;
        HTTPClient* _httpClient;
        Routes* _routes;
        void fetchTripsFor(const String& stopNumber, const String& routeNumber, const String& routeDestination);
        void processTripsFor(const JsonArray tripsJson, const String& routeNumber, const String& routeDestination);
};
#endif