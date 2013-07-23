#ifndef POINT_H
#define POINT_H

#include "pair.h"
struct Point : public Pair
{
	Point();
	Point(int x, int y, int invalid = false);

	bool notValid() const;
	double val = 0;

	private:
	bool invalid;

};

#endif // POINT_H
