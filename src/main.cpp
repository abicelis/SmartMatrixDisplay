#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <WiFiClientSecure.h>

#include <Config.h>
#include <Model.h>

#include <Hyst.h>
#include <OCTranspoAPI.h>
#include <MatrixDisplay.h>
#include <OpenMeteoAPI.h>

#include "util.h"

WiFiClientSecure wifiClient;
HTTPClient httpClient;
MatrixDisplay display;
OCTranspoAPI octranspoAPI(&wifiClient, &httpClient);
OpenMeteoAPI openMeteoAPI(&wifiClient, &httpClient);
Hyst hysteresis;

bool firstPage = true;
uint8_t loadingRecheckAttempt = 0;
AppState appState = Initializing;
AppPage appPage = NoPage;
TaskHandle_t taskHandle = NULL;
uint32_t currentMillis = 0;
uint32_t nextCheckMillis = 0;
uint32_t pageChangedMillis = 0;
uint32_t previousLightSensorUpdateMillis = 0;
uint32_t previousClockUpdateMillis = 0;
uint32_t previousPageBarUpdateMillis = 0;
RouteGroupData routeGroupData;
WeatherData weatherData;

void FetchRoutes(void *pvParameters) {
    Serial.println("FetchRoutes task started..");
    routeGroupData = octranspoAPI.fetchRouteGroup(appPageToRouteGroupType(appPage));

    if(routeGroupData.routeDestinations.size() == 0) {
        #ifdef DEBUG
        Serial.println("Warning: FetchRoutes task returned no trips!");
        #endif
        appState = NextPageErrorLoading;
    } else {
        #ifdef DEBUG
        printTrips(routeGroupData);
        #endif
        appState = NextPageLoaded;
    }

    if(firstPage)
        nextCheckMillis = currentMillis; //Force check now
    vTaskDelete(NULL);
}

void FetchWeather(void *pvParameters) {
    Serial.println("FetchWeather task started..");
    weatherData = openMeteoAPI.fetchCurrentWeather();
    appState = NextPageLoaded;

    if(!weatherData.setCorrectly) {
        #ifdef DEBUG
        Serial.println("Warning: FetchWeather task returned no data!");
        #endif
        appState = NextPageErrorLoading;
    } else {
        appState = NextPageLoaded;
    }

    if(firstPage)
        nextCheckMillis = currentMillis; //Force check now
    vTaskDelete(NULL);
}

void setup() {
    Serial.begin(115200);
    delay(300);
    Serial.println("\n\n     SMART LED MATRIX DISPLAY");
    Serial.println("-----------------------------------");
    Serial.println("-----------------------------------");
    
    // Display
    display.begin(R1_PIN, G1_PIN, B1_PIN, R2_PIN, G2_PIN, B2_PIN, A_PIN, B_PIN, C_PIN, D_PIN, E_PIN, LAT_PIN, OE_PIN, CLK_PIN, PANEL_RES_X, PANEL_RES_Y, PANEL_CHAIN);

    // Hysteresis
    hysteresis.begin(4096, LIGHT_SENSOR_VALUE_MAX, LIGHT_SENSOR_VALUE_MIN, 
                     LIGHT_SENSOR_SAMPLES, DISPLAY_HYSTERESIS_BRIGHTNESS_STEPS-2, DISPLAY_HYSTERESIS_GAP_SIZE);

    // WiFi
    display.drawText(28, 23, "Initializing");
    uint8_t loadingPercent = 10;
    display.drawLoadingBar(loadingPercent);
    WiFi.mode(WIFI_STA);
    WiFi.begin(SSID_NAME, SSID_PASSWORD);
    Serial.print("Connecting to WiFi..");

    while(WiFi.status() != WL_CONNECTED) {
        Serial.print(".");
        delay(200);
        if(loadingPercent <= 20)
            display.drawLoadingBar(++loadingPercent);
    }
    Serial.println("DONE.");
    Serial.print("  > Local IP: ");
    Serial.println(WiFi.localIP());

    // Configure HTTPClient and WiFiClient
    wifiClient.setInsecure();
    httpClient.useHTTP10(true);
    httpClient.setTimeout(5000);

    // Configure NTP
    loadingPercent = 30;
    display.drawLoadingBar(loadingPercent);
    Serial.print("Grabbing time from NTP..");
    configTime(NTP_GMT_OFFSET_SEC, NTP_DAYLIGHT_OFFSET_SEC, NTP_SERVER);
    time_t now;
    while (time(&now) < 1000) { // Wait until NTP request completes
        Serial.print(".");
        delay(200);
        if(loadingPercent <= 50)
            display.drawLoadingBar(++loadingPercent);
    }
    Serial.println("DONE.");
    char timeStringBuff[20];
    currentDateFull(timeStringBuff, sizeof(timeStringBuff));
    Serial.print("  > Time is ");
    Serial.println(timeStringBuff);
    Serial.println("--------------------------------------");
    Serial.println("\n\nSTARTING:");
    
    loadingPercent = 70;
    display.drawLoadingBar(loadingPercent);

    currentMillis = millis();
    nextCheckMillis = currentMillis;
}

void checkAppStateAndContinueFromThere();
void loop() {
    if(currentMillis - previousLightSensorUpdateMillis >= INTERVAL_UPDATE_LIGHT_SENSOR) {
        uint16_t newSample = analogRead(A0);
        uint16_t outValue = hysteresis.add(newSample);
        display.setBrightness(outValue);
        previousLightSensorUpdateMillis = currentMillis;
    }

    if(!firstPage && appPage != NoPage) {
        if(currentMillis - previousClockUpdateMillis >= INTERVAL_UPDATE_CLOCK) {
            char currentTime[6];
            currentHourMinute(currentTime, sizeof(currentTime));
            display.drawClock(currentTime);
            previousClockUpdateMillis = currentMillis;
        }

        if(currentMillis - previousPageBarUpdateMillis >= INTERVAL_UPDATE_PAGE_BAR) {
            uint32_t progressMillis = currentMillis - pageChangedMillis;
            float progressPercentage = (float)progressMillis/INTERVAL_PAGE_LIFETIME;
            display.drawPageBar(progressPercentage);
            previousPageBarUpdateMillis = currentMillis;
        }
    }


    if(currentMillis >= nextCheckMillis) {
        if(appState == Initializing) {
            checkAppStateAndContinueFromThere();
        } else if(appState == NextPageLoaded) {
            
            #ifdef DEBUG
            UBaseType_t uxHighWaterMark = uxTaskGetStackHighWaterMark(taskHandle);
            Serial.print( appPage == WeatherPage ? "FetchWeather" : "FetchRoutes");
            Serial.println(" task used this memory: " + String(uxHighWaterMark));
            #endif

            char currentTime[6];
            currentHourMinute(currentTime, sizeof(currentTime));
            char currentDate[20];
            currentDateShort(currentDate, sizeof(currentDate));
            if(appPage == WeatherPage)
                display.drawWeatherFor(weatherData, currentTime, currentDate);
            else
                display.drawBusScheduleFor(routeGroupData, appPageToRouteGroupType(appPage), currentTime);

            firstPage = false;
            loadingRecheckAttempt = 0;
            pageChangedMillis = currentMillis;
            checkAppStateAndContinueFromThere();

        } else if (appState == NextPageLoading) {
            // TODO, eventually either kill the task, or reset the ESP32 if we're stuck here.
            loadingRecheckAttempt++;

            if(loadingRecheckAttempt >= LOADING_RECHECK_ATTEMPTS) {
                Serial.println("Too many rechecks while loading a next page, restarting ESP.");
                ESP.restart();
            } else {
                nextCheckMillis = currentMillis + INTERVAL_PAGE_LOADING_RETRY;
            }
        } else if (appState == NextPageErrorLoading) {
            Serial.println("Error loading next page, restarting ESP.");
            ESP.restart();
        } else if (appState == Sleeping) {
            checkAppStateAndContinueFromThere();
        }
    }
        
    delay(10);                // Healthy sleep
    currentMillis = millis(); // Refresh for next loop
}

void checkAppStateAndContinueFromThere() {
    updateAppState(appState, appPage);
    Serial.println("AppState changed to '" + String(appState) + "'");
    if(appState == NextPageLoading) {
        Serial.println("Fetching AppPage '" + String(appPage) + "'");
        Serial.print("AVAILABLE HEAP MEMORY =");
        Serial.println(xPortGetFreeHeapSize());

        if(appPage != WeatherPage) {
            BaseType_t result = xTaskCreatePinnedToCore(FetchRoutes, "FetchRoutes", STACK_DEPTH_TRIPS_TASK, NULL, 1, &taskHandle, 0);
            if(result == pdPASS) {
                Serial.println("FetchRoutes task launched successfully!");
                nextCheckMillis = currentMillis + INTERVAL_PAGE_LIFETIME;
            } else if(result == errCOULD_NOT_ALLOCATE_REQUIRED_MEMORY) {
                Serial.print("ERROR: Task creation failed due to insufficient memory! HEAP MEM=");
                Serial.println(xPortGetFreeHeapSize());
            } else {
                Serial.print("ERROR: Task creation failed!!!");
            }
        } else {
            BaseType_t result = xTaskCreatePinnedToCore(FetchWeather, "FetchWeather", STACK_DEPTH_WEATHER_TASK, NULL, 1, &taskHandle, 0);
            if(result == pdPASS) {
                Serial.println("FetchWeather task launched successfully!");
                nextCheckMillis = currentMillis + INTERVAL_PAGE_LIFETIME;

            } else if(result == errCOULD_NOT_ALLOCATE_REQUIRED_MEMORY) {
                Serial.print("ERROR: Task creation failed due to insufficient memory! HEAP MEM=");
                Serial.println(xPortGetFreeHeapSize());
            } else {
                Serial.print("ERROR: Task creation failed!!!");
            }
        }
    } else if(appState == Sleeping) {
        Serial.println("Sleeping now");
        // TODO - Something more interesting here? Maybe a cool animation?
        display.clearScreen();
        display.drawText(48, 30, "Zzzz..");

        // TODO Stay in this state for WAY longer, 
        // TODO But make sure to set nextCheckMillis so that we immediately wake up when we need to
        // At APPSTATE_SLEEPING_HOUR_END
        // For now, we sleep for INTERVAL_PAGE_LIFETIME then re-check and re-check... 
        nextCheckMillis = currentMillis + INTERVAL_PAGE_LIFETIME;
    }
}