#include <ESP32-HUB75-MatrixPanel-I2S-DMA.h>
#include <Fonts/FreeSans9pt7b.h>
#include <Model.h>
#include <Config.h>
#include <Util.h>
#include "MatrixDisplay.h"
#include "images/images.h"
#include "images/icons1BitPerPixel.h"
#include "images/icons2BytePerPixel565.h"
#include "fonts/Font5x5Fixed.h"
#include "fonts/Font5x7Fixed.h"

void MatrixDisplay::begin(int8_t r1_pin, int8_t g1_pin, int8_t b1_pin, int8_t r2_pin, int8_t g2_pin, int8_t b2_pin,
               int8_t a_pin, int8_t b_pin, int8_t c_pin, int8_t d_pin, int8_t e_pin, int8_t lat_pin, int8_t oe_pin,
               int8_t clk_pin, uint16_t panel_res_x, uint16_t panel_res_y, int8_t panel_chain) {

    HUB75_I2S_CFG::i2s_pins _pins={r1_pin, g1_pin, b1_pin, r2_pin, g2_pin, b2_pin, a_pin, 
                                b_pin, c_pin, d_pin, e_pin, lat_pin, oe_pin, clk_pin};
    HUB75_I2S_CFG mxConfig(panel_res_x, panel_res_y, panel_chain, _pins);

    _dma_display = new MatrixPanel_I2S_DMA(mxConfig);
    _dma_display->begin();
    _dma_display->setTextSize(1);
    _dma_display->setTextWrap(false);
    _dma_display->setBrightness8(_panelBrightness);
    _dma_display->clearScreen();
    _dma_display->fillScreen(_colorBlack);


    xTaskCreatePinnedToCore(BrightnessTaskFunction, "BrightnessTaskFunction", 
        STACK_DEPTH_BRIGHTNESS_TASK, this, 1, &brightnessTaskHandle, 1);
}

void MatrixDisplay::drawInitializationPage(uint8_t loadingBarWidthPixels) {
    clearScreen();
    drawCenteredText(INITIALIZATION_PAGE_TITLE_POSITION_X, INITIALIZATION_PAGE_TITLE_POSITION_Y, "Initializing");

    _dma_display->drawRoundRect(14, INITIALIZATION_PAGE_BAR_Y_POSITION_PX, 100, 
    INITIALIZATION_PAGE_BAR_THICKNESS_PX, INITIALIZATION_PAGE_BAR_CORNER_RADIUS_PX, _colorTextPrimary);

    _dma_display->fillRoundRect(14, INITIALIZATION_PAGE_BAR_Y_POSITION_PX, loadingBarWidthPixels, 
    INITIALIZATION_PAGE_BAR_THICKNESS_PX, INITIALIZATION_PAGE_BAR_CORNER_RADIUS_PX, _colorTextPrimary);
}

void MatrixDisplay::drawVickyCommutePage(RouteGroupData& data, WeatherData& weatherData, const char* currentTime) {
    fadeOutScreen();
    clearScreen();

    // Draw icon
    _dma_display->drawRGBBitmap(SCHEDULE_ICON_X_POSITION, SCHEDULE_ICON_Y_POSITION, icon_OCTranspoLogo, 9, 9);

    // Draw Title
    const char* title = "Vicky";
    drawText(SCHEDULE_TITLE_X_POSITION, SCHEDULE_TITLE_Y_POSITION, title);        
    _dma_display->setCursor(_dma_display->getCursorX()+2, _dma_display->getCursorY()-1);
    _dma_display->setFont(); // Use default font
    _dma_display->write(0x03);

    // Draw clock
    drawTextEnd(PAGE_CLOCK_X_POSITION, PAGE_CLOCK_Y_POSITION, currentTime, _colorTextPrimary, &Font5x7Fixed);

    // Draw Horizontal rule
    _dma_display->drawFastHLine(PAGE_HORIZONTAL_MARGIN_PX, PAGE_TOP_HEADER_SIZE_PX-4,
        PANEL_RES_X - PAGE_HORIZONTAL_MARGIN_PX*2, _colorTextSecondary);

    uint8_t signWidth = SCHEDULE_BUS_SIGN_WIDTH_PX;
    uint8_t row = PAGE_TOP_HEADER_SIZE_PX+8;

    _trackingBusIndicatorPositions.clear();

    if(data.routeDestinations.size() >= 1) {  // There should be only one route -> 88 Hurdman.
        const auto &destination = data.routeDestinations[0]; 

        // Draw LTR. RouteNumber, then RouteLabel
        drawRouteSign(destination.routeType, PAGE_HORIZONTAL_MARGIN_PX, row-8, SCHEDULE_BUS_SIGN_WIDTH_PX, destination.routeNumber.c_str());
        drawText(PAGE_HORIZONTAL_MARGIN_PX + SCHEDULE_BUS_SIGN_WIDTH_PX + SCHEDULE_BUS_SIGN_AND_BUS_LABEL_MARGIN_PX, row, shortenRouteDestination(destination.routeDestination).c_str());

        // Foreach trip: 
        uint8_t xPos = PANEL_RES_X - 1 - PAGE_HORIZONTAL_MARGIN_PX;
        int8_t j = SCHEDULE_VICKY_COMMUTE_MAX_TRIPS - 1;
        if(destination.trips.size() < j+1)
            j = destination.trips.size() - 1;

        while(j >= 0) {  
            const auto &trip = destination.trips[j];

            //Draw RTL. MinuteSymbol, then ArrivalTime
            drawMinuteSymbol(xPos, row-7);
            String arrivalStr = String(trip.arrivalTime);
            const char* arrivalCStr = arrivalStr.c_str();
            uint8_t arrivalEndXPos = xPos - SCHEDULE_BUS_MINUTE_SYMBOL_AND_ARRIVAL_TIME_MARGIN_PX;
            uint8_t arrivalStartXPos = getRightAlignStartingPoint(arrivalCStr, arrivalEndXPos);
            drawText(arrivalStartXPos, row, arrivalCStr);

            if(trip.arrivalIsEstimated) {
                uint8_t trackingIndicatorXPos = arrivalStartXPos - SCHEDULE_BUS_ARRIVAL_TIME_AND_TRACKING_INDICATOR_MARGIN_PX;
                _trackingBusIndicatorPositions.push_back(std::make_tuple(trackingIndicatorXPos, row-7, trip.busLocation == FarAwayEnough));
            }

            xPos -= SCHEDULE_VICKY_COMMUTE_BETWEEN_TRIPS_SPACING_PX;
            j--;
        }
    }

    // Weather section
    // Draw lines
    _dma_display->drawFastHLine(PAGE_HORIZONTAL_MARGIN_PX, SCHEDULE_VICKY_COMMUTE_HR_HEIGHT,
        PANEL_RES_X - PAGE_HORIZONTAL_MARGIN_PX*2, _colorTextSecondary);
    _dma_display->drawFastVLine(64, SCHEDULE_VICKY_COMMUTE_HR_HEIGHT+3, SCHEDULE_VICKY_COMMUTE_VR_LENGTH, _colorTextSecondary);
    

    if(weatherData.times.size() == 2) {  // Should be 2 items here
        uint8_t xPos = 0;
        for (size_t i = 0; i < 2; ++i) {
            String hourPretty = hourNumericToPretty(weatherData.times.at(i));
            drawText(xPos + VICKY_COMMUTE_PAGE_TIME_LEFT_SPACING_X, VICKY_COMMUTE_PAGE_TIME_Y, hourPretty.c_str(), _colorTextPrimary, &Font5x5Fixed);
            
            const char* weatherImage = weatherTypeToImage(weatherData.weatherType.at(i), weatherData.isDaytime.at(i));
            if(weatherImage != NULL)
                drawASCWWImage(xPos+VICKY_COMMUTE_PAGE_ICON_LEFT_SPACING_X-12, VICKY_COMMUTE_PAGE_ICON_Y, 25, 25, weatherImage);
            else {
                Serial.print("Warning: weatherTypeToImage() returned NULL for weatherType= ");
                Serial.print(weatherData.weatherType.at(i));
                Serial.print(" daytime=");
                Serial.println(weatherData.weatherType.at(i));
            }

            drawCenteredText(xPos+VICKY_COMMUTE_PAGE_ICON_LEFT_SPACING_X, VICKY_COMMUTE_PAGE_APPARENT_TEMP_Y, weatherData.apparentTemperatureCelcius.at(i).c_str(), _colorTextPrimary, &Font5x5Fixed);
            

            xPos=xPos+64;
        }
    } else {
        Serial.print("Warning: Displaying VickyCommute, wrong number of weatherData items ");
        Serial.println(weatherData.times.size());
    }
    fadeInScreen();

    if(_trackingBusIndicatorPositions.size() > 0) {
        xTaskCreatePinnedToCore(TrackingBusIndicatorTaskFunction, "TrackingBusIndicatorTask", 
            STACK_DEPTH_TRACKING_BUS_INDICATORS_TASK, this, 1, &trackingBusIndicatorTaskHandle, 1);
        // Serial.println("TrackingBusIndicatorTask LAUNCHED");
    }
}

void MatrixDisplay::drawBusSchedulePage(RouteGroupData& data, RouteGroupType tripsType, const char* currentTime) {
    fadeOutScreen();
    clearScreen();

    //Draw icon
    // _dma_display->drawBitmap(SCHEDULE_BUS_X_POSITION, SCHEDULE_BUS_Y_POSITION, icon_Bus, 7, 9, _colorTextPrimary);
    _dma_display->drawRGBBitmap(SCHEDULE_ICON_X_POSITION, SCHEDULE_ICON_Y_POSITION, icon_OCTranspoLogo, 9, 9);

    // Draw Title
    if(tripsType==VickyCommute) {
        const char* title = "Vicky";
        drawText(SCHEDULE_TITLE_X_POSITION, SCHEDULE_TITLE_Y_POSITION, title);
        
        _dma_display->setCursor(_dma_display->getCursorX()+2, _dma_display->getCursorY()-1);
        _dma_display->setFont(); // Use default font
        _dma_display->write(0x03);
    } else if(tripsType==NorthSouth) {
        drawText(SCHEDULE_TITLE_X_POSITION, SCHEDULE_TITLE_Y_POSITION, "North-South");
    } else if(tripsType==EastWest) {
        drawText(SCHEDULE_TITLE_X_POSITION, SCHEDULE_TITLE_Y_POSITION, "East-West");
    }

    // Draw clock
    drawTextEnd(PAGE_CLOCK_X_POSITION, PAGE_CLOCK_Y_POSITION, currentTime, _colorTextPrimary, &Font5x7Fixed);

    // Draw Horizontal rule
    _dma_display->drawFastHLine(PAGE_HORIZONTAL_MARGIN_PX, PAGE_TOP_HEADER_SIZE_PX-4,
        PANEL_RES_X - PAGE_HORIZONTAL_MARGIN_PX*2, _colorTextSecondary);

    uint8_t signWidth = SCHEDULE_BUS_SIGN_WIDTH_PX;
    uint8_t row = PAGE_TOP_HEADER_SIZE_PX+8;

    _trackingBusIndicatorPositions.clear();

    for(uint8_t i = 0; i < data.routeDestinations.size() && i < SCHEDULE_MAX_ROUTES ; i++) {  
        const auto &destination = data.routeDestinations[i]; 

        // Draw LTR. RouteNumber, then RouteLabel
        drawRouteSign(destination.routeType, PAGE_HORIZONTAL_MARGIN_PX, row-8, SCHEDULE_BUS_SIGN_WIDTH_PX, destination.routeNumber.c_str());
        drawText(PAGE_HORIZONTAL_MARGIN_PX + SCHEDULE_BUS_SIGN_WIDTH_PX + SCHEDULE_BUS_SIGN_AND_BUS_LABEL_MARGIN_PX, row, shortenRouteDestination(destination.routeDestination).c_str());

        // Foreach trip: 
        uint8_t xPos = PANEL_RES_X - 3 - PAGE_HORIZONTAL_MARGIN_PX;
        int8_t j = SCHEDULE_MAX_TRIPS - 1;
        if(destination.trips.size() < j+1)
            j = destination.trips.size() - 1;

        while(j >= 0) {  
            const auto &trip = destination.trips[j];

            //Draw RTL. MinuteSymbol, then ArrivalTime
            drawMinuteSymbol(xPos, row-7);
            String arrivalStr = String(trip.arrivalTime);
            const char* arrivalCStr = arrivalStr.c_str();
            uint8_t arrivalEndXPos = xPos - SCHEDULE_BUS_MINUTE_SYMBOL_AND_ARRIVAL_TIME_MARGIN_PX;
            uint8_t arrivalStartXPos = getRightAlignStartingPoint(arrivalCStr, arrivalEndXPos);
            drawText(arrivalStartXPos, row, arrivalCStr);

            if(trip.arrivalIsEstimated) {
                uint8_t trackingIndicatorXPos = arrivalStartXPos - SCHEDULE_BUS_ARRIVAL_TIME_AND_TRACKING_INDICATOR_MARGIN_PX;
                _trackingBusIndicatorPositions.push_back(std::make_tuple(trackingIndicatorXPos, row-7, false));
            }

            xPos -= SCHEDULE_BUS_BETWEEN_TRIPS_SPACING_PX;
            j--;
        }
        row = row + SCHEDULE_VERTICAL_SPACING_BETWEEN_BUSES_PX;
    }
    fadeInScreen();

    if(_trackingBusIndicatorPositions.size() > 0) {
        xTaskCreatePinnedToCore(TrackingBusIndicatorTaskFunction, "TrackingBusIndicatorTask", 
            STACK_DEPTH_TRACKING_BUS_INDICATORS_TASK, this, 1, &trackingBusIndicatorTaskHandle, 1);
        // Serial.println("TrackingBusIndicatorTask LAUNCHED");
    }
}

void MatrixDisplay::drawWeatherPage(WeatherData& weatherData, const char* currentTime, const char* currentDateShort) {
    fadeOutScreen();
    clearScreen();

    // Draw Title
    drawText(WEATHER_PAGE_TITLE_X_POSITION, WEATHER_PAGE_TITLE_Y_POSITION, currentDateShort);

    // Draw clock
    drawTextEnd(PAGE_CLOCK_X_POSITION, PAGE_CLOCK_Y_POSITION, currentTime, _colorTextPrimary, &Font5x7Fixed);

    // Draw Horizontal rule
    _dma_display->drawFastHLine(PAGE_HORIZONTAL_MARGIN_PX, PAGE_TOP_HEADER_SIZE_PX-4,
        PANEL_RES_X - PAGE_HORIZONTAL_MARGIN_PX*2, _colorTextSecondary);

    // Divider Lines
    _dma_display->drawFastVLine(32, 14, 48, _colorTextSecondary);
    _dma_display->drawFastVLine(64, 14, 48, _colorTextSecondary);
    _dma_display->drawFastVLine(96, 14, 48, _colorTextSecondary);

    uint8_t xPos = 16;
    for (size_t i = 0; i < weatherData.times.size(); ++i) {
        String hourPretty = hourNumericToPretty(weatherData.times.at(i));
        drawCenteredText(xPos, WEATHER_PAGE_TIME_POS_Y, hourPretty.c_str(), _colorTextPrimary, &Font5x5Fixed);
        const char* weatherImage = weatherTypeToImage(weatherData.weatherType.at(i), weatherData.isDaytime.at(i));

        if(weatherImage != NULL)
            drawASCWWImage(xPos-12, WEATHER_PAGE_WEATHER_ICON_POS_Y, 25, 25, weatherImage);
        else {
            drawCenteredText(xPos, WEATHER_PAGE_TIME_POS_Y + 10, "ERR", _colorTextSecondary, &Font5x5Fixed);
            Serial.print("Warning: weatherTypeToImage() returned NULL for weatherType= ");
            Serial.print(weatherData.weatherType.at(i));
            Serial.print(" daytime=");
            Serial.println(weatherData.weatherType.at(i));
        }

        xPos=xPos+32;
    }
    fadeInScreen();

    _extraWeatherData = weatherData;
    xTaskCreatePinnedToCore(ExtraWeatherDataTaskFunction, "ExtraWeatherDataTaskFunction",
        STACK_DEPTH_EXTRA_WEATHER_DATA_TASK, this, 1, &extraWeatherDataTaskHandle, 1);
    // Serial.println("ExtraWeatherDataTask LAUNCHED");
}

void MatrixDisplay::drawSleepingPage() {
    clearScreen();

    xTaskCreatePinnedToCore(SleepingAnimationTaskFunction, "SleepingAnimationTaskFunction",
        STACK_DEPTH_SLEEPING_ANIMATION_TASK, this, 1, &sleepingAnimationTaskHandle, 1);
    xTaskCreatePinnedToCore(SleepingAnimationTaskFunction2, "SleepingAnimationTaskFunction2",
        STACK_DEPTH_SLEEPING_ANIMATION_TASK, this, 1, &sleepingAnimationTaskHandle2, 0);
    // Serial.println("SleepingAnimationTaskFunction LAUNCHED");
}


void MatrixDisplay::drawClock(const char* currentTime) {   
    uint8_t maxClockWidth = 28; 
    _dma_display->fillRect(PAGE_CLOCK_X_POSITION - maxClockWidth, PAGE_CLOCK_Y_POSITION-7, 
                           maxClockWidth, 7, _colorBlack);
    drawTextEnd(PAGE_CLOCK_X_POSITION, PAGE_CLOCK_Y_POSITION, currentTime, _colorTextPrimary, &Font5x7Fixed);
}

void MatrixDisplay::drawPageBar(float percentComplete) {
    uint8_t fullBarWidth = PANEL_RES_X - PAGE_HORIZONTAL_MARGIN_PX*2;
    uint8_t barWidth = fullBarWidth;
    if(percentComplete < 1)
        barWidth = percentComplete*fullBarWidth;

    _dma_display->drawFastHLine(PAGE_HORIZONTAL_MARGIN_PX, PAGE_TOP_HEADER_SIZE_PX-4,
        barWidth, _colorTextPrimary);
}


void MatrixDisplay::setBrightness(uint8_t brightnessStep) {

    // Clamp brightness
    uint8_t clampedBrightness;
    if(brightnessStep >= DISPLAY_HYSTERESIS_BRIGHTNESS_STEPS - 1)
        clampedBrightness = DISPLAY_BRIGHTNESS_MAX;
    else if (brightnessStep == 0)
        clampedBrightness = DISPLAY_BRIGHTNESS_MIN;
    else {
        clampedBrightness = DISPLAY_BRIGHTNESS_MIN + (brightnessStep * DISPLAY_BRIGHTNESS_STEP_SIZE);
    }


    if(clampedBrightness != _targetBrightness) {
        _targetBrightness = clampedBrightness;
        
        // Serial.print("Panel brightness target set to ");
        // Serial.println(_targetBrightness);
    }
}

void MatrixDisplay::clearScreen() {
    if( trackingBusIndicatorTaskHandle != NULL ) {
        if(eTaskGetState(trackingBusIndicatorTaskHandle) != eDeleted) {
            Serial.println("TrackingBusIndicatorTask DELETED");
            printHighWaterMarkForTask(trackingBusIndicatorTaskHandle);
            vTaskDelete(trackingBusIndicatorTaskHandle);
        }
        trackingBusIndicatorTaskHandle = nullptr;
    }
    if( extraWeatherDataTaskHandle != NULL ) {
        if(eTaskGetState(extraWeatherDataTaskHandle) != eDeleted) {
            Serial.println("ExtraWeatherDataTaskFunction DELETED");
            printHighWaterMarkForTask(extraWeatherDataTaskHandle);
            vTaskDelete(extraWeatherDataTaskHandle);
        }
        extraWeatherDataTaskHandle = nullptr;
    }
    if( sleepingAnimationTaskHandle != NULL ) {
        if(eTaskGetState(sleepingAnimationTaskHandle) != eDeleted) {
            Serial.println("SleepingAnimationTaskFunction DELETED");
            printHighWaterMarkForTask(sleepingAnimationTaskHandle);
            vTaskDelete(sleepingAnimationTaskHandle);
        }
        sleepingAnimationTaskHandle = nullptr;
    }
    if( sleepingAnimationTaskHandle2 != NULL ) {
        if(eTaskGetState(sleepingAnimationTaskHandle2) != eDeleted) {
            Serial.println("SleepingAnimationTaskFunction2 DELETED");
            printHighWaterMarkForTask(sleepingAnimationTaskHandle2);
            vTaskDelete(sleepingAnimationTaskHandle2);
        }
        sleepingAnimationTaskHandle2 = nullptr;
    }
    _dma_display->clearScreen();
}



////////////////////////////////
// Private /////////////////////
////////////////////////////////
void MatrixDisplay::TrackingBusIndicatorTaskFunction(void *pvParameters) {
    MatrixDisplay* instance = static_cast<MatrixDisplay*>(pvParameters);
    std::vector<std::tuple<uint8_t, uint8_t, bool>>& indicatorPositions = instance->_trackingBusIndicatorPositions;
    MatrixPanel_I2S_DMA* dma_display = instance->_dma_display;

    // Serial.print("TrackingBusIndicatorTask INIT indicatorPositions=");
    // Serial.println(indicatorPositions.size());
    int brightnessSteps = 10;
    int brightness = 0;
    bool directionUp = false;
    for(;;) {
        brightness = 0;
        while(brightness < brightnessSteps) {
            for (const auto &value : indicatorPositions) {
                uint16_t color;
                if(std::get<2>(value))
                    color = colorWithIntensity(dma_display, COLOR_GREEN_R, COLOR_GREEN_G, COLOR_GREEN_B, (float)brightness/brightnessSteps);
                else
                    color = colorWithIntensity(dma_display, COLOR_RED_R, COLOR_RED_G, COLOR_RED_B, (float)brightness/brightnessSteps);
                dma_display->drawPixel(std::get<0>(value), std::get<1>(value), color);
            }
            brightness=brightness+1;
            vTaskDelay(pdMS_TO_TICKS(SCHEDULE_TRACKING_INDICATOR_ANIMATION_STEP_MS));
        }
        vTaskDelay(pdMS_TO_TICKS(SCHEDULE_TRACKING_INDICATOR_ANIMATION_TIME_BETWEEN_PHASE_MS));
        brightness = 0;
        while(brightness < brightnessSteps) {
            for (const auto &value : indicatorPositions) {
                uint16_t color;
                if(std::get<2>(value))
                    color = colorWithIntensity(dma_display, COLOR_GREEN_R, COLOR_GREEN_G, COLOR_GREEN_B, (float)brightness/brightnessSteps);
                else
                    color = colorWithIntensity(dma_display, COLOR_RED_R, COLOR_RED_G, COLOR_RED_B, (float)brightness/brightnessSteps);
                dma_display->drawFastVLine(std::get<0>(value)-2, std::get<1>(value)-1, 2, color);
                dma_display->drawFastHLine(std::get<0>(value)-1, std::get<1>(value)-2, 2, color);
            }
            brightness=brightness+1;
            vTaskDelay(pdMS_TO_TICKS(SCHEDULE_TRACKING_INDICATOR_ANIMATION_STEP_MS));
        }
        vTaskDelay(pdMS_TO_TICKS(SCHEDULE_TRACKING_INDICATOR_ANIMATION_TIME_AFTER_ANIMATION_FULL_MS));
        brightness = brightnessSteps;
        while(brightness >= 0) {
            for (const auto &value : indicatorPositions) {
                uint16_t color;
                if(std::get<2>(value))
                    color = colorWithIntensity(dma_display, COLOR_GREEN_R, COLOR_GREEN_G, COLOR_GREEN_B, (float)brightness/brightnessSteps);
                else
                    color = colorWithIntensity(dma_display, COLOR_RED_R, COLOR_RED_G, COLOR_RED_B, (float)brightness/brightnessSteps);
                dma_display->drawPixel(std::get<0>(value), std::get<1>(value), color);
                dma_display->drawFastVLine(std::get<0>(value)-2, std::get<1>(value)-1, 2, color);
                dma_display->drawFastHLine(std::get<0>(value)-1, std::get<1>(value)-2, 2, color);
            }
            brightness=brightness-1;
            vTaskDelay(pdMS_TO_TICKS(SCHEDULE_TRACKING_INDICATOR_ANIMATION_STEP_MS));
        }
        vTaskDelay(pdMS_TO_TICKS(SCHEDULE_TRACKING_INDICATOR_ANIMATION_TIME_IDLE_MS));
    }
}

void MatrixDisplay::ExtraWeatherDataTaskFunction(void *pvParameters) {
    MatrixDisplay* instance = static_cast<MatrixDisplay*>(pvParameters);
    WeatherData& weatherData = instance->_extraWeatherData;
    MatrixPanel_I2S_DMA* dma_display = instance->_dma_display;
    // Serial.println("WeatherInfoTaskFunction INIT");

    uint8_t page = 0;
    for(;;) {

        int brightness = 0;
        bool fadingIn = true;
        int brightnessSteps = 20;
        for(;;) {
            uint8_t posY = 13;
            uint8_t posX = 70;
            uint16_t color = colorWithIntensity(dma_display, COLOR_TEXT_PRIMARY_R, 
                                COLOR_TEXT_PRIMARY_G, COLOR_TEXT_PRIMARY_B, (float)brightness/brightnessSteps);

            uint8_t xPos = 16;
            if(page == 0) {
                for (size_t i = 0; i < weatherData.times.size(); ++i) {
                    instance->drawCenteredText(xPos, WEATHER_PAGE_EXTRA_WEATHER_DATA_FIRST_TOP_Y, weatherData.temperatureCelcius.at(i).c_str(), color, &Font5x7Fixed);
                    instance->drawCenteredText(xPos, WEATHER_PAGE_EXTRA_WEATHER_DATA_SECOND_Y, weatherData.apparentTemperatureCelcius.at(i).c_str(), color, &Font5x5Fixed);
                    xPos=xPos+32;
                }
            } else { // page == 1
                for (size_t i = 0; i < weatherData.times.size(); ++i) {
                    instance->drawCenteredText(xPos, WEATHER_PAGE_EXTRA_WEATHER_DATA_SECOND_TOP_Y, weatherData.precipitationProbability.at(i).c_str(), color, &Font5x5Fixed);
                    instance->drawCenteredText(xPos, WEATHER_PAGE_EXTRA_WEATHER_DATA_SECOND_Y, weatherData.precipitation.at(i).c_str(), color, &Font5x5Fixed);
                    xPos=xPos+32;
                }
            }

            if(fadingIn)
                brightness=brightness+2;
            else
                brightness--;

            if(brightness > brightnessSteps) {
                brightness = brightnessSteps;
                vTaskDelay(pdMS_TO_TICKS(5000));
                fadingIn = false;
            } else if(brightness == -1) {
                break;
            }  
            else {
                vTaskDelay(pdMS_TO_TICKS(50));
            }
        }
        vTaskDelay(pdMS_TO_TICKS(1000));
        if(page == 0)
            page = 1;
        else
            page = 0;
    }
}

void MatrixDisplay::BrightnessTaskFunction(void *pvParameters) {
    MatrixDisplay* instance = static_cast<MatrixDisplay*>(pvParameters);
    // Serial.println("BrightnessTaskFunction INIT");

    uint8_t currentBrightness = instance->_panelBrightness;
    for (;;) {
        while(currentBrightness != instance->_targetBrightness) {
            if(instance->_targetBrightness > currentBrightness)
                currentBrightness ++;
            else
                currentBrightness --;

            #ifdef DEBUG
            // Serial.print("BrightnessTaskFunction LOOP, currentBrightness=");
            // Serial.println(currentBrightness);
            #endif

            instance->_panelBrightness = currentBrightness;
            instance->_dma_display->setBrightness8(currentBrightness);
            vTaskDelay(pdMS_TO_TICKS(10));
        }
        vTaskDelay(pdMS_TO_TICKS(500));
    }
}

void MatrixDisplay::SleepingAnimationTaskFunction(void *pvParameters) {
    MatrixDisplay* instance = static_cast<MatrixDisplay*>(pvParameters);
    MatrixPanel_I2S_DMA* dma_display = instance->_dma_display;

    Serial.print("SleepingAnimationTaskFunction INIT");
    int brightnessSteps = 10;
    int brightness = 0;
    bool directionUp = false;
    uint8_t x, y;
    for(;;) {

        x = random(5, 91);
        y = random(10, 14);

        brightness = 0;
        while(brightness < brightnessSteps) {
            uint16_t color = colorWithIntensity(dma_display, COLOR_TEXT_PRIMARY_R, 
                                COLOR_TEXT_PRIMARY_G, COLOR_TEXT_PRIMARY_B, (float)brightness/brightnessSteps);
            dma_display->drawBitmap(x, y, icon_Sleep1, 32, 40, color);
            brightness=brightness+1;
            vTaskDelay(pdMS_TO_TICKS(SCHEDULE_TRACKING_INDICATOR_ANIMATION_STEP_MS));
        }
        vTaskDelay(pdMS_TO_TICKS(SCHEDULE_TRACKING_INDICATOR_ANIMATION_TIME_BETWEEN_PHASE_MS));
        brightness = 0;
        while(brightness < brightnessSteps) {
            uint16_t color = colorWithIntensity(dma_display, COLOR_TEXT_PRIMARY_R, 
                                COLOR_TEXT_PRIMARY_G, COLOR_TEXT_PRIMARY_B, (float)brightness/brightnessSteps);
            dma_display->drawBitmap(x, y, icon_Sleep2, 32, 40, color);
            brightness=brightness+1;
            vTaskDelay(pdMS_TO_TICKS(SCHEDULE_TRACKING_INDICATOR_ANIMATION_STEP_MS));
        }
        vTaskDelay(pdMS_TO_TICKS(SCHEDULE_TRACKING_INDICATOR_ANIMATION_TIME_BETWEEN_PHASE_MS));
        brightness = 0;
        while(brightness < brightnessSteps) {
            uint16_t color = colorWithIntensity(dma_display, COLOR_TEXT_PRIMARY_R, 
                                COLOR_TEXT_PRIMARY_G, COLOR_TEXT_PRIMARY_B, (float)brightness/brightnessSteps);
            dma_display->drawBitmap(x, y, icon_Sleep3, 32, 40, color);
            brightness=brightness+1;
            vTaskDelay(pdMS_TO_TICKS(SCHEDULE_TRACKING_INDICATOR_ANIMATION_STEP_MS));
        }
        vTaskDelay(pdMS_TO_TICKS(SCHEDULE_TRACKING_INDICATOR_ANIMATION_TIME_BETWEEN_PHASE_MS));
        brightness = 0;
        while(brightness < brightnessSteps) {
            uint16_t color = colorWithIntensity(dma_display, COLOR_TEXT_PRIMARY_R, 
                                COLOR_TEXT_PRIMARY_G, COLOR_TEXT_PRIMARY_B, (float)brightness/brightnessSteps);
            dma_display->drawBitmap(x, y, icon_Sleep4, 32, 40, color);
            brightness=brightness+1;
            vTaskDelay(pdMS_TO_TICKS(SCHEDULE_TRACKING_INDICATOR_ANIMATION_STEP_MS));
        }
        vTaskDelay(pdMS_TO_TICKS(SCHEDULE_TRACKING_INDICATOR_ANIMATION_TIME_BETWEEN_PHASE_MS));
        brightness = 0;
        while(brightness < brightnessSteps) {
            uint16_t color = colorWithIntensity(dma_display, COLOR_TEXT_PRIMARY_R, 
                                COLOR_TEXT_PRIMARY_G, COLOR_TEXT_PRIMARY_B, (float)brightness/brightnessSteps);
            dma_display->drawBitmap(x, y, icon_Sleep5, 32, 40, color);
            brightness=brightness+1;
            vTaskDelay(pdMS_TO_TICKS(SCHEDULE_TRACKING_INDICATOR_ANIMATION_STEP_MS));
        }
        vTaskDelay(pdMS_TO_TICKS(SCHEDULE_TRACKING_INDICATOR_ANIMATION_TIME_AFTER_ANIMATION_FULL_MS));
        brightness = brightnessSteps;
        while(brightness >= 0) {
            uint16_t color = colorWithIntensity(dma_display, COLOR_TEXT_PRIMARY_R, 
                                COLOR_TEXT_PRIMARY_G, COLOR_TEXT_PRIMARY_B, (float)brightness/brightnessSteps);
            dma_display->drawBitmap(x, y, icon_Sleep6, 32, 40, color);
            brightness=brightness-1;
            vTaskDelay(pdMS_TO_TICKS(SCHEDULE_TRACKING_INDICATOR_ANIMATION_STEP_MS));
        }
        vTaskDelay(pdMS_TO_TICKS(SCHEDULE_TRACKING_INDICATOR_ANIMATION_TIME_IDLE_MS));
    }
}
void MatrixDisplay::SleepingAnimationTaskFunction2(void *pvParameters) {
    MatrixDisplay* instance = static_cast<MatrixDisplay*>(pvParameters);
    MatrixPanel_I2S_DMA* dma_display = instance->_dma_display;

    Serial.print("SleepingAnimationTaskFunction2 INIT");
    float t = 0;
    float brightness = 0.5;
    uint8_t height = 10;
    uint8_t addYBottomRow = PANEL_RES_Y - height;
    
    while(true) {
        for (int x = 0; x < PANEL_RES_X; ++x) {
            for (int y = 0; y < 10; ++y) {
                auto xy = x / 30.0f - y / 30.0f;
                auto mpy = M_PI * 2 / 3;
                auto r = brightness * (sinf(xy + t) * 120 + 120);
                auto g = brightness * (sinf(xy + mpy + t) * 120 + 120);
                auto b = brightness * (sinf(xy + mpy * 2 + t) * 120 + 120);
                dma_display->drawPixelRGB888(x, y, r, g, b);
                dma_display->drawPixelRGB888(x, y+addYBottomRow, r, g, b);
            }
        }

        vTaskDelay(pdMS_TO_TICKS(20));
        t += 0.05;
        if(t > M_PI * 2)
            t=0;
    }
}

void MatrixDisplay::drawPixel(uint8_t x, uint8_t y) {
    _dma_display->drawPixel(x, y, _colorTextPrimary);
}

void MatrixDisplay::drawText(uint8_t x, uint8_t y, const char* text) {
    drawText(x, y, text, _colorTextPrimary, NULL);
}

void MatrixDisplay::drawText(uint8_t x, uint8_t y, const char* text, uint16_t color, const GFXfont *f) {
    if(f != NULL)
        _dma_display->setFont(f);
    else
        _dma_display->setFont(&Font5x7Fixed);

    _dma_display->setTextColor(color);
    _dma_display->setCursor(x, y);
    _dma_display->print(text);
}

uint8_t MatrixDisplay::drawCenteredText(uint8_t x, uint8_t y, const char* text) {
    return drawCenteredText(x, y, text, _colorTextPrimary, NULL);       // Returns the cursor in x
}

uint8_t MatrixDisplay::drawCenteredText(uint8_t x, uint8_t y, const char* text, uint16_t color, const GFXfont *f) {
    uint8_t width = getTextWidth(text, f);
    drawText(x - width/2, y, text, color, f);
    return x + width;       // Returns the cursor in x
}

uint8_t MatrixDisplay::drawTextEnd(uint8_t x, uint8_t y, const char* text) {
    return drawTextEnd(x, y, text, _colorTextPrimary, NULL);       // Returns the cursor in x
}

uint8_t MatrixDisplay::drawTextEnd(uint8_t x, uint8_t y, const char* text, uint16_t color, const GFXfont *f) {
    uint8_t width = getTextWidth(text, f);
    drawText(x - width, y, text, color, f);
    return x + width;       // Returns the cursor in x
}

void MatrixDisplay::drawRouteSign(RouteType type, uint8_t x, uint8_t y, uint8_t width, const char* text) {
    int height = 9;
    if(type == FrequentRoute) {
        int triangPeaks = 4;
        int16_t rectX = x+triangPeaks;
        int16_t rectWidth = width-triangPeaks*2;
        _dma_display->fillRect(rectX, y, rectWidth, height, _colorRouteFrequent);
        _dma_display->fillTriangle(rectX, y, rectX, y+height-1, x, y+(height/2), _colorRouteFrequent);
        _dma_display->fillTriangle(rectX+rectWidth, y, rectX+rectWidth, y+height-1, rectX+rectWidth+triangPeaks, y+(height/2), _colorRouteFrequent);
    } else if(type == LocalRoute) {
        _dma_display->fillRect(x, y, width, height, _colorRouteLocal);
    }

    drawCenteredText(x+width/2, y+8, text, _colorTextRoute, &Font5x7Fixed);
}

void MatrixDisplay::drawMinuteSymbol(uint8_t x, uint8_t y) {
    _dma_display->drawLine(x, y, x, y+2, _colorTextPrimary);
}

void MatrixDisplay::drawASCWWImage(int x, int y, int width, int height, const char* imageArray) {
  unsigned int count = 0;
  char first = 0;
  char second = 0;
  byte b = 0;
  byte g = 0;
  byte r = 0;
  byte a = 0;
  float alpha = 0.0f;
  int i, j;
  for(i=0;i<height;++i)
  {
    for(j=0;j<width;++j)
    {
      first = pgm_read_byte(&imageArray[count]);
      second = pgm_read_byte(&imageArray[count+1]);

      b = (first >> 3) & 0x07;
      g = first & 0x07;
      r = (second >> 3) & 0x07;
      a = second & 0x07;
      alpha = a / 7.0f;

      r = alpha * r;
      g = alpha * g;
      b = alpha * b;

      if (a)
        _dma_display->drawPixel(x + j, y + i,  _dma_display->color333(r, g, b));

      count += 2;
    }
  }
}

String MatrixDisplay::shortenRouteDestination(const String& label) {
    String out = String(label);
    out.replace("Pasture", "P");
    // out.replace("Barrhaven", "Barrhvn");
    out.replace("Centre", "");
    out.replace("Auriga", "A");
    // out.replace("Corners", "Cnrs");
    return out;
}

uint8_t MatrixDisplay::getTextWidth(const char *str) {
    int16_t  x1, y1;
    uint16_t w, h;
    _dma_display->setFont(&Font5x7Fixed);
    _dma_display->getTextBounds(str, 0, 0, &x1, &y1, &w, &h);
    return w;
}

uint8_t MatrixDisplay::getTextWidth(const char *str, const GFXfont *f) {
    int16_t  x1, y1;
    uint16_t w, h;
    if(f != NULL)
        _dma_display->setFont(f);
    else
        _dma_display->setFont(&Font5x7Fixed);
    _dma_display->getTextBounds(str, 0, 0, &x1, &y1, &w, &h);
    return w;
}

uint8_t MatrixDisplay::getRightAlignStartingPoint(const char *str, int8_t xPosition) {
    return (xPosition - getTextWidth(str));
}

void MatrixDisplay::fadeOutScreen() {
    uint8_t brightnessPercentage = 100;
    while(brightnessPercentage > 0) {
        _dma_display->setBrightness8((uint8_t)(((float)brightnessPercentage/100) * _panelBrightness));
        brightnessPercentage = brightnessPercentage - 10;
        delay(50);
    }
}

void MatrixDisplay::fadeInScreen() {
    uint8_t brightnessPercentage = 0;
    while(brightnessPercentage < 100) {
        _dma_display->setBrightness8((uint8_t)(((float)brightnessPercentage/100) * _panelBrightness));
        brightnessPercentage = brightnessPercentage + 10;
        delay(50);
    }
}

uint16_t MatrixDisplay::colorWithIntensity(MatrixPanel_I2S_DMA* dma_display, uint8_t r, uint8_t g, uint8_t b, float fraction) {
    return dma_display->color565((uint8_t)r*fraction,(uint8_t)g*fraction,(uint8_t)b*fraction);
}