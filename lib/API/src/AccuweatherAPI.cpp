#include <HTTPClient.h>
#include <WiFiClientSecure.h>
#include <Preferences.h>
#include <ArduinoJson.h>
#include "AccuweatherAPI.h"
#include "Config.h"
#include "Util.h"

AccuweatherAPI::AccuweatherAPI(WiFiClientSecure* wifiClient, HTTPClient* httpClient, Preferences* preferences, Forecast* forecast)  {
    _wifiClient = wifiClient;
    _httpClient = httpClient;
    _preferences = preferences;
    _forecast = forecast;
}

void AccuweatherAPI::fetchForecast() {

    // Refresh Forecast only if stale or invalid

    if(_forecast->isStaleOrInvalid()) {
        Serial.println("ACCUWE-API: Stale or Invalid forecast data. Refreshing.");
        std::vector<HourlyCondition> newHourlyConditions;
        uint8_t clockHour = currentHourOfDay();
        
        // Grab Current forecast
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

            _forecast->updateUVI(currentConditions["UVIndex"].as<uint8_t>());

            extractHourlyConditionCurrentForecast(currentConditions, newHourlyConditions);
            _httpClient->end();
        } else {
            Serial.println("ACCUWE-API: ERROR Current API returned NOT OK code ("+ String(httpCode) + ")");
            _httpClient->end();
            return;
        }


        // Grab forecast for next hours 
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

            extractHourlyConditionForHour(doc[0].as<JsonVariant>(), newHourlyConditions, clockHour+1);
            extractHourlyConditionForHour(doc[1].as<JsonVariant>(), newHourlyConditions, clockHour+2);
            int fivePmHour = 17;
            extractHourlyConditionForHour(doc[fivePmHour-clockHour].as<JsonVariant>(), newHourlyConditions, fivePmHour);
            _httpClient->end();
        } else {
            Serial.println("ACCUWE-API: ERROR 12H forecast API returned NOT OK code ("+ String(httpCode) + ")");
            _httpClient->end();
            return;
        }

        _forecast->updateHourlyConditions(newHourlyConditions);
        _forecast->saveForecastToFlash(*_preferences);
    }
}

void AccuweatherAPI::extractHourlyConditionCurrentForecast(JsonDocument doc, std::vector<HourlyCondition> &newHourlyConditions) {
    Serial.println("ACCUWE-API: extracting data for day: " + doc["LocalObservationDateTime"].as<String>());
    HourlyCondition newHourlyCondition(
        currentHourOfDay(),
        doc["WeatherIcon"].as<uint8_t>(),
        String((int)round(doc["Temperature"]["Metric"]["Value"].as<float>())) + String("°"),
        String((int)round(doc["RealFeelTemperature"]["Metric"]["Value"].as<float>())) + String("°"),
        doc["RelativeHumidity"].as<String>() + String("%"),
        String((int)round(doc["Wind"]["Speed"]["Metric"]["Value"].as<float>())) + String("Kmh"),
        String((doc["HasPrecipitation"].as<boolean>()) ? "100%" : "0%" ),
        String((int)round(doc["Precip1hr"]["Metric"]["Value"].as<float>())) + String("mm")
    );
    newHourlyConditions.push_back(newHourlyCondition);
}

void AccuweatherAPI::extractHourlyConditionForHour(JsonDocument doc, std::vector<HourlyCondition> &newHourlyConditions, uint8_t hour) {
    Serial.println("ACCUWE-API: extracting data for day: " + doc["DateTime"].as<String>());
    HourlyCondition newHourlyCondition(
        hour,
        doc["WeatherIcon"].as<uint8_t>(),
        String((int)round(doc["Temperature"]["Value"].as<float>())) + String("°"),
        String((int)round(doc["RealFeelTemperature"]["Value"].as<float>())) + String("°"),
        doc["RelativeHumidity"].as<String>() + String("%"),
        String((int)round(doc["Wind"]["Speed"]["Value"].as<float>())) + String("Kmh"),
        String((int)round(doc["PrecipitationProbability"].as<float>())) + String("%"),
        String((int)round(doc["TotalLiquid"]["Value"].as<float>())) + String("mm")
    );
    newHourlyConditions.push_back(newHourlyCondition);
}