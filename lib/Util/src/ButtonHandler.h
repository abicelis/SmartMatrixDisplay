#ifndef BUTTONHANDLER_H
#define BUTTONHANDLER_H
#include <Arduino.h>
#include <atomic>
#include "Config.h"
class ButtonHandler {
    public:
        ButtonHandler(uint8_t buttonPin, std::atomic<bool>* buttonTapped, std::atomic<bool>* buttonLongTapped);
        void start();
    private:
        static uint8_t _buttonPin;
        static std::atomic<bool>* _buttonTapped;
        static std::atomic<bool>* _buttonLongTapped;
        static std::atomic<long> _buttonDownMillis;
        TaskHandle_t taskHandle = NULL;
        static void buttonInterrupt();
        static void longTapTask(void* pvParameters) {
            ButtonHandler *_this = (ButtonHandler *) pvParameters; 
            while(true) {
                long bdm = _this->_buttonDownMillis.load();

                if(bdm != -1) {
                    long diff = millis() - bdm;
                    if(diff > BUTTON_LONG_TAP_DURATION_MS) {
                        _this->_buttonDownMillis.store(-1);
                        _this->_buttonLongTapped->store(true);
                    } else {
                        vTaskDelay(pdMS_TO_TICKS(50));      // Wait a tiny bit and recheck
                    }
                } else {
                    // Chill
                    vTaskDelay(pdMS_TO_TICKS(250));
                }
            }  
        }
};
#endif