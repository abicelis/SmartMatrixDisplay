// #include <Arduino.h>
#include <HTTPClient.h>
#include <WiFiClientSecure.h>
#include <ArduinoJson.h>
#include <Model.h>
#include <Config.h>
#include "OpenMeteoAPI.h"

OpenMeteoAPI::OpenMeteoAPI(WiFiClientSecure* wifiClient, HTTPClient* httpClient)  {
    _wifiClient = wifiClient;
    _httpClient = httpClient;
}

WeatherData OpenMeteoAPI::fetchCurrentWeather() {
    _httpClient->begin(*_wifiClient, OPEN_METEO_API_ENDPOINT);
    _httpClient->GET();

    JsonDocument doc;
    deserializeJson(doc, _httpClient->getStream());

    WeatherData result;

    String out = "";
    serializeJsonPretty(doc, out);
    Serial.println(out);

    result.currentWeatherType = WMOCodeToWeatherType(doc["current"]["weather_code"].as<uint8_t>());
    result.currentTemperatureCelcius = (doc["current"]["temperature_2m"].as<String>() + "°").c_str();
    result.currentApparentTemperatureCelcius = (doc["current"]["apparent_temperature"].as<String>() + "°").c_str();
    result.currentRelativeHumidityPercent = (doc["current"]["relative_humidity_2m"].as<String>() + "%").c_str();
    result.currenWindSpeedKmh = doc["current"]["wind_speed_10m"];
    result.currenWindGustsKmh = doc["current"]["wind_gusts_10m"];
    result.dailyTemperatureMinCelcius = doc["daily"]["temperature_2m_min"][0];
    result.dailyTemperatureMaxCelcius = doc["daily"]["temperature_2m_max"][0];
    result.dailyPrecipitationSumMillimeter = doc["daily"]["precipitation_sum"][0];
    result.dailyMaxUVIndex = UVIndexCodeToUVIndex(doc["daily"]["uv_index_max"][0].as<uint8_t>());
    
    _httpClient->end();
    return result;
}

// Mappings: https://www.nodc.noaa.gov/archive/arc0021/0002199/1.1/data/0-data/HTML/WMO-CODE/WMO4677.HTM
WeatherType OpenMeteoAPI::WMOCodeToWeatherType(uint8_t c) {
    if(c == 0 || c == 1) {
        return Clear;
    } else if (c == 3 || c == 4) {
        return Cloudy;
    } else if (c == 20 || (c >= 50 && c <= 55)) {
        return Drizzle;
    } else if (c == 25 || c == 80 || c == 81 || c == 82) {
        return Showers;
    } else if (c == 21 || (c >= 60 && c <= 65)) {
        return Rain;
    } else if (c == 24 || c == 66 || c == 67) {
        return FreezingRain;
    } else if (c == 22 || c == 23 || (c >= 70 && c <= 75)) {
        return Snow;
    } else if (c >= 95 && c <= 99) {
        return Thunderstorm;
    } else if (c >= 40 && c <= 49) {
        return Fog;
    } else if (c == 24 || (c >= 87 && c <= 90)) {
        return Hail;
    } else if (c == 10) {
        return Mist;
    } else if (c == 4 || c == 5) {
        return SmokeOrHaze;
    }
    return Invalid;
}

UVIndex OpenMeteoAPI::UVIndexCodeToUVIndex(uint8_t index) {
    if(index < 3)
        return Low;
    else if(index < 6)
        return Moderate;
    else if(index < 8)
        return High;
    else if(index < 11)
        return VeryHigh;
    else
        return Extreme;
}