#include <Arduino.h>
#include <time.h>
#include <string.h>
#include <ctime>
#include "Config.h"
#include "Model.h"
#include "octranspo/Route.h"
#include "Util.h"

uint8_t currentHourOfDay() {
    time_t now;
    time(&now);
    struct tm *timeinfo = localtime(&now);
    uint8_t hod = timeinfo->tm_hour;
    return hod;
}
uint8_t currentDayOfMonth() {
    time_t now;
    time(&now);
    struct tm *timeinfo = localtime(&now);
    uint8_t dom = timeinfo->tm_mday;
    return dom;
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


bool inAContentPage(AppState& appState) {
    return (appState == CommutePage || appState == NorthSouthPage || appState == EastWestPage || appState == WeatherPage);
}

void updateAppState(AppState& appState) {
    // if(appState == Initializing || appState == EastWestPage)
    //     appState = NorthSouthPage;
    // else
    //     appState = EastWestPage;
    // return;
    
    // appState = CommutePage;
    // return;

    // appState = WeatherPage;
    // return;

    // appState = NorthSouthPage;
    // return;

    int hourOfDay = currentHourOfDay();
    bool commute = hourOfDay >= APPSTATE_COMMUTE_HOUR_START && hourOfDay <= APPSTATE_COMMUTE_HOUR_END;
    bool sleeping = hourOfDay == APPSTATE_DEEP_SLEEPING_HOUR_START-1;
    sleeping = false;
    bool deepSleeping = hourOfDay >= APPSTATE_DEEP_SLEEPING_HOUR_START || hourOfDay <= APPSTATE_DEEP_SLEEPING_HOUR_END;
    deepSleeping = false;

    // Serial.println("      UTIL: Updating AppState - Hour=" + String(hourOfDay) 
    //         + ", Sleep=" + String(sleeping) +
    //         + ", DeepSleep=" + String(deepSleeping) +
    //          ", Commute=" + String(commute) + " ");

    if(commute) {
        Serial.println("      UTIL: New App State: Commute");
        appState = CommutePage;
    } else if(sleeping) {
        Serial.println("      UTIL: New App State: Sleeping");
        appState = Sleeping;
    }  else if(deepSleeping) {
        Serial.println("      UTIL: New App State: DEEP Sleeping");
        appState = DeepSleeping;
    } else {
        if(appState == Initializing || appState == WeatherPage) {
            Serial.println("      UTIL: New App State: NorthSouthPage");
            appState = NorthSouthPage;
        } else if(appState == NorthSouthPage) {
            Serial.println("      UTIL: New App State: EastWestPage");
            appState = EastWestPage;
        } else { // EastWestPage
        Serial.println("      UTIL: New App State: WeatherPage");
            appState = WeatherPage;
        }
    }
}

void printHighWaterMarkForTask(TaskHandle_t taskHandle) {
    Serial.print("      UTIL: Remaining Stack words for Task ");
    Serial.print(pcTaskGetName(taskHandle));
    Serial.println(" = " + String(uxTaskGetStackHighWaterMark(taskHandle)));
}

void printAvailableHeapMemory() {
    Serial.print("      UTIL: AVAILABLE HEAP MEMORY = ");
    Serial.println(xPortGetFreeHeapSize());
}