#ifndef OCTranspoAPI_h
#define OCTranspoAPI_h
#include <HTTPClient.h>
#include <WiFiClientSecure.h>
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
};
#endif