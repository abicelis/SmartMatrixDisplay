#ifndef ROUTEGROUPDATA_H
#define ROUTEGROUPDATA_H
#include <Arduino.h>
#include <vector>
#include "RouteDestination.h"
#include "Model.h"

class RouteGroupData {
public:
    RouteGroupType type;                                // e.g.: North-South, East-West, etc.
    std::vector<RouteDestination> routeDestinations;
};
#endif // ROUTEGROUPDATA_H