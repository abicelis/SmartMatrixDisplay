#include <ESP32-HUB75-MatrixPanel-I2S-DMA.h>
#include <Fonts/FreeSans9pt7b.h>
#include "Util.h"
#include "PageRenderer.h"
#include "images/icons1BitPerPixel.h"
#include "images/icons2BytePerPixel565.h"
#include "fonts/Font5x5Fixed.h"
#include "fonts/Font5x7Fixed.h"

PageRenderer::PageRenderer(Display* display) {
    _display = display;

    _colorTextRoute = _display->_dma_display->color565(COLOR_TEXT_ROUTE_R, COLOR_TEXT_ROUTE_G, COLOR_TEXT_ROUTE_B);
    _colorRouteFrequent = _display->_dma_display->color565(COLOR_ROUTE_FREQUENT_R, COLOR_ROUTE_FREQUENT_G, COLOR_ROUTE_FREQUENT_B);
    _colorRouteLocal = _display->_dma_display->color565(COLOR_ROUTE_LOCAL_R, COLOR_ROUTE_LOCAL_G, COLOR_ROUTE_LOCAL_B);
    _colorLow = _display->_dma_display->color565(152, 217, 47);
    _colorModerate = _display->_dma_display->color565(255, 207, 65);
    _colorHigh = _display->_dma_display->color565(233, 136, 25);
    _colorVeryHigh = _display->_dma_display->color565(222, 61, 43);
    _colorExtreme = _display->_dma_display->color565(146, 93, 198);
    _colorHazardous = _display->_dma_display->color565(102, 37, 28);
}

void PageRenderer::testDrawWeatherIcons() {
    clearScreen();
    _display->_dma_display->drawRGBBitmap(48-9-32, WEATHER_PAGE_WEATHER_ICON_POS_Y-10, weather_icon_1, 19, 19);
    _display->_dma_display->drawRGBBitmap(48-9, WEATHER_PAGE_WEATHER_ICON_POS_Y-10, weather_icon_2, 19, 19);
    _display->_dma_display->drawRGBBitmap(48-9+32, WEATHER_PAGE_WEATHER_ICON_POS_Y-10, weather_icon_3, 19, 19);
    _display->_dma_display->drawRGBBitmap(48-9+32+32, WEATHER_PAGE_WEATHER_ICON_POS_Y-10, weather_icon_4, 19, 19);
    _display->_dma_display->drawRGBBitmap(48-9-32, WEATHER_PAGE_WEATHER_ICON_POS_Y+15, weather_icon_5, 19, 19);
    _display->_dma_display->drawRGBBitmap(48-9, WEATHER_PAGE_WEATHER_ICON_POS_Y+15, weather_icon_6, 19, 19);
    _display->_dma_display->drawRGBBitmap(48-9+32, WEATHER_PAGE_WEATHER_ICON_POS_Y+15, weather_icon_7, 19, 19);
    _display->_dma_display->drawRGBBitmap(48-9+32+32, WEATHER_PAGE_WEATHER_ICON_POS_Y+15, weather_icon_8, 19, 19);

    // _display->_dma_display->drawRGBBitmap(48-9-32, WEATHER_PAGE_WEATHER_ICON_POS_Y-10, weather_icon_11, 19, 19);
    // _display->_dma_display->drawRGBBitmap(48-9, WEATHER_PAGE_WEATHER_ICON_POS_Y-10, weather_icon_12, 19, 19);
    // _display->_dma_display->drawRGBBitmap(48-9+32, WEATHER_PAGE_WEATHER_ICON_POS_Y-10, weather_icon_13, 19, 19);
    // _display->_dma_display->drawRGBBitmap(48-9+32+32, WEATHER_PAGE_WEATHER_ICON_POS_Y-10, weather_icon_14, 19, 19);
    // _display->_dma_display->drawRGBBitmap(48-9-32, WEATHER_PAGE_WEATHER_ICON_POS_Y+15, weather_icon_15, 19, 19);
    // _display->_dma_display->drawRGBBitmap(48-9, WEATHER_PAGE_WEATHER_ICON_POS_Y+15, weather_icon_16, 19, 19);
    // _display->_dma_display->drawRGBBitmap(48-9+32, WEATHER_PAGE_WEATHER_ICON_POS_Y+15, weather_icon_17, 19, 19);
    // _display->_dma_display->drawRGBBitmap(48-9+32, WEATHER_PAGE_WEATHER_ICON_POS_Y+15, weather_icon_18, 19, 19);

    // _display->_dma_display->drawRGBBitmap(48-9-32, WEATHER_PAGE_WEATHER_ICON_POS_Y-10, weather_icon_19, 19, 19);
    // _display->_dma_display->drawRGBBitmap(48-9, WEATHER_PAGE_WEATHER_ICON_POS_Y-10, weather_icon_20, 19, 19);
    // _display->_dma_display->drawRGBBitmap(48-9+32, WEATHER_PAGE_WEATHER_ICON_POS_Y-10, weather_icon_21, 19, 19);
    // _display->_dma_display->drawRGBBitmap(48-9+32+32, WEATHER_PAGE_WEATHER_ICON_POS_Y-10, weather_icon_22, 19, 19);
    // _display->_dma_display->drawRGBBitmap(48-9-32, WEATHER_PAGE_WEATHER_ICON_POS_Y+15, weather_icon_23, 19, 19);
    // _display->_dma_display->drawRGBBitmap(48-9, WEATHER_PAGE_WEATHER_ICON_POS_Y+15, weather_icon_24, 19, 19);
    // _display->_dma_display->drawRGBBitmap(48-9+32, WEATHER_PAGE_WEATHER_ICON_POS_Y+15, weather_icon_25, 19, 19);
    // _display->_dma_display->drawRGBBitmap(48-9+32, WEATHER_PAGE_WEATHER_ICON_POS_Y+15, weather_icon_26, 19, 19);

    // _display->_dma_display->drawRGBBitmap(48-9-32, WEATHER_PAGE_WEATHER_ICON_POS_Y-10, weather_icon_29, 19, 19);
    // _display->_dma_display->drawRGBBitmap(48-9, WEATHER_PAGE_WEATHER_ICON_POS_Y-10, weather_icon_30, 19, 19);
    // _display->_dma_display->drawRGBBitmap(48-9+32, WEATHER_PAGE_WEATHER_ICON_POS_Y-10, weather_icon_31, 19, 19);
    // _display->_dma_display->drawRGBBitmap(48-9+32+32, WEATHER_PAGE_WEATHER_ICON_POS_Y-10, weather_icon_32, 19, 19);
    // _display->_dma_display->drawRGBBitmap(48-9-32, WEATHER_PAGE_WEATHER_ICON_POS_Y+15, weather_icon_33, 19, 19);
    // _display->_dma_display->drawRGBBitmap(48-9, WEATHER_PAGE_WEATHER_ICON_POS_Y+15, weather_icon_34, 19, 19);
    // _display->_dma_display->drawRGBBitmap(48-9+32, WEATHER_PAGE_WEATHER_ICON_POS_Y+15, weather_icon_35, 19, 19);
    // _display->_dma_display->drawRGBBitmap(48-9+32, WEATHER_PAGE_WEATHER_ICON_POS_Y+15, weather_icon_36, 19, 19);

    // _display->_dma_display->drawRGBBitmap(48-9-32, WEATHER_PAGE_WEATHER_ICON_POS_Y-10, weather_icon_37, 19, 19);
    // _display->_dma_display->drawRGBBitmap(48-9, WEATHER_PAGE_WEATHER_ICON_POS_Y-10, weather_icon_38, 19, 19);
    // _display->_dma_display->drawRGBBitmap(48-9+32, WEATHER_PAGE_WEATHER_ICON_POS_Y-10, weather_icon_39, 19, 19);
    // _display->_dma_display->drawRGBBitmap(48-9+32+32, WEATHER_PAGE_WEATHER_ICON_POS_Y-10, weather_icon_40, 19, 19);
    // _display->_dma_display->drawRGBBitmap(48-9-32, WEATHER_PAGE_WEATHER_ICON_POS_Y+15, weather_icon_41, 19, 19);
    // _display->_dma_display->drawRGBBitmap(48-9, WEATHER_PAGE_WEATHER_ICON_POS_Y+15, weather_icon_42, 19, 19);
    // _display->_dma_display->drawRGBBitmap(48-9+32, WEATHER_PAGE_WEATHER_ICON_POS_Y+15, weather_icon_43, 19, 19);
    // _display->_dma_display->drawRGBBitmap(48-9+32, WEATHER_PAGE_WEATHER_ICON_POS_Y+15, weather_icon_44, 19, 19);
}

void PageRenderer::drawInitializationPage(uint8_t loadingBarWidthPixels) {
    clearScreen();
    _display->drawCenteredText(INITIALIZATION_PAGE_TITLE_POSITION_X, INITIALIZATION_PAGE_TITLE_POSITION_Y, "Initializing");

    _display->_dma_display->drawRoundRect(14, INITIALIZATION_PAGE_BAR_Y_POSITION_PX, 100, 
    INITIALIZATION_PAGE_BAR_THICKNESS_PX, INITIALIZATION_PAGE_BAR_CORNER_RADIUS_PX, _display->_colorTextPrimary);

    _display->_dma_display->fillRoundRect(14, INITIALIZATION_PAGE_BAR_Y_POSITION_PX, loadingBarWidthPixels, 
    INITIALIZATION_PAGE_BAR_THICKNESS_PX, INITIALIZATION_PAGE_BAR_CORNER_RADIUS_PX, _display->_colorTextPrimary);
}

void PageRenderer::drawCommutePage(UITrip& trip, UIForecast& forecast, const char* currentTime) {
    _display->fadeOutScreen();
    clearScreen();

    // Draw icon
    _display->_dma_display->drawRGBBitmap(TRIPS_PAGE_ICON_X_POSITION, TRIPS_PAGE_ICON_Y_POSITION, icon_OCTranspoLogo, 9, 9);

    // Draw Title
    _display->drawText(TRIPS_PAGE_TITLE_X_POSITION, TRIPS_PAGE_TITLE_Y_POSITION, COMMUTE_PAGE_TITLE);        
    _display->_dma_display->setCursor(_display->_dma_display->getCursorX()+2, _display->_dma_display->getCursorY()-1);
    _display->_dma_display->setFont(); // Use default font
    _display->_dma_display->write(0x03);

    // Draw clock
    _display->drawTextEnd(PAGE_CLOCK_X_POSITION, PAGE_CLOCK_Y_POSITION, currentTime, _display->_colorTextPrimary, &Font5x7Fixed);

    // Draw Horizontal rule
    _display->_dma_display->drawFastHLine(PAGE_HORIZONTAL_MARGIN_PX, PAGE_TOP_HEADER_SIZE_PX-4,
        PANEL_RES_X - PAGE_HORIZONTAL_MARGIN_PX*2, _display->_colorTextSecondary);

    uint8_t signWidth = TRIPS_PAGE_BUS_SIGN_WIDTH_PX;
    uint8_t row = PAGE_TOP_HEADER_SIZE_PX+8;

    _trackingBusIndicatorPositions.clear();

    // Draw LTR. RouteNumber, then RouteLabel
    drawRouteSign(trip.routeType, PAGE_HORIZONTAL_MARGIN_PX, row-8, TRIPS_PAGE_BUS_SIGN_WIDTH_PX, trip.routeNumber.c_str());
    _display->drawText(PAGE_HORIZONTAL_MARGIN_PX + TRIPS_PAGE_BUS_SIGN_WIDTH_PX + TRIPS_PAGE_BUS_SIGN_AND_BUS_LABEL_MARGIN_PX, row, shortenRouteDestination(trip.actualDestination).c_str());

    // Foreach trip: 
    uint8_t xPos = PANEL_RES_X - 1 - PAGE_HORIZONTAL_MARGIN_PX;
    int8_t j = TRIPS_PAGE_COMMUTE_MAX_TRIPS - 1;
    if(trip.arrivals.size() < j+1)
        j = trip.arrivals.size() - 1;

    while(j >= 0) {  
        const auto &tripArrival = trip.arrivals[j];

        //Draw RTL. MinuteSymbol, then ArrivalTime
        drawMinuteSymbol(xPos, row-7);
        String arrivalStr = String(tripArrival.time);
        const char* arrivalCStr = arrivalStr.c_str();
        uint8_t arrivalEndXPos = xPos - TRIPS_PAGE_BUS_MINUTE_SYMBOL_AND_ARRIVAL_TIME_MARGIN_PX;
        uint8_t arrivalStartXPos = _display->getRightAlignStartingPoint(arrivalCStr, arrivalEndXPos);
        _display->drawText(arrivalStartXPos, row, arrivalCStr);

        if(tripArrival.IsEstimated) {
            uint8_t trackingIndicatorXPos = arrivalStartXPos - TRIPS_PAGE_BUS_ARRIVAL_TIME_AND_TRACKING_INDICATOR_MARGIN_PX;
            _trackingBusIndicatorPositions.push_back(std::make_tuple(trackingIndicatorXPos, row-7, tripArrival.location == FarAwayEnough));
        }

        xPos -= TRIPS_PAGE_COMMUTE_BETWEEN_TRIPS_SPACING_PX;
        j--;
    }

    // Weather section
    // Draw lines
    _display->_dma_display->drawFastHLine(PAGE_HORIZONTAL_MARGIN_PX, TRIPS_PAGE_COMMUTE_HR_HEIGHT,
        PANEL_RES_X - PAGE_HORIZONTAL_MARGIN_PX*2, _display->_colorTextSecondary);
    _display->_dma_display->drawFastVLine(64, TRIPS_PAGE_COMMUTE_HR_HEIGHT+3, TRIPS_PAGE_COMMUTE_VR_LENGTH, _display->_colorTextSecondary);
    

    xPos = 0;
    for (size_t i = 0; i < 4; i=i+3) {  // Grab index 0, then index 3.
        String hourPretty = hourNumericToPretty(forecast.hourlyConditions.at(i).hour);
        _display->drawText(xPos + COMMUTE_PAGE_TIME_LEFT_SPACING_X, COMMUTE_PAGE_TIME_Y, hourPretty.c_str(), _display->_colorTextPrimary, &Font5x5Fixed);
        
        const uint16_t* weatherImage = weatherIconToImage(forecast.hourlyConditions.at(i).weatherIcon);
        if(weatherImage != NULL)
            _display->_dma_display->drawRGBBitmap(xPos+COMMUTE_PAGE_ICON_LEFT_SPACING_X-9, COMMUTE_PAGE_ICON_Y, weatherImage, 19, 19);
        else {
            Serial.print("Warning: weatherIconToImage() returned NULL for weatherIcon= ");
            Serial.println(forecast.hourlyConditions.at(i).weatherIcon);
        }

        _display->drawCenteredText(xPos+COMMUTE_PAGE_ICON_LEFT_SPACING_X, COMMUTE_PAGE_APPARENT_TEMP_Y, forecast.hourlyConditions.at(i).apparentTemperatureCelcius.c_str(), _display->_colorTextPrimary, &Font5x5Fixed);
        xPos=xPos+64;
    }
    _display->fadeInScreen();

    if(_trackingBusIndicatorPositions.size() > 0) {
        xTaskCreatePinnedToCore(TrackingBusIndicatorTaskFunction, "TrackingBusIndicatorTask", 
            STACK_DEPTH_TRACKING_BUS_INDICATORS_TASK, this, 1, &trackingBusIndicatorTaskHandle, 1);
        // Serial.println("TrackingBusIndicatorTask LAUNCHED");
    }

    _extraForecast = forecast;
    xTaskCreatePinnedToCore(ExtraForecastCommuteTaskFunction, "ExtraForecastCommuteTaskFunction",
        STACK_DEPTH_EXTRA_WEATHER_DATA_TASK, this, 1, &extraForecastTaskHandle, 1);
    // Serial.println("ExtraForecastTask LAUNCHED");
}

void PageRenderer::drawTripsPage(std::vector<UITrip>& trips, AppState appState, const char* currentTime) {
    _display->fadeOutScreen();
    clearScreen();

    //Draw icon
    // _display->_dma_display->drawBitmap(TRIPS_PAGE_BUS_X_POSITION, TRIPS_PAGE_BUS_Y_POSITION, icon_Bus, 7, 9, _display->_colorTextPrimary);
    _display->_dma_display->drawRGBBitmap(TRIPS_PAGE_ICON_X_POSITION, TRIPS_PAGE_ICON_Y_POSITION, icon_OCTranspoLogo, 9, 9);

    // Draw Title
    if(appState == TranspoPage) {
        _display->drawText(TRIPS_PAGE_TITLE_X_POSITION, TRIPS_PAGE_TITLE_Y_POSITION, "OC Transpo");
    } else if(appState == NorthSouthPage) {
        _display->drawText(TRIPS_PAGE_TITLE_X_POSITION, TRIPS_PAGE_TITLE_Y_POSITION, "North-South");
    } else if(appState == EastWestPage) {
        _display->drawText(TRIPS_PAGE_TITLE_X_POSITION, TRIPS_PAGE_TITLE_Y_POSITION, "East-West");
    }

    // Draw clock
    _display->drawTextEnd(PAGE_CLOCK_X_POSITION, PAGE_CLOCK_Y_POSITION, currentTime, _display->_colorTextPrimary, &Font5x7Fixed);

    // Draw Horizontal rule
    _display->_dma_display->drawFastHLine(PAGE_HORIZONTAL_MARGIN_PX, PAGE_TOP_HEADER_SIZE_PX-4,
        PANEL_RES_X - PAGE_HORIZONTAL_MARGIN_PX*2, _display->_colorTextSecondary);

    uint8_t signWidth = TRIPS_PAGE_BUS_SIGN_WIDTH_PX;
    uint8_t row = PAGE_TOP_HEADER_SIZE_PX+8;

    _trackingBusIndicatorPositions.clear();

    for(uint8_t i = 0; i < trips.size() && i < TRIPS_PAGE_MAX_ROUTES ; i++) {  
        const auto &trip = trips[i]; 

        // Draw LTR. RouteNumber, then RouteLabel
        drawRouteSign(trip.routeType, PAGE_HORIZONTAL_MARGIN_PX, row-8, TRIPS_PAGE_BUS_SIGN_WIDTH_PX, trip.routeNumber.c_str());
        _display->drawText(PAGE_HORIZONTAL_MARGIN_PX + TRIPS_PAGE_BUS_SIGN_WIDTH_PX + TRIPS_PAGE_BUS_SIGN_AND_BUS_LABEL_MARGIN_PX, row, shortenRouteDestination(trip.actualDestination).c_str());

        // Foreach trip: 
        uint8_t xPos = PANEL_RES_X - 3 - PAGE_HORIZONTAL_MARGIN_PX;
        int8_t j = TRIPS_PAGE_MAX_TRIPS - 1;
        if(trip.arrivals.size() < j+1)
            j = trip.arrivals.size() - 1;

        while(j >= 0) {  
            const auto &arrival = trip.arrivals[j];

            // Draw RTL. MinuteSymbol, then ArrivalTime
            drawMinuteSymbol(xPos, row-7);
            String arrivalStr = String(arrival.time);
            const char* arrivalCStr = arrivalStr.c_str();
            uint8_t arrivalEndXPos = xPos - TRIPS_PAGE_BUS_MINUTE_SYMBOL_AND_ARRIVAL_TIME_MARGIN_PX;
            uint8_t arrivalStartXPos = _display->getRightAlignStartingPoint(arrivalCStr, arrivalEndXPos);
            _display->drawText(arrivalStartXPos, row, arrivalCStr);

            if(arrival.IsEstimated) {
                uint8_t trackingIndicatorXPos = arrivalStartXPos - TRIPS_PAGE_BUS_ARRIVAL_TIME_AND_TRACKING_INDICATOR_MARGIN_PX;
                _trackingBusIndicatorPositions.push_back(std::make_tuple(trackingIndicatorXPos, row-7, arrival.location == FarAwayEnough));
            }

            xPos -= TRIPS_PAGE_BUS_BETWEEN_TRIPS_SPACING_PX;
            j--;
        }
        row = row + TRIPS_PAGE_VERTICAL_SPACING_BETWEEN_BUSES_PX;
    }
    _display->fadeInScreen();

    if(_trackingBusIndicatorPositions.size() > 0) {
        xTaskCreatePinnedToCore(TrackingBusIndicatorTaskFunction, "TrackingBusIndicatorTask", 
            STACK_DEPTH_TRACKING_BUS_INDICATORS_TASK, this, 1, &trackingBusIndicatorTaskHandle, 1);
        // Serial.println("TrackingBusIndicatorTask LAUNCHED");
    }
}

void PageRenderer::drawWeatherPage(UIForecast& forecast, const char* currentTime) {
    _display->fadeOutScreen();
    clearScreen();

    // Draw icon
    _display->_dma_display->drawRGBBitmap(TRIPS_PAGE_ICON_X_POSITION, TRIPS_PAGE_ICON_Y_POSITION, icon_Accuweather, 9, 9);

    // Draw Title
    _display->drawText(TRIPS_PAGE_TITLE_X_POSITION, TRIPS_PAGE_TITLE_Y_POSITION, "Accuweather");

    // Draw clock
    _display->drawTextEnd(PAGE_CLOCK_X_POSITION, PAGE_CLOCK_Y_POSITION, currentTime, _display->_colorTextPrimary, &Font5x7Fixed);

    // Draw Horizontal rule
    _display->_dma_display->drawFastHLine(PAGE_HORIZONTAL_MARGIN_PX, PAGE_TOP_HEADER_SIZE_PX-4,
        PANEL_RES_X - PAGE_HORIZONTAL_MARGIN_PX*2, _display->_colorTextSecondary);

    // Divider Lines
    // _display->_dma_display->drawFastVLine(32, 14, 27, _display->_colorTextSecondary);
    // _display->_dma_display->drawFastVLine(64, 14, 27, _display->_colorTextSecondary);
    // _display->_dma_display->drawFastVLine(96, 14, 27, _display->_colorTextSecondary);

    // UV index and AQI
    _display->_dma_display->fillRoundRect(3, 15, 27, 28, 3, _display->_colorTextSecondary);
    _display->drawText(5, 40, "UV", _display->_colorTextPrimary, &Font5x5Fixed);
    _display->drawText(19, 40, "AQ", _display->_colorTextPrimary, &Font5x5Fixed);    
    drawVerticalBar(9, 18, 3, 15, barHeightForUVIndex(forecast.UVICurrent), colorForUVIndex(forecast.UVICurrent));
    drawVerticalBar(21, 18, 3, 15, barHeightForAQI(forecast.AQICurrent), colorForAQI(forecast.AQICurrent));
    
    uint8_t xPos = 48;
    for (size_t i = 0; i < 3; ++i) {  // Grab indexes 0, 1, 2. (Not 3. Idx 3 is 5PM)
        String hourPretty = hourNumericToPretty(forecast.hourlyConditions.at(i).hour);
        _display->drawCenteredText(xPos, WEATHER_PAGE_TIME_POS_Y, hourPretty.c_str(), _display->_colorTextPrimary, &Font5x5Fixed);
        const uint16_t* weatherImage = weatherIconToImage(forecast.hourlyConditions.at(i).weatherIcon);

        if(weatherImage != NULL) {
            _display->_dma_display->drawRGBBitmap(xPos-9, WEATHER_PAGE_WEATHER_ICON_POS_Y, weatherImage, 19, 19);
        } else {
            _display->drawCenteredText(xPos, WEATHER_PAGE_TIME_POS_Y + 10, "ERR", _display->_colorRed, &Font5x5Fixed);
            _display->drawCenteredText(xPos, WEATHER_PAGE_TIME_POS_Y + 18, String(forecast.hourlyConditions.at(i).weatherIcon).c_str(), _display->_colorRed, &Font5x5Fixed);
            Serial.print("Warning: weatherIconToImage() returned NULL for weatherIcon= ");
            Serial.println(forecast.hourlyConditions.at(i).weatherIcon);
        }

        xPos=xPos+32;
    }
    _display->fadeInScreen();

    _extraForecast = forecast;
    xTaskCreatePinnedToCore(ExtraForecastTaskFunction, "ExtraForecastTaskFunction",
        STACK_DEPTH_EXTRA_WEATHER_DATA_TASK, this, 1, &extraForecastTaskHandle, 1);
    // Serial.println("ExtraForecastTask LAUNCHED");
}

void PageRenderer::drawSleepingPage() {
    clearScreen();

    xTaskCreatePinnedToCore(SleepingAnimationTaskFunction, "SleepingAnimationTaskFunction",
        STACK_DEPTH_SLEEPING_ANIMATION_TASK, this, 1, &sleepingAnimationTaskHandle, 1);
    xTaskCreatePinnedToCore(SleepingAnimationTaskFunction2, "SleepingAnimationTaskFunction2",
        STACK_DEPTH_SLEEPING_ANIMATION_TASK, this, 1, &sleepingAnimationTaskHandle2, 0);
    // Serial.println("SleepingAnimationTaskFunction LAUNCHED");
}

void PageRenderer::drawSpecial() {
    _display->fadeOutScreen();
    clearScreen();

    _display->_dma_display->drawBitmap(64-3, 32-10, icon_Bus, 7, 9, _display->_colorTextPrimary);
    _display->drawText(TRIPS_PAGE_TITLE_X_POSITION, TRIPS_PAGE_TITLE_Y_POSITION, "Matrix Display v1");

    _display->fadeInScreen();
}

void PageRenderer::drawButtonPressedFeedback() {
    drawPageBar(100);
}

void PageRenderer::drawClock(const char* currentTime) {   
    uint8_t maxClockWidth = 28; 
    _display->_dma_display->fillRect(PAGE_CLOCK_X_POSITION - maxClockWidth, PAGE_CLOCK_Y_POSITION-7, 
                           maxClockWidth, 7, _display->_colorBlack);
    _display->drawTextEnd(PAGE_CLOCK_X_POSITION, PAGE_CLOCK_Y_POSITION, currentTime, _display->_colorTextPrimary, &Font5x7Fixed);
}

void PageRenderer::drawPageBar(float percentComplete) {
    uint8_t fullBarWidth = PANEL_RES_X - PAGE_HORIZONTAL_MARGIN_PX*2;
    uint8_t barWidth = fullBarWidth;
    if(percentComplete < 1)
        barWidth = percentComplete*fullBarWidth;

    _display->_dma_display->drawFastHLine(PAGE_HORIZONTAL_MARGIN_PX, PAGE_TOP_HEADER_SIZE_PX-4,
        barWidth, _display->_colorTextPrimary);
}

void PageRenderer::clearScreen() {
    if( trackingBusIndicatorTaskHandle != NULL ) {
        if(eTaskGetState(trackingBusIndicatorTaskHandle) != eDeleted) {
            // Serial.println("TrackingBusIndicatorTask DELETED");
            printHighWaterMarkForTask(trackingBusIndicatorTaskHandle);
            vTaskDelete(trackingBusIndicatorTaskHandle);
        }
        trackingBusIndicatorTaskHandle = nullptr;
    }
    if( extraForecastTaskHandle != NULL ) {
        if(eTaskGetState(extraForecastTaskHandle) != eDeleted) {
            // Serial.println("ExtraForecastTaskFunction DELETED");
            printHighWaterMarkForTask(extraForecastTaskHandle);
            vTaskDelete(extraForecastTaskHandle);
        }
        extraForecastTaskHandle = nullptr;
    }
    if( sleepingAnimationTaskHandle != NULL ) {
        if(eTaskGetState(sleepingAnimationTaskHandle) != eDeleted) {
            // Serial.println("SleepingAnimationTaskFunction DELETED");
            printHighWaterMarkForTask(sleepingAnimationTaskHandle);
            vTaskDelete(sleepingAnimationTaskHandle);
        }
        sleepingAnimationTaskHandle = nullptr;
    }
    if( sleepingAnimationTaskHandle2 != NULL ) {
        if(eTaskGetState(sleepingAnimationTaskHandle2) != eDeleted) {
            // Serial.println("SleepingAnimationTaskFunction2 DELETED");
            printHighWaterMarkForTask(sleepingAnimationTaskHandle2);
            vTaskDelete(sleepingAnimationTaskHandle2);
        }
        sleepingAnimationTaskHandle2 = nullptr;
    }
    _display->_dma_display->clearScreen();
}



////////////////////////////////
// Private /////////////////////
////////////////////////////////
void PageRenderer::TrackingBusIndicatorTaskFunction(void *pvParameters) {
    PageRenderer* instance = static_cast<PageRenderer*>(pvParameters);
    std::vector<std::tuple<uint8_t, uint8_t, bool>>& indicatorPositions = instance->_trackingBusIndicatorPositions;
    MatrixPanel_I2S_DMA* dma_display = instance->_display->_dma_display;

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
            vTaskDelay(pdMS_TO_TICKS(TRIPS_PAGE_TRACKING_INDICATOR_ANIMATION_STEP_MS));
        }
        vTaskDelay(pdMS_TO_TICKS(TRIPS_PAGE_TRACKING_INDICATOR_ANIMATION_TIME_BETWEEN_PHASE_MS));
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
            vTaskDelay(pdMS_TO_TICKS(TRIPS_PAGE_TRACKING_INDICATOR_ANIMATION_STEP_MS));
        }
        vTaskDelay(pdMS_TO_TICKS(TRIPS_PAGE_TRACKING_INDICATOR_ANIMATION_TIME_AFTER_ANIMATION_FULL_MS));
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
            vTaskDelay(pdMS_TO_TICKS(TRIPS_PAGE_TRACKING_INDICATOR_ANIMATION_STEP_MS));
        }
        vTaskDelay(pdMS_TO_TICKS(TRIPS_PAGE_TRACKING_INDICATOR_ANIMATION_TIME_IDLE_MS));
    }
}

void PageRenderer::ExtraForecastTaskFunction(void *pvParameters) {
    PageRenderer* instance = static_cast<PageRenderer*>(pvParameters);
    UIForecast& forecast = instance->_extraForecast;
    MatrixPanel_I2S_DMA* dma_display = instance->_display->_dma_display;
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

            uint8_t xPos = 48;

            switch (page)
            {
            case 0:
                instance->_display->drawCenteredText(16, WEATHER_PAGE_EXTRA_WEATHER_DATA_FIRST_Y, "Temp", color, &Font5x5Fixed);
                instance->_display->drawCenteredText(16, WEATHER_PAGE_EXTRA_WEATHER_DATA_SECOND_Y, "Feel", color, &Font5x5Fixed);
                for (size_t i = 0; i < 3; ++i) {  // Grab indexes 0, 1, 2. (Not 3. Idx 3 is 5PM)
                    instance->_display->drawCenteredText(xPos, WEATHER_PAGE_EXTRA_WEATHER_DATA_FIRST_Y, forecast.hourlyConditions.at(i).temperatureCelcius.c_str(), color, &Font5x5Fixed);
                    instance->_display->drawCenteredText(xPos, WEATHER_PAGE_EXTRA_WEATHER_DATA_SECOND_Y, forecast.hourlyConditions.at(i).apparentTemperatureCelcius.c_str(), color, &Font5x5Fixed);
                    xPos=xPos+32;
                }
                break;

            case 1:
                instance->_display->drawCenteredText(16, WEATHER_PAGE_EXTRA_WEATHER_DATA_FIRST_Y, "Humidity", color, &Font5x5Fixed);
                instance->_display->drawCenteredText(16, WEATHER_PAGE_EXTRA_WEATHER_DATA_SECOND_Y, "Wind", color, &Font5x5Fixed);
                for (size_t i = 0; i < 3; ++i) {  // Grab indexes 0, 1, 2. (Not 3. Idx 3 is 5PM)
                    instance->_display->drawCenteredText(xPos, WEATHER_PAGE_EXTRA_WEATHER_DATA_FIRST_Y, forecast.hourlyConditions.at(i).relativeHumidity.c_str(), color, &Font5x5Fixed);
                    instance->_display->drawCenteredText(xPos, WEATHER_PAGE_EXTRA_WEATHER_DATA_SECOND_Y, forecast.hourlyConditions.at(i).windSpeed.c_str(), color, &Font5x5Fixed);
                    xPos=xPos+32;
                }
                break;

            case 2:
                instance->_display->drawCenteredText(16, WEATHER_PAGE_EXTRA_WEATHER_DATA_FIRST_Y, "Rain %", color, &Font5x5Fixed);
                instance->_display->drawCenteredText(16, WEATHER_PAGE_EXTRA_WEATHER_DATA_SECOND_Y, "Rain", color, &Font5x5Fixed);
                for (size_t i = 0; i < 3; ++i) {  // Grab indexes 0, 1, 2. (Not 3. Idx 3 is 5PM)
                    instance->_display->drawCenteredText(xPos, WEATHER_PAGE_EXTRA_WEATHER_DATA_FIRST_Y, forecast.hourlyConditions.at(i).precipitationProbability.c_str(), color, &Font5x5Fixed);
                    instance->_display->drawCenteredText(xPos, WEATHER_PAGE_EXTRA_WEATHER_DATA_SECOND_Y, forecast.hourlyConditions.at(i).precipitationAmount.c_str(), color, &Font5x5Fixed);
                    xPos=xPos+32;
                }
                break;
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
        vTaskDelay(pdMS_TO_TICKS(300));
        page += 1;
        if(page == 3)
            page = 0;
    }
}

void PageRenderer::ExtraForecastCommuteTaskFunction(void *pvParameters) {
    PageRenderer* instance = static_cast<PageRenderer*>(pvParameters);
    UIForecast& forecast = instance->_extraForecast;
    MatrixPanel_I2S_DMA* dma_display = instance->_display->_dma_display;
    // Serial.println("ExtraForecastCommuteTaskFunction INIT");

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

            uint8_t xPos = 5;

            switch (page)
            {
            case 0:
                for (size_t i = 0; i < 4; i=i+3) {  // Grab index 0, then index 3.
                    instance->_display->drawText(xPos, COMMUTE_PAGE_EXTRA_FIRST_Y, "Air", color, &Font5x5Fixed);
                    instance->_display->drawText(xPos, COMMUTE_PAGE_EXTRA_SECOND_Y, forecast.hourlyConditions.at(i).relativeHumidity.c_str(), color, &Font5x5Fixed);
                    instance->_display->drawText(xPos, COMMUTE_PAGE_EXTRA_THIRD_Y, forecast.hourlyConditions.at(i).windSpeed.c_str(), color, &Font5x5Fixed);
                    xPos=xPos+64;
                }
                break;

            case 1:
                for (size_t i = 0; i < 4; i=i+3) {  // Grab index 0, then index 3.
                    instance->_display->drawText(xPos, COMMUTE_PAGE_EXTRA_FIRST_Y, "Rain", color, &Font5x5Fixed);
                    instance->_display->drawText(xPos, COMMUTE_PAGE_EXTRA_SECOND_Y, forecast.hourlyConditions.at(i).precipitationProbability.c_str(), color, &Font5x5Fixed);
                    instance->_display->drawText(xPos, COMMUTE_PAGE_EXTRA_THIRD_Y, forecast.hourlyConditions.at(i).precipitationAmount.c_str(), color, &Font5x5Fixed);
                    xPos=xPos+64;
                }
                break;
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
        vTaskDelay(pdMS_TO_TICKS(300));
        page += 1;
        if(page == 2)
            page = 0;
    }
}

void PageRenderer::SleepingAnimationTaskFunction(void *pvParameters) {
    PageRenderer* instance = static_cast<PageRenderer*>(pvParameters);
    MatrixPanel_I2S_DMA* dma_display = instance->_display->_dma_display;

    // Serial.print("SleepingAnimationTaskFunction INIT");
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
            vTaskDelay(pdMS_TO_TICKS(TRIPS_PAGE_TRACKING_INDICATOR_ANIMATION_STEP_MS));
        }
        vTaskDelay(pdMS_TO_TICKS(TRIPS_PAGE_TRACKING_INDICATOR_ANIMATION_TIME_BETWEEN_PHASE_MS));
        brightness = 0;
        while(brightness < brightnessSteps) {
            uint16_t color = colorWithIntensity(dma_display, COLOR_TEXT_PRIMARY_R, 
                                COLOR_TEXT_PRIMARY_G, COLOR_TEXT_PRIMARY_B, (float)brightness/brightnessSteps);
            dma_display->drawBitmap(x, y, icon_Sleep2, 32, 40, color);
            brightness=brightness+1;
            vTaskDelay(pdMS_TO_TICKS(TRIPS_PAGE_TRACKING_INDICATOR_ANIMATION_STEP_MS));
        }
        vTaskDelay(pdMS_TO_TICKS(TRIPS_PAGE_TRACKING_INDICATOR_ANIMATION_TIME_BETWEEN_PHASE_MS));
        brightness = 0;
        while(brightness < brightnessSteps) {
            uint16_t color = colorWithIntensity(dma_display, COLOR_TEXT_PRIMARY_R, 
                                COLOR_TEXT_PRIMARY_G, COLOR_TEXT_PRIMARY_B, (float)brightness/brightnessSteps);
            dma_display->drawBitmap(x, y, icon_Sleep3, 32, 40, color);
            brightness=brightness+1;
            vTaskDelay(pdMS_TO_TICKS(TRIPS_PAGE_TRACKING_INDICATOR_ANIMATION_STEP_MS));
        }
        vTaskDelay(pdMS_TO_TICKS(TRIPS_PAGE_TRACKING_INDICATOR_ANIMATION_TIME_BETWEEN_PHASE_MS));
        brightness = 0;
        while(brightness < brightnessSteps) {
            uint16_t color = colorWithIntensity(dma_display, COLOR_TEXT_PRIMARY_R, 
                                COLOR_TEXT_PRIMARY_G, COLOR_TEXT_PRIMARY_B, (float)brightness/brightnessSteps);
            dma_display->drawBitmap(x, y, icon_Sleep4, 32, 40, color);
            brightness=brightness+1;
            vTaskDelay(pdMS_TO_TICKS(TRIPS_PAGE_TRACKING_INDICATOR_ANIMATION_STEP_MS));
        }
        vTaskDelay(pdMS_TO_TICKS(TRIPS_PAGE_TRACKING_INDICATOR_ANIMATION_TIME_BETWEEN_PHASE_MS));
        brightness = 0;
        while(brightness < brightnessSteps) {
            uint16_t color = colorWithIntensity(dma_display, COLOR_TEXT_PRIMARY_R, 
                                COLOR_TEXT_PRIMARY_G, COLOR_TEXT_PRIMARY_B, (float)brightness/brightnessSteps);
            dma_display->drawBitmap(x, y, icon_Sleep5, 32, 40, color);
            brightness=brightness+1;
            vTaskDelay(pdMS_TO_TICKS(TRIPS_PAGE_TRACKING_INDICATOR_ANIMATION_STEP_MS));
        }
        vTaskDelay(pdMS_TO_TICKS(TRIPS_PAGE_TRACKING_INDICATOR_ANIMATION_TIME_AFTER_ANIMATION_FULL_MS));
        brightness = brightnessSteps;
        while(brightness >= 0) {
            uint16_t color = colorWithIntensity(dma_display, COLOR_TEXT_PRIMARY_R, 
                                COLOR_TEXT_PRIMARY_G, COLOR_TEXT_PRIMARY_B, (float)brightness/brightnessSteps);
            dma_display->drawBitmap(x, y, icon_Sleep6, 32, 40, color);
            brightness=brightness-1;
            vTaskDelay(pdMS_TO_TICKS(TRIPS_PAGE_TRACKING_INDICATOR_ANIMATION_STEP_MS));
        }
        vTaskDelay(pdMS_TO_TICKS(TRIPS_PAGE_TRACKING_INDICATOR_ANIMATION_TIME_IDLE_MS));
    }
}
void PageRenderer::SleepingAnimationTaskFunction2(void *pvParameters) {
    PageRenderer* instance = static_cast<PageRenderer*>(pvParameters);
    MatrixPanel_I2S_DMA* dma_display = instance->_display->_dma_display;

    // Serial.print("SleepingAnimationTaskFunction2 INIT");
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

void PageRenderer::drawRouteSign(RouteType type, uint8_t x, uint8_t y, uint8_t width, const char* text) {
    int height = 9;
    if(type == FrequentRoute) {
        int triangPeaks = 4;
        int16_t rectX = x+triangPeaks;
        int16_t rectWidth = width-triangPeaks*2;
        _display->_dma_display->fillRect(rectX, y, rectWidth, height, _colorRouteFrequent);
        _display->_dma_display->fillTriangle(rectX, y, rectX, y+height-1, x, y+(height/2), _colorRouteFrequent);
        _display->_dma_display->fillTriangle(rectX+rectWidth, y, rectX+rectWidth, y+height-1, rectX+rectWidth+triangPeaks, y+(height/2), _colorRouteFrequent);
    } else if(type == LocalRoute) {
        _display->_dma_display->fillRect(x, y, width, height, _colorRouteLocal);
    }

    _display->drawCenteredText(x+width/2, y+8, text, _colorTextRoute, &Font5x7Fixed);
}

void PageRenderer::drawMinuteSymbol(uint8_t x, uint8_t y) {
    _display->_dma_display->drawLine(x, y, x, y+2, _display->_colorTextPrimary);
}

void PageRenderer::drawVerticalBar(uint8_t x, uint8_t y, uint8_t width, uint8_t fullHeight, uint8_t barHeight, uint16_t colorBar) {
    _display->_dma_display->fillRect(x, y + fullHeight - barHeight, width, barHeight, colorBar);
}

String PageRenderer::shortenRouteDestination(const String& label) {
    String out = String(label);
    out.replace("Pasture", "P");
    out.replace("Centre", "");
    out.replace("Auriga", "A");
    return out;
}

uint16_t PageRenderer::colorWithIntensity(MatrixPanel_I2S_DMA* dma_display, uint8_t r, uint8_t g, uint8_t b, float fraction) {
    return dma_display->color565((uint8_t)r*fraction,(uint8_t)g*fraction,(uint8_t)b*fraction);
}

uint16_t PageRenderer::colorForUVIndex(uint8_t UVIndex){
    if(UVIndex <= 2)
        return _colorLow;
    else if(UVIndex <= 5)
        return _colorModerate;
    else if(UVIndex <= 7)
        return _colorHigh;
    else if(UVIndex <= 10)
        return _colorVeryHigh;
    else 
        return _colorExtreme;
}

uint8_t PageRenderer::barHeightForUVIndex(uint8_t UVIndex) {
    uint8_t div = WEATHER_PAGE_VERTICAL_BAR_HEIGHT_MAX/5;
    if(UVIndex <= 2)
        return div;
    else if(UVIndex <= 5)
        return div*2;
    else if(UVIndex <= 7)
        return div*3;
    else if(UVIndex <= 10)
        return div*4;
    else 
        return div*5;
}

uint16_t PageRenderer::colorForAQI(uint16_t AQI){
    if(AQI <= 50)
        return _colorLow;
    else if(AQI <= 100)
        return _colorModerate;
    else if(AQI <= 150)
        return _colorHigh;
    else if(AQI <= 200)
        return _colorVeryHigh;
    else if(AQI <= 300)
        return _colorExtreme;
    else
        return _colorHazardous;
}

uint8_t PageRenderer::barHeightForAQI(uint16_t AQI) {
    uint8_t div = WEATHER_PAGE_VERTICAL_BAR_HEIGHT_MAX/5;
    if(AQI <= 50)
        return div;
    else if(AQI <= 100)
        return div*2;
    else if(AQI <= 150)
        return div*3;
    else if(AQI <= 200)
        return div*4;
    else if(AQI <= 300)
        return div*5;
    else 
        return div*5;
}