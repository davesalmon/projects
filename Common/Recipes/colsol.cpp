/*++++++++++++++++++++++++++
 **	colsol.cpp
 **
 **	Purpose:	
 **		Implement column skyline solver. Use colsolDecomp(...) to decompose a matrix 
 **		input in skyline form, and colsolBackSub(...) to back substitute rhs values to 
 **		obtain solutions.
 **
 **		This method is only applicable to symmetric matrices which are positive 
 **		definite, and organized in a skyline vector.
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
#include "colsol.h"

using namespace DlArray;

/* ----------------------------------------------------------------------------
 * Colsol	-	Column skyline solver. Performs LU decomposition and back
 *				substitution for a matrix arranged in skyline form.
 *
 * (-> input param, <- output param, <-> i/o param)
 * 
 * decompose	->	true for decomp, false for backsub
 * neq			->	the number of equations
 * aa			<->	the matrix
 * maxaa		->	the diagonal element index list
 * vv			<->	the rhs vector
 * posDef		->	true if the matrix is known to be positive-definite
 * progress		->	the progress reporter object
 * ----------------------------------------------------------------------------
 */
void colsol(bool decompose, DlInt32 neq, 
		std::valarray<DlFloat64>& aa,
		const std::valarray<DlInt32>& maxaa,
		std::valarray<DlFloat64>* vv,
		bool posDef,
		EqSolveProgress* progress)
{
	if (decompose) {
		colsolDecomp(neq, aa, maxaa, posDef, progress);
	} else {
		colsolBackSub(neq, aa, maxaa, vv, progress);
	}
}

/* ----------------------------------------------------------------------------
 * colsolDecomp	-	Column skyline solver. Performs LU decomposition
 *					for a matrix arranged in skyline form.
 *
 * (-> input param, <- output param, <-> i/o param)
 *
 * neq			->	the number of equations
 * aa			<->	the matrix
 * maxaa		->	the diagonal element index list
 * posDef		->	true if the matrix is known to be positive-definite
 * progress		->	the progress reporter object
 * ----------------------------------------------------------------------------
 */
void colsolDecomp(DlInt32 neq,
			std::valarray<DlFloat64>& aa,
			const std::valarray<DlInt32>& maxaa,
			bool posDef,
			EqSolveProgress* progress)
{
	DlOneBasedIter<DlFloat64>		a(aa);
	DlOneBasedConstIter<DlInt32>	maxa(maxaa);
			
	/*	Loop over columns */

	for (auto n = 1; n <= neq; n++) {
		if (progress && !progress->Processing(n, neq))
			throw EqSolveFailure(EqSolveFailure::UserCancelled, n);
		/*	Get max and min column elements (kh is num elems) */
		
		auto kn = maxa[n];
		auto kl = kn + 1;
		auto ku = maxa[n+1] - 1;
		auto kh = ku - kl;
		
		/*	Check for elements */
		
		if (kh >= 0) {
			if (kh > 0) {
				auto k = n - kh;
				auto ic = 0;
				auto klt = ku;
				
				for (auto j = 1; j <= kh; j++, k++) {
					ic = ic + 1;
					klt = klt - 1;
					auto ki = maxa[k];
					auto nd = maxa[k+1] - ki - 1;
					if (nd > 0) {
						auto kk = ((ic < nd) ? ic : nd);
						auto c = 0.0;
						
						for (auto l = 1; l <= kk; l++)
							c += a[ki+l] * a[klt+l];
						
						a[klt] -= c;
					}
				}
			}
			
			auto k = n;
			auto b = 0.0;
			
			for (auto kk = kl; kk <= ku; kk++) {
				auto ki = maxa[--k];
				auto c = a[kk] / a[ki];
				b += c * a[kk];
				a[kk] = c;
			}
			a[kn] -= b;
		}
		
		if (posDef) {
			if (a[kn] < 1.0e-10)
				throw EqSolveFailure(EqSolveFailure::Singular, n);
		} else {
			if (fabs(a[kn]) < 1.0e-10)
				throw EqSolveFailure(
									 a[kn] < 0 ? EqSolveFailure::NonPositiveDefinite :
									 EqSolveFailure::Singular, n
									 );
		}
	}
}

/* ----------------------------------------------------------------------------
 * Colsol	-	Column skyline solver. Performs LU back substitution for the
 *				decomposed matrix in aa.
 *
 * (-> input param, <- output param, <-> i/o param)
 *
 * neq			->	the number of equations
 * aa			<->	the matrix
 * maxaa		->	the diagonal element index list
 * vv			<->	the rhs vector
 * progress		->	the progress reporter object
 * ----------------------------------------------------------------------------
 */
void
colsolBackSub(DlInt32 neq,
			  const std::valarray<DlFloat64>& aa,
			  const std::valarray<DlInt32>& maxaa,
			  std::valarray<DlFloat64>* vv,
			  EqSolveProgress* progress)
{
	/*	Reduce rhs vector */
	DlOneBasedIter<DlFloat64>		v(*vv);
	DlOneBasedConstIter<DlFloat64>	a(aa);
	DlOneBasedConstIter<DlInt32>	maxa(maxaa);

	for (auto n = 1; n <= neq; n++) {
		if (progress && !progress->Processing(n, neq))
			throw EqSolveFailure(EqSolveFailure::UserCancelled, n);
		DlInt32 kl = maxa[n] + 1;
		DlInt32 ku = maxa[n+1] - 1;
		
		if (ku >= kl) {
			DlInt32 k = n;
			DlFloat64 c = 0.0;
			
			for (auto kk = kl; kk <= ku; kk++)
				c += a[kk] * v[--k];
			
			v[n] -= c;
		}
	}
	
	/*	back substitute */
	
	for (auto n = 1; n <= neq; n++)
		v[n] /= a[maxa[n]];
	
	if (neq > 1) {
		auto n = neq;
		
		for (auto l = 2; l <= neq; l++, n--) {
			auto kl = maxa[n] + 1;
			auto ku = maxa[n+1] - 1;
			if (ku >= kl)
				for (auto k = n, kk = kl; kk <= ku; kk++)
					v[--k] -= a[kk] * v[n];
		}
	}

}
