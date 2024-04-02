#include <time.h>
#include <string.h>
#include <ctime>
#include <Model.h>
#include <Config.h>
#include "util.h"

int currentHourOfDay() {
    time_t now;
    time(&now);
    struct tm *timeinfo = localtime(&now);
    int hour = timeinfo->tm_hour;
    return hour;
}
void currentDateFull(char* buffer, size_t bufferSize) {
    time_t now;
    time(&now);
    struct tm * timeinfo = localtime(&now);
    strftime (buffer, bufferSize, "%Y-%m-%d %H:%M", timeinfo);
}
void currentDateShort(char* buffer, size_t bufferSize) {
    time_t now;
    time(&now);
    struct tm * timeinfo = localtime(&now);
    strftime (buffer, bufferSize, "%a, %d %b", timeinfo);
}
void currentHourMinute(char* buffer, size_t bufferSize) {
    time_t now;
    time(&now);
    struct tm * timeinfo = localtime(&now);
    strftime (buffer, bufferSize, "%H:%M", timeinfo);
}

void printTrips(const RouteGroupData& data) {
    Serial.println("Trips:");
    for(const auto &destination: data.routeDestinations) {
        String frequent = destination.routeType == FrequentRoute ? "Yes" : "No";
        Serial.print("  " + destination.routeNumber 
            + " " + destination.routeDestination 
            + " (Frequent=" + frequent + ") in: ");

        for(const auto &trip: destination.trips) {
            String estimated = trip.arrivalIsEstimated ? "*" : "";
            Serial.print(estimated + String(trip.arrivalTime) + "min, ");
        }
        Serial.println("");
    }
}

void updateAppState(AppState& appState, uint16_t lightSensorValue) {
    // appState = Weather;

    // if (appState == RoutesNorthSouth) {
    //     appState = RoutesEastWest;
    // } else {
    //     appState = RoutesNorthSouth;
    // }

    int hourOfDay = currentHourOfDay();
    bool vickyCommute = hourOfDay >= APPSTATE_VICKY_COMMUTE_HOUR_START && hourOfDay <= APPSTATE_VICKY_COMMUTE_HOUR_END;
    bool sleeping = hourOfDay >= APPSTATE_SLEEPING_HOUR_START || hourOfDay <= APPSTATE_SLEEPING_HOUR_END;
    // bool sleeping = false;

    Serial.print("UpdateAppState - Hour of day=" + String(hourOfDay) + ", Sleeping=" 
        + String(sleeping) + ", VCommute=" + String(vickyCommute) + ", LightSensor=" + String(lightSensorValue) + " ");

    if(vickyCommute) {
        Serial.println("Result: VickyCommute");
        if(appState == Weather) {
            appState = RoutesVickyCommute;
        } else {
            appState = Weather;
        }
    } else if(sleeping || lightSensorValue <= APPSTATE_SLEEPING_LIGHT_SENSOR_VALUE_TRESHOLD) {
        appState = Sleeping;
        Serial.println("Result: Sleeping");
    } else {
        Serial.println("Result: WeatherOrRoutes");
        if (appState == Weather) {
            appState = RoutesNorthSouth;
        } else if (appState == RoutesNorthSouth) {
            appState = RoutesEastWest;
        } else {
            appState = Weather;
        }
    }
}

RouteGroupType appStateToRouteGroupType(AppState appState) {
    RouteGroupType type = VickyCommute;

    if(appState == RoutesNorthSouth)
        type = NorthSouth;
    else if(appState == RoutesEastWest)
        type = EastWest;

    return type;
}

/**
 * Returns a value that's in the range DISPLAY_BRIGHTNESS_MIN - DISPLAY_BRIGHTNESS_MAX
 *  - Return DISPLAY_BRIGHTNESS_MIN for any value < LIGHT_SENSOR_VALUE_MIN
 *  - Return DISPLAY_BRIGHTNESS_MAX for any value > LIGHT_SENSOR_VALUE_MAX
 *  - Map a value in between DISPLAY_BRIGHTNESS_MIN and DISPLAY_BRIGHTNESS_MAX for light sensor values in between
 *    In incremental steps based on DISPLAY_BRIGHTNESS_STEPS
*/
int8_t lightSensorToDisplayBrightness(uint16_t lightSensorValue) {
    if(lightSensorValue < LIGHT_SENSOR_VALUE_MIN) {
        return DISPLAY_BRIGHTNESS_MIN;
    } else if(lightSensorValue > LIGHT_SENSOR_VALUE_MAX) {
        return DISPLAY_BRIGHTNESS_MAX;
    } else {
        uint8_t mappedValue = map(lightSensorValue, LIGHT_SENSOR_VALUE_MIN, LIGHT_SENSOR_VALUE_MAX, 1, DISPLAY_BRIGHTNESS_STEPS);
        return DISPLAY_BRIGHTNESS_MIN + ((float)mappedValue/DISPLAY_BRIGHTNESS_STEPS) * (DISPLAY_BRIGHTNESS_MAX - DISPLAY_BRIGHTNESS_MIN);
    }    
}