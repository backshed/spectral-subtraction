#ifndef MATRIX_H
#define MATRIX_H

#include <vector>
#include "point.h"
#include "defines.h"



class Matrix : public std::vector<std::vector<double>>
{
	public:
		Matrix(size_type cols, size_type rows);
		bool is_adjacent_to_zero(int i, int j);
		Point next_adjacent_to_zero(unsigned int i, unsigned int j, unsigned int x0);



		uint getFirstCol() const;
		void setFirstCol(const uint &value);

		uint getNumCols() const;
		void setNumCols(const uint &value);

		uint getFirstRow() const;
		void setFirstRow(const uint &value);

		uint getNumRows() const;
		void setNumRows(const uint &value);

		const unsigned int mask_value = 2;

		bool isMasked(unsigned int i, unsigned int j);
		void mask(unsigned int i, unsigned int j);
		void unmask(unsigned int i, unsigned int j);

	private:
		uint firstCol;
		uint numCols;
		uint firstRow;
		uint numRows;

		std::vector<std::vector<double>> _mask;
};


#endif // MATRIX_H
