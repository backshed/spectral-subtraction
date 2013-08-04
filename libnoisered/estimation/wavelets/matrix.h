#ifndef MATRIX_H
#define MATRIX_H

#include <vector>
#include "point.h"
#include "defines.h"


//TODO : make a pure matrix class and inherit from it to serve CWT purposes.
/**
 * @brief Represents a double-valued matrix.
 *
 * Access is in column - row fashion, because this is more efficient for the algorithm used and for cache locality.
 *
 */
class Matrix : public std::vector<std::vector<double>>
{
	public:
		/**
		 * @brief Constructor
		 *
		 * @param cols Number of columns.
		 * @param rows Number of rows.
		 */
		Matrix(size_type cols, size_type rows);

		/**
		 * @brief Returns true if a point is adjacent to zero.
		 *
		 * Doesn't check diagonals.
		 *
		 * @param i Column.
		 * @param j Line.
		 * @return bool True if matrix[i][j] is adjacent to 0.
		 */
		bool is_adjacent_to_zero(int i, int j);

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
		Point next_adjacent_to_zero(unsigned int i, unsigned int j, unsigned int x0);

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
		void setColPadding(const unsigned int &value);

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
		void setRowPadding(const unsigned int &value);

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
		bool isMasked(unsigned int i, unsigned int j);

		/**
		 * @brief Masks the value at (i, j).
		 *
		 * @param i Column.
		 * @param j Row.
		 */
		void mask(unsigned int i, unsigned int j);

		/**
		 * @brief Unmasks the value at (i, j).
		 *
		 * @param i Column.
		 * @param j Row.
		 */
		void unmask(unsigned int i, unsigned int j);

	private:
		unsigned int _colPadding; /**< TODO */
		unsigned int numCols; /**< TODO */
		unsigned int _rowPadding; /**< TODO */
		unsigned int numRows; /**< TODO */
		unsigned int _tmp_cols;
		unsigned int _tmp_rows;

		std::vector<std::vector<double>> _mask; /**< TODO */
};


#endif // MATRIX_H
