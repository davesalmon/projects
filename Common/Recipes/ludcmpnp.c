/*+++++++++++++++++++++++++++++++
**	ludcmpnp.c
**
**		Purpose:	lu decomposition without pivoting
**
 *	Copyright:	COPYRIGHT (C) 1989-2016 By David C. Salmon.  *WORLDWIDE RIGHTS RESERVED*
 *
**		Author:		David C. Salmon
**		Created:	Sat, Apr 8, 1989
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
/*
**	---------------------------------------------------
**	ludcmpnp:	LU decomposition routine -- no pivoting
**		Input:
**			a[1..n] 	- address of an array of pointers to vectors
**						of n DlFloats. 
**			n 			- rank of matrix.
**		Output:
**			a			- decomposed matrix
*/

void
ludcmpnp(DlMatrix<DlFloat, 1>& a)
{
	DlInt32 n = a.rows();

	/*	Loop over the columns of Crouts Method
		*/

	for(DlInt32 j = 1; j <= n; j ++) {
		DlSliceIter<DlFloat> jCol(a.col(j)); 
		for(DlInt32 i = 1; i < j; i ++) {	/* Row loop */
			DlSliceIter<DlFloat> iRow(a.row(i)); 
			DlFloatEx sum = iRow[j];
			for (DlInt32 k = 1; k < i; k ++) 
				sum -= iRow[k] * jCol[k];
			iRow[j] = sum;
		}

		for (DlInt32 i = j; i <= n; i ++) {	/* Col loop */
			DlSliceIter<DlFloat> iRow(a.row(i)); 
			DlFloatEx sum = iRow[j];
			for (DlInt32 k = 1; k < j; k ++) 
				sum -= iRow[k] * jCol[k];
			iRow[j] = sum;
		}

		/* 	Check for zero pivot.  NOTE: we don't flag these!!
			*/

		if(jCol[j] == 0.0) {
			throw EqSolveFailure(EqSolveFailure::NeedPivot, j);
		}

		/*	If not the last column, divide the row by the pivot
			*/

		if (j != n) {
			DlFloatEx dum =  1.0 / (jCol[j]);
			for (DlInt32 i = j+1; i <= n; i ++) 
				jCol[i] *= dum;
		}
	}	/* Do next column */
}

/*
**	---------------------------------------------------
**	lubksb:  LU decomposition back substitution routine
**		Input:
**			a		- decomposed matrix returned by ludcmp
**			n		- rank of the matrix
**			b		- vector to back substitute
**		Output:
**			b		- replaced by solution.
*/

void
lubksbnp(const DlMatrix<DlFloat, 1>& a, std::valarray<DlFloat>& bb)
{
	DlInt32 n = a.rows();
	DlOneBasedIter<DlFloat> b(bb);
	
	DlInt32 ii = 0;

	for (DlInt32 i = 1; i <= n; i ++) {
		DlSliceConstIter<DlFloat> iRow(a.row(i));
		DlFloatEx sum = b[i];
		if (ii) {
			for (DlInt32 j = ii; j <= i-1; j++) 
				sum -= iRow[j] * b[j];
		} else {
			if (sum) 
				ii = i;
		}
		b[i] = sum;
	}

	for (DlInt32 i = n; i >= 1; i--) {
		DlSliceConstIter<DlFloat> iRow(a.row(i));
		DlFloatEx sum = b[i];
		for (DlInt32 j = i+1; j <= n; j++) 
			sum -= iRow[j] * b[j];
		b[i] = sum/iRow[i];
	}
}


