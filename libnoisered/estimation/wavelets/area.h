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
		Area();

		/**
		 * @brief Removes the area from the mask.
		 *
		 * @param m
		 */
		void removeArea(MaskedMatrix &m);

		/**
		 * @brief Fills the area on the mask.
		 *
		 * Used for debug purposes.
		 *
		 * @param m
		 */
		void plotArea(MaskedMatrix &m);

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
		void plotContour(MaskedMatrix &m, MaskedMatrix::size_type i, MaskedMatrix::size_type j);

		/**
		 * @brief Computes useful parameters from the values in the matrix.
		 *
		 * Computes : number of pixels, median pixel, sum of values.
		 *
		 * @param m
		 */
		void computeParameters(MaskedMatrix &m);

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
		MaskedMatrix::size_type verticalSize();

		/**
		 * @brief Returns the median height pixel.
		 *
		 * @return int Median height.
		 */
		MaskedMatrix::size_type getMedianHeight();

		/**
		 * @brief Returns the biggest horizontal size of the area.
		 *
		 * @return unsigned int Biggest width.
		 */
		MaskedMatrix::size_type getWidth() const;

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
		const Point<MaskedMatrix::size_type> getMax() const;

	private:
		Point<MaskedMatrix::size_type> max_pt = Point<MaskedMatrix::size_type>(); /**< TODO */
		MaskedMatrix::size_type minHeight = 0; /**< TODO */
		MaskedMatrix::size_type maxHeight = 0; /**< TODO */
		MaskedMatrix::size_type x0 = 0; /**< TODO */
		MaskedMatrix::size_type length = 0; /**< TODO */
		std::vector<Pair<MaskedMatrix::size_type>> pairList = std::vector<Pair<MaskedMatrix::size_type>>(); /**< TODO */

		double sumOfValues = 0; /**< TODO */
		unsigned int numPixels = 0; /**< TODO */

		MaskedMatrix::size_type medianHeight = 0; /**< TODO */
};
