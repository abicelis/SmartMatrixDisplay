#include <Arduino.h>
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
    strftime (buffer, bufferSize, "%I:%M", timeinfo);


    if (buffer[0] == '0')   // HH:MM format, Remove first 0 if necessary. e.g '08:03\0' -> '8:03\0'  (\0 = null char)
        memmove(buffer, buffer+1, 5);
}

String hourNumericToPretty(uint8_t hour) {
    return (hour > 12 ? String(hour - 12) + String(" pm") : String(hour) + String(" am")); 
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
            String farAwayEnough = trip.busLocation == FarAwayEnough ? "!" : "";
            Serial.print(farAwayEnough + estimated + String(trip.arrivalTime) + "min, ");
        }
        Serial.println("");
    }
}

void updateAppState(AppState& appState, AppPage& appPage) {
    // appState = NextPageLoading;
    // appPage = VickyCommutePage;
    // return;

    // appState = NextPageLoading;
    // appPage = WeatherPage;
    // return;

    int hourOfDay = currentHourOfDay();
    bool vickyCommute = hourOfDay >= APPSTATE_VICKY_COMMUTE_HOUR_START && hourOfDay <= APPSTATE_VICKY_COMMUTE_HOUR_END;
    bool sleeping = hourOfDay == APPSTATE_DEEP_SLEEPING_HOUR_START-1;
    bool deepSleeping = hourOfDay >= APPSTATE_DEEP_SLEEPING_HOUR_START || hourOfDay <= APPSTATE_DEEP_SLEEPING_HOUR_END;

    Serial.print("UpdateAppState - Hour=" + String(hourOfDay) 
            + ", Sleep=" + String(sleeping) +
            + ", DeepSleep=" + String(deepSleeping) +
             ", VCommute=" + String(vickyCommute) + " ");

    if(vickyCommute) {
        Serial.println("- Result: VickyCommute");
        appState = NextPageLoading;        
        appPage = VickyCommutePage;
    } else if(sleeping) {
        Serial.println("- Result: Sleeping");
        appState = Sleeping;
        appPage = NoPage;
    }  else if(deepSleeping) {
        Serial.println("- Result: Deep Sleeping");
        appState = DeepSleeping;
        appPage = NoPage;
    } else {
        appState = NextPageLoading;
        if(appPage == NoPage || appPage == WeatherPage) {
            Serial.println("- Result: NorthSouth");
            appPage = NorthSouthPage;
        } else if(appPage == NorthSouthPage) {
            Serial.println("- Result: EastWestPage");
            appPage = EastWestPage;
        } else { //EastWestPage 
        Serial.println("- Result: WeatherPage");
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

void printHighWaterMarkForTask(TaskHandle_t taskHandle) {
    Serial.print(">>> Remaining Stack words for Task ");
    Serial.print(pcTaskGetName(taskHandle));
    Serial.println(" = " + String(uxTaskGetStackHighWaterMark(taskHandle)));
}

void printAvailableHeapMemory() {
    Serial.print(">>>> AVAILABLE HEAP MEMORY = ");
    Serial.println(xPortGetFreeHeapSize());
}