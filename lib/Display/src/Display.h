#ifndef DISPLAY_H
#define DISPLAY_H
#include <ESP32-HUB75-MatrixPanel-I2S-DMA.h>
class Display {
    public:
        void begin(int8_t r1_pin,
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
               int8_t panel_chain);

        void drawPixel(uint8_t x, uint8_t y);
        void drawText(uint8_t x, uint8_t y, const char* text);
        void drawText(uint8_t x, uint8_t y, const char* text, uint16_t color, const GFXfont *f);
        uint8_t drawCenteredText(uint8_t x, uint8_t y, const char* text);
        uint8_t drawCenteredText(uint8_t x, uint8_t y, const char* text, uint16_t color, const GFXfont *f);
        uint8_t drawTextEnd(uint8_t x, uint8_t y, const char* text);
        uint8_t drawTextEnd(uint8_t x, uint8_t y, const char* text, uint16_t color, const GFXfont *f);

        uint8_t getTextWidth(const char *str);
        uint8_t getTextWidth(const char *str, const GFXfont *f);
        uint8_t getRightAlignStartingPoint(const char* str, int8_t xPosition);
        
        void setBrightness(uint8_t brightnessStep);
        void fadeOutScreen();
        void fadeInScreen();

        MatrixPanel_I2S_DMA *_dma_display = nullptr;
        uint16_t _colorBlack;
        uint16_t _colorWhite;
        uint16_t _colorRed;
        uint16_t _colorGreen;
        uint16_t _colorTextPrimary;
        uint16_t _colorTextSecondary;
    private:
        uint8_t _panelBrightness = 20;
        uint8_t _targetBrightness = 20;
        TaskHandle_t brightnessTaskHandle = NULL;
        static void BrightnessTaskFunction(void *pvParameters);
};
#endif