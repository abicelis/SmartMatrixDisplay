#ifndef DataFetcher_h
#define DataFetcher_h
#include <HTTPClient.h>
#include <WiFiClientSecure.h>
#include <Preferences.h>
#include "octranspo/Routes.h"
#include "forecast/Forecast.h"
#include "OCTranspoAPI.h"
#include "OpenMeteoAPI.h"
#include "AccuweatherAPI.h"

class DataFetcher {
    public:
        DataFetcher(WiFiClientSecure* wifiClient, HTTPClient* httpClient, Preferences* preferences,
            Routes* routes, Forecast* forecast, AppState* appState);
        void fetchDataSynchronously();
        void startFetcherTask();
        
        ~DataFetcher();

    private:
        OCTranspoAPI* _ocTranspoAPI;
        OpenMeteoAPI* _openMeteoAPI;
        AccuweatherAPI* _accuweatherAPI;
        Routes* _routes;
        AppState* _appState;
        TaskHandle_t taskHandle = NULL;

        static void fetcherTask(void* pvParameters) {
            DataFetcher *_this = (DataFetcher *) pvParameters;   
            while(true) {

                if(*_this->_appState != Sleeping && *_this->_appState != DeepSleeping) {
                    _this->_ocTranspoAPI->fetchRoutes(false);
                    _this->_openMeteoAPI->fetchAQI();
                    _this->_accuweatherAPI->fetchForecast();

                    if(_this->_routes->routesAreStale()) {
                        Serial.println(" DATAFTCHR: ERROR routes are stale, investigate this! Restarting ESP.");
                        ESP.restart();
                    }
                } else {
                    Serial.println(" DATAFTCHR: SLEEPING 5 seconds");
                    vTaskDelay(pdMS_TO_TICKS(5000));
                }
            }
        }
};
#endif //DataFetcher_h