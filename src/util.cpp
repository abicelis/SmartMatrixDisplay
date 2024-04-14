#include <time.h>
#include <string.h>
#include <ctime>
#include <Model.h>
#include <Config.h>
#include "util.h"

uint8_t currentHourOfDay() {
    time_t now;
    time(&now);
    struct tm *timeinfo = localtime(&now);
    uint8_t hour = timeinfo->tm_hour;
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

void updateAppState(AppState& appState, AppPage& appPage) {
    appState = NextPageLoading;
    appPage = WeatherPage;
    return;


    int hourOfDay = currentHourOfDay();
    bool vickyCommute = hourOfDay >= APPSTATE_VICKY_COMMUTE_HOUR_START && hourOfDay <= APPSTATE_VICKY_COMMUTE_HOUR_END;
    bool sleeping = hourOfDay >= APPSTATE_SLEEPING_HOUR_START || hourOfDay <= APPSTATE_SLEEPING_HOUR_END;
    // bool sleeping = false;

    Serial.print("UpdateAppState - Hour of day=" + String(hourOfDay) + ", Sleeping=" 
        + String(sleeping) + ", VCommute=" + String(vickyCommute));

    if(vickyCommute) {
        Serial.println("- Result: VickyCommute");
        appState = NextPageLoading;
        
        if(appPage == NoPage || appPage == WeatherPage)
            appPage = VickyCommutePage;
        else
            appPage = WeatherPage;
    } else if(sleeping) {
        Serial.println("- Result: Sleeping");
        appState = Sleeping;
        appPage = NoPage;
    } else {
        Serial.println("- Result: Normal Mode");
        appState = NextPageLoading;
        
        if(appPage == NoPage || appPage == WeatherPage) {
            appPage = NorthSouthPage;
        } else if(appPage == NorthSouthPage) {
            appPage = EastWestPage;
        } else { //EastWestPage 
            appPage = WeatherPage;
        }
    }
}

RouteGroupType appPageToRouteGroupType(AppPage appPage) {
    RouteGroupType type = VickyCommute;

    if(appPage == NorthSouthPage)
        type = NorthSouth;
    else if(appPage == EastWestPage)
        type = EastWest;

    return type;
}