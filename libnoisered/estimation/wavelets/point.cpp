#include "point.h"

Point::Point():
	Pair(0, 0),
	invalid(false)
{ }

Point::Point(int x, int y, bool invalid):
	Pair(x, y),
	invalid(invalid)
{ }


bool Point::notValid() const
{
	return invalid;
}

