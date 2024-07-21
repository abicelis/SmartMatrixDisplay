#ifndef PAGE_RENDERER_H
#define PAGE_RENDERER_H
#include "Display.h"
#include "Model.h"
#include "Config.h"
#include "forecast/UIForecast.h"
#include "octranspo/UITrip.h"
class PageRenderer {
    public:
        PageRenderer(Display* display);

        void testDrawWeatherIcons();
        void drawInitializationPage(uint8_t loadingBarWidthPixels);
        void drawCommutePage(UITrip& trip, UIForecast& forecast, const char* currentTime);
        void drawTripsPage(std::vector<UITrip>& trips, AppState appState, const char* currentTime);
        void drawWeatherPage(UIForecast& forecast, const char* currentTime);
        void drawSleepingPage();
        void drawButtonPressedFeedback();
        void drawSpecial();

        void drawClock(const char* currentTime);
        void drawPageBar(float percentComplete);

        void clearScreen();
    private:
        Display *_display = nullptr;
        uint16_t _colorTextRoute;
        uint16_t _colorRouteFrequent;
        uint16_t _colorRouteLocal;
        uint16_t _colorLow;
        uint16_t _colorModerate;
        uint16_t _colorHigh;
        uint16_t _colorVeryHigh;
        uint16_t _colorExtreme;
        uint16_t _colorHazardous;
        
        std::vector<std::tuple<uint8_t, uint8_t, bool>> _trackingBusIndicatorPositions;
        TaskHandle_t trackingBusIndicatorTaskHandle = NULL;
        static void TrackingBusIndicatorTaskFunction(void *pvParameters);

        UIForecast _extraForecast;
        TaskHandle_t extraForecastTaskHandle = NULL;
        static void ExtraForecastTaskFunction(void *pvParameters);
        static void ExtraForecastCommuteTaskFunction(void *pvParameters);

        TaskHandle_t sleepingAnimationTaskHandle = NULL;
        static void SleepingAnimationTaskFunction(void *pvParameters);
        TaskHandle_t sleepingAnimationTaskHandle2 = NULL;
        static void SleepingAnimationTaskFunction2(void *pvParameters);

        void drawRouteSign(RouteType type, uint8_t x, uint8_t y, uint8_t width, const char* text);
        void drawMinuteSymbol(uint8_t x, uint8_t y);
        void drawVerticalBar(uint8_t x, uint8_t y, uint8_t width, uint8_t fullHeight, uint8_t barHeight, uint16_t colorBar);

        String shortenRouteDestination(const String& label);
        static uint16_t colorWithIntensity(MatrixPanel_I2S_DMA* dma_display, uint8_t r, uint8_t g, uint8_t b, float fraction);
        uint16_t colorForUVIndex(uint8_t UVIndex);
        uint8_t barHeightForUVIndex(uint8_t UVIndex);
        uint16_t colorForAQI(uint16_t AQI);
        uint8_t barHeightForAQI(uint16_t AQI);
};
#endif