#ifndef OpenMeteoAPI_h
#define OpenMeteoAPI_h
#include <HTTPClient.h>
#include <WiFiClientSecure.h>
#include <ArduinoJson.h>
#include <Model.h>
class OpenMeteoAPI {
    public:
        OpenMeteoAPI(WiFiClientSecure* wifiClient, HTTPClient* httpClient);
        WeatherData fetchCurrentWeather(const uint8_t clockHour, bool commuteMode);
    private:
        WiFiClientSecure* _wifiClient;
        HTTPClient* _httpClient;
        void insertWeatherData(uint8_t currentHour, JsonDocument doc, WeatherData& result);
        WeatherType WMOCodeToWeatherType(uint8_t wmoCode);
        String UVIndexCodeToString(uint8_t index);
        time_t iso8601DateStringToUnixTimestamp(const char* dateTime);
        
};
#endif