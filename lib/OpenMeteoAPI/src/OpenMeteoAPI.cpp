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

void OpenMeteoAPI::fetchWeatherData(WeatherData& weatherData, const uint8_t clockHour, bool commuteMode) {
    weatherData.clear();
    if(commuteMode)
        weatherData.weatherDataType = VickyCommuteForecast;
    else
        weatherData.weatherDataType = ThreeHourForecast;

    _httpClient->begin(*_wifiClient, ACCUWEATHER_API_CURRENT);
    int tries = ACCUWEATHER_API_ENDPOINT_TRIES;
    int httpCode = -1;
    while (httpCode != 200 && tries > 0) {
        httpCode = _httpClient->GET();
        tries--;
    }
    if(httpCode == 200) {
        JsonDocument doc;
        deserializeJson(doc, _httpClient->getStream());
        // deserializeJson(doc, mockJsonAccuweatherCurrent);

        // String out = "";
        // serializeJsonPretty(doc, out);
        // Serial.println(out);

        JsonVariant currentConditions = doc[0];

        weatherData.UVICurrent = currentConditions["UVIndex"].as<uint8_t>();
        insertWeatherData(currentConditions, weatherData, clockHour, true);
        _httpClient->end();
    } else {
        _httpClient->end();
        weatherData.clear();
        return;
    }
    
    _httpClient->begin(*_wifiClient, ACCUWEATHER_API_12H_FORECAST);
    int tries2 = ACCUWEATHER_API_ENDPOINT_TRIES;
    int httpCode2 = -1;
    while (httpCode2 != 200 && tries2 > 0) {
        httpCode2 = _httpClient->GET();
        tries2--;
    }
    if(httpCode2 == 200) {
        JsonDocument doc;
        deserializeJson(doc, _httpClient->getStream());
        // deserializeJson(doc, mockJsonAccuweatherForecast12h);

        // String out = "";
        // serializeJsonPretty(doc, out);
        // Serial.println(out);

        if(commuteMode) {
            int fivePmHour = 17;
            insertWeatherData(doc[fivePmHour-clockHour].as<JsonVariant>(), weatherData, fivePmHour, false);
        } else {
            insertWeatherData(doc[0].as<JsonVariant>(), weatherData, clockHour+1, false);
            insertWeatherData(doc[1].as<JsonVariant>(), weatherData, clockHour+2, false);
        }
        _httpClient->end();
    } else {
        _httpClient->end();
        weatherData.clear();
        return;
    }


    if(!commuteMode) {
        _httpClient->begin(*_wifiClient, OPEN_METEO_API_AIR_QUALITY);
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
            weatherData.AQICurrent = doc["current"]["us_aqi"].as<uint16_t>();
        }
        _httpClient->end();
    }
}

void OpenMeteoAPI::insertWeatherData(JsonDocument doc, WeatherData& weatherData, uint8_t hour, boolean jsonIsCurrentAPI) {
    Serial.print("insertWeatherData() called for day: ");
    if(jsonIsCurrentAPI)
        Serial.println(doc["LocalObservationDateTime"].as<String>());
    else
        Serial.println(doc["DateTime"].as<String>());

    weatherData.times.push_back(hour);
    weatherData.weatherIcons.push_back(doc["WeatherIcon"].as<uint8_t>());
    
    if(jsonIsCurrentAPI) {
        weatherData.temperaturesCelcius.push_back(            String((int)round(doc["Temperature"]["Metric"]["Value"].as<float>())) + String("°"));
        weatherData.apparentTemperaturesCelcius.push_back(    String((int)round(doc["RealFeelTemperature"]["Metric"]["Value"].as<float>())) + String("°"));
        weatherData.relativeHumidities.push_back(             doc["RelativeHumidity"].as<String>() + String("%"));
        weatherData.windSpeeds.push_back(                     String((int)round(doc["Wind"]["Speed"]["Metric"]["Value"].as<float>())) + String("Kmh"));
        weatherData.precipitationProbabilities.push_back(     String((doc["HasPrecipitation"].as<boolean>()) ? "100%" : "0%" ));
        weatherData.precipitationAmounts.push_back            (String((int)round(doc["Precip1hr"]["Metric"]["Value"].as<float>())) + String("mm"));
    } else {
        weatherData.temperaturesCelcius.push_back(            String((int)round(doc["Temperature"]["Value"].as<float>())) + String("°"));
        weatherData.apparentTemperaturesCelcius.push_back(    String((int)round(doc["RealFeelTemperature"]["Value"].as<float>())) + String("°"));
        weatherData.relativeHumidities.push_back(             doc["RelativeHumidity"].as<String>() + String("%"));
        weatherData.windSpeeds.push_back(                     String((int)round(doc["Wind"]["Speed"]["Value"].as<float>())) + String("Kmh"));
        weatherData.precipitationProbabilities.push_back(     String((int)round(doc["PrecipitationProbability"].as<float>())) + String("%"));
        weatherData.precipitationAmounts.push_back(           String((int)round(doc["TotalLiquid"]["Value"].as<float>())) + String("mm"));
    }
}