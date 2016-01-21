/*++++++++++++++++++++++++++
 **	DlMatrix.h
 **
 **	Purpose:
 **
 **		Define matrix objects.
 **
 *	Copyright:	COPYRIGHT (C) 1991-2016 By David C. Salmon.  *WORLDWIDE RIGHTS RESERVED*
 *
 **	Author:		David C. Salmon
 **	Created:	Sat, Mar 2, 1991
 **	Modified:
 */
/*
 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation; either version 2 of the License, or
 (at your option) any later version.
 
 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.
 
 You should have received a copy of the GNU General Public License along
 with this program; if not, write to the Free Software Foundation, Inc.,
 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#ifndef _H_DlMatrix
#define _H_DlMatrix

#include "DlArray.h"
#include <string>
#include "recipes.h"

namespace DlArray {
	
template <class T, DlUInt32 OFFSET=kDefaultArrayOffset> class DlMatrix;
template <class T, DlUInt32 OFFSET=kDefaultArrayOffset> class DlMatrixTranspose;

// define an object representing the multiplication or m1 and m2
template <class T, DlUInt32 OFFSET=kDefaultArrayOffset> class DlMatMatMul
{
public:
	DlMatMatMul(const DlMatrix<T, OFFSET> &m1, const DlMatrix<T, OFFSET> &m2)
		: _m1(m1), _m2(m2) {}
	
	DlUInt32 rows() const;
	DlUInt32 cols() const;
	std::valarray<T>	perform() const;
private:
	const DlMatrix<T, OFFSET>& _m1;
	const DlMatrix<T, OFFSET>& _m2;
};

// define an object representing the multiplication of m1 by the transpose of m2.
template <class T, DlUInt32 OFFSET=kDefaultArrayOffset> class DlMatTMatMul
{
public:
	DlMatTMatMul(const DlMatrix<T, OFFSET> &m1, const DlMatrixTranspose<T, OFFSET> &m2)
		: _m1(m1), _m2(m2.matrix()) {}
	
	DlUInt32 rows() const;
	DlUInt32 cols() const;
	std::valarray<T>	perform() const;
private:
	const DlMatrix<T, OFFSET>& _m1;
	const DlMatrix<T, OFFSET>& _m2;
};

// define an object representing the multiplication of the transpose of m1 by m2.
template <class T, DlUInt32 OFFSET=kDefaultArrayOffset> class DlTMatMatMul
{
public:
	DlTMatMatMul(const DlMatrixTranspose<T, OFFSET>& m1, const DlMatrix<T, OFFSET> &m2)
		: _m1(m1.matrix()), _m2(m2) {}
	
	DlUInt32 rows() const;
	DlUInt32 cols() const;
	std::valarray<T>	perform() const;
private:
	const DlMatrix<T, OFFSET>& _m1;
	const DlMatrix<T, OFFSET>& _m2;
};

// define an object representing a matrix.
template <class T, DlUInt32 OFFSET> class DlMatrix
{
	typedef std::valarray<T>	arrType;
	
public:
	
	// empty matrix
	DlMatrix(DlUInt32 nRows, DlUInt32 nCols)
		: _nr(nRows)
		, _nc(nCols)
		, _arr(nRows * nCols)
	{}

	// matrix with c-style array of values
	DlMatrix(const T* vals, DlUInt32 nRows, DlUInt32 nCols)
		: _nr(nRows)
		, _nc(nCols)
		, _arr(vals, nRows * nCols)
	{}

	// matrix with values in arr
	DlMatrix(const std::valarray<T>& arr, DlUInt32 nRows, DlUInt32 nCols)
		: _nr(nRows)
		, _nc(nCols)
		, _arr(arr)
	{
		_RecipesAssert(_nr * _nc <= _arr.size());
	}
	
	// matrix from initializer list
	DlMatrix(const std::initializer_list<std::initializer_list<T>>& arr)
		: _nr(static_cast<DlUInt32>(arr.size()))
		, _nc(static_cast<DlUInt32>(arr.begin()->size()))
		, _arr(_nr * _nc)
	{
		_RecipesAssert(_arr.size() == _nr * _nc);
		DlUInt32 ofs = 0;
		for (auto r : arr)
			for (auto c: r)
				_arr[ofs++] = c;
	}
	
	// matrix from copy
	DlMatrix(const DlMatrix<T, OFFSET>& m)
		: _nr(m._nr)
		, _nc(m._nc)
		, _arr(m._arr) {
#if DEBUG
		printf("DlMatrix::copy constructor\n");
#endif
	}
	
	// matrix from move
	DlMatrix(DlMatrix<T, OFFSET>&& m)
		: _nr(m._nr)
		, _nc(m._nc)
		, _arr(std::move(m._arr)) {
			m._nr = 0;
			m._nc = 0;
#if DEBUG
		printf("DlMatrix::move constructor\n");
#endif
	}

	// matrix from matrix product.
	DlMatrix(DlMatMatMul<T, OFFSET>&& mul)
		: _nr(mul.rows())
		, _nc(mul.cols())
		, _arr(std::move(mul.perform())) {
#if DEBUG
		printf("DlMatrix::DlMatTMatMul move\n");
#endif
		}

	// matrix from matrix product.
	DlMatrix(DlMatTMatMul<T, OFFSET>&& mul)
		: _nr(mul.rows())
		, _nc(mul.cols())
		, _arr(std::move(mul.perform())) {
#if DEBUG
		printf("DlMatrix::DlMatTMatMul move\n");
#endif
	}

	// matrix from matrix product.
	DlMatrix(DlTMatMatMul<T, OFFSET>&& mul)
		: _nr(mul.rows())
		, _nc(mul.cols())
		, _arr(std::move(mul.perform())) {
#if DEBUG
		printf("DlMatrix::DlTMatMatMul move\n");
#endif
	}

	// set this matrix equal to m.
	DlMatrix<T, OFFSET>& operator=(const DlMatrix<T, OFFSET>& m) {
#if DEBUG
		printf("DlMatrix::copy assignment\n");
#endif
		if (this != &m) {

			_nr = m._nr;
			_nc = m._nc;
			_arr = m._arr;
		}
		return *this;
	}

	// set this matrix equal to m.
	DlMatrix<T, OFFSET>& operator=(DlMatrix<T, OFFSET>&& m) {
#if DEBUG
		printf("DlMatrix::move assignment\n");
#endif
		if (this != &m) {
			_nr = m._nr;
			_nc = m._nc;
			_arr = std::move(m._arr);
		}
		return *this;
	}

//	~DlMatrix() { delete _arr; }

	DlUInt32 rows() const {return _nr; }
	DlUInt32 cols() const {return _nc; }
	
	DlMatMatMul<T, OFFSET> operator*(const DlMatrix<T, OFFSET>& m) const;
	DlMatTMatMul<T, OFFSET> operator*(const DlMatrixTranspose<T, OFFSET>& m) const;
	
	DlSliceIter<T, OFFSET> row(DlUInt32 i);
	DlSliceIter<T, OFFSET> col(DlUInt32 i);

	DlSliceConstIter<T, OFFSET> row(DlUInt32 i) const;
	DlSliceConstIter<T, OFFSET> col(DlUInt32 i) const;

	DlSliceIter<T, OFFSET> operator[] (DlUInt32 i) { return row(i); }
	DlSliceConstIter<T, OFFSET> operator[] (DlUInt32 i) const { return row(i); }

	arrType& array() { return _arr; }
	arrType release() { return _arr; }

private:

	DlUInt32 	_nr;
	DlUInt32 	_nc;
	arrType		_arr;
};

	// define matrix transpose
template <class T, DlUInt32 OFFSET>
class DlMatrixTranspose
{
public:
	DlMatrixTranspose(const DlMatrix<T, OFFSET>& m) : _m(m) {}
	
	const DlMatrix<T, OFFSET>& matrix() const { return _m; }
	
	DlTMatMatMul<T, OFFSET> operator*(const DlMatrix<T, OFFSET>& m) const;
	
private:
	const DlMatrix<T, OFFSET>& _m;

};
}

// ============================================================================
// DlMatrix<T, OFFSET>
// ============================================================================

template <class T, DlUInt32 OFFSET> inline
DlArray::DlSliceIter<T, OFFSET>
DlArray::DlMatrix<T, OFFSET>::row(DlUInt32 i)
{
	return DlSliceIter<T, OFFSET>(_arr, slice((i - OFFSET) * _nc, _nc, 1));
}

template <class T, DlUInt32 OFFSET> inline
DlArray::DlSliceConstIter<T, OFFSET>
DlArray::DlMatrix<T, OFFSET>::row(DlUInt32 i) const
{
	return DlSliceConstIter<T, OFFSET>(_arr, slice((i - OFFSET) * _nc, _nc, 1));
}

template <class T, DlUInt32 OFFSET> inline
DlArray::DlSliceIter<T, OFFSET>
DlArray::DlMatrix<T, OFFSET>::col(DlUInt32 i)
{
	return DlSliceIter<T, OFFSET>(_arr, slice((i - OFFSET), _nr, _nc));
}

template <class T, DlUInt32 OFFSET> inline
DlArray::DlSliceConstIter<T, OFFSET>
DlArray::DlMatrix<T, OFFSET>::col(DlUInt32 i) const
{
	return DlSliceConstIter<T, OFFSET>(_arr, slice((i - OFFSET), _nr, _nc));
}

template <class T, DlUInt32 OFFSET> inline
DlArray::DlMatMatMul<T, OFFSET>
DlArray::DlMatrix<T, OFFSET>::operator*(const DlMatrix<T, OFFSET>& m) const
{
	if (cols() != m.rows())
		throw RecipesError("matrices are not the right dimensions");
	return DlMatMatMul<T, OFFSET>(*this, m);
}

template <class T, DlUInt32 OFFSET> inline
DlArray::DlMatTMatMul<T, OFFSET>
DlArray::DlMatrix<T, OFFSET>::operator*(const DlMatrixTranspose<T, OFFSET>& m) const
{
	if (cols() != m.matrix().cols())
		throw RecipesError("matrices are not the right dimensions");
	return DlMatTMatMul<T, OFFSET>(*this, m);
}

// ============================================================================
// DlMatrixTranspose<T, OFFSET>
//	transpose
// ============================================================================

template <class T, DlUInt32 OFFSET> inline
DlArray::DlTMatMatMul<T, OFFSET>
DlArray::DlMatrixTranspose<T, OFFSET>::operator*(const DlMatrix<T, OFFSET>& m) const
{
	if (_m.cols() != m.cols())
		throw RecipesError("matrices are not the right dimensions");
	return DlTMatMatMul<T, OFFSET>(*this, m);
}

// ============================================================================
// DlMatMatMul<T, OFFSET>
//	matrix multiplication
// ============================================================================

template <class T, DlUInt32 OFFSET> inline
DlUInt32
DlArray::DlMatMatMul<T, OFFSET>::rows() const { return _m1.rows(); }

template <class T, DlUInt32 OFFSET> inline
DlUInt32
DlArray::DlMatMatMul<T, OFFSET>::cols() const { return _m2.cols(); }

template <class T, DlUInt32 OFFSET> inline
std::valarray<T>
DlArray::DlMatMatMul<T, OFFSET>::perform() const
{
	DlUInt32 rows = _m1.rows();
	DlUInt32 cols = _m2.cols();
	DlUInt32 mid = _m1.cols();
	DlMatrix<T, OFFSET>	m(rows, cols);
	
	for (DlUInt32 i = 1; i <= rows; i++) {
	
		DlSliceIter<T, OFFSET> outRow(m.row(i));
		DlSliceConstIter<T, OFFSET> r(_m1.row(i));
		
		for (DlUInt32 j = 1; j <= cols; j++) {

			DlSliceConstIter<T, OFFSET> c(_m2.col(j));
			*outRow = 0;
			r.reset();
			for (DlUInt32 k = 1; k <= mid; k++) {
				*outRow += *r * *c;
				++r;
				++c;
			}
			
			++outRow;
		}
	}
	
	return m.release();
}

// ============================================================================
// DlMatTMatMul<T, OFFSET>
//	matrix transpose multiplication
// ============================================================================

template <class T, DlUInt32 OFFSET> inline
DlUInt32
DlArray::DlMatTMatMul<T, OFFSET>::rows() const { return _m1.rows(); }

template <class T, DlUInt32 OFFSET> inline
DlUInt32
DlArray::DlMatTMatMul<T, OFFSET>::cols() const { return _m2.rows(); }

template <class T, DlUInt32 OFFSET> inline
std::valarray<T>
DlArray::DlMatTMatMul<T, OFFSET>::perform() const
{
	DlUInt32 rows = _m1.rows();
	DlUInt32 cols = _m2.rows();
	DlUInt32 mid = _m1.cols();
	DlMatrix<T, OFFSET>	m(rows, cols);
	
	for (DlUInt32 i = 1; i <= rows; i++) {
	
		DlSliceIter<T, OFFSET> outRow(m.row(i));
		DlSliceConstIter<T, OFFSET> r(_m1.row(i));
		
		for (DlUInt32 j = 1; j <= cols; j++) {

			DlSliceConstIter<T, OFFSET> c(_m2.row(j));
			*outRow = 0;
			r.reset();
			for (DlUInt32 k = 1; k <= mid; k++) {
				*outRow += *r * *c;
				++r;
				++c;
			}
			
			++outRow;
		}
	}
	
	return m.release();
}

// ============================================================================
// DlTMatMatMul<T, OFFSET>
//	matrix transpose multiplication
// ============================================================================

template <class T, DlUInt32 OFFSET> inline
DlUInt32
DlArray::DlTMatMatMul<T, OFFSET>::rows() const { return _m1.cols(); }

template <class T, DlUInt32 OFFSET> inline
DlUInt32
DlArray::DlTMatMatMul<T, OFFSET>::cols() const { return _m2.cols(); }

template <class T, DlUInt32 OFFSET> inline
std::valarray<T>
DlArray::DlTMatMatMul<T, OFFSET>::perform() const
{
	DlUInt32 rows = _m1.cols();
	DlUInt32 cols = _m2.cols();
	DlUInt32 mid = _m1.rows();
	DlMatrix<T, OFFSET>	m(rows, cols);
	
	for (DlUInt32 i = 1; i <= rows; i++) {
	
		DlSliceIter<T, OFFSET> outRow(m.col(i));
		DlSliceConstIter<T, OFFSET> r(_m1.col(i));
		
		for (DlUInt32 j = 1; j <= cols; j++) {

			DlSliceConstIter<T, OFFSET> c(_m2.col(j));
			*outRow = 0;
			r.reset();
			for (DlUInt32 k = 1; k <= mid; k++) {
				*outRow += *r * *c;
				++r;
				++c;
			}
			
			++outRow;
		}
	}
	
	return m.release();
}

#endif

