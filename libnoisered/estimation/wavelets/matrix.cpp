#include "matrix.h"

Matrix::Matrix(const size_type cols, const size_type rows):
	std::vector<std::vector<double>>(cols, std::vector<double>(rows)),
								  _mask(cols, std::vector<double>(rows))
{
	_tmp_cols = cols;
	_tmp_rows = rows;
}

bool Matrix::is_adjacent_to_zero(const size_type i, const size_type j)
{
	return (
			   //~ m[i+1][j+1] == 0 ||
			   (at(i + 1)[j])   == 0 ||
			   //~ m[i+1][j-1] == 0 ||
			   at(i)[j + 1]   == 0 ||
			   at(i)[j - 1]   == 0 ||
			   //~ m[i-1][j+1] == 0 ||
			   at(i - 1)[j]   == 0
			   //~ m[i-1][j-1] == 0
		   )
		   &&
		   (
			   at(i)[j] != 0 &&
			   !isMasked(i, j)
		   )
		   ;
}

Point Matrix::next_adjacent_to_zero(const size_type i, const size_type j, const size_type  x0)
{
	if (is_adjacent_to_zero(i + 1, j + 1)) return Point(i + 1, j + 1);
	if (is_adjacent_to_zero(i + 1, j))   return Point(i + 1, j);
	if (is_adjacent_to_zero(i + 1, j - 1)) return Point(i + 1, j - 1);
	if (is_adjacent_to_zero(i, j + 1))   return Point(i,   j + 1);
	if (is_adjacent_to_zero(i, j - 1))   return Point(i,   j - 1);
	if (is_adjacent_to_zero(i - 1, j + 1) && i > x0) return Point(i - 1, j + 1);
	if (is_adjacent_to_zero(i - 1, j)   && i > x0) return Point(i - 1, j);
	if (is_adjacent_to_zero(i - 1, j - 1) && i > x0) return Point(i - 1, j - 1);

	// Let's hope it does not happen!
	// std::cout << "bad things happens next_adjacent_to_zero\n";
	return (Point(0, 0, true));
}

const std::vector<std::vector<double>> &Matrix::getMask() const
{
	return _mask;
}

unsigned int Matrix::getColPadding() const
{
	return _colPadding;
}

void Matrix::setColPadding(const size_type &value)
{
	_colPadding = value;
}

unsigned int Matrix::getRowPadding() const
{
	return _rowPadding;
}

void Matrix::setRowPadding(const size_type &value)
{
	_rowPadding = value;
}

bool Matrix::isMasked(const size_type i, const size_type j)
{
	return _mask[i][j] == mask_value;
}

void Matrix::mask(const size_type i, const size_type j)
{
	_mask[i][j] = mask_value;
}

void Matrix::unmask(const size_type i, const size_type j)
{
	_mask[i][j] = 0;
}
