#ifndef UTIL_H
#define UTIL_H
#include <Model.h>
#include <RouteGroupData.h>
int currentHourOfDay();
void currentDateFull(char* buffer, size_t bufferSize);
void currentDateShort(char* buffer, size_t bufferSize);
void currentHourMinute(char* buffer, size_t bufferSize);
void printTrips(const RouteGroupData& data);
void updateAppState(AppState& appState, AppPage& appPage);
RouteGroupType appPageToRouteGroupType(AppPage appPage);
int8_t lightSensorToDisplayBrightness(uint16_t lightSensorValue);
#endif