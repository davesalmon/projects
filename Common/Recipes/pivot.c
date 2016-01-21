/*+++++++++++++++++++++
**	pivot.c
**
**		Implement routines to perform pivoting on the
**		rows of a matrix.
**
**		order - initialize an index array and a scale array for
**				pivoting.
**
**		pivot - interchange the rows of the index array to perform
**				row swap on the matrix
**
**		unscramble-	take a rhs vector computed with the pivoting 
**					scheme and put it in the proper order.
**
**		The pivoting algorithm follows Chapra & Canale, "Numerical Methods
**		for Engineers", 2nd ed.
**
 *	Copyright:	COPYRIGHT (C) 1991-2016 By David C. Salmon.  *WORLDWIDE RIGHTS RESERVED*
 *
**		Author:		David C. Salmon
**		Created:	Mar. 2, 1991
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

#include "matio.h"

using namespace DlArray;

#if !defined(THINK_C)
#define	noErr	0
#endif

/*-------------------------------
**	order( aMatrix, n, scale, indx )
**
**		Initialize the index list, o, and the scale
**		vector s.
*/

void
order(const DlMatrix<DlFloat>& m, std::valarray<DlFloat>& scale, 
				std::valarray<DlInt32>& indx)
{
	_RecipesAssert(m.rows() == m.cols());

	DlInt32 n = m.rows();
	
	if (scale.size() < n)
		scale.resize(n);
	if (indx.size() < n)
		indx.resize(n);
	
	DlOneBasedIter<DlFloat> sPtr(scale);
	DlOneBasedIter<DlInt32> iPtr(indx);
	
	/*	Loop over the rows of a, and assemble the list
		of largest elements in each row. sPtr points to 
		the list. o is initialized with the original row numbers
		*/
	for (DlInt32 i = 1; i <= n; ++i) {
		DlSliceConstIter<DlFloat>	row(m.row(i));
		iPtr[i] = i;					/* indx gets row number */
				
		DlFloatEx big = fabs(row[1]);	/* set s to the first element */

		/*	Loop over the remaining columns to find any larger elements
			*/
		for (DlInt32 j = 2; j <= n; ++j)
		{
			DlFloat val = fabs(row[j]);
			if ( val > big ) 
				big = val;
		}

		if (big == 0.0) 
			throw EqSolveFailure(EqSolveFailure::Singular, i);
		
		sPtr[i] = 1.0 / big;
	}
}

#if 0
DlStatus
order(DlFloat** aMatrix, DlInt32 n, DlFloat* scale, DlInt32* indx)
{
	DlInt32				i, j;
	DlFloat	*sPtr, *aPtr;
	DlFloatEx	big;

	sPtr = scale+1;

	/*	Loop over the rows of a, and assemble the list
		of largest elements in each row. sPtr points to 
		the list. o is initialized with the original row numbers
		*/

	for ( i = 1, indx++; i <= n; i++, sPtr++, indx++ ) {
		(*indx) = i;					/* indx gets row number */
		aPtr = aMatrix[i]+1;			/* point to the first column in row i */
		big = fabs(*aPtr++);			/* set s to the first element */

		/*	Loop over the remaining columns to find any larger elements
			*/

		for ( j = 2; j <= n; j++, aPtr++ )
			if ( fabs(*aPtr) > big ) 
				big = fabs( *aPtr );

		if ( big == 0.0 ) 
			return( GEN_SINGULAR_MATRIX );
		(*sPtr) = 1.0 / big;
	}
	return( 0 );
}
#endif

/*-----------------------------
**	pivot
**
**		This is a general pivot routine, which when called with
**		order, will pivot on the optimum row in the matrix. The
**		swap is performed with an index array, rindx.
*/

bool	
pivot(const DlArray::DlMatrix<DlFloat>& m, std::valarray<DlFloat>& scale, 
			std::valarray<DlInt32>& rindx, DlInt32 actCol)
{
	_RecipesAssert(m.rows() == m.cols());

	DlInt32 n = m.rows();
	DlOneBasedIter<DlFloat> s(scale);
	DlOneBasedIter<DlInt32> indx(rindx);

	//	subtract the column start for index	
	DlInt32 pivit = actCol;
	DlInt32 irow = indx[actCol];

	//	add back the row start for the real row
//	DlSliceConstIter<DlFloat>	row(m.row(irow+rs));

	//	initize big
	DlFloatEx big = fabs(m[irow][actCol] * s[irow]);
	
	/*	Loop over remaining rows to find a scaled column value
		greater than the current one
		*/
	for (DlInt32 icol = actCol+1; icol <= n; ++icol) {
		irow = indx[icol];
		DlFloatEx dummy = fabs(m[irow][actCol] * s[irow]);
		if (dummy > big) {
			big = dummy;
			pivit = icol;
		}
	}

	/*	Swap the rows */
	if (pivit != actCol) {
		DlInt32 idum = indx[pivit];
		indx[pivit] = indx[actCol];
		indx[actCol] = idum;
		return true;
	}
	return false;
}

#if 0
/*-----------------------------
**	pivot
**
**		This is a general pivot routine, which when called with
**		order, will pivot on the optimum row in the matrix. The
**		swap is performed with an index array, rindx.
*/

void	
pivot(DlFloat** aMatrix, DlInt32 nCols, DlFloat* scale, DlInt32* rindx, DlInt32 actCol )
{
	DlInt32		pivit, icol, idum, irow;
	DlFloatEx	big, dummy;

	pivit = actCol;
	irow = rindx[actCol];
	big = fabs( aMatrix[irow][actCol] * scale[irow] );
	
	/*	Loop over remaining rows to find a scaled column value
		greater than the current one
		*/

	for ( icol = actCol+1; icol <= nCols; icol++ ) {
		irow = rindx[icol];
		dummy = fabs( aMatrix[irow][actCol] * scale[irow] );
		if ( dummy > big ) {
			big = dummy;
			pivit = icol;
		}
	}

	/*	Swap the rows */

	idum = rindx[pivit];
	rindx[pivit] = rindx[actCol];
	rindx[actCol] = idum;
}
#endif

/*----------------------
**	unscramble
**
**		unscramble the output vector from ludcmp and
**		gauss elim. This unscrambles the output inplace
**		starting with the first element and always replacing
**		the next element through recursive swapping
*/

void	
unscramble(std::valarray<DlFloat>& results, const std::valarray<DlInt32>& rindx)
{
	DlInt32 n = results.size();
	_RecipesAssert(n == rindx.size());

	DlOneBasedIter<DlFloat>	res(results);
	DlOneBasedConstIter<DlInt32> indx(rindx);

	for (DlInt32 i = 1; i <= n; ++i) {
		DlInt32 irow = indx[i];
		if (i != irow) {

			/*	As long a i > irow, the element in question
				has already been swapped, so we follow the
				swap trail.
				*/

			while (i > irow) 
				irow = indx[irow];
			DlFloatEx tmp = res[i];
			res[i] = res[irow];
			res[irow] = tmp;
		}
	}
}

void unscramble(DlArray::DlSliceIter<DlFloat, 1>& res, const std::valarray<DlInt32>& rindx)
{
	DlInt32 n = res.size();
	_RecipesAssert(n == rindx.size());
	
	DlOneBasedConstIter<DlInt32> indx(rindx);
	
	for (DlInt32 i = 1; i <= n; ++i) {
		DlInt32 irow = indx[i];
		if (i != irow) {
			
			/*	As long a i > irow, the element in question
			 has already been swapped, so we follow the
			 swap trail.
			 */
			
			while (i > irow)
				irow = indx[irow];
			DlFloatEx tmp = res[i];
			res[i] = res[irow];
			res[irow] = tmp;
		}
	}
}

void unscramble(DlArray::DlMatrix<DlFloat, 1>& m, const std::valarray<DlInt32>& indx)
{
	for (auto i = 1; i <= m.cols(); i++)
	{
		DlSliceIter<DlFloat> col(m.col(i));
		unscramble(col, indx);
	}
}


#if 0
/*----------------------
**	unscramble
**
**		unscramble the output vector from ludcmp and
**		gauss elim. This unscrambles the output inplace
**		starting with the first element and always replacing
**		the next element through recursive swapping
*/

void	
unscramble(DlFloat* results, DlInt32* indx, DlInt32 n)
{
	DlFloatEx	tmp;
	DlInt32	irow, i;

	for ( i = 1; i <= n; i ++ ) {
		irow = indx[i];
		if ( i != irow ) {

			/*	As long a i > irow, the element in question
				has already been swapped, so we follow the
				swap trail.
				*/

			while ( i > irow ) irow = indx[irow];
			tmp = results[i];
			results[i] = results[irow];
			results[irow] = tmp;
		}
	}
}
#endif
