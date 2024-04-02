#include <HTTPClient.h>
#include <WiFiClientSecure.h>
#include <ArduinoJson.h>
#include <vector>
#include <Trip.h>
#include <RouteGroupData.h>
#include <Config.h>
#include "OCTranspoAPI.h"

OCTranspoAPI::OCTranspoAPI(WiFiClientSecure* wifiClient, HTTPClient* httpClient)  {
    _wifiClient = wifiClient;
    _httpClient = httpClient;
}

RouteGroupData OCTranspoAPI::fetchTrips(RouteGroupType type) {
    RouteGroupData data;
    data.type = type;
    if(type == VickyCommute) {
        fetchTripsFor(data, "4483", "88");
    } else if(type == NorthSouth) {
        fetchTripsFor(data, "1267", "80");
        fetchTripsFor(data, "2491", "80");
        fetchTripsFor(data, "2491", "81");
        fetchTripsFor(data, "1251", "81");
    } else if(type == EastWest) {
        fetchTripsFor(data, "4483", "88");
        fetchTripsFor(data, "4491", "88");
    }
    sortTrips(data);
    return data;
}

void OCTranspoAPI::fetchTripsFor(RouteGroupData& data, const String& stopNo, const String& routeNo) {
    #ifdef DEBUG
    Serial.println("fetchTripsFor Stop#" + stopNo + " Route#"+ routeNo);
    #endif
    String endpoint = String(OC_TRANSPO_API_NEXT_TRIPS_FOR_STOP_ENDPOINT);
    endpoint += "&stopNo=" + stopNo + "&routeNo=" + routeNo;

    _httpClient->begin(*_wifiClient, endpoint);
    int httpCode = _httpClient->GET();

    if(httpCode != 200) {
        #ifdef DEBUG
        Serial.println("Warning: OC transpo endpoint returned" + String(httpCode));
        #endif
    } else {
        JsonDocument doc;
        deserializeJson(doc, _httpClient->getStream());

        // String out = "";
        // serializeJsonPretty(doc, out);
        // Serial.println(out);

        String routeNumber = doc["GetNextTripsForStopResult"]["Route"]["RouteDirection"]["RouteNo"];
        JsonArray trips = doc["GetNextTripsForStopResult"]["Route"]["RouteDirection"]["Trips"]["Trip"];
        for (JsonObject trip: trips) {
            // String out = "";
            // serializeJsonPretty(trip, out);
            // Serial.println(out);
            int arrivalTime = trip["AdjustedScheduleTime"].as<int>();
            if(arrivalTime > MAX_ARRIVAL_TIME_MINUTES)
                break;
            String routeDestination = trip["TripDestination"];
            float adjustmentAge = trip["AdjustmentAge"].as<float>();
            addTrip(data, routeNumber, routeDestination, arrivalTime, adjustmentAge != -1);
        }
    }

    _httpClient->end();

    #ifdef DEBUG
    Serial.println("fetchTripsFor end");
    #endif
}

void OCTranspoAPI::addTrip(RouteGroupData& data, String& routeNumber, String& routeDestination, uint8_t arrivalTime, bool arrivalIsEstimated) {
    bool inserted = false;
    for (auto &destination : data.routeDestinations) {
        if(destination.routeNumber == routeNumber && destination.routeDestination == routeDestination) {
            inserted = true;
            destination.trips.push_back(Trip(arrivalTime, arrivalIsEstimated));
        }
    }
    if(inserted == false) {
        RouteDestination destination(routeNumber, routeDestination, getRouteType(routeNumber));
        destination.trips.push_back(Trip(arrivalTime, arrivalIsEstimated));
        data.routeDestinations.push_back(destination);
    }
}

void OCTranspoAPI::sortTrips(RouteGroupData& data) {
    #ifdef DEBUG
    Serial.println("Sort start");
    #endif
    uint8_t minArrival;
    uint8_t right;
    uint8_t size = data.routeDestinations.size();
    for(uint8_t left = 0; left < size-1; left++) {
        minArrival = 255;
        uint8_t right;
        for(uint8_t i = left; i < size; i++) {
            uint8_t currentArrival = data.routeDestinations[i].trips[0].arrivalTime;
            if(currentArrival < minArrival) {
                minArrival = currentArrival;
                right = i;
            }
        }

        if(right != left) {
            std::swap(data.routeDestinations[left], data.routeDestinations[right]);
        }
    }
    #ifdef DEBUG
    Serial.println("Sort end");
    #endif
}


RouteType OCTranspoAPI::getRouteType(String& routeNumber) {
    if(routeNumber == "88" || routeNumber == "80")
        return FrequentRoute;
    else 
        return LocalRoute;
}