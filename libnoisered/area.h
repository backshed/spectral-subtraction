#ifndef AREA_H
#define AREA_H


#include "matrix.h"

/**
 * @brief
 *
 */
class Area
{
	public:
		Area(Matrix& m);
		/**
		 * @brief
		 *
		 * @param m
		 */
		/**
		 * @brief
		 *
		 * @param m
		 */
		void removeArea(Matrix& m);
		/**
		 * @brief
		 *
		 * @param m
		 */
		void plotArea(Matrix& m);

		/**
		 * @brief
		 *
		 * @param m
		 * @param i
		 * @param j
		 */
		void plotContour(Matrix& m, unsigned int i, unsigned int j);

		/**
		 * @brief
		 *
		 * @param m
		 */
		void computeParameters(Matrix& m);
		/**
		 * @brief
		 *
		 */
		void printParameters();

		/**
		 * @brief
		 *
		 * @return int
		 */
		int verticalSize();
		/**
		 * @brief
		 *
		 * @return int
		 */
		int getMedianHeight();

		/**
		 * @brief
		 *
		 * @return unsigned int
		 */
		unsigned int getLength() const;
		/**
		 * @brief
		 *
		 * @return double
		 */
		double getSumOfValues() const;
		/**
		 * @brief
		 *
		 * @return unsigned int
		 */
		unsigned int getNumPixels() const;

		/**
		 * @brief
		 *
		 * @return const Point
		 */
		const Point getMax() const;

	private:
		Matrix& m; /**< TODO */
		Point max_pt; /**< TODO */
		unsigned int minHeight; /**< TODO */
		unsigned int maxHeight = 0; /**< TODO */
		unsigned int x0 = 0; /**< TODO */
		unsigned int length = 0; /**< TODO */
		std::vector<Pair> pairList; /**< TODO */

		double sumOfValues = 0; /**< TODO */
		unsigned int numPixels = 0; /**< TODO */

		unsigned int medianHeight = 0; /**< TODO */
};
#endif // AREA_H
