#include "matrix.h"

MaskedMatrix::MaskedMatrix(const size_type cols, const size_type rows):
	_values(cols, std::vector<double>(rows)),
	_mask(cols, std::vector<double>(rows))
{

}

MaskedMatrix::MaskedMatrix():
	_values(0, std::vector<double>(0)),
	_mask(0, std::vector<double>(0))
{

}

MaskedMatrix &MaskedMatrix::operator=(const MaskedMatrix &xm)
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

std::vector<double> &MaskedMatrix::operator[](MaskedMatrix::size_type n)
{
	return _values[n];
}

MaskedMatrix::size_type MaskedMatrix::size()
{
	return _values.size();
}

bool MaskedMatrix::is_adjacent_to_zero(const size_type i, const size_type j)
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

Point MaskedMatrix::next_adjacent_to_zero(const size_type i, const size_type j, const size_type  x0)
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

const std::vector<std::vector<double>> &MaskedMatrix::getMask() const
{
	return _mask;
}

unsigned int MaskedMatrix::getColPadding() const
{
	return _colPadding;
}

void MaskedMatrix::setColPadding(const size_type &value)
{
	_colPadding = value;
}

unsigned int MaskedMatrix::getRowPadding() const
{
	return _rowPadding;
}

void MaskedMatrix::setRowPadding(const size_type &value)
{
	_rowPadding = value;
}

bool MaskedMatrix::isMasked(const size_type i, const size_type j)
{
	return _mask[i][j] == mask_value;
}

void MaskedMatrix::mask(const size_type i, const size_type j)
{
	_mask[i][j] = mask_value;
}

void MaskedMatrix::unmask(const size_type i, const size_type j)
{
	_mask[i][j] = 0;
}
