#include "pair.h"
#include <iostream>

Pair::Pair(int x, int y):
    x(x), y(y)
{
}

void Pair::display()
{
    std::cout << x << "\t" << y << std::endl;
}
