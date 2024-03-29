#include <HTTPClient.h>
#include <WiFiClientSecure.h>
#include <ArduinoJson.h>
#include <Model.h>
#include <Config.h>
#include "OCTranspoAPI.h"

OCTranspoAPI::OCTranspoAPI(WiFiClientSecure* wifiClient, HTTPClient* httpClient)  {
    _wifiClient = wifiClient;
    _httpClient = httpClient;
}
TripsData OCTranspoAPI::fetchNextTripsFor(const String& stopNo, const String& routeNo) {
    String endpoint = String(OC_TRANSPO_API_NEXT_TRIPS_FOR_STOP_ENDPOINT);
    endpoint += "&stopNo=" + stopNo + "&routeNo=" + routeNo;

    _httpClient->begin(*_wifiClient, endpoint);
    _httpClient->GET();

    JsonDocument doc;
    deserializeJson(doc, _httpClient->getStream());
    // String out = "";
    // serializeJsonPretty(doc, out);
    // Serial.println(out);

    TripsData result;
    int routeNumber = doc["GetNextTripsForStopResult"]["Route"]["RouteDirection"]["RouteNo"].as<int>();
    const char* routeLabel = doc["GetNextTripsForStopResult"]["Route"]["RouteDirection"]["RouteLabel"];

    JsonArray trips = doc["GetNextTripsForStopResult"]["Route"]["RouteDirection"]["Trips"]["Trip"];
    for (JsonObject trip: trips) {
        // String out = "";
        // serializeJsonPretty(trip, out);
        // Serial.println(out);
        float adjustmentAge = trip["AdjustmentAge"].as<float>();
        int adjustedScheduleTime = trip["AdjustedScheduleTime"].as<int>();
        if(adjustedScheduleTime > MAX_ARRIVAL_TIME_MINUTES)
            break;
        result.routeNumbers.push_back(routeNumber);
        result.routeLabels.push_back(routeLabel);
        result.busArrivalTimes.push_back(adjustedScheduleTime);
        result.arrivalIsEstimated.push_back(adjustmentAge != -1);
        result.routeIsFrequent.push_back(routeIsFrequent(routeNumber));
    }

    _httpClient->end();
    return result;
}
TripsData OCTranspoAPI::fetchNextTrips(TripsType tripsType) {
    TripsData allTrips;
    if(tripsType == VickyCommute) {
        allTrips = fetchNextTripsFor("4483", "88");
    } else if(tripsType == NorthSouth) {
        TripsData tripsA;
        TripsData tripsB;
        tripsA = fetchNextTripsFor("1267", "80");
        tripsB = fetchNextTripsFor("2491", "80");
        mergeAndSortTrips(allTrips, tripsA, tripsB);

        tripsA = fetchNextTripsFor("2491", "81");
        tripsB = allTrips;
        allTrips = TripsData();
        mergeAndSortTrips(allTrips, tripsA, tripsB);

        tripsA = fetchNextTripsFor("1251", "81");
        tripsB = allTrips;
        allTrips = TripsData();
        mergeAndSortTrips(allTrips, tripsA, tripsB);
    } else if(tripsType == EastWest) {
        TripsData tripsA;
        TripsData tripsB;
        tripsA = fetchNextTripsFor("4483", "88");
        tripsB = fetchNextTripsFor("4491", "88");
        mergeAndSortTrips(allTrips, tripsA, tripsB);
    }
    return allTrips;
}

void OCTranspoAPI::mergeAndSortTrips(TripsData& result, TripsData& A, TripsData& B) {
    int i = 0;
    int j = 0;
    // Serial.println("Size of A = " + String(A.routeNumbers.size()));
    // Serial.println("Size of B = " + String(B.routeNumbers.size()));
    while(i < A.routeNumbers.size() || j < B.routeNumbers.size()) {
        // Serial.println("loop i=" + String(i) + " j=" + String(j));

        if(i == A.routeNumbers.size()) {
            // Serial.println("I reached max");
            appendTrip(result, B, j);
            j++;
        } else if(j == B.routeNumbers.size()) {
            // Serial.println("J reached max");
            appendTrip(result, A, i);
            i++;
        } else if (A.busArrivalTimes.at(i) < B.busArrivalTimes.at(j)) {
            // Serial.println("A[i] = " + String(A.scheduleArrivalTimes.at(i)) + " is less.");
            appendTrip(result, A, i);
            i++;
        } else {
            // Serial.println("B[j] = " + String(B.scheduleArrivalTimes.at(i)) + " is less.");
            appendTrip(result, B, j);
            j++;
        }
        // Serial.println("END LOOP");
    }
    // Serial.println("END FUNC, final trips count = " + String(result.routeLabels.size()));

}

void OCTranspoAPI::appendTrip(TripsData& appendee, TripsData& appended, const int& pos) {
    appendee.routeNumbers.push_back(appended.routeNumbers.at(pos));
    appendee.routeLabels.push_back(appended.routeLabels.at(pos));
    appendee.busArrivalTimes.push_back(appended.busArrivalTimes.at(pos));
    appendee.arrivalIsEstimated.push_back(appended.arrivalIsEstimated.at(pos));
    appendee.routeIsFrequent.push_back(appended.routeIsFrequent.at(pos));
}

boolean OCTranspoAPI::routeIsFrequent(int routeNumber) {
    return (routeNumber == 88 || routeNumber == 80);
}