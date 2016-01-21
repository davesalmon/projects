/*++++++++++++++++++++++++++
**	cholesky.c
**
**	Purpose:	Implement cholesky decomposition as described
**				in Chapra & Canale, "Numerical Methods for 
**				Engineers", 2nd ed. pp 290.
**
**		This method is only applicable to symmetric matrices
**		which are positive definite.
**
 *	Copyright:	COPYRIGHT (C) 1991-2016 By David C. Salmon.  *WORLDWIDE RIGHTS RESERVED*
 *
**	Author:		David C. Salmon
**	Created:	Sat, Mar 2, 1991
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
#include "DlArray.h"

using namespace DlArray;

/*------------------------
**	choleskyImp
**
**		The symmetric matrix a is expected in an lower diagonal array
**		a11
**		a21	a22
**		a31	a32	a33
**		... ... ... ...
**		an1 an2 an3 ... ann
**
**		the offset to row i is 0 + 1 + 2 + 3 + ... i (0 based) = (i)(i+1)/2;
**		The matrix a is replace by L (the lower form of lu)
**
**	T is required to have:
**		operator*()
**		operator++
**		operator-=
*/
template <class T> void
choleskyImp(T a, DlInt32 n, EqSolveProgress* progress)
{
	T kPtr = a;
	for (DlInt32 k = 0; k < n; ++k, ++kPtr) {		/* loop over rows */
		if (progress && !progress->Processing(k+1, n))
			throw EqSolveFailure(EqSolveFailure::UserCancelled, n);
		/*	rPtr points to first element of a */
		T iPtr = a;
		for (DlInt32 i = 0; i < k; ++i, ++iPtr) {	/* loop over rows 1 to k-1 */
			DlFloatEx	sum = 0.0;
			for (DlInt32 j = 0; j < i; ++j, ++kPtr, ++iPtr) /* columns 0 to i-1 */
				sum += (*kPtr) * (*iPtr);

			if ((*iPtr) == 0.0) {
				(*iPtr) = TINY;		//	fix up zero elements
//				anErr = CHLSKY_SINGULAR;
			}

			(*kPtr) -= sum;
			(*kPtr) /= (*iPtr);		/*	elem(a,k,i) = (elem(a,k,i) - sum)/elem(a,i,i) */
									/* iPtr points to the diagonal element */
			kPtr -= i;				/* revert a[k][0] */
		}
		
		DlFloatEx sum = 0.0;
		for (DlInt32 j = 0; j < k; ++j, ++kPtr)
			sum += (*kPtr) * (*kPtr);

		(*kPtr) -= sum;
	
		if ((*kPtr) < 0.0) 
			throw EqSolveFailure(EqSolveFailure::NonPositiveDefinite, n);
		(*kPtr) = sqrt(*kPtr);	/* kPtr points to the diagonal */
		
	}
}

/*---------------------------
**	cholesbs
**
**		backsubstitution routine for cholesky
**		a contains L, and a = L(L)T x = b
**
**		forward substitution:
**					x[i] - sum(a[i][j] x[j], j = 1 to i-1)
**			x[i] = 	----------------------------------------
**									a[i][i]
**
**		back substitution
**
**			x[i] = x[i] - sum(a[j][i] x[j], j = i+1 to n)
**
**		operator*()
**		operator++
**		operator-=
**
**	In general, U is const T
*/
template <class U, class T> void
cholesbsImp(U a, DlInt32 n, T x)
{
	U kPtr = a;
	for (DlInt32 k = 0; k < n; ++k, ++kPtr) {
		T xPtr = x;

		DlFloatEx sum = x[k];
		for (DlInt32 j = 0; j < k; ++j, ++kPtr, ++xPtr)
			sum -= (*kPtr) * (*xPtr);

		x[k] = sum / (*kPtr);
	}

	{
		U kPtr(a);
		kPtr += ((n * n + n)>>1) - 1;
		U lastPtr = kPtr;
		for (DlInt32 k = n-1; k >= 0; k--) {
			DlFloatEx sum = x[k];
			U xPtr = lastPtr;
			xPtr += (-n + 1 + k);
			for (DlInt32 j = n-1; j > k; j--) {
				sum -= (*xPtr) * x[j];
				xPtr -= j;
			}
			x[k] = sum / (*kPtr);
			kPtr -= (k+1);
		}
	}
}

//----------------------------------------------------------------------------------------
//  cholesky
//
//      decompose the specified matrix using choleskys method.
//
//  std::valarray<DlFloat>& lowerDiag  -> the matix in lower diag form
//	EqSolveProgress* progress          -> optional progress callback
//
//  returns nothing
//----------------------------------------------------------------------------------------
void
cholesky(std::valarray<DlFloat>& lowerDiag, EqSolveProgress* progress)
{
    // this gives the number of rows and cols from the total
    //	number of lower-diagonal elements.
	DlInt32 n = lroundl(sqrt(8 * lowerDiag.size() + 1) - 1) / 2;
	
	choleskyImp(DlArrayIter<DlFloat>(lowerDiag), n, progress);
	//	this is a bit funky, but the only way to access the value pointer.
//	cholesky(&lowerDiag[0], n, progress);
}

//----------------------------------------------------------------------------------------
//  cholesbs
//
//      back substitute rhs into cholesky decomp for solution.
//
//  std::valarray<DlFloat>& decomp  -> the decomposed form of matix in lower diag form
//	std::valarray<DlFloat>& rhs     -> rhs vector
//
//  returns nothing
//----------------------------------------------------------------------------------------
void
cholesbs(const std::valarray<DlFloat>& decomp, std::valarray<DlFloat>& rhs)
{
	DlInt32 n = rhs.size();
	_RecipesAssert(decomp.size() >= (n * (n + 1))>>1);
	cholesbsImp(DlArrayConstIter<DlFloat>(decomp), n, DlArrayIter<DlFloat>(rhs));
}

/*------------------------
**	cholesky
**
**		The symmetric matrix a is expected in an lower diagonal array
**		a11
**		a21	a22
**		a31	a32	a33
**		... ... ... ...
**		an1 an2 an3 ... ann
**
**		the offset to row i is 0 + 1 + 2 + 3 + ... i (0 based) = (i)(i+1)/2;
**		The matrix a is replace by L (the lower form of lu)
*/

//----------------------------------------------------------------------------------------
//  cholesky
//
//      decompose the specified matrix using choleskys method
//
//  DlFloat* a                 -> 
//  DlInt32 n                  -> 
//  EqSolveProgress* progress  -> 
//
//  returns nothing
//----------------------------------------------------------------------------------------
void
cholesky(DlFloat* a, DlInt32 n, EqSolveProgress* progress)
{
	choleskyImp(a, n, progress);
}

/*---------------------------
**	cholesbs
**
**		backsubstitution routine for cholesky
**		a contains L, and a = L(L)T x = b
**
**		forward substitution:
**					x[i] - sum(a[i][j] x[j], j = 1 to i-1)
**			x[i] = 	----------------------------------------
**									a[i][i]
**
**		back substitution
**
**			x[i] = x[i] - sum(a[j][i] x[j], j = i+1 to n)
*/
void
cholesbs(const DlFloat* a, DlInt32 n, DlFloat* x)
{
	cholesbsImp(a, n, x);
}
