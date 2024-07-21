#ifndef AccuweatherAPI_h
#define AccuweatherAPI_h
#include <HTTPClient.h>
#include <WiFiClientSecure.h>
#include <Preferences.h>
#include <ArduinoJson.h>
#include "forecast/Forecast.h"

class AccuweatherAPI {
    public:
        AccuweatherAPI(WiFiClientSecure* wifiClient, HTTPClient* httpClient, Preferences* preferences, Forecast* forecast);
        void fetchForecast();
    private:
        WiFiClientSecure* _wifiClient;
        HTTPClient* _httpClient;
        Preferences* _preferences;
        Forecast* _forecast;
        void extractHourlyConditionCurrentForecast(JsonDocument doc, std::vector<HourlyCondition> &newHourlyConditions);
        void extractHourlyConditionForHour(JsonDocument doc, std::vector<HourlyCondition> &newHourlyConditions, uint8_t hour);
};

#endif // AccuweatherAPI