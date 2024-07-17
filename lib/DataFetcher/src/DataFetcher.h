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
#include "Util.h"
#include <Arduino.h>

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
                if(inAContentPageOrWakingUp(*_this->_appState)) {
                    // Serial.println("------- HEAP MEM fetchRoutes.   HEAP= " + String(xPortGetFreeHeapSize()) + " HWM=" + String(uxTaskGetStackHighWaterMark(_this->taskHandle)));
                    _this->_ocTranspoAPI->fetchRoutes(false);

                    // Serial.println("------- HEAP MEM fetchAQI.   HEAP= " + String(xPortGetFreeHeapSize()) + " HWM=" + String(uxTaskGetStackHighWaterMark(_this->taskHandle)));
                    _this->_openMeteoAPI->fetchAQI();

                    // Serial.println("------- HEAP MEM fetchForecast.   HEAP= " + String(xPortGetFreeHeapSize()) + " HWM=" + String(uxTaskGetStackHighWaterMark(_this->taskHandle)));
                    _this->_accuweatherAPI->fetchForecast();

                    if(_this->_routes->routesAreStale()) {
                        Serial.println(" DATAFTCHR: ERROR routes are stale! Restarting ESP");
                        ESP.restart();
                    }
                } else {
                    // Sleep for a minute
                    Serial.println(" DATAFTCHR: SLEEP");
                    vTaskDelay(pdMS_TO_TICKS(60000));
                }
            }
        }
};
#endif //DataFetcher_h