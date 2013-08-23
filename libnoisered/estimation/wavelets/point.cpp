#include "point.h"

Point::Point():
	Pair(0, 0),
	invalid(false)
{ }

Point::Point(const unsigned int x, const unsigned int y, const bool invalid):
	Pair(x, y),
	invalid(invalid)
{ }


bool Point::notValid() const
{
	return invalid;
}

