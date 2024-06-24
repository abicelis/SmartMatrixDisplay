#ifndef OpenMeteoAPI_h
#define OpenMeteoAPI_h
#include <HTTPClient.h>
#include <WiFiClientSecure.h>
#include <ArduinoJson.h>
#include <Model.h>
#include <WeatherData.h>
class OpenMeteoAPI {
    public:
        OpenMeteoAPI(WiFiClientSecure* wifiClient, HTTPClient* httpClient);
        void fetchWeatherData(WeatherData& weatherData, const uint8_t clockHour, bool commuteMode);
    private:
        WiFiClientSecure* _wifiClient;
        HTTPClient* _httpClient;
        void insertWeatherData(JsonDocument doc, WeatherData& result, uint8_t hour, boolean jsonIsCurrentAPI);
};
#endif