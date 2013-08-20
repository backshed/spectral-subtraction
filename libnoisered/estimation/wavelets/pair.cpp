#include <iostream>

#include "pair.h"


Pair::Pair(int x, int y):
	x(x), y(y)
{
}

void Pair::display()
{
	std::cout << x << "\t" << y << std::endl;
}
