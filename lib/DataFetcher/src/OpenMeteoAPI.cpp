// #include <Arduino.h>
#include <HTTPClient.h>
#include <WiFiClientSecure.h>
#include <ArduinoJson.h>
#include <Model.h>
#include <Config.h>
#include "OpenMeteoAPI.h"
#include "Util.h"

OpenMeteoAPI::OpenMeteoAPI(WiFiClientSecure* wifiClient, HTTPClient* httpClient, Forecast* forecast)  {
    _wifiClient = wifiClient;
    _httpClient = httpClient;
    _forecast = forecast;
}

void OpenMeteoAPI::fetchAQI() {
    Serial.println("  OMTO-API: Fetching AQI from OpenMeteo");
    // Refreshing CurrentAQI every time.
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
        _forecast->updateAQI(doc["current"]["us_aqi"].as<uint16_t>());
    } else {
        Serial.println("  OMTO-API: WARNING API returned NOT OK code ("+ String(httpCode) + ")");
    }
    _httpClient->end();
}