#ifndef MATRIX_DISPLAY_H
#define MATRIX_DISPLAY_H
#include <ESP32-HUB75-MatrixPanel-I2S-DMA.h>
#include <Model.h>
class MatrixDisplay {
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
        void drawBusScheduleFor(TripsData trips, TripsType tripsType, const char* currentTime);
        void drawText(uint8_t x, uint8_t y, const char* text);
        void clearScreen();

        void DrawImage(int x, int y, int width, int height, const char* imageArray);
        void drawIcon(uint16_t *ico, int8_t x, int8_t y, int8_t cols, int8_t rows);
    private:
        MatrixPanel_I2S_DMA *_dma_display = nullptr;
        uint16_t _colorBlack = _dma_display->color565(0, 0, 0);
        uint16_t _colorWhite = _dma_display->color565(255, 255, 255);
        uint16_t _colorTextPrimary = _dma_display->color565(COLOR_TEXT_PRIMARY_R, COLOR_TEXT_PRIMARY_G, COLOR_TEXT_PRIMARY_B);
        uint16_t _colorTextRoute = _dma_display->color565(COLOR_TEXT_ROUTE_R, COLOR_TEXT_ROUTE_G, COLOR_TEXT_ROUTE_B);
        uint16_t _colorRouteFrequent = _dma_display->color565(COLOR_ROUTE_FREQUENT_R, COLOR_ROUTE_FREQUENT_G, COLOR_ROUTE_FREQUENT_B);
        uint16_t _colorRouteLocal = _dma_display->color565(COLOR_ROUTE_LOCAL_R, COLOR_ROUTE_LOCAL_G, COLOR_ROUTE_LOCAL_B);
        
        std::vector<std::pair<uint8_t, uint8_t>> _trackingBusIndicatorPositions;
        TaskHandle_t trackingBusIndicatorTaskHandle = NULL;
        static void TrackingBusIndicatorTaskFunction(void *pvParameters);

        void drawBusSign(BusType type, uint8_t x, uint8_t y, uint8_t width, const char* text);
        static void drawTrackingBusIndicatorSymbol(MatrixPanel_I2S_DMA* dma_display, uint8_t x, uint8_t y, uint16_t color);
        void drawMinuteSymbol(uint8_t x, uint8_t y);
        void shortenRouteLabel(String& label);
        uint8_t getRightAlignStartingPoint(const char* str, int8_t xPosition);
};
#endif