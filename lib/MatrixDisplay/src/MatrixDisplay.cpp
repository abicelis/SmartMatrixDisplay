#include <ESP32-HUB75-MatrixPanel-I2S-DMA.h>
#include <Model.h>
#include <Config.h>
#include "MatrixDisplay.h"

void MatrixDisplay::begin(int8_t r1_pin,
               int8_t g1_pin,
               int8_t b1_pin,
               int8_t r2_pin,
               int8_t g2_pin,
               int8_t b2_pin,
               int8_t a_pin,
               int8_t b_pin,
               int8_t c_pin,
               int8_t d_pin,
               int8_t e_pin,
               int8_t lat_pin,
               int8_t oe_pin,
               int8_t clk_pin,
               uint16_t panel_res_x,
               uint16_t panel_res_y,
               int8_t panel_chain) {

    HUB75_I2S_CFG::i2s_pins _pins={r1_pin, g1_pin, b1_pin, r2_pin, g2_pin, b2_pin, a_pin, 
                                b_pin, c_pin, d_pin, e_pin, lat_pin, oe_pin, clk_pin};
    HUB75_I2S_CFG mxConfig(panel_res_x, panel_res_y, panel_chain, _pins);

    _dma_display = new MatrixPanel_I2S_DMA(mxConfig);
    _dma_display->begin();
    _dma_display->setTextSize(1);
    _dma_display->setTextWrap(false);
    _dma_display->setBrightness8(20); //0-255
    _dma_display->clearScreen();
    _dma_display->fillScreen(_colorBlack);
}


void MatrixDisplay::TrackingBusIndicatorTaskFunction(void *pvParameters) {
    MatrixDisplay* instance = static_cast<MatrixDisplay*>(pvParameters);
    std::vector<std::pair<uint8_t, uint8_t>>& indicatorPositions = instance->_trackingBusIndicatorPositions;
    MatrixPanel_I2S_DMA* dma_display = instance->_dma_display;

    Serial.print("TrackingBusIndicatorTask INIT indicatorPositions=");
    Serial.println(indicatorPositions.size());
    int brightnessSteps = 20;
    int brightness = brightnessSteps;
    bool directionUp = false;
    for(;;) {
        uint16_t color = dma_display->color565(
            (uint8_t)COLOR_TEXT_PRIMARY_R*(float)brightness/brightnessSteps,
            (uint8_t)COLOR_TEXT_PRIMARY_G*(float)brightness/brightnessSteps,
            (uint8_t)COLOR_TEXT_PRIMARY_B*(float)brightness/brightnessSteps
        );
        Serial.println("Brightness is now " + String(brightness));

        for (const auto &value : indicatorPositions)
            drawTrackingBusIndicatorSymbol(dma_display, value.first, value.second, color);

        if(directionUp)
            brightness=brightness+2;
        else
            brightness--;

        if(brightness == -1) {
            brightness = 0;
            vTaskDelay(pdMS_TO_TICKS(150));
            directionUp = true;
        } else if(brightness > brightnessSteps) {
            brightness = brightnessSteps;
            vTaskDelay(pdMS_TO_TICKS(2000));
            directionUp = false;
        } else {
            vTaskDelay(pdMS_TO_TICKS(50));
        }
    }
}

void MatrixDisplay::drawBusScheduleFor(TripsData trips, TripsType tripsType, const char* currentTime) {
    clearScreen();

    // Draw Title
    if(tripsType==VickyCommute) {
        drawText(SCHEDULE_TITLE_X_POSITION, SCHEDULE_TITLE_Y_POSITION, "Morning Commute");
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
    uint8_t row = SCHEDULE_TOP_HEADER_SIZE_PX;

    _trackingBusIndicatorPositions.clear();

    for(uint8_t i = 0; i < trips.routeLabels.size() && i < SCHEDULE_MAX_TRIPS ; i++) {
        String type = "Scheduled";
        if(trips.arrivalIsEstimated.at(i) == true)
            type = "Estimated";
        
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
            drawTrackingBusIndicatorSymbol(_dma_display, trackingIndicatorXPos, row, _colorTextPrimary);
            Serial.println("ADDING TBI for row " + String(row));
            _trackingBusIndicatorPositions.push_back(std::make_pair(trackingIndicatorXPos, row));
        }

        row = row + SCHEDULE_VERTICAL_SPACING_BETWEEN_BUSES_PX;
    }

    // Start DisplayLooper Task
    if(_trackingBusIndicatorPositions.size() > 0) {
        xTaskCreatePinnedToCore(TrackingBusIndicatorTaskFunction, "TrackingBusIndicatorTask", 10000, this, 1, &trackingBusIndicatorTaskHandle, 1);
        Serial.println("TrackingBusIndicatorTask LAUNCHED");
    }
}

void MatrixDisplay::drawText(uint8_t x, uint8_t y, const char* text) {
    _dma_display->setTextColor(_colorTextPrimary);
    _dma_display->setCursor(x, y);
    _dma_display->print(text);
}

void MatrixDisplay::clearScreen() {
    if( trackingBusIndicatorTaskHandle != NULL ) {
        Serial.println("TrackingBusIndicatorTask DELETED");
        vTaskDelete( trackingBusIndicatorTaskHandle );
        trackingBusIndicatorTaskHandle = nullptr;
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

void MatrixDisplay::drawTrackingBusIndicatorSymbol(MatrixPanel_I2S_DMA* dma_display, uint8_t x, uint8_t y, uint16_t color) {
    
    dma_display->drawFastVLine(x, y, 3, color);             // L
    dma_display->drawFastHLine(x+1, y+2, 2, color);
    dma_display->drawFastVLine(x+4, y, 3, color);           // I
    dma_display->drawLine(x+6, y, x+8, y+2, color);         // V
    dma_display->drawLine(x+9, y+1, x+10, y, color);
    dma_display->drawFastHLine(x+12, y, 3, color);          // E
    dma_display->drawFastHLine(x+12, y+1, 2, color);
    dma_display->drawFastHLine(x+12, y+2, 3, color);

    // 'T' icon
    // dma_display->drawLine(x, y, x+2, y, color);
    // dma_display->drawLine(x+1, y+1, x+1, y+2, color);
}

void MatrixDisplay::drawMinuteSymbol(uint8_t x, uint8_t y) {
    _dma_display->drawLine(x, y, x, y+2, _colorTextPrimary);
}

void MatrixDisplay::shortenRouteLabel(String& label) {
    label.replace("Pasture", "P");
    label.replace("Centre", "C");
}

uint8_t MatrixDisplay::getRightAlignStartingPoint(const char *str, int8_t xPosition) {
    int16_t  x1, y1;
    uint16_t w, h;
    _dma_display->getTextBounds(str, 0, 0, &x1, &y1, &w, &h);
    return (xPosition - w);
}