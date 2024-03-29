#include <ESP32-HUB75-MatrixPanel-I2S-DMA.h>
#include <Fonts/FreeSans9pt7b.h>
#include <Model.h>
#include <Config.h>
#include "MatrixDisplay.h"
#include "images.h"
#include "icons.h"

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
}

void MatrixDisplay::drawBusScheduleFor(TripsData& trips, TripsType tripsType, const char* currentTime) {
    fadeOutScreen();
    clearScreen();

    //Draw lil bus
    _dma_display->drawBitmap(SCHEDULE_BUS_X_POSITION, SCHEDULE_BUS_Y_POSITION, icon_Bus, 7, 9, _colorTextPrimary);

    // Draw Title
    if(tripsType==VickyCommute) {
        const char* title = "Vicky";
        drawText(SCHEDULE_TITLE_X_POSITION, SCHEDULE_TITLE_Y_POSITION, title);
        drawChar(SCHEDULE_TITLE_X_POSITION + getTextWidth(title)+2, SCHEDULE_TITLE_Y_POSITION, 0x03);
    } else if(tripsType==NorthSouth) {
        drawText(SCHEDULE_TITLE_X_POSITION, SCHEDULE_TITLE_Y_POSITION, "North-South");
    } else if(tripsType==EastWest) {
        drawText(SCHEDULE_TITLE_X_POSITION, SCHEDULE_TITLE_Y_POSITION, "East-West");
    }

    // Draw clock
    drawText(SCHEDULE_BUS_CLOCK_X_POSITION, SCHEDULE_BUS_CLOCK_Y_POSITION, currentTime);

    // Draw Horizontal rule
    _dma_display->drawFastHLine(SCHEDULE_HORIZONTAL_MARGIN_PX, SCHEDULE_TOP_HEADER_SIZE_PX-4,
        PANEL_RES_X - SCHEDULE_HORIZONTAL_MARGIN_PX*2, _colorTextPrimary);

    uint8_t signWidth = SCHEDULE_BUS_SIGN_WIDTH_PX;
    uint8_t row = SCHEDULE_TOP_HEADER_SIZE_PX+1;

    _trackingBusIndicatorPositions.clear();

    for(uint8_t i = 0; i < trips.routeLabels.size() && i < SCHEDULE_MAX_TRIPS ; i++) {        
        // Draw LTR. RouteNumber, then RouteLabel
        BusType busType = LocalBus;
        if(trips.routeIsFrequent.at(i))
            busType = FrequentBus;
        drawBusSign(busType, SCHEDULE_HORIZONTAL_MARGIN_PX, row-1, SCHEDULE_BUS_SIGN_WIDTH_PX, String(trips.routeNumbers.at(i)).c_str());
        shortenRouteLabel(trips.routeLabels.at(i));
        drawText(SCHEDULE_HORIZONTAL_MARGIN_PX + SCHEDULE_BUS_SIGN_WIDTH_PX + SCHEDULE_BUS_SIGN_AND_BUS_LABEL_MARGIN_PX, row, trips.routeLabels.at(i).c_str());

        // Draw RTL. MinuteSymbol, then ArrivalTime, then Tracking Indicator if applicable.
        uint8_t minuteSymbolXPos = PANEL_RES_X - 1 - SCHEDULE_HORIZONTAL_MARGIN_PX;
        drawMinuteSymbol(minuteSymbolXPos, row);

        String arrivalStr = String(trips.busArrivalTimes.at(i));
        const char* arrivalTime = arrivalStr.c_str();
        uint8_t arrivalEndXPos = minuteSymbolXPos - SCHEDULE_BUS_MINUTE_SYMBOL_AND_ARRIVAL_TIME_MARGIN_PX;
        uint8_t arrivalStartXPos = getRightAlignStartingPoint(arrivalTime, arrivalEndXPos);
        drawText(arrivalStartXPos, row, arrivalTime);
        
        if(trips.arrivalIsEstimated.at(i)) {
            uint8_t trackingIndicatorXPos = arrivalStartXPos - SCHEDULE_BUS_ARRIVAL_TIME_AND_TRACKING_INDICATOR_MARGIN_PX;
            _trackingBusIndicatorPositions.push_back(std::make_pair(trackingIndicatorXPos, row));
        }

        row = row + SCHEDULE_VERTICAL_SPACING_BETWEEN_BUSES_PX;
    }
    fadeInScreen();

    // Start DisplayLooper Task
    if(_trackingBusIndicatorPositions.size() > 0) {
        xTaskCreatePinnedToCore(TrackingBusIndicatorTaskFunction, "TrackingBusIndicatorTask", 
            STACK_DEPTH_TRACKING_BUS_INDICATORS_TASK, this, 1, &trackingBusIndicatorTaskHandle, 1);
        Serial.println("TrackingBusIndicatorTask LAUNCHED");
    }
}

void MatrixDisplay::drawWeatherFor(WeatherData& weatherData, const char* currentHHMM, const char* currentDateShort) {
    fadeOutScreen();
    clearScreen();

    // Draw lil sun
    _dma_display->drawBitmap(SCHEDULE_BUS_X_POSITION, SCHEDULE_BUS_Y_POSITION, icon_Sun, 7, 9, _colorTextPrimary);

    // Draw Title
    // String weatherTypeStr = weatherTypeToString(weatherData.currentWeatherType);
    // drawText(SCHEDULE_TITLE_X_POSITION, SCHEDULE_TITLE_Y_POSITION, weatherTypeStr.c_str(), _colorTextPrimary);
    drawText(SCHEDULE_TITLE_X_POSITION, SCHEDULE_TITLE_Y_POSITION, currentDateShort, _colorTextPrimary);

    // Draw clock
    drawText(SCHEDULE_BUS_CLOCK_X_POSITION, SCHEDULE_BUS_CLOCK_Y_POSITION, currentHHMM);

    // Draw Horizontal rule
    _dma_display->drawFastHLine(SCHEDULE_HORIZONTAL_MARGIN_PX, SCHEDULE_TOP_HEADER_SIZE_PX-4,
        PANEL_RES_X - SCHEDULE_HORIZONTAL_MARGIN_PX*2, _colorTextPrimary);

    // Weather Type Image
    uint8_t weatherTypePosX = 4;
    uint8_t weatherTypePosY = 15;
	DrawImage(weatherTypePosX, weatherTypePosY, 25, 25, weatherTypeToImage(weatherData.currentWeatherType, weatherData.isDaytime));

    // MaxMin temp Pos
    uint8_t maxMinIconPosX = 4;
    uint8_t maxIconPosY = 45;
    uint8_t minIconPosY = 55;
    
    // Max temp
    _dma_display->drawBitmap(maxMinIconPosX, maxIconPosY, icon_Max, 5, 5, _colorTextPrimary);
    int maxTempTextWidth = getTextWidth(weatherData.dailyTemperatureMaxCelcius.c_str());
    drawText(maxMinIconPosX+7, maxIconPosY-1, weatherData.dailyTemperatureMaxCelcius.c_str());
    _dma_display->drawRect(maxMinIconPosX+5+maxTempTextWidth+2, maxIconPosY-1, 2, 2, _colorTextPrimary);

    // Min temp value
    _dma_display->drawBitmap(maxMinIconPosX, minIconPosY, icon_Min, 5, 5, _colorTextPrimary);
    int minTempTextWidth = getTextWidth(weatherData.dailyTemperatureMinCelcius.c_str());
    drawText(maxMinIconPosX+7, minIconPosY-1, weatherData.dailyTemperatureMinCelcius.c_str());
    _dma_display->drawRect(maxMinIconPosX+5+minTempTextWidth+2, minIconPosY-1, 2, 2, _colorTextPrimary);




    // Divider Lines
    _dma_display->drawFastVLine(32, 15, 46, _colorTextSecondary);
    _dma_display->drawFastVLine(65, 15, 46, _colorTextSecondary);
    
    // Draw current Temp
    uint8_t tempPosX = 46;
    uint8_t tempPosY = 30;
    _dma_display->setFont(&FreeSans9pt7b);
    int tempTextHalfWidth = getTextWidth(weatherData.currentTemperatureCelcius.c_str())/2;
    drawText(tempPosX-tempTextHalfWidth, tempPosY, weatherData.currentTemperatureCelcius.c_str());
    _dma_display->setFont();
    // _dma_display->drawPixel(tempPosX-tempTextHalfWidth, tempPosY, _dma_display->color565(255,0,0));
    // _dma_display->drawPixel(tempPosX, tempPosY, _colorRouteFrequent);
    
    // Draw degree Symbol
    _dma_display->drawRect(tempPosX+tempTextHalfWidth+4, tempPosY-14, 3, 3, _colorTextPrimary);
    
    // FEELS LIKE
    uint8_t feelsLikePosX = 37;
    uint8_t feelsLikePosY = 35;
    _dma_display->drawBitmap(feelsLikePosX, feelsLikePosY, icon_FeelsLike, 24, 11, _colorTextPrimary);

    // Apparent Temp
    uint8_t feelTempPosX = 46;
    uint8_t feelTempPosY = 50;
    int feelTempTextHalfWidth = getTextWidth(weatherData.currentApparentTemperatureCelcius.c_str())/2 - 1;
    drawText(feelTempPosX-feelTempTextHalfWidth, feelTempPosY, weatherData.currentApparentTemperatureCelcius.c_str());
    _dma_display->drawRect(feelTempPosX+feelTempTextHalfWidth+2, feelTempPosY-1, 2, 2, _colorTextPrimary);
    // _dma_display->drawPixel(feelTempPosX-feelTempTextHalfWidth, feelTempPosY, _dma_display->color565(255,0,0));
    // _dma_display->drawPixel(feelTempPosX, feelTempPosY, _colorRouteFrequent);

    fadeInScreen();
    _extraWeatherData = weatherData.extraWeatherData;
    xTaskCreatePinnedToCore(ExtraWeatherDataTaskFunction, "ExtraWeatherDataTaskFunction", 
        STACK_DEPTH_EXTRA_WEATHER_DATA_TASK, this, 1, &extraWeatherDataTaskHandle, 1);
    Serial.println("ExtraWeatherDataTask LAUNCHED");
}

void MatrixDisplay::drawChar(uint8_t x, uint8_t y, uint8_t chaar) {
    _dma_display->setTextColor(_colorTextPrimary);
    _dma_display->setCursor(x, y);
    _dma_display->write(chaar);
}

void MatrixDisplay::drawText(uint8_t x, uint8_t y, const char* text) {
    drawText(x, y, text, _colorTextPrimary);
}

void MatrixDisplay::drawText(uint8_t x, uint8_t y, const char* text, uint16_t textColor) {
    _dma_display->setTextColor(textColor);
    _dma_display->setCursor(x, y);
    _dma_display->print(text);
}

void MatrixDisplay::drawPixel(uint8_t x, uint8_t y) {
    _dma_display->drawPixel(x, y, _colorTextPrimary);
}
void MatrixDisplay::setBrightness(uint8_t brightness) {
    if(brightness != _panelBrightness) {
        if(brightness > DISPLAY_BRIGHTNESS_MAX)
            _panelBrightness = DISPLAY_BRIGHTNESS_MAX;
        else if (brightness < DISPLAY_BRIGHTNESS_MIN)
            _panelBrightness = DISPLAY_BRIGHTNESS_MIN;
        else
            _panelBrightness = brightness;

        _dma_display->setBrightness8(_panelBrightness);

        Serial.print("Panel brightness set to ");
        Serial.println(_panelBrightness);
    }
}

void MatrixDisplay::clearScreen() {
    if( trackingBusIndicatorTaskHandle != NULL ) {
        Serial.println("TrackingBusIndicatorTask DELETED");
        vTaskDelete( trackingBusIndicatorTaskHandle );
        trackingBusIndicatorTaskHandle = nullptr;

        #ifdef DEBUG
        UBaseType_t uxHighWaterMark = uxTaskGetStackHighWaterMark(trackingBusIndicatorTaskHandle);
        Serial.println("TrackingBusIndicatorTask stack used this memory: " + String(uxHighWaterMark));
        #endif

    }
    if( extraWeatherDataTaskHandle != NULL ) {
        Serial.println("ExtraWeatherDataTaskFunction DELETED");
        vTaskDelete( extraWeatherDataTaskHandle );
        extraWeatherDataTaskHandle = nullptr;

        #ifdef DEBUG
        UBaseType_t uxHighWaterMark = uxTaskGetStackHighWaterMark(extraWeatherDataTaskHandle);
        Serial.println("ExtraWeatherDataTaskFunction stack used this memory: " + String(uxHighWaterMark));
        #endif

    }
    _dma_display->clearScreen();
}

void MatrixDisplay::DrawImage(int x, int y, int width, int height, const char* imageArray) {
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

void MatrixDisplay::drawIcon(uint16_t *ico, int8_t x, int8_t y, int8_t cols, int8_t rows) {
    int i, j;
    for (i = 0; i < rows; i++)
    {
        for (j = 0; j < cols; j++)
        {
        _dma_display->drawPixel(x + j, y + i, ico[i * cols + j]);
        }
    }
}

void MatrixDisplay::TrackingBusIndicatorTaskFunction(void *pvParameters) {
    MatrixDisplay* instance = static_cast<MatrixDisplay*>(pvParameters);
    std::vector<std::pair<uint8_t, uint8_t>>& indicatorPositions = instance->_trackingBusIndicatorPositions;
    MatrixPanel_I2S_DMA* dma_display = instance->_dma_display;

    Serial.print("TrackingBusIndicatorTask INIT indicatorPositions=");
    Serial.println(indicatorPositions.size());
    int brightnessSteps = 10;
    int brightness = 0;
    bool directionUp = false;
    
    for(;;) {
        brightness = 0;
        while(brightness < brightnessSteps) {
            uint16_t color = colorWithIntensity(dma_display, COLOR_RED_LIVE_R, 
                                COLOR_RED_LIVE_G, COLOR_RED_LIVE_B, (float)brightness/brightnessSteps);
            for (const auto &value : indicatorPositions)
                dma_display->drawBitmap(value.first, value.second, icon_BusTrackingIndicatorCenter, 12, 6, color);
            brightness=brightness+1;
            vTaskDelay(pdMS_TO_TICKS(SCHEDULE_TRACKING_INDICATOR_ANIMATION_STEP_MS));
        }
        vTaskDelay(pdMS_TO_TICKS(SCHEDULE_TRACKING_INDICATOR_ANIMATION_TIME_BETWEEN_PHASE_MS));
        brightness = 0;
        while(brightness < brightnessSteps) {
            uint16_t color = colorWithIntensity(dma_display, COLOR_RED_LIVE_R, 
                                COLOR_RED_LIVE_G, COLOR_RED_LIVE_B, (float)brightness/brightnessSteps);
            for (const auto &value : indicatorPositions)
                dma_display->drawBitmap(value.first, value.second, icon_BusTrackingIndicatorInner, 12, 6, color);
            brightness=brightness+1;
            vTaskDelay(pdMS_TO_TICKS(SCHEDULE_TRACKING_INDICATOR_ANIMATION_STEP_MS));
        }
        vTaskDelay(pdMS_TO_TICKS(SCHEDULE_TRACKING_INDICATOR_ANIMATION_TIME_BETWEEN_PHASE_MS));
        brightness = 0;
        while(brightness < brightnessSteps) {
            uint16_t color = colorWithIntensity(dma_display, COLOR_RED_LIVE_R, 
                                COLOR_RED_LIVE_G, COLOR_RED_LIVE_B, (float)brightness/brightnessSteps);
            for (const auto &value : indicatorPositions)
                dma_display->drawBitmap(value.first, value.second, icon_BusTrackingIndicatorOuter, 12, 6, color);
            brightness=brightness+1;
            vTaskDelay(pdMS_TO_TICKS(SCHEDULE_TRACKING_INDICATOR_ANIMATION_STEP_MS));
        }
        vTaskDelay(pdMS_TO_TICKS(SCHEDULE_TRACKING_INDICATOR_ANIMATION_TIME_AFTER_ANIMATION_FULL_MS));
        brightness = brightnessSteps;
        while(brightness >= 0) {
            uint16_t color = colorWithIntensity(dma_display, COLOR_RED_LIVE_R, 
                                COLOR_RED_LIVE_G, COLOR_RED_LIVE_B, (float)brightness/brightnessSteps);
            for (const auto &value : indicatorPositions)
                dma_display->drawBitmap(value.first, value.second, icon_BusTrackingIndicatorFull, 12, 6, color);
            brightness=brightness-1;
            vTaskDelay(pdMS_TO_TICKS(SCHEDULE_TRACKING_INDICATOR_ANIMATION_STEP_MS));
        }
        vTaskDelay(pdMS_TO_TICKS(SCHEDULE_TRACKING_INDICATOR_ANIMATION_TIME_IDLE_MS));
    }
}

void MatrixDisplay::ExtraWeatherDataTaskFunction(void *pvParameters) {
    MatrixDisplay* instance = static_cast<MatrixDisplay*>(pvParameters);
    std::vector<std::pair<ExtraWeatherDataType, String>> extraWeatherData = instance->_extraWeatherData;
    MatrixPanel_I2S_DMA* dma_display = instance->_dma_display;
    Serial.println("WeatherInfoTaskFunction INIT");

    uint8_t page = 0;
    uint8_t dataPerPage = 3;
    for(;;) {

        int brightness = 0;
        bool fadingIn = true;
        int brightnessSteps = 20;
        for(;;) {
            uint8_t posY = 13;
            uint8_t posX = 70;
            uint16_t color = colorWithIntensity(dma_display, COLOR_TEXT_PRIMARY_R, 
                                COLOR_TEXT_PRIMARY_G, COLOR_TEXT_PRIMARY_B, (float)brightness/brightnessSteps);

            for(int i = page*dataPerPage; i<(dataPerPage*(page+1)); i++) {
                const unsigned char* icon;
                ExtraWeatherDataType type = extraWeatherData.at(i).first;
                if(type == CurrentRelativeHumidity)
                    icon = icon_Humidity;
                else if (type == CurrentWindSpeed)
                    icon = icon_WindSpeed;
                else if (type == DailyPrecipitation)
                    icon = icon_Precipitation;
                else if (type == Sunrise)
                    icon = icon_Sunrise;
                else if (type == Sunset)
                    icon = icon_Sunset;
                else if (type == MaxUVIndex)
                    icon = icon_UV;
                    
                dma_display->drawBitmap(posX, posY, icon, 16, 16, color);
                dma_display->setCursor(posX+20, posY+4);
                dma_display->setTextColor(color);
                dma_display->print(extraWeatherData.at(i).second);
                posY+=17;
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

void MatrixDisplay::drawBusSign(BusType type, uint8_t x, uint8_t y, uint8_t width, const char* text) {
    int height = 9;
    if(type == FrequentBus) {
        int triangPeaks = 4;
        int16_t rectX = x+triangPeaks;
        int16_t rectWidth = width-triangPeaks*2;
        _dma_display->fillRect(rectX, y, rectWidth, height, _colorRouteFrequent);
        _dma_display->fillTriangle(rectX, y, rectX, y+height-1, x, y+(height/2), _colorRouteFrequent);
        _dma_display->fillTriangle(rectX+rectWidth, y, rectX+rectWidth, y+height-1, rectX+rectWidth+triangPeaks, y+(height/2), _colorRouteFrequent);
    } else if(type == LocalBus) {
        _dma_display->fillRect(x, y, width, height, _colorRouteLocal);
    }

    int16_t  x1, y1;
    uint16_t w, h;
    _dma_display->getTextBounds(text, 0, 0, &x1, &y1, &w, &h);

    uint16_t cursorX = (width-w)/2+x+1;
    uint16_t cursorY = y+1;
    _dma_display->setCursor(cursorX, cursorY);
    _dma_display->setTextColor(_colorTextRoute);
    _dma_display->print(text);
}

void MatrixDisplay::drawMinuteSymbol(uint8_t x, uint8_t y) {
    _dma_display->drawLine(x, y, x, y+2, _colorTextPrimary);
}

void MatrixDisplay::shortenRouteLabel(String& label) {
    label.replace("Pasture", "P");
    label.replace("Centre", "C");
}

uint8_t MatrixDisplay::getTextWidth(const char *str) {
    int16_t  x1, y1;
    uint16_t w, h;
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