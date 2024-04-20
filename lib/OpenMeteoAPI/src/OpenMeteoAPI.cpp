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

        result.setCorrectly = true;
        for(uint8_t currentHour = clockHour; currentHour <= clockHour+(OPEN_METEO_API_HOUR_STEP*3); currentHour += OPEN_METEO_API_HOUR_STEP) {
            String timeStr = (currentHour > 12 ? String(currentHour - 12) + String(" pm") : String(currentHour) + String(" am")); 
            result.times.push_back(timeStr);

            time_t currentTimestamp = iso8601DateStringToUnixTimestamp(doc["hourly"]["time"][currentHour]);
            result.isDaytime.push_back(currentTimestamp >= sunriseTimestamp && currentTimestamp <= sunsetTimestamp);

            WeatherType type = WMOCodeToWeatherType(doc["hourly"]["weather_code"][currentHour].as<int>());
            if(type == Unknown) {
                Serial.print("Warning: WMOCodeToWeatherType returned 'Unknown for code");
                Serial.println(doc["hourly"]["weather_code"][currentHour].as<int>());
            }
            result.weatherType.push_back(type);
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


// Mappings based on https://open-meteo.com/en/docs/ (scroll way down)
// And https://www.nodc.noaa.gov/archive/arc0021/0002199/1.1/data/0-data/HTML/WMO-CODE/WMO4677.HTM
WeatherType OpenMeteoAPI::WMOCodeToWeatherType(uint8_t c) {
    // Serial.print("WMOCodeToWeatherType called with code ");
    // Serial.println(c);
    if(c == 0) { 
        return Clear;
    } else if (c == 1) {
        return MainlyClear;
    } else if (c == 2) {
        return PartlyCloudy;
    } else if (c == 3 ) {
        return Overcast;
    } else if (c == 45 || c == 48) {
        return Fog;
    
    } else if (c == 51 || c == 53 || c == 55) {
        return Drizzle;
    } else if (c == 56 || c == 57) {
        return FreezingDrizzle;
    
    } else if (c == 61) {
        return SlightRain;
    } else if (c == 63) {
        return ModerateRain;
    } else if (c == 65) {
        return HeavyRain;

    } else if (c == 66) {
        return LightFreezingRain;
    } else if (c == 67) {
        return HeavyFreezingRain;

    } else if (c == 71) {
        return LightSnow;
    } else if (c == 73) {
        return ModerateSnow;
    } else if (c == 75) {
        return HeavySnow;
    } else if (c == 77) {
        return SnowGrains;

    } else if (c == 80) {
        return SlightRainShowers;
    } else if (c == 81) {
        return ModerateRainShowers;
    } else if (c == 82) {
        return ViolentRainShowers;
                   
    } else if (c == 85) {
        return SlightSnowShowers;
    } else if (c == 86) {
        return HeavySnowShowers;

    } else if (c == 96) {
        return Thunderstorm;
    } 
    return Unknown;
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

