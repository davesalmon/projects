/*++++++++++++++++++++++
**	Optimize.h
**
 *	Copyright:	COPYRIGHT (C) 1989-2016 By David C. Salmon.  *WORLDWIDE RIGHTS RESERVED*
 *
**		Define the routines in optimize
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

#if !defined(_H_optimize)
#define	_H_optimize

#if !defined(_H_recipes)
#include	"recipes.h"
#endif

#include <utility>

//	point is (x, f(x))
typedef std::pair<DlFloat,DlFloat> DlOptPoint;

/*	routines in optimize.c 
	*/
//	givin distinct a and b, determine three points, a, b, c which
//	bracket a minimum of the function func. Also, return f(a)...
void	mnbrak(DlOptPoint& ax, DlOptPoint& bx, DlOptPoint& cx, 
			DlFloat (*f)(DlFloat x, void* arg), void* arg);

//	locate a mimumum given three points, a, b, and c which bracket
//	a minimum. Use golden section search.
DlOptPoint	golden(DlFloat a, DlFloat b, DlFloat c, 
			DlFloat (*f)(DlFloat x, void* arg), DlFloat tol, 
			void* arg);

//	locate a mimumum given three points, a, b, and c which bracket
//	a minimum. Use brents method.
DlOptPoint	brent(DlFloat a, DlFloat b, DlFloat c, 
			DlFloat (*f)(DlFloat x, void* arg), DlFloat tol, 
			void* arg);


#endif
