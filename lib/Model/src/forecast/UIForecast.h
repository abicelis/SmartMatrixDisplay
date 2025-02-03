#ifndef UIFORECAST_H
#define UIFORECAST_H
#include <vector>
#include "HourlyCondition.h"

class UIForecast {
public:
    uint8_t UVICurrent;
    uint16_t AQICurrent;
    std::vector<HourlyCondition> hourlyConditions;  // Should contain 4 entries. Current hour, plus the next two, plus 3PM (~ commute back)
};

#endif // UIFORECAST_H

