#include <HTTPClient.h>
#include <WiFiClientSecure.h>
#include <Preferences.h>
#include <ArduinoJson.h>
#include <vector>
#include <Config.h>
#include "DataFetcher.h"
#include "OCTranspoAPI.h"
#include "OpenMeteoAPI.h"
#include "Util.h"

DataFetcher::DataFetcher(WiFiClientSecure* wifiClient, HTTPClient* httpClient, Preferences* preferences,
            Routes* routes, Forecast* forecast, AppState* appState) {

    _routes = routes;
    _appState = appState;
    _ocTranspoAPI = new OCTranspoAPI(wifiClient, httpClient, routes);
    _openMeteoAPI = new OpenMeteoAPI(wifiClient, httpClient, forecast);
    _accuweatherAPI = new AccuweatherAPI(wifiClient, httpClient, preferences, forecast);
}

DataFetcher::~DataFetcher() {
    delete _ocTranspoAPI;
    delete _openMeteoAPI;
    delete _accuweatherAPI;
}

void DataFetcher::fetchDataSynchronously() {
    _ocTranspoAPI->fetchRoutes(true);
    _openMeteoAPI->fetchAQI();
    _accuweatherAPI->fetchForecast();
}

void DataFetcher::startFetcherTask() {
    BaseType_t result = xTaskCreatePinnedToCore(fetcherTask, "FetcherTask", 
                STACK_DEPTH_DATA_FETCHER_TASK, this, TASK_PRIORITY_NETWORK, &taskHandle, CORE_ID_NETWORK);
    if(result == pdPASS) {
        Serial.println("  OCTR-API: FetcherTask launched");
    } else if(result == errCOULD_NOT_ALLOCATE_REQUIRED_MEMORY) {
        Serial.print("OCT-API: ERROR FetcherTask creation failed due to insufficient memory! HEAP MEM=");
        Serial.println(xPortGetFreeHeapSize());
        vTaskDelay(pdMS_TO_TICKS(5000));
        ESP.restart();
    } else {
        Serial.print("OCT-API: ERROR FetcherTask creation failed!!!");
        vTaskDelay(pdMS_TO_TICKS(5000));
        ESP.restart();
    }
}