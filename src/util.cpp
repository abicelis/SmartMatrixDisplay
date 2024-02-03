#include <time.h>
#include <string.h>
#include <ctime>
#include <Model.h>
#include "util.h"
int currentHourOfDay() {
    time_t now;
    time(&now);
    struct tm *timeinfo = localtime(&now);
    int hour = timeinfo->tm_hour;
    return hour;
}
const char* currentTimePretty() {
    time_t now;
    time(&now);
    char* timeStr = ctime(&now);
    timeStr[strlen(timeStr) - 1] = '\0';  // Replace the newline with a null character
    return timeStr;
}
void currentHourMinute(char* buffer, size_t bufferSize) {
    time_t now;
    time(&now);
    struct tm * timeinfo = localtime(&now);
    strftime (buffer, bufferSize, "%H:%M",timeinfo);
}

void printTrips(const TripsData& trips) {
    Serial.println("Trips:");
    for(int i = 0; i < trips.routeLabels.size(); i++) {
        String type = "Scheduled";
        if(trips.arrivalIsEstimated.at(i) == true)
            type = "Estimated";
        Serial.println("  " + String(trips.routeNumbers.at(i)) + " " + trips.routeLabels.at(i) + " in " + String(trips.busArrivalTimes.at(i)) + "min ("+ type +")");
    }
}

void updateAppState(AppState& appState) {
    int hourOfDay = currentHourOfDay();
    bool vickyCommute = hourOfDay == 6 || hourOfDay == 7;
    bool sleeping = hourOfDay >= 20 || hourOfDay <= 5;

    Serial.println("Hour of day " + String(hourOfDay) + ". Sleeping=" + String(sleeping) + " VCommute="+String(vickyCommute));

    if(sleeping) {
        appState = Sleeping;
    } else if(vickyCommute) {
        if(appState == Weather) {
            appState = RoutesVickyCommute;
        } else  {
            appState = Weather;
        }
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