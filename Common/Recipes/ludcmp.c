/*+++++++++++++++++++++++++++++++
**	ludcmp.c
**
**		Purpose:	lu decomposition routines
**
 *	Copyright:	COPYRIGHT (C) 1989-2016 By David C. Salmon.  *WORLDWIDE RIGHTS RESERVED*
 *
**		Author:		David C. Salmon
**		Created:	Sat, Apr 8, 1989
**		Modified:	Sat, Mar 2, 1991
**
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

#if !defined(THINK_C)
#define	noErr	0
#endif

using namespace std;
using namespace DlArray;

/*
**	---------------------------------------------------
**	ludcmp:	LU decomposition routine
**		Input:
**			a[1..n] 	- address of an array of pointers to vectors
**						of n DlFloats. 
**			n 			- rank of matrix.
**			indx		- array of int
**		Output:
**			a			- decomposed matrix
**			indx		- records row permutations of partial pivoting.
**			d			- records odd or even row interchanges.
*/

void
ludcmp(DlMatrix<DlFloat>& m, valarray<DlInt32>& index, DlFloat& d)
{
//	DlInt32		i, imax, j, k, irow;
//	DlFloat	big, dum, sum;
//	DlFloat	*vv;
//	DlStatus	anErr;
	
	DlInt32 n = m.rows();
	/*	Grab space for temp vector vv.
		*/
	valarray<DlFloat> tmp(n);
	DlOneBasedIter<DlFloat> vv(tmp);
	DlOneBasedIter<DlInt32> indx(index);

	d = 1.0;
	
	/*	Get implicit scaling information
		*/
	order(m, tmp, index);
//	PrintVector(index);

	/*	Loop over the columns of Crouts Method
		*/

	DlInt32 zCount = 0;
	for(DlInt32 j = 1; j <= n; ++j) {
		DlInt32 imax = 0;
	
		for(DlInt32 i = 1; i < j; ++i) {	/* Row loop */
			DlInt32 				irow = indx[i];
			DlSliceIter<DlFloat> 	row(m.row(irow));
			DlSliceIter<DlFloat> 	col(m.col(j));
			DlFloatEx 				sum = row[j];
			for (DlInt32 k = 1; k < i; ++k) 
				sum -= row[k] * col[indx[k]];
			row[j] = sum;
		}

		/*	Search for pivot
			*/

		DlFloatEx big = 0.0;
		for (DlInt32 i = j; i <= n; ++i) {
			DlInt32 				irow = indx[i];
			DlSliceIter<DlFloat> 	row(m.row(irow));
			DlSliceIter<DlFloat> 	col(m.col(j));
			DlFloatEx 				sum = row[j];
			for (DlInt32 k = 1; k < j; ++k) 
				sum -= row[k] * col[indx[k]];
			row[j] = sum;
			DlFloatEx dum = vv[irow]*fabs(sum);
			if (dum >= big) {
				big = dum;
				imax = i;
			}
		}

		if (imax == 0)
			throw EqSolveFailure(EqSolveFailure::Singular, j);

		/*	Check for row interchange
			*/

		DlInt32 irow = indx[imax];
		if (j != imax) {
			indx[imax] = indx[j];
			indx[j] = irow;
			d = -d;
		}

		/* 	Check for zero pivot.  NOTE: we don't flag these!!
			*/
		DlSliceIter<DlFloat> row(m.row(irow));

		if( row[j] == 0.0) {
			if (zCount > 0)
				throw EqSolveFailure(EqSolveFailure::Singular, j);
			row[j] = TINY;
		}

		/*	If not the last column, divide the row by the pivot
			*/

		if (j != n) {
			DlFloatEx dum =  1.0 / row[j];
			DlSliceIter<DlFloat> col(m.col(j));
			for (DlInt32 i = j+1; i <= n; ++i) 
				col[indx[i]] *= dum;
		}
	}	/* Do next column */
}

/*---------------------------------------------------
**	lubksb:  LU decomposition back substitution routine
**		Input:
**			a		- decomposed matrix returned by ludcmp
**			index	- row permutation record from ludcmp
**			bb		- vector to back substitute
**		Output:
**			bb		- replaced by solution.
**					  Note: bb is scrambled by the permutations
**					  in indx. use unscramble to get bb.
*/

void
lubksb(const DlMatrix<DlFloat>& a, valarray<DlInt32>& index, valarray<DlFloat>& bb)
{
	_RecipesAssert(a.rows() == a.cols() && a.rows() == index.size() && a.rows() == bb.size());
	DlInt32 n = a.cols();
	
	DlOneBasedIter<DlInt32> indx(index);
	DlOneBasedIter<DlFloat> b(bb);
	
	/*	Forward substitution */
	DlInt32 ii = 0;
	for (DlInt32 i = 1; i <= n; ++i) {
		DlInt32		ip = indx[i];
		DlFloatEx	sum = b[ip];
		if (ii) {
			DlSliceConstIter<DlFloat> row(a.row(ip));
			for (DlInt32 j = ii; j <= i-1; ++j)
				sum -= row[j] * b[indx[j]];
		} else if (sum) {
			ii = i;
		}
		b[ip] = sum;
	}

	/*	back substitution */

	for (DlInt32 i = n; i >= 1; --i) {
		DlInt32 				ip = indx[i];
		DlFloatEx 				sum = b[ip];
		DlSliceConstIter<DlFloat> 	row(a.row(ip));
		for (DlInt32 j = i+1; j <= n; ++j) 
			sum -= row[j] * b[indx[j]];
		b[ip] = sum/row[i];
	}
}


/*
**	---------------------------------------------------
**	invert_matrix_lu:	perform matrix inversion by LU decomp.
**		Input:
**			a		- the matrix to be inverted as array of pointers
**			a_inv	- pointer to space for inverse
**			n		- rank of a
**		Output:
**			a		- decomposed matrix
**			a_inv	- inverse of a
*/

void
invert_matrix_lu(DlMatrix<DlFloat>& a, DlMatrix<DlFloat>& a_inv)
{
	_RecipesAssert(a.rows() == a.cols() && a.rows() ==  a_inv.rows() && a_inv.rows() == a_inv.cols());
	DlInt32 n = a.cols();
	valarray<DlInt32> indx(n);
	valarray<DlFloat> col(n);

	DlOneBasedIter<DlInt32> index(indx);
	DlOneBasedIter<DlFloat> cc(col);
	/*	Do the decomposition
		*/

	DlFloat		d;
	ludcmp(a, indx, d);
			
	/*	Back substitute unit vectors
		*/
	
	for(DlInt32 j = 1; j <= n; ++j) {
		for(DlInt32 i = 1; i <=n; ++i)
			cc[i] = 0.0;
		cc[j] = 1.0;

		lubksb(a, indx, col);
		
		DlSliceIter<DlFloat> currCol(a_inv.col(j));
		for (DlInt32 i = 1; i <= n; ++i) {
			DlInt32 irow = index[i];
			currCol[i] = cc[irow];
		}
	}
}

