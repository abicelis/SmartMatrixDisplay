#ifndef OpenMeteoAPI_h
#define OpenMeteoAPI_h
#include <HTTPClient.h>
#include <WiFiClientSecure.h>
#include <Model.h>
class OpenMeteoAPI {
    public:
        OpenMeteoAPI(WiFiClientSecure* wifiClient, HTTPClient* httpClient);
        WeatherData fetchCurrentWeather();
    private:
        WiFiClientSecure* _wifiClient;
        HTTPClient* _httpClient;
        WeatherType wmoCodeToWeatherType(int& wmoCode);
};
#endif