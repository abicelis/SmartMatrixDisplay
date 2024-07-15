#ifndef FORECAST_H
#define FORECAST_H
#include <Arduino.h>
#include <Preferences.h>
#include <vector>
#include "HourlyCondition.h"
#include "forecast/UIForecast.h"

// enum UVIndex { Low, Moderate, High, VeryHigh, Extreme };

class Forecast {
public:    
    bool isStaleOrInvalid();
    
    bool tryLoadForecastFromFlash(Preferences& preferences);
    void saveForecastToFlash(Preferences& preferences);

    void updateAQI(uint16_t newAQI);
    void updateUVI(uint8_t newUVI);
    void updateHourlyConditions(std::vector<HourlyCondition> newHourlyConditions);

    UIForecast getUIForecast();

private: 
    uint8_t UVICurrent;
    uint16_t AQICurrent;
    std::vector<HourlyCondition> hourlyConditions;

    SemaphoreHandle_t mutex = xSemaphoreCreateMutex();
    void clearForecastFromFlash(Preferences& preferences);
    void clear();
};

#endif // FORECAST_H

