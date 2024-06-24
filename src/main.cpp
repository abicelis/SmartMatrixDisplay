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

Preferences preferences;
WiFiClientSecure wifiClient;
HTTPClient httpClient;
MatrixDisplay display;
OCTranspoAPI octranspoAPI(&wifiClient, &httpClient);
OpenMeteoAPI openMeteoAPI(&wifiClient, &httpClient);
Hysteresis hysteresis;

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
    Serial.println("FetchRoutes task - Started");
    routeGroupData = octranspoAPI.fetchRouteGroup(appPageToRouteGroupType(appPage));

    if(routeGroupData.routeDestinations.size() == 0) {
        Serial.println("FetchRoutes task WARNING - fetchRouteGroup() returned no trips!");
        appState = NextPageErrorLoading;
    } else {
        printTrips(routeGroupData);
        appState = NextPageLoaded;
    }

    if(firstPage)
        nextCheckMillis = currentMillis; // Force check now
    Serial.println("FetchRoutes task - Finished");
    printHighWaterMarkForTask(taskHandle);
    vTaskDelete(NULL);
}

void FetchWeather(void *pvParameters) {
    Serial.println("FetchWeather task - Started");

    if(weatherData.isStaleOrInvalid(ThreeHourForecast)) {
        // Re-fetch weather data
         openMeteoAPI.fetchWeatherData(weatherData, currentHourOfDay(), false);
     
        if(weatherData.isStaleOrInvalid(ThreeHourForecast)) {
            Serial.println("FetchWeather task WARNING - fetchWeatherData() returned no data!");
            appState = NextPageErrorLoading;
        } else {
            Serial.println("FetchWeather task - Saving weatherData to FLASH");
            weatherData.saveWeatherDataToFlash(preferences);
            appState = NextPageLoaded;
        }
    } else {
        Serial.println("FetchWeather task - WeatherData is good still.");
        appState = NextPageLoaded;
    }

    if(firstPage)
        nextCheckMillis = currentMillis; // Force check now
    Serial.println("FetchWeather task - Finished");
    printHighWaterMarkForTask(taskHandle);
    vTaskDelete(NULL);
}

void FetchCommute(void *pvParameters) {
    Serial.println("FetchCommute task - Started");

    if(weatherData.isStaleOrInvalid(VickyCommuteForecast)) {
        // Re-fetch weather data
        openMeteoAPI.fetchWeatherData(weatherData, currentHourOfDay(), true);

        if(weatherData.isStaleOrInvalid(VickyCommuteForecast)) {
            Serial.println("FetchCommute task WARNING - fetchWeatherData() returned no data!");
            appState = NextPageErrorLoading;
        } else {
            Serial.println("FetchCommute task - Saving weatherData to FLASH");
            weatherData.saveWeatherDataToFlash(preferences);
            appState = NextPageLoaded;
        }
    } else {
        Serial.println("FetchCommute task - WeatherData is good still.");
        appState = NextPageLoaded;
    }

    if(appState == NextPageLoaded) {
        routeGroupData = octranspoAPI.fetchRouteGroup(VickyCommute);
        if(routeGroupData.routeDestinations.size() == 0) {
            Serial.println("FetchCommute task WARNING - fetchRouteGroup() returned no trips!");
            appState = NextPageErrorLoading;
        } else {
            printTrips(routeGroupData);
        }
    }

    if(firstPage)
        nextCheckMillis = currentMillis; // Force check now
    Serial.println("FetchCommute task - Finished");
    printHighWaterMarkForTask(taskHandle);
    vTaskDelete(NULL);
}

void checkAppStateAndContinueFromThere() {
    updateAppState(appState, appPage);
    if(appState == NextPageLoading) {
        // Serial.println("Fetching AppPage '" + String(appPage) + "'");
        printAvailableHeapMemory();

        if(appPage == VickyCommutePage) {
            BaseType_t result = xTaskCreatePinnedToCore(FetchCommute, "FetchCommute", STACK_DEPTH_FETCH_COMMUTE_TASK, NULL, 1, &taskHandle, 0);
            if(result == pdPASS) {
                Serial.println("FetchCommute task launched");
                nextCheckMillis = currentMillis + INTERVAL_PAGE_LIFETIME;
            } else if(result == errCOULD_NOT_ALLOCATE_REQUIRED_MEMORY) {
                Serial.print("ERROR: Task creation failed due to insufficient memory! HEAP MEM=");
                Serial.println(xPortGetFreeHeapSize());
            } else {
                Serial.print("ERROR: Task creation failed!!!");
            }
        } else if(appPage == NorthSouthPage || appPage == EastWestPage) {
            BaseType_t result = xTaskCreatePinnedToCore(FetchRoutes, "FetchRoutes", STACK_DEPTH_FETCH_ROUTES_TASK, NULL, 1, &taskHandle, 0);
            if(result == pdPASS) {
                Serial.println("FetchRoutes task launched");
                nextCheckMillis = currentMillis + INTERVAL_PAGE_LIFETIME;
            } else if(result == errCOULD_NOT_ALLOCATE_REQUIRED_MEMORY) {
                Serial.print("ERROR: Task creation failed due to insufficient memory! HEAP MEM=");
                Serial.println(xPortGetFreeHeapSize());
            } else {
                Serial.print("ERROR: Task creation failed!!!");
            }
        } else { // WeatherPage
            BaseType_t result = xTaskCreatePinnedToCore(FetchWeather, "FetchWeather", STACK_DEPTH_FETCH_WEATHER_TASK, NULL, 1, &taskHandle, 0);
            if(result == pdPASS) {
                Serial.println("FetchWeather task launched");
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
        display.drawSleepingPage();
        nextCheckMillis = currentMillis + 3600000; // 1 hour
    } else if(appState == DeepSleeping) {
        Serial.println("DEEP Sleeping now");
        display.clearScreen();

        // TODO Stay in this state for WAY longer, 
        // TODO But make sure to set nextCheckMillis so that we immediately wake up when we need to
        // At APPSTATE_DEEP_SLEEPING_HOUR_END
        // For now, we sleep for INTERVAL_PAGE_LIFETIME then re-check and re-check... 
        // also consider shutting down wifi and other things in this state..?
        nextCheckMillis = currentMillis + INTERVAL_PAGE_LIFETIME;
    }

}

void setup() {
    randomSeed(analogRead(0)); // Seed pseudorandom random()

    Serial.begin(115200);
    delay(300);
    Serial.println("\n\n     SMART LED MATRIX DISPLAY");
    Serial.println("-----------------------------------");
    Serial.println("-----------------------------------");

    // Display
    display.begin(R1_PIN, G1_PIN, B1_PIN, R2_PIN, G2_PIN, B2_PIN, A_PIN, B_PIN, C_PIN, D_PIN, E_PIN, LAT_PIN, OE_PIN, CLK_PIN, PANEL_RES_X, PANEL_RES_Y, PANEL_CHAIN);
    // display.testDrawWeatherIcons(); while(true) {}

    // Hysteresis
    hysteresis.begin(4096, LIGHT_SENSOR_VALUE_MAX, LIGHT_SENSOR_VALUE_MIN, 
                     LIGHT_SENSOR_SAMPLES, DISPLAY_HYSTERESIS_BRIGHTNESS_STEPS-2, DISPLAY_HYSTERESIS_GAP_SIZE);

    // WiFi
    uint8_t loadingPercent = 10;
    display.drawInitializationPage(loadingPercent);
    WiFi.mode(WIFI_STA);
    WiFi.begin(SSID_NAME, SSID_PASSWORD);
    Serial.print("Connecting to WiFi..");

    while(WiFi.status() != WL_CONNECTED) {
        Serial.print(".");
        delay(200);
        if(loadingPercent <= 20)
            display.drawInitializationPage(++loadingPercent);
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
    display.drawInitializationPage(loadingPercent);
    Serial.print("Grabbing time from NTP..");
    configTime(NTP_GMT_OFFSET_SEC, NTP_DAYLIGHT_OFFSET_SEC, NTP_SERVER);
    time_t now;
    while (time(&now) < 1000) { // Wait until NTP request completes
        Serial.print(".");
        delay(200);
        if(loadingPercent <= 50)
            display.drawInitializationPage(++loadingPercent);
    }
    Serial.println("DONE.");
    char timeStringBuff[20];
    currentDateFull(timeStringBuff, sizeof(timeStringBuff));
    Serial.print("  > Time is ");
    Serial.println(timeStringBuff);
    
    // Load Weather data from flash, if not stale.
    Serial.print("Grabbing WeatherData from flash..");
    preferences.begin("app", false);
    // preferences.clear();
    weatherData.tryLoadWeatherDataFromFlash(preferences);
    Serial.println("DONE.");
    if(weatherData.times.size() > 0) {
        Serial.println("  > WeatherData loaded successfully");
    } else {
        Serial.println("  > WeatherData NOT loaded");
    }
    
    Serial.println("--------------------------------------");
    Serial.println("\n\nSTARTING:");
    
    loadingPercent = 70;
    display.drawInitializationPage(loadingPercent);

    currentMillis = millis();
    nextCheckMillis = currentMillis;
}

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
            char currentTime[6];
            currentHourMinute(currentTime, sizeof(currentTime));
            char currentDate[20];
            currentDateShort(currentDate, sizeof(currentDate));
            if(appPage == WeatherPage)
                display.drawWeatherPage(weatherData, currentTime, currentDate);
            else {
                if(appPage == VickyCommutePage)
                    display.drawVickyCommutePage(routeGroupData, weatherData, currentTime);
                else
                    display.drawBusSchedulePage(routeGroupData, appPageToRouteGroupType(appPage), currentTime);
            }
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
        } else if (appState == Sleeping || appState == DeepSleeping) {
            checkAppStateAndContinueFromThere();
        }
    }
        
    delay(10);                // Healthy sleep
    currentMillis = millis(); // Refresh for next loop
}