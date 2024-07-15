#ifndef UIFORECAST_H
#define UIFORECAST_H
#include <vector>
#include "HourlyCondition.h"

class UIForecast {
public:
    uint8_t UVICurrent;
    uint16_t AQICurrent;
    std::vector<HourlyCondition> hourlyConditions;
};

#endif // UIFORECAST_H

