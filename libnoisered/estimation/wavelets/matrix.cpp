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
	_values.assign(xm._values.begin(), xm._values.end());
	_mask.assign(xm._mask.begin(), xm._mask.end());

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

Point<MaskedMatrix::size_type> MaskedMatrix::next_adjacent_to_zero(const size_type i, const size_type j, const size_type  x0)
{
	if (is_adjacent_to_zero(i + 1UL, j + 1UL)) return Point<MaskedMatrix::size_type>(i + 1UL, j + 1UL);
	if (is_adjacent_to_zero(i + 1UL, j))   return Point<MaskedMatrix::size_type>(i + 1UL, j);
	if (is_adjacent_to_zero(i + 1UL, j - 1UL)) return Point<MaskedMatrix::size_type>(i + 1UL, j - 1UL);
	if (is_adjacent_to_zero(i, j + 1UL))   return Point<MaskedMatrix::size_type>(i,   j + 1UL);
	if (is_adjacent_to_zero(i, j - 1UL))   return Point<MaskedMatrix::size_type>(i,   j - 1UL);
	if (is_adjacent_to_zero(i - 1UL, j + 1UL) && i > x0) return Point<MaskedMatrix::size_type>(i - 1UL, j + 1UL);
	if (is_adjacent_to_zero(i - 1UL, j)   && i > x0) return Point<MaskedMatrix::size_type>(i - 1UL, j);
	if (is_adjacent_to_zero(i - 1UL, j - 1UL) && i > x0) return Point<MaskedMatrix::size_type>(i - 1UL, j - 1UL);

	// Let's hope it does not happen!
	// std::cout << "bad things happens next_adjacent_to_zero\n";
	return (Point<MaskedMatrix::size_type>(0, 0, true));
}

const std::vector<std::vector<double>> &MaskedMatrix::getMask() const
{
	return _mask;
}

MaskedMatrix::size_type MaskedMatrix::getColPadding() const
{
	return _colPadding;
}

void MaskedMatrix::setColPadding(const size_type &value)
{
	_colPadding = value;
}

MaskedMatrix::size_type MaskedMatrix::getRowPadding() const
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
