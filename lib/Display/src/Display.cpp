#include <ESP32-HUB75-MatrixPanel-I2S-DMA.h>
#include <Fonts/FreeSans9pt7b.h>
#include "Config.h"
#include "Util.h"
#include "Display.h"
#include "images/icons1BitPerPixel.h"
#include "images/icons2BytePerPixel565.h"
#include "fonts/Font5x5Fixed.h"
#include "fonts/Font5x7Fixed.h"

void Display::begin(int8_t r1_pin, int8_t g1_pin, int8_t b1_pin, int8_t r2_pin, int8_t g2_pin, int8_t b2_pin,
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

    _colorBlack = _dma_display->color565(0, 0, 0);
    _colorWhite = _dma_display->color565(255, 255, 255);
    _colorRed = _dma_display->color565(COLOR_RED_R, COLOR_RED_G, COLOR_RED_B);
    _colorGreen = _dma_display->color565(COLOR_GREEN_R, COLOR_GREEN_G, COLOR_GREEN_B);
    _colorTextPrimary = _dma_display->color565(COLOR_TEXT_PRIMARY_R, COLOR_TEXT_PRIMARY_G, COLOR_TEXT_PRIMARY_B);
    _colorTextSecondary = _dma_display->color565(COLOR_TEXT_SECONDARY_R, COLOR_TEXT_SECONDARY_G, COLOR_TEXT_SECONDARY_B);

    _dma_display->fillScreen(_colorBlack);

    xTaskCreatePinnedToCore(BrightnessTaskFunction, "BrightnessTaskFunction", 
        STACK_DEPTH_BRIGHTNESS_TASK, this, 1, &brightnessTaskHandle, 1);
}


void Display::drawPixel(uint8_t x, uint8_t y) {
    _dma_display->drawPixel(x, y, _colorTextPrimary);
}

void Display::drawText(uint8_t x, uint8_t y, const char* text) {
    drawText(x, y, text, _colorTextPrimary, NULL);
}

void Display::drawText(uint8_t x, uint8_t y, const char* text, uint16_t color, const GFXfont *f) {
    if(f != NULL)
        _dma_display->setFont(f);
    else
        _dma_display->setFont(&Font5x7Fixed);

    _dma_display->setTextColor(color);
    _dma_display->setCursor(x, y);
    _dma_display->print(text);
}

uint8_t Display::drawCenteredText(uint8_t x, uint8_t y, const char* text) {
    return drawCenteredText(x, y, text, _colorTextPrimary, NULL);       // Returns the cursor in x
}

uint8_t Display::drawCenteredText(uint8_t x, uint8_t y, const char* text, uint16_t color, const GFXfont *f) {
    uint8_t width = getTextWidth(text, f);
    drawText(x - width/2, y, text, color, f);
    return x + width;       // Returns the cursor in x
}

uint8_t Display::drawTextEnd(uint8_t x, uint8_t y, const char* text) {
    return drawTextEnd(x, y, text, _colorTextPrimary, NULL);       // Returns the cursor in x
}

uint8_t Display::drawTextEnd(uint8_t x, uint8_t y, const char* text, uint16_t color, const GFXfont *f) {
    uint8_t width = getTextWidth(text, f);
    drawText(x - width, y, text, color, f);
    return x + width;       // Returns the cursor in x
}

uint8_t Display::getTextWidth(const char *str) {
    int16_t  x1, y1;
    uint16_t w, h;
    _dma_display->setFont(&Font5x7Fixed);
    _dma_display->getTextBounds(str, 0, 0, &x1, &y1, &w, &h);
    return w;
}

uint8_t Display::getTextWidth(const char *str, const GFXfont *f) {
    int16_t  x1, y1;
    uint16_t w, h;
    if(f != NULL)
        _dma_display->setFont(f);
    else
        _dma_display->setFont(&Font5x7Fixed);
    _dma_display->getTextBounds(str, 0, 0, &x1, &y1, &w, &h);
    return w;
}

uint8_t Display::getRightAlignStartingPoint(const char *str, int8_t xPosition) {
    return (xPosition - getTextWidth(str));
}

void Display::fadeOutScreen() {
    uint8_t brightnessPercentage = 100;
    while(brightnessPercentage > 0) {
        _dma_display->setBrightness8((uint8_t)(((float)brightnessPercentage/100) * _panelBrightness));
        brightnessPercentage = brightnessPercentage - 10;
        vTaskDelay(pdMS_TO_TICKS(50));
    }
}

void Display::fadeInScreen() {
    uint8_t brightnessPercentage = 0;
    while(brightnessPercentage < 100) {
        _dma_display->setBrightness8((uint8_t)(((float)brightnessPercentage/100) * _panelBrightness));
        brightnessPercentage = brightnessPercentage + 10;
        vTaskDelay(pdMS_TO_TICKS(50));
    }
}

void Display::setBrightness(uint8_t brightnessStep) {

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




////////////////////////////////
// Private /////////////////////
////////////////////////////////
void Display::BrightnessTaskFunction(void *pvParameters) {
    Display* instance = static_cast<Display*>(pvParameters);
    // Serial.println("BrightnessTaskFunction INIT");

    uint8_t currentBrightness = instance->_panelBrightness;
    for (;;) {
        while(currentBrightness != instance->_targetBrightness) {
            if(instance->_targetBrightness > currentBrightness)
                currentBrightness ++;
            else
                currentBrightness --;

            // Serial.print("BrightnessTaskFunction LOOP, currentBrightness=");
            // Serial.println(currentBrightness);

            instance->_panelBrightness = currentBrightness;
            instance->_dma_display->setBrightness8(currentBrightness);
            vTaskDelay(pdMS_TO_TICKS(10));
        }
        vTaskDelay(pdMS_TO_TICKS(500));
    }
}