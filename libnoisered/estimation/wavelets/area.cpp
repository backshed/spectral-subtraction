#include <cmath>
#include <climits>
#include <iostream>

#include "area.h"
Area::Area():
	minHeight(INT_MAX)
{
}

void Area::removeArea(MaskedMatrix &m)
{
	for (auto i = x0; i < x0 + length; ++i)
	{
		for (auto j = pairList[i - x0]._x - 1; j < pairList[i - x0]._y + 1; ++j)
		{
			m[i][j] = 0;
			m.unmask(i, j);
		}
	}
}

void Area::plotArea(MaskedMatrix &m)
{
	std::cout << "x0 " << x0 << "   length " << length << std::endl;
	for (auto i = 0U; i < m.size(); ++i)
	{
		for (auto j = 0U; j < m[i].size(); ++j)
		{
			if (i >= x0 &&
				(i < (x0 + length)) &&
				(j >= pairList[i - x0]._x) &&
				(j < pairList[i - x0]._y))
			{
				m.mask(i, j);
			}
			else
			{
				m[i][j] = 0; //TODO might erase surroundings ?
			}
		}
	}
}

void Area::plotContour(MaskedMatrix &m, MaskedMatrix::size_type i, MaskedMatrix::size_type j)
{
	auto i_orig = i, j_orig = j;

	x0 = i;
	Point<MaskedMatrix::size_type> p;

	auto add_point = [&]()
	{
		i = p._x;
		j = p._y;
		if (i >= x0 + length)
		{
			pairList.push_back(Pair<MaskedMatrix::size_type>(j, j));
			++length;
		}
		else
		{
			if (j < pairList[i - x0]._x)
			{
				pairList[i - x0]._x = j;
			}
			else if (j > pairList[i - x0]._y)
			{
				pairList[i - x0]._y = j;
			}
		}
	};

	for (auto k = 0U; k < 2U; ++k)
	{
		while (!m.isMasked(i, j))
		{
			m.mask(i, j);
			p = m.next_adjacent_to_zero(i, j, x0);
			if (p.notValid()) break;
			add_point();
		}

		i = i_orig;
		j = j_orig;
		p = m.next_adjacent_to_zero(i, j, x0);
		if (p.notValid()) break;
		add_point();
	}
	m.mask(i_orig, j_orig);
}

void Area::computeParameters(MaskedMatrix &m)
{
	for (auto i = x0 + 1; i < x0 + length - 1; ++i)
	{
		for (auto j = pairList[i - x0]._x + 1; j < pairList[i - x0]._y - 1; ++j)
		{
			numPixels++;
			sumOfValues += m[i][j];
			if (max_pt.val < m[i][j])
			{
				max_pt.val = m[i][j];
				max_pt._x = i;
				max_pt._y = j;
			}
		}
		medianHeight += (pairList[i - x0]._x + pairList[i - x0]._y) / 2;
		if (pairList[i - x0]._x < minHeight) minHeight = pairList[i - x0]._x;
		if (pairList[i - x0]._y > maxHeight) maxHeight = pairList[i - x0]._y;
	}
	medianHeight /= length;
}

void Area::printParameters()
{
	std::cout << "Parameters of the area" << std::endl;
	std::cout << "Beginning:\t" << x0 << "\t\tLength: " << length << "\tHeight: " << verticalSize() << std::endl;
	std::cout << "Mean:\t\t" << sumOfValues / numPixels << "\t\tMedian pxl: " << getMedianHeight() << std::endl;
	std::cout << "Max:  Val: " << max_pt.val << "\t\t (x, y) : (" << max_pt._x << ", " << max_pt._y << ")" << std::endl;
}

MaskedMatrix::size_type Area::getMedianHeight()
{
	return medianHeight - 2; // we cover for the 2 pixels displacement
}

MaskedMatrix::size_type Area::getWidth() const
{
	return length;
}

double Area::getSumOfValues() const
{
	return sumOfValues;
}

unsigned int Area::getNumPixels() const
{
	return numPixels;
}

const Point<MaskedMatrix::size_type> Area::getMax() const
{
	return max_pt;
}

MaskedMatrix::size_type Area::verticalSize()
{
	return maxHeight - minHeight;
}
