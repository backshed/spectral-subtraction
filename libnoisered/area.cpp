#include "area.h"
#include <cmath>

Area::Area(Matrix &m):
	m(m)
{
}

void Area::removeArea(Matrix& m)
{
	for(unsigned int i = x0; i < x0 + length; ++i)
	{
		for(unsigned int j = pairList[i-x0].x-1; j < pairList[i-x0].y+1; ++j)
		{
			m[i][j] = 0;
			m.mask[i][j] = 0;
		}
	}
}

void Area::plotArea(Matrix& m)
{
	std::cout << "x0 " << x0 << "   length " << length << std::endl;
	for(unsigned int i = 0; i < m.size(); ++i)
	{
		for(unsigned int j = 0; j < m[i].size(); ++j)
		{
			if(i >= x0 &&
					(i < (x0 + length)) &&
					(j >= pairList[i-x0].x) &&
					(j < pairList[i-x0].y))
			{
				m.mask[i][j] = UPPER_BORN;
			}
			else
			{
				m[i][j] = 0;
			}
		}
	}
}

void Area::plotContour(Matrix& m, unsigned int i, unsigned int j)
{
	int i_orig = i, j_orig = j;
	double contour_val = UPPER_BORN;

	this->x0 = i;
	Point p;

	auto add_point = [&] ()
	{
		i = p.x;
		j = p.y;
		if(i >= this->x0 + this->length)
		{
			this->pairList.push_back(Pair(j, j));
			++this->length;
		}
		else
		{
			if(j < this->pairList[i-this->x0].x)
			{
				this->pairList[i-this->x0].x = j;
			}
			else if(j > this->pairList[i-this->x0].y)
			{
				this->pairList[i-this->x0].y = j;
			}
		}
	};

	for(int k = 0; k < 2; ++k)
	{
		while(m.mask[i][j] != UPPER_BORN)
		{
			m.mask[i][j] = UPPER_BORN;
			p = m.next_adjacent_to_zero(i, j, x0);
			if(p.fuckedup) break;
			add_point();
		}

		i = i_orig;
		j = j_orig;
		p = m.next_adjacent_to_zero(i, j, x0);
		if(p.fuckedup) break;
		add_point();
	}
	m.mask[i_orig][j_orig] = contour_val;
}

void Area::computeParameters(Matrix& m)
{

	for(unsigned int i = x0 + 1; i < x0 + length - 1; ++i)
	{
		for(unsigned int j = pairList[i-x0].x + 1; j < pairList[i-x0].y - 1; ++j)
		{
			numPixels++;
			sumOfValues += m[i][j];
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
	std::cout << "Mean:\t\t" << sumOfValues / numPixels << "\t\tMedian freq: " << getFreq() << std::endl;
}


double Area::getFreq()
{
	return 259500.0 / (medianHeight - 2.0); // padding de deux pixels olol
}

int Area::getFFTBin()
{
	static double f_per_bin = (SAMPLING_RATE / 2.0) / SPECTRUM_SIZE;
	return std::max(10, std::min((int) std::round(getFreq() / f_per_bin), SPECTRUM_SIZE - 1)); // TODO 10 empirique
}

int Area::verticalSize()
{
	return maxHeight - minHeight;
}


