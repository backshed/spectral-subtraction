#ifndef AREA_H
#define AREA_H


#include "matrix.h"

class Area
{
	public:
		Area(Matrix& m);
		void removeArea(Matrix& m);
		void plotArea(Matrix& m);

		void plotContour(Matrix& m, unsigned int i, unsigned int j);

		void computeParameters(Matrix& m);
		void printParameters();

		int verticalSize();
		int getMedianHeight();

		unsigned int getLength() const;
		double getSumOfValues() const;
		unsigned int getNumPixels() const;

	private:
		Matrix& m;
		unsigned int minHeight;
		unsigned int maxHeight;
		unsigned int x0 = 0;
		unsigned int length = 0;
		std::vector<Pair> pairList;

		double sumOfValues = 0;
		unsigned int numPixels = 0;

		unsigned int medianHeight = 0;
};
#endif // AREA_H
