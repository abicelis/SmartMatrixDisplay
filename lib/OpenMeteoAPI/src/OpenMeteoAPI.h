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
        WeatherType WMOCodeToWeatherType(uint8_t wmoCode);
        String UVIndexCodeToString(uint8_t index);
        String TimeStringFromUnixTimestamp(time_t timestamp);
};
#endif