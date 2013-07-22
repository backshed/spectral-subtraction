#ifndef POINT_H
#define POINT_H

#include "pair.h"
struct Point : public Pair
{
    Point();
    Point(int x, int y, int fuckedup = false);
    bool fuckedup;
};

#endif // POINT_H
