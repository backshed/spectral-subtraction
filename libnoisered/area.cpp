#include "area.h"
#include <cmath>
#include <climits>
#include <iostream>
#include <QDebug>
Area::Area(Matrix &m):
	m(m),
	minHeight(INT_MAX)
{
}

void Area::removeArea(Matrix& m)
{
	for(auto i = x0; i < x0 + length; ++i)
	{
		for(auto j = pairList[i-x0].x-1; j < pairList[i-x0].y+1; ++j)
		{
			m[i][j] = 0;
			m.unmask(i, j);
		}
	}
}

void Area::plotArea(Matrix& m)
{
	std::cout << "x0 " << x0 << "   length " << length << std::endl;
	for(auto i = 0U; i < m.size(); ++i)
	{
		for(auto j = 0U; j < m[i].size(); ++j)
		{
			if(i >= x0 &&
					(i < (x0 + length)) &&
					(j >= pairList[i-x0].x) &&
					(j < pairList[i-x0].y))
			{
				m.mask(i, j);
			}
			else
			{
				m[i][j] = 0; //TODO why ?
			}
		}
	}
}

void Area::plotContour(Matrix& m, unsigned int i, unsigned int j)
{
	int i_orig = i, j_orig = j;

	x0 = i;
	Point p;

	auto add_point = [&] ()
	{
		i = p.x;
		j = p.y;
		if(i >= x0 + length)
		{
			pairList.push_back(Pair(j, j));
			++length;
		}
		else
		{
			if(j < pairList[i-x0].x)
			{
				pairList[i-x0].x = j;
			}
			else if(j > pairList[i-x0].y)
			{
				pairList[i-x0].y = j;
			}
		}
		;
	};

	for(auto k = 0U; k < 2U; ++k)
	{
		while(!m.isMasked(i, j))
		{
			m.mask(i, j);
			p = m.next_adjacent_to_zero(i, j, x0);
			if(p.notValid()) break;
			add_point();
		}

		i = i_orig;
		j = j_orig;
		p = m.next_adjacent_to_zero(i, j, x0);
		if(p.notValid()) break;
		add_point();
	}
	m.mask(i_orig, j_orig);
}

void Area::computeParameters(Matrix& m)
{
	for(auto i = x0 + 1; i < x0 + length - 1; ++i)
	{
		for(auto j = pairList[i-x0].x + 1; j < pairList[i-x0].y - 1; ++j)
		{
			numPixels++;
			sumOfValues += m[i][j];
			if(max_pt.val < m[i][j])
			{
				max_pt.val = m[i][j];
				max_pt.x = i;
				max_pt.y = j;
			}
		}
		medianHeight += (pairList[i-x0].x + pairList[i-x0].y) / 2;
		if(pairList[i-x0].x < minHeight) minHeight = pairList[i-x0].x;
		if(pairList[i-x0].y > maxHeight) maxHeight = pairList[i-x0].y;
	}
	medianHeight /= length;
}

void Area::printParameters()
{
	std::cout << "Parameters of the area" << std::endl;
	std::cout << "Beginning:\t" << x0 << "\t\tLength: " << length << "\tHeight: " << verticalSize() << std::endl;
	std::cout << "Mean:\t\t" << sumOfValues / numPixels << "\t\tMedian pxl: " << getMedianHeight() << std::endl;
	std::cout << "Max:  Val: " << max_pt.val << "\t\t (x, y) : (" << max_pt.x << ", " << max_pt.y << ")" << std::endl;
}

int Area::getMedianHeight()
{
	return medianHeight - 2; // we cover for the 2 pixels displacement
}

unsigned int Area::getLength() const
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

const Point Area::getMax() const
{
	return max_pt;
}

int Area::verticalSize()
{
	return maxHeight - minHeight;
}