#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <WiFiClientSecure.h>

#include <Config.h>
#include <Model.h>

#include <OCTranspoAPI.h>
#include <MatrixDisplay.h>
#include <OpenMeteoAPI.h>

#include "util.h"

WiFiClientSecure wifiClient;
HTTPClient httpClient;
MatrixDisplay display;
OCTranspoAPI octranspoAPI(&wifiClient, &httpClient);
OpenMeteoAPI openMeteoAPI(&wifiClient, &httpClient);

AppState appState = Weather;
uint16_t lightSensorValue = 0;
uint32_t currentMillis = 0;
uint32_t previousAppStateChangeMillis = 0;
uint32_t previousTrackingBusIndicatorMillis = 0;
uint32_t previousLightSensorUpdateMillis = 0;
TaskHandle_t fetchTripsTaskHandle = NULL;
TaskHandle_t fetchWeatherTaskHandle = NULL;

TripsData newTrips;
bool newTripsFetched = false;
void FetchTrips(void *pvParameters) {
    Serial.println("FetchTrips task started..");
    newTrips = octranspoAPI.fetchNextTrips(appStateToTripsType(appState));

    #ifdef DEBUG
    printTrips(newTrips);
    #endif
    
    newTripsFetched = true;
    vTaskDelete(NULL);
}

WeatherData newWeather;
bool newWeatherFetched = false;
void FetchWeather(void *pvParameters) {
    Serial.println("FetchWeather task started..");
    newWeather = openMeteoAPI.fetchCurrentWeather();
    newWeatherFetched = true;
    vTaskDelete(NULL);
}

void setup() {
    Serial.begin(115200);
    delay(300);
    Serial.println("\nSETUP");
    Serial.println("-----------------------------------");

    // Display
    display.begin(R1_PIN, G1_PIN, B1_PIN, R2_PIN, G2_PIN, B2_PIN, A_PIN, B_PIN, C_PIN, D_PIN, E_PIN, LAT_PIN, OE_PIN, CLK_PIN, PANEL_RES_X, PANEL_RES_Y, PANEL_CHAIN);

    // WiFi
    display.drawText(0, 0, "Init Wi-Fi");
    WiFi.mode(WIFI_STA);
    WiFi.begin(SSID_NAME, SSID_PASSWORD);
    Serial.print("Connecting to WiFi..");

    while(WiFi.status() != WL_CONNECTED) {
        Serial.print(".");
        delay(100);
    }
    Serial.println("DONE.");
    Serial.print("Local IP: ");
    Serial.println(WiFi.localIP());
    display.clearScreen();

    // Configure HTTPClient and WiFiClient
    wifiClient.setInsecure();
    httpClient.useHTTP10(true);
    httpClient.setTimeout(5000);

    // Configure NTP
    display.drawText(0, 0, "Init NTP");
    Serial.print("Grabbing time from NTP..");
    configTime(NTP_GMT_OFFSET_SEC, NTP_DAYLIGHT_OFFSET_SEC, NTP_SERVER);
    time_t now;
    while (time(&now) < 1000) { // Wait until NTP request completes
        Serial.print(".");
        delay(100);
    }
    Serial.println("DONE.");
    Serial.println("Time is " + String(currentTimePretty()));
    Serial.println("--------------------------------------\n");
    Serial.println("\nSTARTING:");
    display.clearScreen();
}

void loop() {
    if(currentMillis == 0 || currentMillis - previousLightSensorUpdateMillis >= INTERVAL_UPDATE_LIGHT_SENSOR) {
        // Update LDR sensor value
        lightSensorValue = analogRead(A0);
        // Serial.print("Light sensor value ");
        // Serial.println(lightSensorValue);
        display.setBrightness(lightSensorToDisplayBrightness(lightSensorValue));
        previousLightSensorUpdateMillis = currentMillis;
    }

    if(currentMillis == 0 || currentMillis - previousAppStateChangeMillis >= INTERVAL_APP_STATE) {
        updateAppState(appState, lightSensorValue);
        Serial.println("AppState changed to '" + String(appState) + "'");
        Serial.print("AVAILABLE HEAP MEMORY =");
        Serial.println(xPortGetFreeHeapSize());

        if(appState == Sleeping) {
            Serial.println("Sleeping now");
            // TODO - Something more interesting here? Maybe a cool animation?
            display.clearScreen();
            display.drawText(48, 30, "Zzzz..");
        } else if(appState != Weather) {
            Serial.println("Launching FetchTrips task");
            BaseType_t result = xTaskCreatePinnedToCore(FetchTrips, "FetchTrips", STACK_DEPTH_TRIPS_TASK, NULL, 1, &fetchTripsTaskHandle, 0);
            if(result == errCOULD_NOT_ALLOCATE_REQUIRED_MEMORY) {
                Serial.print("ERROR: Task creation failed due to insufficient memory! HEAP MEM=");
                Serial.println(xPortGetFreeHeapSize());
            }
        } else {
            Serial.println("Launching FetchWeather task");
            BaseType_t result = xTaskCreatePinnedToCore(FetchWeather, "FetchWeather", STACK_DEPTH_WEATHER_TASK, NULL, 1, &fetchWeatherTaskHandle, 0);
            if(result == errCOULD_NOT_ALLOCATE_REQUIRED_MEMORY) {
                Serial.print("ERROR: Task creation failed due to insufficient memory! HEAP MEM=");
                Serial.println(xPortGetFreeHeapSize());
            }
        }
        previousAppStateChangeMillis = currentMillis;
    }

    // Evaluate if FetchTrips task is done
    if(newTripsFetched) {
        #ifdef DEBUG
        UBaseType_t uxHighWaterMark = uxTaskGetStackHighWaterMark(fetchTripsTaskHandle);
        Serial.println("FetchTrips stack used this memory: " + String(uxHighWaterMark));
        #endif

        newTripsFetched = false;
        char currentHHMM[6];
        currentHourMinute(currentHHMM, 6);
        display.drawBusScheduleFor(newTrips, appStateToTripsType(appState), currentHHMM);
    }

    // Evaluate if FetchTrips task is done
    if(newWeatherFetched) {
        #ifdef DEBUG
        UBaseType_t uxHighWaterMark = uxTaskGetStackHighWaterMark(fetchWeatherTaskHandle);
        Serial.println("FetchWeather stack used this memory: " + String(uxHighWaterMark));
        #endif

        newWeatherFetched = false;
        char currentHHMM[6];
        currentHourMinute(currentHHMM, 6);
        display.drawWeatherFor(newWeather, currentHHMM);
    }
    
    currentMillis = millis(); // Refresh for next loop
}