#include <iostream>

#include "pair.h"


Pair::Pair(const unsigned int x, const unsigned int y):
	x(x), y(y)
{
}

void Pair::display()
{
	std::cout << x << "\t" << y << std::endl;
}
