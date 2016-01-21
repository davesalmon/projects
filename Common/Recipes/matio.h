/*+
 *	matio.h
 *
 *	Purpose:	Interface for matrix io routines.
 *
 *	Copyright:	COPYRIGHT (C) 1990-2016 By David C. Salmon.  *WORLDWIDE RIGHTS RESERVED*
 *
 *	Author:		David C. Salmon
 *	Created:	1990
-*/
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

#if !defined(_H_matio)
#define	_H_matio

#include "DlMatrix.h"
#include	<stdio.h>

typedef struct VectorFormat {
	const char* first;
	const char* sep;
	const char* vsep;
	const char* last;
	const char* value;
} VectorFormat;

const VectorFormat kNormalVector = { "", "\t", "\n", "", "%14.9lg"};
const VectorFormat kMathVector = { "{", ", ", "};\n", "", "%.9lg"};
const VectorFormat kMathMatrix = { "{", ", ", "},\n", "};\n", "%.9lg"};

//----------------------------------------------------------------------------------------
//  PrintVector
//
//      print the vector.
//
//  DlInt32 start                          -> index to start
//  T& v                                   -> vector
//  const VectorFormat& fmt=kNormalVector  -> print format
//  FILE* f=stdout                         -> file
//
//  returns nothing
//----------------------------------------------------------------------------------------
template <class T>
void
PrintVector(DlInt32 start, T& v, 
		const VectorFormat& fmt=kNormalVector, FILE* f=stdout)
{
	DlUInt32 n = static_cast<DlUInt32>(v.size());
	
	fputs(fmt.first, f);
	for (DlInt32 i = 0; i < n; i++) {
		if (i != 0)
			fputs(fmt.sep, f);
		fprintf(f, fmt.value, (double)(v[i+start]));
	}
	fputs(fmt.vsep, f);
}

//----------------------------------------------------------------------------------------
//  PrintVector
//
//      print the vector.
//
//  const std::valarray<T>& v              -> valarray to print
//  const VectorFormat& fmt=kNormalVector  -> format
//  FILE* f=stdout                         -> file
//
//  returns nothing
//----------------------------------------------------------------------------------------
template <class T>
void
PrintVector(const std::valarray<T>& v, 
		const VectorFormat& fmt=kNormalVector, FILE* f=stdout)
{
	PrintVector(0L, v, fmt, f);
}

//----------------------------------------------------------------------------------------
//  PrintVector
//
//      Print the vector.
//
//  DlArray::DlSliceConstIter<T>& v        -> matrix vector
//  const VectorFormat& fmt=kNormalVector  -> format
//  FILE* f=stdout                         -> file
//
//  returns nothing
//----------------------------------------------------------------------------------------
template <class T>
void
PrintVector(DlArray::DlSliceConstIter<T>& v, 
		const VectorFormat& fmt=kNormalVector, FILE* f=stdout)
{
	PrintVector(1, v, fmt, f);
}

//----------------------------------------------------------------------------------------
//  PrintMatrix
//
//      Print the matrix
//
//  const DlArray::DlMatrix<T>& m          -> matrix
//  const VectorFormat& fmt=kNormalVector  -> format
//  FILE* f=stdout                         -> file
//
//  returns nothing
//----------------------------------------------------------------------------------------
template <class T>
void
PrintMatrix(const DlArray::DlMatrix<T>& m, 
		const VectorFormat& fmt=kNormalVector, FILE* f=stdout)
{
	fputs(fmt.first, f);
	DlInt32 rows = m.rows();
//	DlInt32 cols = m.cols();
	for (DlInt32 i = 1; i <= rows; ++i) {
		DlArray::DlSliceConstIter<T> r(m.row(i));
		PrintVector(r, fmt, f);
	}
	fputs(fmt.last, f);
}

//----------------------------------------------------------------------------------------
//  PrintMatrix
//
//      Print the matrix.
//
//  const DlArray::DlMatrixTranspose<T>& m -> matrix transpose
//  const VectorFormat& fmt=kNormalVector  -> format
//  FILE* f=stdout                         -> file
//
//  returns nothing
//----------------------------------------------------------------------------------------
template <class T>
void
PrintMatrix(const DlArray::DlMatrixTranspose<T>& m, 
		const VectorFormat& fmt=kNormalVector, FILE* f=stdout)
{
	fputs(fmt.first, f);

	DlInt32 rows = m.matrix().cols();
//	DlInt32 cols = m.rows();
	for (DlInt32 i = 1; i <= rows; ++i) {
		DlArray::DlSliceConstIter<T> r(m.matrix().col(i));
		PrintVector(r, fmt, f);
	}
	fputs(fmt.last, f);
}

//----------------------------------------------------------------------------------------
//  PrintMatrix
//
//      Print the matrix.
//
//  const DlArray::DlMatrix<T>& m          -> matrix
//  const std::valarray<DlInt32>& ord      -> row swap indices
//  const VectorFormat& fmt=kNormalVector  -> format
//  FILE* f=stdout                         -> file
//
//  returns nothing
//----------------------------------------------------------------------------------------
template <class T>
void
PrintMatrix(const DlArray::DlMatrix<T>& m, const std::valarray<DlInt32>& ord, 
		const VectorFormat& fmt=kNormalVector, FILE* f=stdout)
{
	fputs(fmt.first, f);
	DlInt32 nrows = m.rows();
//	DlInt32 ncols = m.cols();
	DlArray::DlOneBasedConstIter<DlInt32> o(ord);
	for (DlInt32 i = 1; i <= nrows; ++i) {
		DlArray::DlSliceConstIter<T> r(m.row(o[i]));
		PrintVector(r, fmt, f);
	}
	fputs(fmt.last, f);
}

#endif
