#include "matrix.h"

Matrix::Matrix(const size_type cols, const size_type rows):
	_values(cols, std::vector<double>(rows)),
	_mask(cols, std::vector<double>(rows))
{

}

Matrix::Matrix():
	_values(0, std::vector<double>(0)),
	_mask(0, std::vector<double>(0))
{

}

Matrix &Matrix::operator=(const Matrix &xm)
{
	_values.clear();
	_mask.clear();
	std::copy(xm._values.begin(), xm._values.end(), _values.begin());
	std::copy(xm._mask.begin(), xm._mask.end(), _mask.begin());

	return *this;
}
//Matrix &Matrix::operator=(Matrix &&xm)
//{
//	_mask = std::move(xm._mask);
//	_values = std::move(xm._values);

//	return *this;
//}

std::vector<double> &Matrix::operator[](Matrix::size_type n)
{
	return _values[n];
}

Matrix::size_type Matrix::size()
{
	return _values.size();
}

bool Matrix::is_adjacent_to_zero(const size_type i, const size_type j)
{
	return (
			   //~ m[i+1][j+1] == 0 ||
			   (_values[i + 1][j])   == 0 ||
			   //~ m[i+1][j-1] == 0 ||
			   _values[i][j + 1]   == 0 ||
			   _values[i][j - 1]   == 0 ||
			   //~ m[i-1][j+1] == 0 ||
			   _values[i - 1][j]   == 0
			   //~ m[i-1][j-1] == 0
		   )
		   &&
		   (
			   _values[i][j] != 0 &&
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
