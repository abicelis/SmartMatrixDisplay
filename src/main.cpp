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
unsigned int lightSensorValue = 0;
unsigned long currentMillis = 0;
unsigned long previousAppStateChangeMillis = 0;
unsigned long previousTrackingBusIndicatorMillis = 0;
unsigned long previousLightSensorUpdateMillis = 0;
TaskHandle_t fetchTripsTaskHandle = NULL;

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
        previousLightSensorUpdateMillis = currentMillis;

        // Update LDR sensor value
        lightSensorValue = analogRead(A0);
        lightSensorValue = map(lightSensorValue, 0, 4095, 12, 255);
        Serial.println("Light sensor value " + String(lightSensorValue));
    }

    if(currentMillis == 0 || currentMillis - previousAppStateChangeMillis >= INTERVAL_APP_STATE) {
        previousAppStateChangeMillis = currentMillis;
        updateAppState(appState);
        Serial.println("AppState changed to '" + String(appState) + "'");

        if(appState == Sleeping) {
            Serial.println("Sleeping now");

            // TODO - Something more interesting here? Maybe a cool animation?
            display.clearScreen();
            display.drawText(0, 0, "Sleeping");
        } else if(appState != Weather) {
            Serial.println("Launching FetchTrips task");
            xTaskCreatePinnedToCore(FetchTrips, "FetchTrips", STACK_DPETH_TRIPS_TASK, NULL, 1, &fetchTripsTaskHandle, 0);
        } else {
            Serial.println("Weather mode now");
            // TODO - Weather
            display.clearScreen();
            display.drawText(0,0,"Weather!");
        }
    }

    // Evaluate if FetchTrips task is done
    if(newTripsFetched) {
        #ifdef DEBUG
        UBaseType_t uxHighWaterMark = uxTaskGetStackHighWaterMark(fetchTripsTaskHandle);
        Serial.println("FetchTrips stack used this memory: " + String(uxHighWaterMark));
        #endif

        newTripsFetched = false;
        char currentTime[6];
        currentHourMinute(currentTime, 6);
        display.drawBusScheduleFor(newTrips, appStateToTripsType(appState), currentTime);
    }
    
    currentMillis = millis(); // Refresh for next loop
}