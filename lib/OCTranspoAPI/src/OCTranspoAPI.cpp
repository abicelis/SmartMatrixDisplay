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

RouteGroupData OCTranspoAPI::fetchRouteGroup(RouteGroupType type) {
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
    delay(500); // Delay a bit?

    Serial.println("fetchTripsFor Stop#" + stopNo + " Route#"+ routeNo);
    String endpoint = String(OCTRANSPO_API_NEXT_TRIPS_FOR_STOP_ENDPOINT);
    endpoint += "&stopNo=" + stopNo + "&routeNo=" + routeNo;

    _httpClient->begin(*_wifiClient, endpoint);
    int httpCode = _httpClient->GET();

    if(httpCode != 200) {
        Serial.println("Warning: OC transpo endpoint returned " + String(httpCode));
    } else {
        JsonDocument doc;
        deserializeJson(doc, _httpClient->getStream());

        // Validate that OC transpo's response is valid JSON.
        // June 26th, 2024: 
        //  - OC Server went down. Endpoints returned HTTP 200 OK but instead of JSON
        //    we got "<h4>A PHP Error was encountered</h4>" HTML nonsense!
        if(doc.containsKey("GetNextTripsForStopResult")) {
            String routeNumber = doc["GetNextTripsForStopResult"]["Route"]["RouteDirection"]["RouteNo"];
            JsonArray trips = doc["GetNextTripsForStopResult"]["Route"]["RouteDirection"]["Trips"]["Trip"];
            for (JsonObject trip: trips) {
                // String out = "";
                // serializeJsonPretty(trip, out);
                // Serial.println(out);
                int arrivalTime = trip["AdjustedScheduleTime"].as<int>();
                if(arrivalTime > OCTRANSPO_API_MAX_ARRIVAL_TIME_MINUTES || arrivalTime < OCTRANSPO_API_MIN_ARRIVAL_TIME_MINUTES) {
                    // Serial.print("Removed trip with extreme arrivalTime = ");
                    // Serial.println(arrivalTime);
                    continue;
                }
                arrivalTime -= OCTRANSPO_API_MINUTES_TO_SUBTRACT_FROM_ARRIVAL_TIME;
                String routeDestination = trip["TripDestination"];
                bool arrivalIsEstimated = trip["AdjustmentAge"].as<float>() != -1 && trip["Longitude"] != "";

                BusLocation busLocation = None;
                if(routeNo == "88" && stopNo == "4483" && arrivalIsEstimated) {
                    busLocation = TooClose;
                    if(trip["Longitude"].as<float>() < OCTRANSPO_API_88_HURDMAN_LONGITUDE_THRESHOLD_BUS_FAR_ENOUGH_AWAY)
                        busLocation = FarAwayEnough;
                }
                
                addTrip(data, routeNumber, routeDestination, arrivalTime, arrivalIsEstimated, busLocation);
            }
        } else {
            Serial.println("Warning: OC transpo endpoint returned INVALID JSON");
        }
    }

    _httpClient->end();
    Serial.println("fetchTripsFor end");
}

void OCTranspoAPI::addTrip(RouteGroupData& data, String& routeNumber, String& routeDestination, uint8_t arrivalTime, bool arrivalIsEstimated, BusLocation busLocation) {
    bool inserted = false;
    for (auto &destination : data.routeDestinations) {
        if(destination.routeNumber == routeNumber && destination.routeDestination == routeDestination) {
            inserted = true;
            destination.trips.push_back(Trip(arrivalTime, arrivalIsEstimated, busLocation));
        }
    }
    if(inserted == false) {
        RouteDestination destination(routeNumber, routeDestination, getRouteType(routeNumber));
        destination.trips.push_back(Trip(arrivalTime, arrivalIsEstimated, busLocation));
        data.routeDestinations.push_back(destination);
    }
}

void OCTranspoAPI::sortTrips(RouteGroupData& data) {
    // Serial.println("Sort start");
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
    // Serial.println("Sort end");
}


RouteType OCTranspoAPI::getRouteType(String& routeNumber) {
    if(routeNumber == "88" || routeNumber == "80")
        return FrequentRoute;
    else 
        return LocalRoute;
}