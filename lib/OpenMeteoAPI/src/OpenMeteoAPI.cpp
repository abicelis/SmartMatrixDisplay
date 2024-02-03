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

    // int routeNumber = doc["GetNextTripsForStopResult"]["Route"]["RouteDirection"]["RouteNo"].as<int>();
    // const char* routeLabel = doc["GetNextTripsForStopResult"]["Route"]["RouteDirection"]["RouteLabel"];

    // JsonArray trips = doc["GetNextTripsForStopResult"]["Route"]["RouteDirection"]["Trips"]["Trip"];
    // for (JsonObject trip: trips) {
    //     // Serial.println("\nfetchNextTrips is Parsing trip. i=" + String(i));
    //     // String out = "";
    //     // serializeJsonPretty(trip, out);
    //     // Serial.println(out);
    //     float adjustmentAge = trip["AdjustmentAge"].as<float>();
    //     int adjustedScheduleTime = trip["AdjustedScheduleTime"].as<int>();
    //     if(adjustedScheduleTime > MAX_ARRIVAL_TIME_MINUTES)
    //         break;
    //     result.routeNumbers.push_back(routeNumber);
    //     result.routeLabels.push_back(routeLabel);
    //     result.scheduleArrivalTimes.push_back(adjustedScheduleTime);
    //     result.scheduleIsEstimated.push_back(adjustmentAge != -1);
    // }

    // _httpClient->end();
    return result;
}

// Mappings: https://www.nodc.noaa.gov/archive/arc0021/0002199/1.1/data/0-data/HTML/WMO-CODE/WMO4677.HTM
WeatherType OpenMeteoAPI::wmoCodeToWeatherType(int& c) {
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