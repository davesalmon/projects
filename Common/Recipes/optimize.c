/*+++++++++++++++++++++++++++++++
**	optimize.c
**
**		Purpose:	Routines for optimization
**
 *	Copyright:	COPYRIGHT (C) 1989-2016 By David C. Salmon.  *WORLDWIDE RIGHTS RESERVED*
 *
**		Author:		David C. Salmon
**		Created:	Mon, Oct 9, 1989
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
#include "optimize.h"
//#include	"SANE.h"

#define	GOLD			1.618034
#define	GLIMIT			100.0
#define	TINY			1.0e-20

#ifndef MAX
#define	MAX(a,b)		((a)>(b)?(a):(b))
#endif
#define	SIGN(a,b)		((b) >= 0 ? fabs(a) : -fabs(a))

#define	SHFT(a,b,c,d)	{(a)=(b);(b)=(c);(c)=(d);}

#define	RGOLD			0.61803399
#define	CGOLD			(1.0-RGOLD)
#define	ZEPS			1.0e-10
#define	ITMAX			100

inline DlOptPoint mkPoint(DlFloat u, DlFloat (*f)(DlFloat, void*), void* arg)
{
	return DlOptPoint(u, f(u, arg));
}

/*-----------------------
**	mnbrak
**
**		Routine uses parabolic fit to bracket
**		a minimum in a function of one variable
**		Numerical Recipes in C p. 297
*/

void
mnbrak(DlOptPoint& a, DlOptPoint& b, DlOptPoint& c, 
			DlFloat (*func)(DlFloat, void*), void* arg)
{
	a.second = (*func)(a.first, arg);
	b.second = (*func)(b.first, arg);
	
	if (b.second > a.second) {
		DlOptPoint dum;
		SHFT(dum, a, b, dum)
	}
	c = mkPoint(b.first + GOLD * (b.first - a.first), func, arg);	/* first guess for c */

	while(b.second > c.second) {					/* do this until bracket */
		/*	Parabolic extrapolation, u,  based on a, b, c
			*/

		DlFloat r = (b.first - a.first) * (b.second - c.second);
		DlFloat q = (b.first - c.first) * (b.second - a.second);
		DlOptPoint u = mkPoint((b.first - (b.first - c.first) * q - (b.first - a.first) * r) /
			(2.0 * SIGN(MAX(fabs(q-r),TINY),q-r)), func, arg);

		/*	u cannot be greater than ulim
			*/

		DlFloat ulim = (b.first) + GLIMIT * (c.first-b.first);
		
		/*	Test the possibilities
			*/

		if ((b.first-u.first)*(u.first-c.first) > 0.0) {	/* u is between b and c */
			if (u.second < c.second) {			/* min is between b and c */
				a = b;
				b = u;
				return;
			} else if (u.second > b.second) {	/* min is between a and u */
				c = u;
				return;
			}
			u = mkPoint((c.first) + GOLD * (c.first - b.first), func, arg);
		} else if ((c.first-u.first)*(u.first-ulim) > 0.0) {	/* u between c and ulim */
			if (u.second < c.second) { /* c->b, u->c, new->u */
				SHFT(b, c, u, mkPoint(c.first+GOLD*(c.first-b.first), func, arg))
			}
		} else if ((u.first-ulim)*(ulim-c.first) >= 0.0) {	/* u is max */
			u = mkPoint(ulim, func, arg);
		} else {									/* reject u */
			u = mkPoint((c.first) + GOLD * (c.first - b.first), func, arg);
		}

		/*	eliminate far point: b->a, c->b, u->c 
			*/

		SHFT(a, b, c, u)
	}	/* end while */
}

/*---------------------------
**	brent
**
**		For a function, f, and a bracket triplet (ax, bx, cx)
**		this routine finds the minimum to within a fractional 
**		value tol using Brent's method.  Numerical Recipes p 301.
*/

DlOptPoint	
brent(DlFloat ax, DlFloat bx, DlFloat cx, DlFloat (*f)(DlFloat x, void* arg), 
			DlFloat tol, void *arg)
{
	DlFloat	e = 0.0;	/* distance moved on previous step */
/*	void	nrwarning(); */
	
	DlFloat d = 0;
	if (tol < ROOT_MAXPREC) 
		tol = ROOT_MAXPREC;

	DlFloat a;	/* a and b in ascending order */
	DlFloat b;
	
	if (ax < cx) {
		a = ax;
		b = cx;
	} else {
		a = cx;
		b = ax;
	}
	
	DlOptPoint x = DlOptPoint(bx, (*f)(bx,arg));
	DlOptPoint w = x;
	DlOptPoint v = x;
	DlOptPoint u;
	for (int iter = 1; iter <= ITMAX; iter++)	{ /* Main loop */
		DlFloat xm = 0.5 * (a + b);
		DlFloat tol1 = tol * fabs(x.first) + ZEPS;
		DlFloat tol2 = 2.0 * tol1;

		/* •• Test for done */

		if (fabs(x.first-xm) <= (tol2 - 0.5 * (b-a))) {
			return x;
		}

		if (fabs(e) > tol1) {	/* •• Trial parabolic fit */
			DlFloat r = (x.first-w.first)*(x.second-v.second);
			DlFloat q = (x.first-v.first)*(x.second-w.second);
			DlFloat p = (x.first-v.first)*q-(x.first-w.first)*r;
			q = 2.0 * (q-r);
			if (q > 0.0)
				p = -p;
			q = fabs(q);
			DlFloat etemp = e;
			e = d;

			/*	•• Check the parabolic fit */

			if (fabs(p) >= fabs(0.5 * q * etemp) || p <= q*(a-x.first) || p>=q*(b-x.first)) {
				e = x.first >= xm ? a-x.first : b-x.first;
				d = CGOLD * e;				/* Golden section step */
			} else {						/* Parabolic step */
				d = p/q;
				DlFloat u = x.first + d;
				if (u - a < tol2 || b - u < tol2)
					d = SIGN(tol1, xm-x.first);
			}
		} else {		/* Just take Golden step */
			d = CGOLD * (e = (x.first >= xm ? a-x.first : b-x.first));
		}

		/*	•• New argument and function evaluation 
			Never step farther than tol1 
			*/

		u = mkPoint(fabs(d) >= tol1 ? x.first+d : x.first + SIGN(tol1,d), f, arg);

		/*	•• Decide on new interval
			*/

		if (u.second <= x.second)	{ /* use w, x, and u */
			if (u.first >= x.first) 
				a = x.first; 
			else
				b = x.first;
			SHFT(v,w,x,u)
		} else {
			if (u < x) 
				a = u.first; 
			else
				b = u.first;
			if (u.second <= w.second || w.first == x.first) {
				v = w; 
				w = u; 
			} else if (u.second <= v.second || v.first == x.first || v.first == w.first) {
				v = u;
			}
		}
	}	/* end of Main Loop */
	
	return x;
}

/*----------------------------
**	golden
**
**		golden section search for minimum
*/

DlOptPoint
golden(DlFloat ax, DlFloat bx, DlFloat cx, DlFloat (*f)(DlFloat, void *), 
			DlFloat tol, void *arg)
{
	/*	set up the bracketing values 
		*/
	
	if (tol < ROOT_MAXPREC) 
		tol = ROOT_MAXPREC;

	DlOptPoint x0 = DlOptPoint(ax, 0);
	DlOptPoint x3 = DlOptPoint(cx, 0);
	DlOptPoint x1;
	DlOptPoint x2;

	/*	see which limit the middle point is closest to.
		Place new point in larger segment
		*/

	if (fabs(cx-bx) > fabs(bx-ax)) {
		x1 = mkPoint(bx, f, arg);				/* x1 at b */
		x2 = mkPoint(bx + CGOLD * (cx-bx), f, arg);	/* x2 between b and c */
	} else {
		x2 = mkPoint(bx, f, arg);				/* x2 at b */
		x1 = mkPoint(bx - CGOLD * (bx-ax), f, arg);	/* x1 between a and b */
	}
	
	/*	Loop while |x3 - x0|/(|x1|+|x2|) > tol
		*/
	while (fabs(x3.first-x0.first) > tol * (fabs(x1.first)+fabs(x2.first))) {
		if (x2.second < x1.second) {	/* new triplet is x1, x2, x3. new x2 is between x1 and x3 (nearer x3) */
			SHFT(x0, x1, x2, mkPoint(RGOLD * x1.first + CGOLD * x3.first, f, arg))
		} else {						/* new triplet is x0, x1, x2 new x1 is between x0 and x2 (nearer x0)*/
			SHFT(x3, x2, x1, mkPoint(RGOLD * x2.first + CGOLD * x0.first, f, arg))
		}
	}	/* end of while, check for minimum */

	return x1.second < x2.second ? x1 : x2;
}
