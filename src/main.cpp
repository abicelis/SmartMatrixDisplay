#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <WiFiClientSecure.h>
#include <Preferences.h>

#include <Config.h>
#include <Model.h>

#include <Hysteresis.h>
#include <OCTranspoAPI.h>
#include <MatrixDisplay.h>
#include <OpenMeteoAPI.h>
#include <Util.h>

#define buttonPin 33

Routes routes;
WeatherData weatherData;

Preferences preferences;
WiFiClientSecure wifiClient;
HTTPClient httpClient;
MatrixDisplay display;
OCTranspoAPI octranspoAPI(&wifiClient, &httpClient, &routes);
OpenMeteoAPI openMeteoAPI(&wifiClient, &httpClient);
Hysteresis hysteresis;

unsigned long lastButtonInterruptMillis = 0;
AppState appState = Initializing;

uint32_t currentMillis = 0;
volatile uint32_t nextPageMillis = 0;
uint32_t pageChangedMillis = 0;
uint32_t previousLightSensorUpdateMillis = 0;
uint32_t previousClockUpdateMillis = 0;
uint32_t previousPageBarUpdateMillis = 0;

void IRAM_ATTR buttonInterrupt() {
  unsigned long interruptMillis = millis();
  if (interruptMillis - lastButtonInterruptMillis > 500) {
    Serial.println("   MAIN: Button pressed! Changing page");
    lastButtonInterruptMillis = interruptMillis;
    nextPageMillis = currentMillis;
  }
}

void setup() {
    randomSeed(analogRead(0)); // Seed pseudorandom random()
    Serial.begin(115200);
    vTaskDelay(pdMS_TO_TICKS(300));

    Serial.print("\n\n\n\n\n\n");
    Serial.println("     SMART LED MATRIX DISPLAY");
    Serial.println("-----------------------------------");
    Serial.println("-----------------------------------");

    // Display
    display.begin(R1_PIN, G1_PIN, B1_PIN, R2_PIN, G2_PIN, B2_PIN, A_PIN, B_PIN, C_PIN, D_PIN, E_PIN, LAT_PIN, OE_PIN, CLK_PIN, PANEL_RES_X, PANEL_RES_Y, PANEL_CHAIN);
    // display.testDrawWeatherIcons(); while(true) {}

    // Hysteresis
    hysteresis.begin(4096, LIGHT_SENSOR_VALUE_MAX, LIGHT_SENSOR_VALUE_MIN, 
                     LIGHT_SENSOR_SAMPLES, DISPLAY_HYSTERESIS_BRIGHTNESS_STEPS-2, DISPLAY_HYSTERESIS_GAP_SIZE);

    // 'Next screen' button interrupt
    pinMode(buttonPin, INPUT);
    attachInterrupt(digitalPinToInterrupt(buttonPin), buttonInterrupt, RISING);

    // WiFi
    uint8_t loadingPercent = 10;
    display.drawInitializationPage(loadingPercent);
    WiFi.mode(WIFI_STA);
    WiFi.begin(SSID_NAME, SSID_PASSWORD);
    Serial.print("  SETUP: Connecting to WiFi..");

    while(WiFi.status() != WL_CONNECTED) {
        Serial.print(".");
        vTaskDelay(pdMS_TO_TICKS(200));
        if(loadingPercent <= 20)
            display.drawInitializationPage(++loadingPercent);
    }
    Serial.println("DONE.");
    Serial.print("  SETUP:   > Local IP: ");
    Serial.println(WiFi.localIP());

    // Configure HTTPClient and WiFiClient
    wifiClient.setInsecure();
    httpClient.useHTTP10(true);
    httpClient.setTimeout(5000);

    // Configure NTP
    loadingPercent = 30;
    display.drawInitializationPage(loadingPercent);
    Serial.print("  SETUP: Grabbing time from NTP..");
    configTime(NTP_GMT_OFFSET_SEC, NTP_DAYLIGHT_OFFSET_SEC, NTP_SERVER);
    time_t now;
    while (time(&now) < 1000) { // Wait until NTP request completes
        Serial.print(".");
        vTaskDelay(pdMS_TO_TICKS(200));
        if(loadingPercent <= 50)
            display.drawInitializationPage(++loadingPercent);
    }
    Serial.println("DONE.");
    char timeStringBuff[20];
    currentDateFull(timeStringBuff, sizeof(timeStringBuff));
    Serial.print("  SETUP:   > Time is ");
    Serial.println(timeStringBuff);
    
    // Load Weather data from flash.
    Serial.print("  SETUP: Attempting to load WeatherData from flash..");
    preferences.begin("app", false);
    // preferences.clear();
    weatherData.tryLoadWeatherDataFromFlash(preferences);
    Serial.println("DONE.");
    if(weatherData.times.size() > 0) {
        Serial.println("  SETUP:   > WeatherData loaded successfully");
    } else {
        Serial.println("  SETUP:   > WeatherData NOT loaded");
    }
    loadingPercent = 70;
    display.drawInitializationPage(loadingPercent);

    // Fetch initial data
    Serial.println("  SETUP: Fetching initial data..");
    octranspoAPI.fetchRoutes();
    loadingPercent = 90;
    display.drawInitializationPage(loadingPercent);
    Serial.println("DONE.");

    // START
    Serial.println("\n\n              MAIN");
    Serial.println("-----------------------------------");
    currentMillis = millis();
    nextPageMillis = currentMillis;
    octranspoAPI.startFetchRoutesTask();
}

void loop() {
    if(currentMillis - previousLightSensorUpdateMillis >= INTERVAL_UPDATE_LIGHT_SENSOR) {
        uint16_t newSample = analogRead(A0);
        uint16_t outValue = hysteresis.add(newSample);
        display.setBrightness(outValue);
        previousLightSensorUpdateMillis = currentMillis;
    }

    if(inAContentPage(appState)) {
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

    if(currentMillis >= nextPageMillis) {
        updateAppState(appState);
        if (appState == Sleeping) {
            Serial.println("   MAIN: Change to Sleeping");
            display.drawSleepingPage();
            nextPageMillis = currentMillis + 3600000; // Sleep for 1 hour
        } else if (appState == DeepSleeping) {
            Serial.println("   MAIN: Change to DEEP Sleeping");
            display.clearScreen();
            // TODO Stay in this state for WAY longer, 
            // TODO But make sure to set nextPageMillis so that we immediately wake up when we need to
            // At APPSTATE_DEEP_SLEEPING_HOUR_END
            // For now, we sleep for INTERVAL_PAGE_LIFETIME then re-check and re-check... 
            // also consider shutting down wifi and other things in this state..?
            nextPageMillis = currentMillis + INTERVAL_PAGE_LIFETIME;
        } else {
            char currentTime[6];
            currentHourMinute(currentTime, sizeof(currentTime));

            if(appState == CommutePage) {
                Serial.println("   MAIN: Change to CommutePage");
                // std::vector<UITrip> uiTrips = routes.getSortedUITripsForCommute();
                // display.drawCommutePage(routes, weatherData, currentTime);
            } else if(appState == WeatherPage) {
                Serial.println("   MAIN: Change to CommutePage");
                // display.drawWeatherPage(weatherData, currentTime);
            } else if(appState == NorthSouthPage) {
                Serial.println("   MAIN: Change to NorthSouthPage");
                std::vector<UITrip> uiTrips = routes.getSortedUITripsByDirection(NorthSouth);
                display.drawTripsPage(uiTrips, appState, currentTime);
            }  else if(appState == EastWestPage) {
                Serial.println("   MAIN: Change to EastWestPage");
                std::vector<UITrip> uiTrips = routes.getSortedUITripsByDirection(EastWest);
                display.drawTripsPage(uiTrips, appState, currentTime);
            }
        
            pageChangedMillis = currentMillis;
            nextPageMillis = currentMillis + INTERVAL_PAGE_LIFETIME;
        }
    }
        
    vTaskDelay(pdMS_TO_TICKS(10));  // Healthy sleep
    currentMillis = millis();       // Refresh for next loop
}