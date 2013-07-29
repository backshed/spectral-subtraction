#ifndef MATRIX_H
#define MATRIX_H

#include <vector>
#include "point.h"
#include "defines.h"



/**
 * @brief
 *
 */
class Matrix : public std::vector<std::vector<double>>
{
	public:
		Matrix(size_type cols, size_type rows);
		/**
		 * @brief
		 *
		 * @param i
		 * @param j
		 * @return bool
		 */
		/**
		 * @brief
		 *
		 * @param value
		 */
		bool is_adjacent_to_zero(int i, int j);
		/**
		 * @brief
		 *
		 * @param i
		 * @param j
		 * @param x0
		 * @return Point
		 */
		Point next_adjacent_to_zero(unsigned int i, unsigned int j, unsigned int x0);

		/**
		 * @brief
		 *
		 * @return const std::vector<std::vector<double> >
		 */
		const std::vector<std::vector<double> > &getMask() const;


		/**
		 * @brief
		 *
		 * @return uint
		 */
		uint getFirstCol() const;
		/**
		 * @brief
		 *
		 * @param value
		 */
		void setFirstCol(const uint &value);

		/**
		 * @brief
		 *
		 * @return uint
		 */
		uint getNumCols() const;
		/**
		 * @brief
		 *
		 * @param value
		 */
		void setNumCols(const uint &value);

		/**
		 * @brief
		 *
		 * @return uint
		 */
		uint getFirstRow() const;
		/**
		 * @brief
		 *
		 * @param value
		 */
		void setFirstRow(const uint &value);

		/**
		 * @brief
		 *
		 * @return uint
		 */
		uint getNumRows() const;
		/**
		 * @brief
		 *
		 * @param value
		 */
		void setNumRows(const uint &value);

		const unsigned int mask_value = 2; /**< TODO */

		/**
		 * @brief
		 *
		 * @param i
		 * @param j
		 * @return bool
		 */
		bool isMasked(unsigned int i, unsigned int j);
		/**
		 * @brief
		 *
		 * @param i
		 * @param j
		 */
		void mask(unsigned int i, unsigned int j);
		/**
		 * @brief
		 *
		 * @param i
		 * @param j
		 */
		void unmask(unsigned int i, unsigned int j);

	private:
		uint firstCol; /**< TODO */
		uint numCols; /**< TODO */
		uint firstRow; /**< TODO */
		uint numRows; /**< TODO */

		std::vector<std::vector<double>> _mask; /**< TODO */
};


#endif // MATRIX_H
