/*++++++++++++++++++++++++++++++++++
**	svdcmp.c
**
**	Purpose:	Implementation of Singular value decomposition.
**				Adapted from Numerical Recipes.
**
 *	Copyright:	COPYRIGHT (C) 1989-2016 By David C. Salmon.  *WORLDWIDE RIGHTS RESERVED*
 *
**	Author:		David C. Salmon
**	Created:	Sat, Apr 8, 1989
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

#include <algorithm>
/*
**	----------------------------------------------------
**	svdcmp:		Perform singular value decomposition.
**		Input:
**			a		- matrix to be svd'd
**			w		- space for vector [1..n]
**			v		- space for matrix [1..n][1..n]
**			m,n		- size of a[1..m][1..n]
**		Output:
**			a		- replaced by U[1..m][1..n]
**			w		- vector [1..n]
**			v		- v matrix [1..n][1..n]
**
**
**	Use for decomposing singular or non-square matrices
**
**	Transforms an m x n matrix into its svd.
**		[a] = [U][w][VT]
**		[U] - m x n orthoganal matrix
**		[VT] - transpose of square, orthogonal matrix
**		[w] - matrix with singular values on diagonal
**			the routine returns only the diagonal elements
**			in the vector, w
**
**	Mostly used for diagnosing singular matrices or solving
**	Least squares problems.  The rank deficiency of a
**	singular matrix is determined by the number of 0 w's
**	returned.
**		[a_inv] = [V][1/w][UT]
**		UT - transpose of U
**		[1/w] - diagonal matrix of w's
*/

using namespace std;
using namespace DlArray;

inline DlFloat	pythag(DlFloat a, DlFloat b)
{
	DlFloat at = fabs(a);
	DlFloat bt = fabs(b);
	if (at > bt) {
		DlFloat ct = bt/at;
		return at * sqrt(1 + ct * ct);
	} else if (bt != 0) {
		DlFloat ct = at/bt;
		return bt * sqrt(1 + ct * ct);
	} else {
		return 0;
	}
}

#define PYTHAG(a,b) pythag(a,b)

//static		DlFloat			maxa, maxb;

#ifndef MAX
#define		MAX(a,b)		std::max(a, b);
#endif
#define		SIGN(a,b)		((b) >= 0 ? fabs(a) : -fabs(a))

void
svdcmp(DlMatrix<DlFloat>& a, valarray<DlFloat>& ww, DlMatrix<DlFloat>& v)
{
	DlInt32 m = a.rows();
	DlInt32 n = a.cols();

//	DlInt32			flag, i, its, j, jj, k, l, nm;
//	DlFloat		c, f, h, s, x, y, z;
//	DlFloat		anorm = 0.0, g = 0.0, scale = 0.0;
//	DlFloat		*rv1;
	
	if (m < n) 
		throw RecipesError("svdcmp requires rows >= cols in input");
	
	valarray<DlFloat> rv(n);
	DlOneBasedIter<DlFloat> rv1(rv);
	DlOneBasedIter<DlFloat> w(ww);

	DlFloatEx g = 0;
	DlFloatEx scale = 0;
	DlFloatEx anorm = 0;
	DlFloatEx c, f, h, s, x, y, z;
	/*	Householder reduction to bidagonal form
		*/
	
	DlInt32 l, nm;
	for (DlInt32 i = 1; i <= n; i++) {
		l = i + 1;
		rv1[i] = scale * g;
		g = scale = 0.0;
		if (i <= m) {
			DlSliceIter<DlFloat> currCol(a.col(i));
			for(DlInt32 k = i; k <= m; k++) 
				scale += fabs(currCol[k]);
			if (scale) {
				s = 0;
				for(DlInt32 k = i; k <= m; k++) {
					currCol[k] /= scale;
					s += currCol[k] * currCol[k];
				}
				f = currCol[i];
				g = -SIGN(sqrt(s),f);
				h = f * g - s;
				currCol[i] = f - g;
				if (i != n) {
					for (DlInt32 j = l; j <= n; j++) {
						DlSliceIter<DlFloat> nextCol(a.col(j));
						s = 0;
						for (DlInt32 k=i; k<=m; k++) 
							s+= currCol[k] * nextCol[k];
						f = s/h;
						for (DlInt32 k=i; k<=m; k++) 
							nextCol[k] += f * currCol[k];
					}
				}
				for(DlInt32 k = i; k <= m; k++) 
					currCol[k] *= scale;
			}
		}
		
		w[i] = scale * g;
		g = scale = 0.0;
		if (i <= m && i != n) {
			DlSliceIter<DlFloat> currRow(a.row(i));
			for (DlInt32 k=l; k <= n; k++) 
				scale += fabs(currRow[k]);
			if (scale) {
				s = 0;
				for (DlInt32 k=l; k <= n; k++) {
					currRow[k] /= scale;
					s += currRow[k] * currRow[k];
				}
				f = currRow[l];
				g = -SIGN(sqrt(s),f);
				h = f * g - s;
				currRow[l] = f - g;
				for (DlInt32 k = l; k <= n; k ++) 
					rv1[k] = currRow[k] / h;
				if (i != m) {
					for (DlInt32 j = l; j <= m; j ++) {
						DlSliceIter<DlFloat> nextRow(a.row(j));
						s = 0;
						for (DlInt32 k = l; k <= n; k++)
							s += nextRow[k] * currRow[k];
						for (DlInt32 k = l; k <= n; k ++) 
							nextRow[k] += s * rv1[k];
					}
				}
				for(DlInt32 k = l; k <= n; k++) 
					currRow[k] *= scale;
			}
		}
		anorm = MAX(anorm, (fabs(w[i]) + fabs(rv1[i])));
	}

	/*	Accumulation of right hand transformations
		*/
	
	for (DlInt32 i = n ; i >= 1; i--) {
		DlSliceIter<DlFloat> vRow(v.row(i));
		if (i < n) {
			DlSliceIter<DlFloat> vCol(v.col(i));
			if (g) {
				DlSliceIter<DlFloat> aRow(a.row(i));
				for (DlInt32 j = l; j <= n; j ++)
					vCol[j] = (aRow[j] / aRow[l]) / g;
				for (DlInt32 j = l; j <= n; j++) {
					DlSliceIter<DlFloat> vPos(v.col(j));
					s = 0;
					for (DlInt32 k = l; k <= n; k++)
						s += aRow[k] * vPos[k];
					for (DlInt32 k = l; k <= n; k ++) 
						vPos[k] += s * vCol[k];
				}
			}
			for (DlInt32 j = l; j <= n; j++) 
				vRow[j] = vCol[j] = 0.0;
		}
		vRow[i] = 1.0;
		g = rv1[i];
		l = i;
	}

	/*	Accumulation of left hand transformations
		*/

	for (DlInt32 i = n; i >= 1; i--) {
		l = i+1;
		g = w[i];
		DlSliceIter<DlFloat> aRow(a.row(i));
		DlSliceIter<DlFloat> aCol(a.col(i));
		if (i < n) {
			for (DlInt32 j = l; j <= n; j++) 
				aRow[j] = 0.0;
		}
		if (g) {
			g = 1.0/g;
			if (i != n) {
				for (DlInt32 j = l; j <= n; j++) {
					DlSliceIter<DlFloat> aPos(a.col(j));
					s = 0;
					for (DlInt32 k = l; k <= m; k++)
						s += aCol[k] * aPos[k];
					f = (s / aRow[i]) * g;
					for (DlInt32 k=i; k <= m; k++) 
						aPos[k] += f * aCol[k];
				}
			}
			for (DlInt32 j = i; j <= m; j++) 
				aCol[j] *= g;
		} else {
			for (DlInt32 j = i; j <= m; j++) 
				aCol[j] = 0.0;
		}
		++aRow[i];
	}
	
	/*	Diagonalization of Bi-diagonal form
		*/
	
	for (DlInt32 k = n; k >= 1; k--) {					/* Loop over singular values */
		for (DlInt32 its = 1; its<=30; its++) {			/* iterate */
			bool flag = true;
			for(l=k; l >= 1; l--) {				/* test for splitting */
													/* rv1[1] always zero */
				nm = l-1;
				if (fabs(rv1[l]) + anorm == anorm) {
					flag = false;
					break;
				}
				if (fabs(w[nm]) + anorm == anorm) 
					break;
			}
			
			if (flag) {							/* cancelation of rv1[l] if */
				c = 0.0;							/* l >1 */
				s = 1.0;
				for (DlInt32 i = l; i <= k ; i ++) {
					f = s * rv1[i];
					if (fabs(f) + anorm != anorm) {
						g = w[i];
						h = PYTHAG(f,g);
						w[i] = h;
						h = 1.0 / h;
						c = g * h;
						s = (-f*h);
						
						DlSliceIter<DlFloat> nmPos(a.col(nm));
						DlSliceIter<DlFloat> iPos(a.col(i));
						for (DlInt32 j = 1; j <= m; j++) {
							y = nmPos[j];
							z = iPos[j];
							nmPos[j] = y * c + z * s;
							iPos[j] = z*c - y*s;
						}
					}
				}
			}
			z = w[k];
			if (l == k) {			/* convergence */
				if (z < 0.0) {	/* make sing val positive */
					w[k] = -z;
					DlSliceIter<DlFloat> vkPos(v.col(k));
					for (DlInt32 j = 1; j <= n; j++) 
						vkPos[j] = (-vkPos[j]);
				}
				break;
			}
			
			if (its == 30) {
				throw RecipesError("svdcomp did not converge after 30 iterations.");
			}
			x = w[l];
			nm = k - 1;
			y = w[nm];
			g = rv1[nm];
			h = rv1[k];
			f = ((y-z)*(y+z)+(g-h)*(g+h))/(2.0 * h * y);
			g = PYTHAG(f,1.0);
			f = ((x-z)*(x+z)+h*((y/(f+SIGN(g,f)))-h))/x;
			
			/* Next QR transform */
			
			c = 1.0;
			s = 1.0;

			for (DlInt32 j = l; j <= nm; j++) {
				DlInt32 i = j+1;
				g = rv1[i];
				y = w[i];
				h = s * g;
				g = c * g;
				z = PYTHAG(f,h);
				rv1[j] = z;
				c = f/z;
				s = h/z;
				f = x*c + g*s;
				g = g*c - x*s;
				h = y*s;
				y = y * c;
				DlSliceIter<DlFloat> viPos(v.col(i));
				DlSliceIter<DlFloat> vjPos(v.col(j));

				for (DlInt32 jj=1; jj<=n; jj++) {
					x = vjPos[jj];
					z = viPos[jj];
					vjPos[jj] = x * c + z * s;
					viPos[jj] = z * c - x * s;
				}
				z = PYTHAG(f,h);
				w[j] = z;
				if (z) {			/* if z == 0 then rotation is arbitrary */
					z = 1.0/z;
					c = f * z;
					s = h * z;
				}
				f = c * g + s * y;
				x = c * y - s * g;
				DlSliceIter<DlFloat> aiPos(a.col(i));
				DlSliceIter<DlFloat> ajPos(a.col(j));
				for (DlInt32 jj = 1; jj <= m; jj++) {
					y = ajPos[jj];
					z = aiPos[jj];
					ajPos[jj] = y * c + z * s;
					aiPos[jj] = z * c - y * s;
				}
			}
			rv1[l] = 0.0;
			rv1[k] = f;
			w[k] = x;
		}
	}
}

					
