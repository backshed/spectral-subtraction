#include <iostream>

#include "pair.h"


Pair::Pair(const unsigned int x, const unsigned int y):
	_x(x), _y(y)
{
}

Pair::~Pair()
{

}

void Pair::display()
{
	std::cout << _x << "\t" << _y << std::endl;
}
