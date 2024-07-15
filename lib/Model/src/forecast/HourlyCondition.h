#ifndef HOURLYCONDITION_H
#define HOURLYCONDITION_H
#include <Arduino.h>

class HourlyCondition {
public:
    uint8_t hour;
    uint8_t weatherIcon;               // https://developer.accuweather.com/weather-icons
    String temperatureCelcius;
    String apparentTemperatureCelcius; // Realfeel
    String relativeHumidity;
    String windSpeed;
    String precipitationProbability;
    String precipitationAmount;

    HourlyCondition(uint8_t hour, uint8_t weatherIcon, String temperatureCelcius,
        String apparentTemperatureCelcius, String relativeHumidity, String windSpeed, 
        String precipitationProbability, String precipitationAmount) 
    {
        this->hour = hour;
        this->weatherIcon = weatherIcon;
        this->temperatureCelcius = temperatureCelcius;
        this->apparentTemperatureCelcius = apparentTemperatureCelcius;
        this->relativeHumidity = relativeHumidity;
        this->windSpeed = windSpeed;
        this->precipitationProbability = precipitationProbability;
        this->precipitationAmount = precipitationAmount;
    }
};
#endif // HOURLYCONDITION_H