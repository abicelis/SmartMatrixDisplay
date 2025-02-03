#include <Arduino.h>
#include <time.h>
#include <string.h>
#include <ctime>
#include "Config.h"
#include "Model.h"
#include "octranspo/Route.h"
#include "Util.h"

struct tm *applocaltime (const time_t *_timer) {
    struct tm *timeinfo = localtime(_timer);
    // timeinfo->tm_hour = timeinfo->tm_hour-5;
    return timeinfo;
}

uint8_t currentHourOfDay() {
    time_t now;
    time(&now);
    struct tm *timeinfo = applocaltime(&now);
    uint8_t hod = timeinfo->tm_hour;
    return hod;
}
uint8_t currentMinute() {
    time_t now;
    time(&now);
    struct tm *timeinfo = applocaltime(&now);
    uint8_t min = timeinfo->tm_min;
    return min;
}
uint8_t currentDayOfMonth() {
    time_t now;
    time(&now);
    struct tm *timeinfo = applocaltime(&now);
    uint8_t dom = timeinfo->tm_mday;
    return dom;
}
void currentDateFull(char* buffer, size_t bufferSize) {
    time_t now;
    time(&now);
    struct tm * timeinfo = applocaltime(&now);
    strftime (buffer, bufferSize, "%Y-%m-%d %H:%M", timeinfo);
}
void currentDateShort(char* buffer, size_t bufferSize) {
    time_t now;
    time(&now);
    struct tm * timeinfo = applocaltime(&now);
    strftime (buffer, bufferSize, "%a, %d %b", timeinfo);
}
void currentHourMinute(char* buffer, size_t bufferSize) {
    time_t now;
    time(&now);
    struct tm * timeinfo = applocaltime(&now);
    strftime (buffer, bufferSize, "%I:%M", timeinfo);

    if (buffer[0] == '0')   // HH:MM format, Remove first 0 if necessary. e.g '08:03\0' -> '8:03\0'  (\0 = null char)
        memmove(buffer, buffer+1, 5);
}

String hourNumericToPretty(uint8_t hour) {
    return (hour > 12 ? String(hour - 12) + String(" pm") : String(hour) + String(" am")); 
}


bool inAContentPage(AppState& appState) {
    return appState == CommutePage || appState == TranspoPage || appState == NorthSouthPage || appState == EastWestPage || appState == WeatherPage;
}

bool inAContentPageOrWakingUp(AppState& appState) {
    return inAContentPage(appState) || appState == WakingUp;
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

    int hour = currentHourOfDay();
    int minute = currentMinute();
    Serial.print("      UTIL: AppState for " + String(hour) + ":" + String(minute));
    if(hour == APPSTATE_WAKING_UP_HOUR_START && minute >= APPSTATE_WAKING_UP_MINUTE_START) {
        Serial.println(" is WakingUp");
        appState = WakingUp;
    } else if (hour >= APPSTATE_COMMUTE_HOUR_START && hour < APPSTATE_NORMAL_HOUR_START) {
        Serial.println(" is CommutePage");
        appState = CommutePage;
    } else if (hour >= APPSTATE_NORMAL_HOUR_START && hour < APPSTATE_SLEEPING_HOUR_START) {
        // if(appState == NorthSouthPage) {
        //     Serial.println(" is EastWestPage");
        //     appState = EastWestPage;
        // } else if(appState == EastWestPage) {
        //     Serial.println(" is WeatherPage");
        //     appState = WeatherPage;
        // } else if(appState == WeatherPage) {
        //     Serial.println(" is NorthSouthPage");
        //     appState = NorthSouthPage;
        // }

        if(appState == TranspoPage) {
            Serial.println(" is WeatherPage");
            appState = WeatherPage;
        } else {
            Serial.println(" is TranspoPage");
            appState = TranspoPage;
        }
    } else if (hour == APPSTATE_SLEEPING_HOUR_START) {
        Serial.println(" is Sleeping");
        appState = Sleeping;
    } else {
        Serial.println(" is DeepSleeping");
        appState = DeepSleeping;
    }
}

void updateAppStateIfDifferentThan(AppState& currentAppState, AppState newAppState) {
    if(currentAppState != newAppState)
        currentAppState = newAppState;
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

String appStateToString(AppState appState) { 
    if(appState == Initializing) {
        return "Initializing";
    } else if(appState == WakingUp) {
        return "WakingUp";
    } else if(appState == CommutePage) {
        return "CommutePage";
    } else if(appState == TranspoPage) {
        return "TranspoPage";
    } else if(appState == NorthSouthPage) {
        return "NorthSouthPage";
    } else if(appState == EastWestPage) {
        return "EastWestPage";
    } else if(appState == WeatherPage) {
        return "WeatherPage";
    } else if(appState == Sleeping) {
        return "Sleeping";
    } else if(appState == DeepSleeping) {
        return "DeepSleeping";
    } else {
        Serial.println("     MODEL: ERROR appStateToString(). Restarting ESP");
        ESP.restart();
        return "";
    } 
}