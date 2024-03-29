#ifndef UTIL_H
#define UTIL_H
#include <Model.h>
int currentHourOfDay();
void currentDateFull(char* buffer, size_t bufferSize);
void currentDateShort(char* buffer, size_t bufferSize);
void currentHourMinute(char* buffer, size_t bufferSize);
void printTrips(const TripsData& trips);
void updateAppState(AppState& appState, uint16_t lightSensorValue);
TripsType appStateToTripsType(AppState appState);
int8_t lightSensorToDisplayBrightness(uint16_t lightSensorValue);
#endif