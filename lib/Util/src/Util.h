#ifndef UTIL_H
#define UTIL_H
#include "Model.h"
#include "octranspo/Route.h"

uint8_t currentHourOfDay();
uint8_t currentMinute();
uint8_t currentDayOfMonth();
void currentDateFull(char* buffer, size_t bufferSize);
void currentDateShort(char* buffer, size_t bufferSize);
void currentHourMinute(char* buffer, size_t bufferSize);
String hourNumericToPretty(uint8_t hour);

bool inAContentPage(AppState& appState);

void updateAppState(AppState& appState);
void updateAppStateIfDifferentThan(AppState& currentAppState, AppState newAppState);
String appStateToString(AppState appState);

void printHighWaterMarkForTask(TaskHandle_t taskHandle);
void printAvailableHeapMemory();

#endif