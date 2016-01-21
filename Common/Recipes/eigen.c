/*++++++++++++++++++++++
**	Eigen.c
**
**	Purpose:	Compute eigen values for matrices.
**				several methods including: power method, jacobi iteration and 
**				householders method.
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
#include "eigen.h"
//#include "matio.h"

using namespace std;
using namespace DlArray;

#define	ROTATE(a,i,j,k,l) { g = a[i][j]; \
							h = a[k][l]; \
							a[i][j] = g - s * (h + g * tau);\
							a[k][l] = h + s * (g - h * tau);}

#define	MAX_JACOBI	50
#if !defined(THINK_C)
#define	noErr		0
#endif

inline void identMatrix(DlArray::DlMatrix<DlFloat>& m)
{
	DlInt32 rows = m.rows();
	DlInt32 cols = m.cols();
	for (DlInt32 i = 1; i <= rows; ++i)
	{
		DlSliceIter<DlFloat> r(m.row(i));
		for (DlInt32 j = 1; j <= cols; ++j)
			r[j] = (i == j) ? 1.0 : 0.0;
	}
}

inline void zeroVector(valarray<DlFloat>& v)
{
	DlInt32 s = v.size();
	for (DlInt32 i = 0; i < s; ++i)
		v[i] = 0;
}

/*---------------------------
**	jacobi
**
**		Computes all eigenvalues and eigen vectors of a real symmetric matrix
**		a[1..n][1..n]. On output, the elements of a above the disagonal are
**		destroyed. d[1..n] returns the eigenvalues of a. v[1..n][1..n] is a
**		matrix whose columns contain, on output, the normalized eigenvectors 
**		of a. nrot returns the number of Jacobi iterations that were required.
*/

DlInt32	
jacobi(DlArray::DlMatrix<DlFloat>& m, std::valarray<DlFloat> &eigenVals, 
				DlArray::DlMatrix<DlFloat>& v)
{
	_RecipesAssert(m.cols() == m.rows());
	_RecipesAssert(v.cols() == m.cols());
	_RecipesAssert(v.rows() == m.rows());
	
	_RecipesAssert(eigenVals.size() >= m.cols());
	
	DlInt32 n = m.cols();

//	DlInt32		j, iq;
//	DlFloat		tresh;
//	DlFloat		theta, tau, t;
//	DlFloat		sm;
//	DlFloat		s, h, g, c;//, *b, *z;
//	DlFloat*	tmp;
//	DlInt32		ip;
/*	void	nrerror();*/

	valarray<DlFloat> bb(n);
	valarray<DlFloat> zz(n);
	
	DlOneBasedIter<DlFloat> d(eigenVals);
	DlOneBasedIter<DlFloat> b(bb);
	DlOneBasedIter<DlFloat> z(zz);
	
//	b = fvector(1,n);
//	z = fvector(1,n);
	
	/*	Initialize v to the identity matrix, z to zero, b and d to the 
		diagonal of a.
		*/
	identMatrix(v);
	for (DlInt32 i = 1; i <= n; ++i) {
		b[i] = d[i] = m[i][i];
	}
		
	//PrintMatrix(v);
	/*	Begin the iteration
		*/
	
	DlInt32 nrot = 0;
	for (DlInt32 i = 0; i < MAX_JACOBI; i++) {

		zeroVector(zz);	/* zero out the z vector */

		DlFloatEx sm = 0.0;
		for (DlInt32 ip = 1; ip < n; ip++) {
			DlSliceIter<DlFloat> tmp = m[ip];//+ip+1;
			for (DlInt32 iq = ip+1; iq <= n; iq++) 
				sm += fabs(tmp[iq]);
		}
		
		/*	Here we have reduced all off diagonal terms to machine precision
			*/

		if (sm == 0.0) {
			return nrot;
		}
		
		DlFloatEx tresh = (i < 3) ? 0.2 * sm / (n*n) : 0.0;
		
		
		for (DlInt32 ip = 1; ip < n; ip++) {
			DlSliceIter<DlFloat> tmp = m[ip];// + ip+1;
			for (DlInt32 iq = ip+1; iq <= n; iq++) {
				DlFloatEx curr = fabs(tmp[iq]);
				DlFloatEx g = 100.0 * curr;
				
				/*	After four sweeps, skip the rotation if the off-diagonal
					term is small
					*/
				
				if ((i > 3) && (d[ip] + g == d[ip]) && (d[iq] + g == d[iq])) {
				
					tmp[iq] = 0.0;
				
				} else if (curr > tresh) {
					DlFloatEx h = d[iq] - d[ip];
					DlFloatEx t;
					if ((h+g) == h) {
						t = tmp[iq]/h;
					} else {
						DlFloatEx theta = 0.5 * h / tmp[iq];
						t = 1.0 / (fabs(theta) + sqrt(1.0 + theta * theta));
						if (theta < 0.0) 
							t = -t;
					}
					
					DlFloatEx c = 1.0 / sqrt(1.0 + t*t);
					DlFloatEx s = t*c;
					
					DlFloatEx tau = s/(1.0 + c);
					h = t * tmp[iq];

					z[ip] -= h;
					z[iq] += h;
					d[ip] -= h;
					d[iq] += h;
					
					tmp[iq] = 0;
					
					DlSliceIter<DlFloat> first(m.col(ip));
					DlSliceIter<DlFloat> second(m.col(iq));
					
					for (DlInt32 j = 1; j < ip; j++) {
//						ROTATE(m,j,ip,j,iq)
						g = first[j];
						h = second[j];
						first[j] = g - s * (h + g * tau);
						second[j] = h + s * (g - h * tau);
					}
					
					{
						DlSliceIter<DlFloat> first = DlSliceIter<DlFloat>(m.row(ip));
						for (DlInt32 j = ip+1; j < iq; j++) {
							g = first[j];
							h = second[j];
							first[j] = g - s * (h + g * tau);
							second[j] = h + s * (g - h * tau);
	//						ROTATE(m,ip,j,j,iq)
						}
					}
					
					{
						DlSliceIter<DlFloat> second = DlSliceIter<DlFloat>(m.row(iq));
						for (DlInt32 j = iq+1; j <= n; j++) {
	//						ROTATE(m,ip,j,iq,j)
							g = first[j];
							h = second[j];
							first[j] = g - s * (h + g * tau);
							second[j] = h + s * (g - h * tau);
						}
					}
					
					{
						DlSliceIter<DlFloat>first = DlSliceIter<DlFloat>(v.col(ip));
						DlSliceIter<DlFloat>second = DlSliceIter<DlFloat>(v.col(iq));

						for (DlInt32 j = 1; j <= n; j++) {
	//						ROTATE(v,j,ip,j,iq)
							g = first[j];
							h = second[j];
							first[j] = g - s * (h + g * tau);
							second[j] = h + s * (g - h * tau);
						}
					}
					
					++nrot;
				}
			}
		}
		
		for (DlInt32 ip = 1; ip <= n; ip++) {
			b[ip] += z[ip];
			d[ip] = b[ip];
		}
	}
	
	throw RecipesError("No convergence in %d iterations", MAX_JACOBI);
	
}


/*==============================
**	Power Method
**
**		Use epower and nevalpower to find a few eigen values and
**		eigen vectors for a real-symmetric matrix. Note that
**		the power method will work (somewhat) for non-symmetric
**		matrices as long as the eigen values found are +.
*/

/*--------------------------
**	epower
**
**		Determine the largest eigen value (vector) of a matrix
**		by the power method.
*/

#define	MAX_POWER	1000
#define	POWER_TOL	1.0e-10

DlFloat
epower(const DlMatrix<DlFloat>& m, DlSliceIter<DlFloat>& v)
{
	_RecipesAssert(m.rows() == m.cols() && v.size() == m.cols());
	DlInt32 n = m.cols();
//	DlInt32		i, j, count, anErr;
//	DlFloatEx	l, newl;
//	DlFloat*	v2;
	
	valarray<DlFloat> tmp(n);
//	DlOneBasedIter<DlFloat> v(vec);
	DlOneBasedIter<DlFloat> v2(tmp);

	DlFloatEx newl = 1;
	for (DlInt32 i = 1; i <= n; i++) 
		v[i] = 1;
	DlFloatEx l = newl + 4 * POWER_TOL;

	DlInt32 count = 0;
	
	while (fabs(l - newl) > POWER_TOL) {
		l = newl;
		for (DlInt32 i = 1; i <= n; i++) {
			v2[i] = 0.0;
			DlSliceConstIter<DlFloat> row(m.row(i));
			for (DlInt32 j = 1; j <= n; j++) 
				v2[i] += row[j] * v[j];
		}
		
		newl = v2[1];
		for (DlInt32 i = 1; i <= n; i++) 
			v[i] = v2[i] / newl;
		if (++count > MAX_POWER) {
			throw RecipesError("No convergence in %d iterations", MAX_POWER);
		}
	}

	l = 0;
	for (DlInt32 i = 1; i <= n; i++) 
		l += v[i] * v[i];
	l = 1.0 / sqrt(l);
	for (DlInt32 i = 1; i <= n; i++) 
		v[i] *= l;

	return newl;
}

/*---------------------------------
**	nevalpower
**
**		Compute nv eigen values and vectors starting
**		with the largest. Calls epower repeatedly to
**		find the first nv.
*/

void
nevalpower(DlMatrix<DlFloat>& m, DlMatrix<DlFloat>& v, valarray<DlFloat>& evals)
{
	_RecipesAssert(m.rows() == m.cols() && m.rows() == v.cols());

	DlInt32	nv = v.rows();
	DlInt32 n = m.cols();
	
	DlOneBasedIter<DlFloat> e(evals);

	for (DlInt32 i = 1; i <= nv; i++) {
		DlSliceIter<DlFloat> vrow(v.row(i));
		e[i] = epower(m, vrow);
		if (i != nv) {
			for (DlInt32 j = 1; j <= n; j++) {
				DlSliceIter<DlFloat> mrow(m.row(j));
				for (DlInt32 k = 1; k <= n; k++)
					mrow[k] -= vrow[j] * vrow[k] * e[i];
			}
		}
	}
}

/*==============================
**	Householder and QL method
**
**		General method, most efficient for finding all
**		evals and evects for real symmetric matrix.
*/

/*-------------------------
**	tred2(a, n, d, e)
**
**		Perform householder of a real symmetric matrix
**		to tridiagonal form. Used with tqli to find eigen
**		values and eigen vectors.
**
**		a[1..n][1..n] is replaced with the transform Q. d[1..n]
**		returns the diagonal elements and e[1..n] the off-diagonal
**		elements.
*/

void
tred2(DlMatrix<DlFloat>& a, valarray<DlFloat>& dd, valarray<DlFloat>& ee)
{
	_RecipesAssert(a.cols() == a.rows() && a.cols() == dd.size() && a.cols() == ee.size());
	//DlInt32		m, k, j, i;
	//DlFloatEx	scale, hh, h, g, f;
	DlInt32 n = a.cols();
	
	DlOneBasedIter<DlFloat> d(dd);
	DlOneBasedIter<DlFloat> e(ee);
	
	for (DlInt32 i = n; i >= 2; i--) {
		DlInt32 m = i - 1;
		DlFloatEx h = 0.0;
		DlFloatEx scale = 0.0;
		DlSliceIter<DlFloat> currRow(a.row(i));
		if (m > 1) {
			for (DlInt32 k = 1; k <= m; k++) 
				scale += fabs(currRow[k]);

			if (scale == 0.0) {					/* Skip Transformation */
				e[i] = currRow[m];
			} else {
				for (DlInt32 k = 1; k <= m; k++) {
					currRow[k] /= scale;			/* Use scaled a's for transform */
					h += currRow[k] * currRow[k];		/* Form sigma in h */
				}

				DlFloatEx f = currRow[m];
				DlFloatEx g = ((f > 0) ? -sqrt(h) : sqrt(h));
				e[i] = scale * g;
				h -= f * g;						/* Now h is eq 11.2.4 */
				currRow[m] = f - g;				/* store u in ith row of a */
				f = 0.0;
				
				DlSliceIter<DlFloat> theCol(a.col(i));
				for (DlInt32 j = 1; j <= m; j++) {
					theCol[j] = currRow[j] / h;		/* store u/H in ith column of a */
					g = 0.0;					/* Form an element of A.u in g */
					DlSliceIter<DlFloat> tmp(a.row(j));
					for (DlInt32 k = 1; k <= j; k++) 
						g += tmp[k] * currRow[k];
					
					{
						DlSliceIter<DlFloat> tmp = DlSliceIter<DlFloat>(a.col(j));
						for (DlInt32 k = j + 1; k <= m; k++) 
							g += tmp[k] * currRow[k];
					}
					
					e[j] = g/h;					/* Form an elemen of p in temporarily */
					f += e[j] * currRow[j];		/*	unused element of e */
				}

				DlFloatEx hh = f / (h + h);				/* Form K, eq. 11.2.11 */

				for (DlInt32 j = 1; j <= m; j++) {	/* Form q in e overwriting p */
												/* Note that e[l] = e[i-1] survives */
					f = currRow[j];
					g = g = e[j] - hh * f;
					e[j] = g;
					DlSliceIter<DlFloat> tmp(a.row(j));
					for (DlInt32 k = 1; k <= j; k++)	/* Reduce a, eq. 11.2.13 */
						tmp[k] -= (f * e[k] + g * currRow[k]);
				}
			}
		} else {
			e[i] = currRow[m];
		}

		d[i] = h;
	}

	d[1] = 0.0;
	e[1] = 0.0;
	
	for (DlInt32 i = 1; i <= n; i++) {				/* Accumulate transformation matrices */
		DlInt32 m = i - 1;
		DlSliceIter<DlFloat> currRow(a.row(i));
		DlSliceIter<DlFloat> currCol(a.col(i));
		if (d[i]) {							/* This block skipped when i = 1 */
			for (DlInt32 j = 1; j <= m; j++) {
				DlFloatEx g = 0.0;						/* use u and u/H to make P.Q */

				DlSliceIter<DlFloat> tmp(a.col(j));
				for (DlInt32 k = 1; k <= m; k++) 
					g += currRow[k] * tmp[k];
				
				for (DlInt32 k = 1; k <= m; k++) 
					tmp[k] -= g * currCol[k];
			}
		}
		d[i] = currRow[i];
		currRow[i] = 1.0;							/* Set row and col of a to id */
		for (DlInt32 j = 1; j <= m; j++) {
			currCol[j] = 0.0;
			currRow[j] = 0.0;
		}
	}
}

/*------------------------------
**	tqli(d, e, n, z)
**
**		Use the QL algorithm with implicit shifts to determine
**		the eigenvalues and eigenvectors of a real symmetric
**		tridiagonal matrix, reduced by tred2. d[1..n] contains the
**		diagonal elements. On output it contains the eigenvalues.
**		e[1..n] contains the off-diagonal elements, and is destroyed.
**		The matrix z is input as the identity matrix for a tridagonal
**		matrix or as the output matrix a from tred2. The kth column of
**		z contains the eigen vector associated with d[k].
*/

#define	SIGN(a,b)	((b) < 0 ? -fabs(a) : fabs(a))

void
tqli(valarray<DlFloat>& dd, valarray<DlFloat>& ee, DlMatrix<DlFloat>& z)
{
	_RecipesAssert(dd.size() == ee.size() && z.rows() == z.cols() && dd.size() == z.cols());
	DlInt32 n = dd.size();
	
//	DlInt32		m, ll, iter, i, k;
//	DlFloatEx	s, r, p, g, f, dd, c, b;
	
	DlOneBasedIter<DlFloat> d(dd);
	DlOneBasedIter<DlFloat> e(ee);
	
	identMatrix(z);
	
	for (DlInt32 i = 2; i <= n; i++) 
		e[i-1] = e[i];	/* renumber e */
	
	e[n] = 0.0;
	
	for (DlInt32 ll = 1; ll <= n; ll++) {
		DlInt32 iter = 0;
		DlInt32 m;
		do {
			for (m = ll; m <= n-1; m++) {
				DlFloatEx dv = fabs(d[m]) + fabs(d[m+1]);
				if (fabs(e[m]) + dv == dv) 
					break;
			}
			if (m != ll) {
				if (iter++ == 30) 
					throw RecipesError("No convergence in 30 iterations");
				DlFloatEx g =(d[ll+1] - d[ll])/(2.0 * e[ll]);
				DlFloatEx r = sqrt((g*g) + 1.0);
				g = d[m] - d[ll] + e[ll] / (g + SIGN(r,g));
				DlFloatEx s = 1.0;
				DlFloatEx c = 1.0;
				DlFloatEx p = 0.0;
				for (DlInt32 i = m-1; i >= ll; i--) {
					DlFloatEx f = s * e[i];
					DlFloatEx b = c * e[i];
					if (fabs(f) >= fabs(g)) {
						c = g/f;
						r = sqrt((c*c) + 1.0);
						e[i+1] = f * r;
						s = 1.0/r;
						c *= s;
					} else {
						s = f/g;
						r = sqrt((s*s) + 1.0);
						e[i+1] = g * r;
						c = 1.0/r;
						s *= c;
					}
					g = d[i+1] - p;
					r = (d[i] - g) * s + 2.0 * c * b;
					p = s * r;
					d[i+1] = g + p;
					g = c * r - b;
					DlSliceIter<DlFloat> currCol(z.col(i));
					DlSliceIter<DlFloat> nextCol(z.col(i+1));
					for (DlInt32 k = 1; k <= n; k++) {
						f = nextCol[k];
						nextCol[k] = s * currCol[k] + c * f;
						currCol[k] = c * currCol[k] - s * f;
					}
				}
				d[ll] -= p;
				e[ll] = g;
				e[m] = 0.0;
			}
		} while(m != ll);
	}
}
