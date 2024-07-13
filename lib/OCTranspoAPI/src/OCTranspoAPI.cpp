#include <HTTPClient.h>
#include <WiFiClientSecure.h>
#include <ArduinoJson.h>
#include <vector>
#include <Config.h>
#include "OCTranspoAPI.h"

OCTranspoAPI::OCTranspoAPI(WiFiClientSecure* wifiClient, HTTPClient* httpClient, Routes* routes) {
    _wifiClient = wifiClient;
    _httpClient = httpClient;
    _routes = routes;
}

void OCTranspoAPI::fetchRoutes() {        
    for (const auto &route : _routes->routes) {
        vTaskDelay(pdMS_TO_TICKS(OCTRANSPO_DELAY_BETWEEN_FETCHES_MS));
        fetchTripsFor(route.stopNumber, route.number, route.destination);
    }
    _routes->printRoutes();
}

void OCTranspoAPI::startFetchRoutesTask() {
    BaseType_t result = xTaskCreatePinnedToCore(fetchRoutesTask, "FetchRoutesTask", 
                STACK_DEPTH_FETCH_ROUTES_TASK, this, TASK_PRIORITY_NETWORK, &fetchRoutesTaskHandle, CORE_ID_NETWORK);
    if(result == pdPASS) {
        Serial.println("OCT-API: FetchRoutesTask launched");
    } else if(result == errCOULD_NOT_ALLOCATE_REQUIRED_MEMORY) {
        Serial.print("OCT-API: ERROR FetchRoutesTask creation failed due to insufficient memory! HEAP MEM=");
        Serial.println(xPortGetFreeHeapSize());
        vTaskDelay(pdMS_TO_TICKS(5000));
        ESP.restart();
    } else {
        Serial.print("OCT-API: ERROR Task creation failed!!!");
        vTaskDelay(pdMS_TO_TICKS(5000));
        ESP.restart();
    }
}

void OCTranspoAPI::fetchTripsFor(const String& stopNumber, const String& routeNumber, const String& routeDestination) {
    Serial.println("OCT-API: Fetching Trips for Stop#" + stopNumber + " Route#"+ routeNumber);
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
            JsonArray tripsJson = doc["GetNextTripsForStopResult"]["Route"]["RouteDirection"]["Trips"]["Trip"];

        
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
                if(routeNumber == "88" && stopNumber == "4483" && arrivalIsEstimated) {
                    location = TooClose;
                    if(tripJson["Longitude"].as<float>() < OCTRANSPO_API_88_HURDMAN_LONGITUDE_THRESHOLD_BUS_FAR_ENOUGH_AWAY)
                        location = FarAwayEnough;
                }
                flatTrips.push_back(std::make_tuple(actualDestination, arrivalTime, arrivalIsEstimated, location));
            }

            _routes->replaceTripsForRoute(routeNumber, routeDestination, flatTrips);
        } else {
            Serial.println("OCT-API: WARNING API returned INVALID JSON");
        }
    } else {
        Serial.println("OCT-API: WARNING API returned NOT OK code ("+ String(httpCode) + ")");
    }
    _httpClient->end();
}

