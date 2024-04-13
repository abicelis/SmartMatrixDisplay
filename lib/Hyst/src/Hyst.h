#ifndef HYST_H
#define HYST_H
#include <Arduino.h>
class Hyst {
    public:
        void begin(uint16_t initialIn, uint16_t topStepIn, uint16_t bottomStepIn, 
                 uint16_t sizeAccumInSamples, uint8_t innerSteps, uint16_t gapSize);
        uint16_t add(uint16_t sample);
        uint16_t get();        
    private:
        uint16_t _topStepIn;
        uint16_t _bottomStepIn;
        uint16_t _sizeAccumInSamples;
        uint8_t _steps;
        uint16_t _innerStepSize;
        uint16_t _gapSize;
        uint16_t _currentInAccum;
        uint8_t _currentOut;
};
#endif