#ifndef UTIL_H
#define UTIL_H
#include <Model.h>
#include <RouteGroupData.h>
uint8_t currentHourOfDay();
void currentDateFull(char* buffer, size_t bufferSize);
void currentDateShort(char* buffer, size_t bufferSize);
void currentHourMinute(char* buffer, size_t bufferSize);
String hourNumericToPretty(uint8_t hour);
void printTrips(const RouteGroupData& data);
void updateAppState(AppState& appState, AppPage& appPage);
RouteGroupType appPageToRouteGroupType(AppPage appPage);
void printHighWaterMarkForTask(TaskHandle_t taskHandle);
void printAvailableHeapMemory();
#endif