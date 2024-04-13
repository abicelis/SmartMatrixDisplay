#include "Hyst.h"

void Hyst::begin(uint16_t initialIn, uint16_t topStepIn, uint16_t bottomStepIn, 
                 uint16_t sizeAccumInSamples, uint8_t innerSteps, uint16_t gapSize) {

    _topStepIn = topStepIn;
    _bottomStepIn = bottomStepIn;
    _sizeAccumInSamples = sizeAccumInSamples;
    _steps = innerSteps+2;
    _gapSize = gapSize;
    _innerStepSize = (_topStepIn - _bottomStepIn) / innerSteps;

    _currentInAccum = initialIn;             // Start currentIn with initial value
    _currentInAccum *= _sizeAccumInSamples;

    _currentOut = innerSteps + 1;           // Start currentOut on Top Step
}

uint16_t Hyst::add(uint16_t sample) {
    _currentInAccum -= (_currentInAccum / _sizeAccumInSamples);
    _currentInAccum += sample;

    uint16_t currentIn = _currentInAccum/_sizeAccumInSamples;
    // Serial.println("Hyst - get() - _currentInAccum now is " + String(_currentInAccum) + ". currentIn is " + String(currentIn));

    if(_currentOut == _steps - 1 ) {    // We're in the Top step
        if(currentIn < _topStepIn - _gapSize) {
            _currentOut --;
            Serial.println("Hyst - get() - _currentOut --");
        }
    } else if (_currentOut == 0) {        // Bottom step
        if(currentIn > _bottomStepIn + _gapSize) {
            _currentOut ++;
            Serial.println("Hyst - get() - _currentOut ++");
        }
    } else {                              // Inner step
        uint16_t currentOutBottom = _bottomStepIn + (_currentOut - 1) * _innerStepSize;
        if(currentIn < currentOutBottom - _gapSize) {
            Serial.println("Hyst - get() - _currentOut --");
            _currentOut --;
        } else if (currentIn > currentOutBottom + _innerStepSize + _gapSize) {
            Serial.println("Hyst - get() - _currentOut ++");
            _currentOut ++;
        }
    }
    // Serial.println("Hyst - get() - _currentOut now is " + String(_currentOut));

    return _currentOut;
}

uint16_t Hyst::get() {
    return _currentOut;
}