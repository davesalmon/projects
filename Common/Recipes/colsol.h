/*++++++++++++++++++++++++++
 **	colsol.h
 **
 **	Purpose:
 **
 **		Define column skyline solver. Use colsolDecomp(...) to decompose a matrix
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

#ifndef _H_colsol
#define _H_colsol

#include "matrix.h"
#include "DlArray.h"

void
colsol(bool decompose, DlInt32 neq, 
		std::valarray<DlFloat64>& a,
		const std::valarray<DlInt32>& maxa,
		std::valarray<DlFloat64>* v,
		bool positiveDefinite = true,
		EqSolveProgress* progress = 0);

void
colsolDecomp(DlInt32 neq,
	   std::valarray<DlFloat64>& a,
	   const std::valarray<DlInt32>& maxa,
	   bool positiveDefinite = true,
	   EqSolveProgress* progress = 0);

void
colsolBackSub(DlInt32 neq,
	  const std::valarray<DlFloat64>& a,
	  const std::valarray<DlInt32>& maxa,
	  std::valarray<DlFloat64>* v,
	  EqSolveProgress* progress = 0);

#endif
