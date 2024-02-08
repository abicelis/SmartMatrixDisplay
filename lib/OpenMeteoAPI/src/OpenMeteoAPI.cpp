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

    long sunriseTimestamp = doc["daily"]["sunrise"][0].as<long>();
    long sunsetTimestamp = doc["daily"]["sunset"][0].as<long>();
    time_t nowTimestamp;
    time(&nowTimestamp);

    result.currentWeatherType = WMOCodeToWeatherType(doc["current"]["weather_code"].as<int>());
    result.currentTemperatureCelcius = String(doc["current"]["temperature_2m"].as<int>());
    result.currentApparentTemperatureCelcius = String(doc["current"]["apparent_temperature"].as<int>());
    result.dailyTemperatureMinCelcius = String(doc["daily"]["temperature_2m_min"][0].as<int>());
    result.dailyTemperatureMaxCelcius = String(doc["daily"]["temperature_2m_max"][0].as<int>());
    result.isDaytime = (nowTimestamp >= sunriseTimestamp && nowTimestamp <= sunsetTimestamp);
    
    result.extraWeatherData.push_back(std::make_pair(CurrentRelativeHumidity, String(doc["current"]["relative_humidity_2m"].as<int>()) + " %"));
    result.extraWeatherData.push_back(std::make_pair(CurrentWindSpeed, String(doc["current"]["wind_speed_10m"].as<int>()) + " Kmh"));
    result.extraWeatherData.push_back(std::make_pair(DailyPrecipitation, String(doc["daily"]["precipitation_sum"][0].as<int>()) + " mm"));

    result.extraWeatherData.push_back(std::make_pair(Sunrise, TimeStringFromUnixTimestamp(sunriseTimestamp)));
    result.extraWeatherData.push_back(std::make_pair(Sunset, TimeStringFromUnixTimestamp(sunsetTimestamp)));
    result.extraWeatherData.push_back(std::make_pair(MaxUVIndex, UVIndexCodeToString(doc["daily"]["uv_index_max"][0].as<int>())));
    
    Serial.println("PRINTING VALUES NOW");
    for(const auto &value: result.extraWeatherData) {
        Serial.println(value.second);
    }

    _httpClient->end();
    return result;
}

// Mappings: https://www.nodc.noaa.gov/archive/arc0021/0002199/1.1/data/0-data/HTML/WMO-CODE/WMO4677.HTM
WeatherType OpenMeteoAPI::WMOCodeToWeatherType(uint8_t c) {
    Serial.print("WMOCodeToWeatherType called with code");
    Serial.println(c);
    if(c == 0 || c == 1) { 
        return Clear;
    } else if (c == 2 || c == 3 || c == 4) {  // 2 means actually "unchanged"... But we'll assume Cloudy.
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

String OpenMeteoAPI::UVIndexCodeToString(uint8_t index) {
    if(index < 3)
        return "Low";
    else if(index < 6)
        return "Mid";
    else if(index < 8)
        return "High";
    else if(index < 11)
        return "High+";
    else
        return "High++";
}

String OpenMeteoAPI::TimeStringFromUnixTimestamp(time_t timestamp) {
    char timeStr[6]; // to hold the time string

    struct tm *timeinfo;
    timeinfo = localtime(&timestamp);

  // Format the time into a HH:mm string
  strftime(timeStr, sizeof(timeStr), "%H:%M", timeinfo);

  return String(timeStr);
}

