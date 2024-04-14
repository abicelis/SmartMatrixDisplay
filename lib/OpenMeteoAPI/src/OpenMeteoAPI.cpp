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

WeatherData OpenMeteoAPI::fetchCurrentWeather(const uint8_t clockHour) {
    WeatherData result;
    _httpClient->begin(*_wifiClient, OPEN_METEO_API_ENDPOINT);
    
    int tries = OPEN_METEO_API_ENDPOINT_TRIES;
    int httpCode = -1;
    while (httpCode != 200 && tries > 0) {
        httpCode = _httpClient->GET();
        tries--;
    }

    if(httpCode == 200) {
        JsonDocument doc;
        deserializeJson(doc, _httpClient->getStream());

        // String out = "";
        // serializeJsonPretty(doc, out);
        // Serial.println(out);

        time_t sunriseTimestamp = iso8601DateStringToUnixTimestamp(doc["daily"]["sunrise"][0]);
        time_t sunsetTimestamp = iso8601DateStringToUnixTimestamp(doc["daily"]["sunset"][0]);


        Serial.print("SUNRISE IS ");
        Serial.println(sunriseTimestamp);

        Serial.print("SUNSET IS ");
        Serial.println(sunsetTimestamp);
        

        result.setCorrectly = true;
        for(uint8_t currentHour = clockHour; currentHour <= clockHour+(OPEN_METEO_API_HOUR_STEP*3); currentHour += OPEN_METEO_API_HOUR_STEP) {
            Serial.print("Current hour ");
            Serial.println(currentHour);

            String timeStr = (currentHour > 12 ? String(currentHour - 12) + String(" pm") : String(currentHour) + String(" am")); 
            result.times.push_back(timeStr);

            time_t currentTimestamp = iso8601DateStringToUnixTimestamp(doc["hourly"]["time"][currentHour]);
            result.isDaytime.push_back(currentTimestamp >= sunriseTimestamp && currentTimestamp <= sunsetTimestamp);

            Serial.print("CURRENT TIMESTAMP IS ");
            Serial.println(currentTimestamp);
            Serial.print("RESULT IS ");
            Serial.println(currentTimestamp >= sunriseTimestamp && currentTimestamp <= sunsetTimestamp);
        

            result.weatherType.push_back(WMOCodeToWeatherType(doc["hourly"]["weather_code"][currentHour].as<int>()));
            result.temperatureCelcius.push_back(String(doc["hourly"]["temperature_2m"][currentHour].as<int>()) + String("°"));
            result.apparentTemperatureCelcius.push_back(String(doc["hourly"]["apparent_temperature"][currentHour].as<int>()) + String("°"));
            result.relativeHumidity.push_back(String(doc["hourly"]["relative_humidity_2m"][currentHour].as<int>()) + String("%"));
            result.precipitationProbability.push_back(String(doc["hourly"]["precipitation_probability"][currentHour].as<int>()) + String("%"));
            result.precipitation.push_back(String(doc["hourly"]["precipitation"][currentHour].as<float>(), 1) + String("mm"));
        }
    }
    _httpClient->end();
    return result;
}


// Mappings: https://www.nodc.noaa.gov/archive/arc0021/0002199/1.1/data/0-data/HTML/WMO-CODE/WMO4677.HTM
WeatherType OpenMeteoAPI::WMOCodeToWeatherType(uint8_t c) {
    Serial.print("WMOCodeToWeatherType called with code ");
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

time_t OpenMeteoAPI::iso8601DateStringToUnixTimestamp(const char* dateTime) {
    struct tm tm{};
    strptime(dateTime, "%Y-%m-%dT%H:%M", &tm);
    time_t t = mktime(&tm);
    return t;
}

