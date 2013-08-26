#pragma once
#include <vector>
#include "point.h"

/**
 * @brief Represents a double-valued matrix.
 *
 * Access is in column - row fashion, because this is more efficient for the algorithm used and for cache locality.
 *
 */
class MaskedMatrix
{
	public:
		typedef std::vector<std::vector<double>>::size_type size_type;
		/**
		 * @brief Constructor
		 *
		 * @param cols Number of columns.
		 * @param rows Number of rows.
		 */
		MaskedMatrix(const size_type cols, const size_type rows);
		MaskedMatrix();

		MaskedMatrix& operator=(const MaskedMatrix&);
		//Matrix& operator=(Matrix&&);
		std::vector<double>& operator[](size_type n);
		size_type size();
		/**
		 * @brief Returns true if a point is adjacent to zero.
		 *
		 * Doesn't check diagonals.
		 *
		 * @param i Column.
		 * @param j Line.
		 * @return bool True if matrix[i][j] is adjacent to 0.
		 */
		bool is_adjacent_to_zero(const size_type i, const size_type j);

		/**
		 * @brief Returns one of the nearest unexplored point which is adjacent to zero.
		 *
		 * The point must have a value > 0.
		 * If there is no satisfying point, one with the invalid status set to true will be returned.
		 *
		 * @param i Column.
		 * @param j Line.
		 * @param x0 Left limit.
		 *
		 * @return Point A Point which satisfies the conditions.
		 */
		Point next_adjacent_to_zero(const size_type i, const size_type j, const size_type x0);

		/**
		 * @brief Returns the mask of the matrix.
		 *
		 * @return const std::vector<std::vector<double>> Corresponding mask.
		 */
		const std::vector<std::vector<double>> &getMask() const;


		/**
		 * @brief Returns the column at which the data starts.
		 *
		 * There is a padding around the data (usually 2 pixels) to allow the algorithm to work.
		 *
		 * @return unsigned int Column.
		 */
		unsigned int getColPadding() const;

		/**
		 * @brief Sets the column at which the data starts.
		 *
		 * @param value Column.
		 */
		void setColPadding(const size_type  &value);

		/**
		 * @brief Returns the row at which the data starts.
		 *
		 * There is a padding around the data (usually 2 pixels) to allow the algorithm to work.
		 *
		 * @return unsigned int Row.
		 */
		unsigned int getRowPadding() const;

		/**
		 * @brief Sets the row at which the data starts.
		 *
		 * @param value Row.
		 */
		void setRowPadding(const size_type  &value);

		const unsigned int mask_value = 2; /**< TODO */

		/**
		 * @brief Returns true if the value at (i, j) is masked.
		 *
		 * The mask is used to delimit areas.
		 *
		 * @param i Column.
		 * @param j Row.
		 * @return bool True if matrix[i][j] is masked.
		 */
		bool isMasked(const size_type i, const size_type j);

		/**
		 * @brief Masks the value at (i, j).
		 *
		 * @param i Column.
		 * @param j Row.
		 */
		void mask(const size_type i, const size_type j);

		/**
		 * @brief Unmasks the value at (i, j).
		 *
		 * @param i Column.
		 * @param j Row.
		 */
		void unmask(const size_type i, const size_type j);

	private:
		size_type _colPadding = 0; /**< TODO */
		size_type _rowPadding = 0; /**< TODO */

		std::vector<std::vector<double>> _values; /**< TODO */
		std::vector<std::vector<double>> _mask; /**< TODO */

};
