#include "point.h"

Point::Point():
    Pair(0, 0),
    fuckedup(false)
{ }

Point::Point(int x, int y, int fuckedup):
    Pair(x, y),
    fuckedup(fuckedup)
{ }
