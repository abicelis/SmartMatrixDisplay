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

void printTrips(const TripsData& trips) {
    Serial.println("Trips:");
    for(int i = 0; i < trips.routeLabels.size(); i++) {
        String type = "Scheduled";
        if(trips.arrivalIsEstimated.at(i) == true)
            type = "Estimated";
        Serial.println("  " + String(trips.routeNumbers.at(i)) + " " 
            + trips.routeLabels.at(i) + " in " 
            + String(trips.busArrivalTimes.at(i)) + "min ("+ type +")")
            + " Frequent=" + String(trips.routeIsFrequent.at(i));

        
    }
}

void updateAppState(AppState& appState, uint16_t lightSensorValue) {
    int hourOfDay = currentHourOfDay();
    bool vickyCommute = hourOfDay >= APPSTATE_VICKY_COMMUTE_HOUR_START && hourOfDay <= APPSTATE_VICKY_COMMUTE_HOUR_END;
    bool sleeping = hourOfDay >= APPSTATE_SLEEPING_HOUR_START || hourOfDay <= APPSTATE_SLEEPING_HOUR_END;
    // bool sleeping = false;

    Serial.println("Hour of day=" + String(hourOfDay) + ", Sleeping=" 
        + String(sleeping) + ", VCommute=" + String(vickyCommute) + ", LightSensor=" + String(lightSensorValue));

    if(vickyCommute) {
        if(appState == Weather) {
            appState = RoutesVickyCommute;
        } else {
            appState = Weather;
        }
    } else if(sleeping || lightSensorValue <= APPSTATE_SLEEPING_LIGHT_SENSOR_VALUE_TRESHOLD) {
        appState = Sleeping;
    } else {
        if (appState == Weather) {
            appState = RoutesNorthSouth;
        } else if (appState == RoutesNorthSouth) {
            appState = RoutesEastWest;
        } else {
            appState = Weather;
        }
    }
}

TripsType appStateToTripsType(AppState appState) {
    TripsType tripsType = VickyCommute;

    if(appState == RoutesNorthSouth)
        tripsType = NorthSouth;
    else if(appState == RoutesEastWest)
        tripsType = EastWest;

    return tripsType;
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