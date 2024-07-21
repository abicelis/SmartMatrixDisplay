#ifndef OpenMeteoAPI_h
#define OpenMeteoAPI_h
#include <HTTPClient.h>
#include <WiFiClientSecure.h>
#include <ArduinoJson.h>
#include "forecast/Forecast.h"

class OpenMeteoAPI {
    public:
        OpenMeteoAPI(WiFiClientSecure* wifiClient, HTTPClient* httpClient, Forecast* forecast);
        void fetchAQI();
    private:
        WiFiClientSecure* _wifiClient;
        HTTPClient* _httpClient;
        Forecast* _forecast;
};
#endif