#ifndef OCTranspoAPI_h
#define OCTranspoAPI_h
#include <HTTPClient.h>
#include <WiFiClientSecure.h>
#include "octranspo/Routes.h"

class OCTranspoAPI {
    public:
        OCTranspoAPI(WiFiClientSecure* wifiClient, HTTPClient* httpClient, Routes* routes);
        void startFetchRoutesTask();
        void fetchRoutes();

    private:
        WiFiClientSecure* _wifiClient;
        HTTPClient* _httpClient;
        Routes* _routes;
        TaskHandle_t fetchRoutesTaskHandle = NULL;
        void fetchTripsFor(const String& stopNumber, const String& routeNumber, const String& routeDestination);

        static void fetchRoutesTask(void* pvParameters) {
            OCTranspoAPI *_this = (OCTranspoAPI *) pvParameters;   
            while(true) {
                _this->fetchRoutes();
            }
        }
};
#endif