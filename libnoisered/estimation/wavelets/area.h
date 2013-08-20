#pragma once
#include "matrix.h"

/**
 * @brief Represents an area in a Matrix. Used for CWT MT reduction algorithm.
 * The area is delimited by a vertical pair of points. (i.e. lowest and highest point at a given column).
 *
 * It is written in the matrix mask.
 *
 * For every function, the m parameter corresponds to the matrix the area is linked to.
 *
 */
class Area
{
	public:
		/**
		 * @brief Constructor.
		 *
		 * @param m Matrix on which this area applies.
		 */
		Area(Matrix &m);

		/**
		 * @brief Removes the area from the mask.
		 *
		 * @param m
		 */
		void removeArea(Matrix &m);

		/**
		 * @brief Fills the area on the mask.
		 *
		 * Used for debug purposes.
		 *
		 * @param m
		 */
		void plotArea(Matrix &m);

		/**
		 * @brief Detects and plots the contour of the area on the mask of the matrix.
		 *
		 * This function is called when searching for areas after applying a ceiling operation.
		 * Every time a non-zero pixel is found, it is called and plots the countour of the corresponding area.
		 *
		 * @param m
		 * @param i Column.
		 * @param j Line.
		 */
		void plotContour(Matrix &m, unsigned int i, unsigned int j);

		/**
		 * @brief Computes useful parameters from the values in the matrix.
		 *
		 * Computes : number of pixels, median pixel, sum of values.
		 *
		 * @param m
		 */
		void computeParameters(Matrix &m);

		/**
		 * @brief Prints the parameters to stdout.
		 *
		 */
		void printParameters();

		/**
		 * @brief Returns the biggest vertical size of the area.
		 *
		 * @return int Biggest height.
		 */
		int verticalSize();

		/**
		 * @brief Returns the median height pixel.
		 *
		 * @return int Median height.
		 */
		int getMedianHeight();

		/**
		 * @brief Returns the biggest horizontal size of the area.
		 *
		 * @return unsigned int Biggest width.
		 */
		unsigned int getWidth() const;

		/**
		 * @brief Returns the sum of the values in the area.
		 *
		 * Used for computing mean power.
		 *
		 * @return double Sum of values.
		 */
		double getSumOfValues() const;

		/**
		 * @brief Returns the number of pixels in the area.
		 *
		 * @return unsigned int Number of pixels.
		 */
		unsigned int getNumPixels() const;

		/**
		 * @brief Returns the point which yields the biggest value in the area.
		 *
		 * @return const Point Point with the max. value in the area.
		 */
		const Point getMax() const;

	private:
		Matrix &m; /**< TODO */
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
