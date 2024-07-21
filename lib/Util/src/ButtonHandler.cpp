#include <Arduino.h>
#include "ButtonHandler.h"



// Static memger variables need to be initialized here
uint8_t ButtonHandler::_buttonPin;
std::atomic<bool>* ButtonHandler::_buttonTapped;
std::atomic<bool>* ButtonHandler::_buttonLongTapped;
std::atomic<long> ButtonHandler::_buttonDownMillis;

ButtonHandler::ButtonHandler(uint8_t buttonPin, std::atomic<bool>* buttonTapped, std::atomic<bool>* buttonLongTapped) {
    _buttonPin = buttonPin;
    _buttonTapped = buttonTapped;
    _buttonLongTapped = buttonLongTapped;   
     
     _buttonDownMillis.store(-1);
}

void ButtonHandler::start() {
    Serial.println(" BTN-HNDLR: Attaching to pin " + String(_buttonPin));
    pinMode(_buttonPin, INPUT);
    attachInterrupt(digitalPinToInterrupt(_buttonPin), buttonInterrupt, CHANGE);

    BaseType_t result = xTaskCreatePinnedToCore(longTapTask, "LongTapTask", 
                STACK_DEPTH_DATA_FETCHER_TASK, this, TASK_PRIORITY_HIGH, &taskHandle, CORE_ID_0);
    if(result == pdPASS) {
        Serial.println(" BTN-HNDLR: LongTapTask launched");
    } else if(result == errCOULD_NOT_ALLOCATE_REQUIRED_MEMORY) {
        Serial.print(" BTN-HNDLR: ERROR LongTapTask creation failed due to insufficient memory! HEAP MEM=");
        Serial.println(xPortGetFreeHeapSize());
        vTaskDelay(pdMS_TO_TICKS(5000));
        ESP.restart();
    } else {
        Serial.print(" BTN-HNDLR: ERROR LongTapTask creation failed!!!");
        vTaskDelay(pdMS_TO_TICKS(5000));
        ESP.restart();
    }
}

void IRAM_ATTR ButtonHandler::buttonInterrupt() {
    if(digitalRead(_buttonPin) == HIGH) {
        // Rising signal = button pressed
        _buttonDownMillis.store(millis());
        // Serial.println(" BTN-HNDLR: RISING");
    } else {
        // Serial.println(" BTN-HNDLR: FALLING");
        long bdm = _buttonDownMillis.load();
        if(bdm != -1) {
            _buttonDownMillis.store(-1);
            
            long diff = millis() - bdm;
            if(diff < BUTTON_LONG_TAP_DURATION_MS)
                _buttonTapped->store(true);
            else
                _buttonLongTapped->store(true);
        }
    }
}