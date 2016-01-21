/*+++++++++++++++++++++++
**	gaussElim.c
**
**	Purpose:	Perform a gauss elimination on the given matrix and form
**				the solution vectors. The input matrix and original rhs
**				are replaced with solution stuff.
**
 *	Copyright:	COPYRIGHT (C) 1990-2016 By David C. Salmon.  *WORLDWIDE RIGHTS RESERVED*
 *
**	Author:		David C. Salmon
**	Created:	1990
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

#include "DlPlatform.h"
#include "matrix.h"

using namespace DlArray;
using namespace std;

static void	elim(DlMatrix<DlFloat>& m, DlMatrix<DlFloat>& x, 
				valarray<DlFloat>&scale, valarray<DlInt32>& ord);
static void	subst(const DlMatrix<DlFloat>& m, DlMatrix<DlFloat>& x, const valarray<DlInt32>& ord);

/*--------------------------------
**	gaussElim( a, n, x, m, o )
**
**		Perform gaussian elimination of the matrix a.
**		The matrix a is destroyed, the matrix x is replaced by the
**		solution. o is a list of indices into the solution matrix
**		which maps to the original order.
**	DlFloat	**a;		The input matrix
**	DlFloat	**x;		The matrix of rhs
**	DlInt32		n, m;	n = size of a, m = # cols of x
**	DlInt32		*ord;	a vector of indexes
*/
void
gaussElim(DlMatrix<DlFloat>& m, DlMatrix<DlFloat>& x, 
	valarray<DlInt32>& ord)
{
	DlInt32 n = m.rows();
	ord.resize(n);
	valarray<DlFloat>	scale(n);
	
	order(m, scale, ord);
	elim( m, x, scale, ord );
	subst(m, x, ord );
	
//	free_fvector( s, 1, n );
//	return( rVal );
}


/*------------------------
**	elim
*/

static void	
elim(DlMatrix<DlFloat>& m, DlMatrix<DlFloat>& x, valarray<DlFloat>&scale, valarray<DlInt32>& ord)
{
	DlInt32 n = m.rows();
	DlInt32 mm = x.rows();
	
	DlInt32 fixCount = 0;
	DlOneBasedIter<DlFloat>			s(scale);
	DlOneBasedConstIter<DlInt32>	o(ord);
	
	
//	DlInt32		i, j, k;
//	DlInt32		pivrow, irow;
//	DlFloatEx	fact1, factor;
//	DlFloat	*aPtr, *pivPtr;
//	DlInt32		anErr = 0;

	for (DlInt32 k = 1; k <= n-1; ++k) {

		/*	Find the pivot element for row k. This element is o[k].
			*/
	
		pivot(m, scale, ord, k);
		DlInt32 pivrow = o[k];
		
		DlSliceIter<DlFloat> row(m.row(pivrow));
		DlSliceIter<DlFloat> xPivPtr(x.row(pivrow));
		
		if (row[k] == 0.0) {
			row[k] = TINY;
			fixCount++;
		}

		DlFloatEx fact1 = 1/row[k];

		/*	For each of the remaining rows, subtract each element of 
			the pivot row, multiplied by the column value over the pivot value,
			from each element in the row. Do the same to the solution vectors.
			*/

		for (DlInt32 i = k+1; i <= n; ++i) {
			DlInt32 irow = o[i];
			DlSliceIter<DlFloat> aPtr(m.row(irow));
			
			DlFloatEx factor = aPtr[k] * fact1;
			if (factor != 0) {
				for (DlInt32 j = k+1; j <= n; ++j)
					aPtr[j] -= factor * row[j];
			
//			aPtr = a[irow]+k;
//			pivPtr = a[pivrow]+k+1;
//			factor = (*aPtr++) * fact1;	/* factor for row irow */

//			for ( j = k+1; j <= n; j++, aPtr++, pivPtr++ )
//				(*aPtr) -= factor * (*pivPtr);

				DlSliceIter<DlFloat> xPtr(x.row(irow));
				for (DlInt32 j = 1; j <= mm; j++)
					xPtr[j] -= factor * xPivPtr[j];
			}
//			aPtr = x[irow]+1;
//			pivPtr = x[pivrow]+1;
//			for ( j = 1; j <= m; j++, aPtr++, pivPtr++ )
//				(*aPtr) -= factor * (*pivPtr);
		}
	}
//	return( anErr );
}

static void
subst(const DlMatrix<DlFloat>& m, DlMatrix<DlFloat>& x, const valarray<DlInt32>& ord)
{
	DlInt32 n = m.rows();
	DlInt32 mm = x.rows();
	DlOneBasedConstIter<DlInt32> o(ord);

	for (DlInt32 k = 1; k <= mm; k++) {
		DlInt32 irow = o[n];
		DlSliceConstIter<DlFloat> row(m.row(irow));
		DlSliceIter<DlFloat> xrow(x.row(irow));
		
		if (row[n] == 0.0) 
			throw EqSolveFailure(EqSolveFailure::Singular, k);
		xrow[k] = xrow[k] / row[n];
		
		for (DlInt32 i = n-1; i > 0; i--) {
			irow = o[i];
			DlSliceConstIter<DlFloat> row(m.row(irow));
			if (row[i] == 0.0) 
				throw EqSolveFailure(EqSolveFailure::Singular,  i);
			DlFloatEx sum = 0.0;
			DlSliceIter<DlFloat> xCol(x.col(k));
			for (DlInt32 j = i+1; j <= n; j++ )
				sum += row[j] * xCol[o[j]];

			xCol[irow] = (xCol[irow] - sum) / row[i];
		}
	}

}

#if 0
static DlStatus
elim(DlFloat** a, DlInt32 n, DlFloat** x, DlInt32 m, DlFloat* s, DlInt32* o)
{
	DlInt32		i, j, k;
	DlInt32		pivrow, irow;
	DlFloatEx	fact1, factor;
	DlFloat	*aPtr, *pivPtr;
	DlInt32		anErr = 0;
	
	for ( k = 1; k <= n-1; k++ ) {
		
		/*	Find the pivot element for row k. This element is o[k].
		 */
		
		pivot( a, n, s, o, k );
		pivrow = o[k];
		if ( a[pivrow][k] == 0.0 ) {
			a[pivrow][k] = TINY;
			anErr = GAUSS_SINGULAR;
		}
		
		fact1 = 1/a[pivrow][k];
		
		/*	For each of the remaining rows, subtract each element of
		 the pivot row, multiplied by the column value over the pivot value,
		 from each element in the row. Do the same to the solution vectors.
		 */
		
		for ( i = k+1; i <= n; i++ ) {
			irow = o[i];
			aPtr = a[irow]+k;
			pivPtr = a[pivrow]+k+1;
			factor = (*aPtr++) * fact1;	/* factor for row irow */
			
			for ( j = k+1; j <= n; j++, aPtr++, pivPtr++ )
				(*aPtr) -= factor * (*pivPtr);
			
			aPtr = x[irow]+1;
			pivPtr = x[pivrow]+1;
			for ( j = 1; j <= m; j++, aPtr++, pivPtr++ )
				(*aPtr) -= factor * (*pivPtr);
		}
	}
	return( anErr );
}
#endif

#if 0
/*----------------------
**	subst
**
**		Perform the back substitution phase of the solution
*/

static DlStatus	
subst(DlFloat** a, DlInt32 n, DlFloat** x, DlInt32 m, DlInt32 *o)
{
	DlInt32		k, j, i, irow;
	DlFloatEx	sum;

	/*	Loop over the columns of the solution
		*/

	for ( k = 1; k <= m; k++ ) {
		irow = o[n];
		if ( a[irow][n] == 0.0 ) return( GAUSS_SINGULARBS );
		x[irow][k] = x[irow][k] / a[irow][n];
		
		for ( i = n-1; i > 0; i-- ) {
			irow = o[i];
			if ( a[irow][i] == 0.0 ) return( GAUSS_SINGULARBS );
			sum = 0.0;
			for ( j = i+1; j <= n; j++ )
				sum += a[irow][j] * x[o[j]][k];

			x[irow][k] = (x[irow][k] - sum) / a[irow][i];
		}
	}
	return( 0 );
}
#endif

