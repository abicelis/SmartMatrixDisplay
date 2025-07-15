#include <HTTPClient.h>
#include <WiFiClientSecure.h>
#include <vector>
#include <Config.h>
#include "OCTranspoAPI.h"
#include <cstdlib>
#include <ctime>

OCTranspoAPI::OCTranspoAPI(WiFiClientSecure* wifiClient, HTTPClient* httpClient, Routes* routes) {
    _wifiClient = wifiClient;
    _httpClient = httpClient;
    _routes = routes;
}

void OCTranspoAPI::fetchRoutes(bool doItQuickly) {    
    for (const auto &route : _routes->routes) {
        // if(!doItQuickly)
        //     vTaskDelay(pdMS_TO_TICKS(OCTRANSPO_DELAY_BETWEEN_FETCHES_MS));
        // else
        //     vTaskDelay(pdMS_TO_TICKS(500));
        // fetchTripsFor(route.stopNumber, route.number, route.destination);

        

        JsonDocument doc;
        if(route.destination == "Hurdman") {
            // Nostalgia
            std::srand(std::time(nullptr));      // Seed with current time
            int trip1 = 4 + std::rand() % 6;     // Range: [4, 9]
            int trip2 = 12 + std::rand() % 24;   // Range: [12, 35]
            std::string json = "[{'AdjustedScheduleTime':"+ std::to_string(trip1)+",'TripDestination':'Hurdman','AdjustmentAge':1.0,'Longitude':'123'},{'AdjustedScheduleTime':"+ std::to_string(trip2)+",'TripDestination':'Hurdman','AdjustmentAge':-1,'Longitude':''}]";
            //deserializeJson(doc, "[{'AdjustedScheduleTime':8,'TripDestination':'Hurdman','AdjustmentAge':1.0,'Longitude':'123'},{'AdjustedScheduleTime':15,'TripDestination':'Hurdman','AdjustmentAge':-1,'Longitude':''}]");
            deserializeJson(doc, json);
        } else { // Terry Fox
            // Nostalgia
            std::srand(std::time(nullptr));      // Seed with current time
            int trip1 = 4 + std::rand() % 6;     // Range: [4, 9]
            int trip2 = 12 + std::rand() % 13;   // Range: [12, 24]
            int trip3 = 25 + std::rand() % 11;   // Range: [25, 35]
            std::string json = "[{'AdjustedScheduleTime':"+ std::to_string(trip1)+",'TripDestination':'Terry Fox','AdjustmentAge':1.0,'Longitude':'123'},{'AdjustedScheduleTime':"+ std::to_string(trip2)+",'TripDestination':'Terry Fox','AdjustmentAge':1.0,'Longitude':'123'},{'AdjustedScheduleTime':"+ std::to_string(trip3)+",'TripDestination':'Terry Fox','AdjustmentAge':-1,'Longitude':''}]";
            deserializeJson(doc, json);
        }
        JsonArray jsonArray = doc.as<JsonArray>();
        processTripsFor(jsonArray, route.number, route.destination);
    }
    _routes->printRoutes();
}

void OCTranspoAPI::fetchTripsFor(const String& stopNumber, const String& routeNumber, const String& routeDestination) {
    Serial.println("  OCTR-API: Fetching Trips for Stop#" + stopNumber + " Route#"+ routeNumber + " Destination="+ routeDestination);
    String endpoint = String(OCTRANSPO_API_NEXT_TRIPS_FOR_STOP_ENDPOINT);
    endpoint += "&stopNo=" + stopNumber + "&routeNo=" + routeNumber;

    _httpClient->begin(*_wifiClient, endpoint);
    int httpCode = _httpClient->GET();

    if(httpCode == 200) {
        JsonDocument doc;
        deserializeJson(doc, _httpClient->getStream());

        // Validate that OC transpo's response is valid JSON.
        // June 26th, 2024: 
        //  - OC Server went down. Endpoints returned HTTP 200 OK but instead of JSON
        //    we got "<h4>A PHP Error was encountered</h4>" HTML nonsense!
        if(doc.containsKey("GetNextTripsForStopResult")) {

            // Sometimes there's ONE physical Stop for both directions of a Route
            // In these cases "RouteDirection" is an array, so we need to differentiate Routes using "RouteLabel"
            if(doc["GetNextTripsForStopResult"]["Route"]["RouteDirection"].is<JsonArray>()) {
                boolean found = false;
                JsonArray routeDirectionArray = doc["GetNextTripsForStopResult"]["Route"]["RouteDirection"].as<JsonArray>();
                for (JsonObject routeDirectionJson: routeDirectionArray) {
                    String routeLabel = routeDirectionJson["RouteLabel"].as<String>();
                    if(routeLabel.indexOf(routeDestination) != -1) {
                        JsonArray tripsJsonArray = routeDirectionJson["Trips"]["Trip"].as<JsonArray>();
                        processTripsFor(tripsJsonArray, routeNumber, routeDestination);
                        found = true;
                        break;
                    }
                }
                if (!found) {
                    Serial.println("  OCTR-API: WARNING API returned 'RouteDirection' as an array. Could not find routeDestination containing '"+ routeDestination + "')");
                }

            // Usually, there's different physical Stops (one on each side of the road) for the two directions of a Route
            // In this case, "RouteDirection" returns one object.
            } else {
                JsonArray tripsJsonArray = doc["GetNextTripsForStopResult"]["Route"]["RouteDirection"]["Trips"]["Trip"].as<JsonArray>();
                processTripsFor(tripsJsonArray, routeNumber, routeDestination);
            }
        } else {
            Serial.println("  OCTR-API: WARNING API returned INVALID JSON");
        }
    } else {
        Serial.println("  OCTR-API: WARNING API returned NOT OK code ("+ String(httpCode) + ")");
    }
    _httpClient->end();
}


void OCTranspoAPI::processTripsFor(const JsonArray tripsJson, const String& routeNumber, const String& routeDestination) {
    std::vector<std::tuple<String, int, bool, TripArrivalLocation>> flatTrips;
    for (JsonObject tripJson: tripsJson) {
        // String out = "";
        // serializeJsonPretty(trip, out);
        // Serial.println(out);
        int arrivalTime = tripJson["AdjustedScheduleTime"].as<int>();
        if(arrivalTime > OCTRANSPO_API_MAX_ARRIVAL_TIME_MINUTES || arrivalTime < OCTRANSPO_API_MIN_ARRIVAL_TIME_MINUTES) {
            // Serial.print("OCT-API: Removed trip with extreme arrivalTime = ");
            // Serial.println(arrivalTime);
            continue;
        }
        arrivalTime -= OCTRANSPO_API_MINUTES_TO_SUBTRACT_FROM_ARRIVAL_TIME;
        String actualDestination = tripJson["TripDestination"];
        bool arrivalIsEstimated = tripJson["AdjustmentAge"].as<float>() != -1 && tripJson["Longitude"] != "";

        TripArrivalLocation location = None;
        // if(routeNumber == "88" && stopNumber == "4483" && arrivalIsEstimated) {
        //     location = TooClose;
        //     if(tripJson["Longitude"].as<float>() < OCTRANSPO_API_88_HURDMAN_LONGITUDE_THRESHOLD_BUS_FAR_ENOUGH_AWAY)
        //         location = FarAwayEnough;
        // }
        flatTrips.push_back(std::make_tuple(actualDestination, arrivalTime, arrivalIsEstimated, location));
    }

    _routes->replaceTripsForRoute(routeNumber, routeDestination, flatTrips);
}

