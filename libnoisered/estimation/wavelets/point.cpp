#include "point.h"

Point::Point():
	Pair(0, 0),
	_invalid(false)
{ }

Point::~Point()
{

}

Point::Point(const unsigned int x, const unsigned int y, const bool invalid):
	Pair(x, y),
	_invalid(invalid)
{ }


bool Point::notValid() const
{
	return _invalid;
}

