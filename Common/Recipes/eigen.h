/*+++++++++++++++++++++
**	Eigen.h
**
**		Include file for eigen values
 *
 *	Copyright:	COPYRIGHT (C) 1989-2016 By David C. Salmon.  *WORLDWIDE RIGHTS RESERVED*
 *
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

#if !defined(_H_eigen)
#define	_H_eigen

#include "DlMatrix.h"
//#include	"recipes.h"


//	compute the eigenvalues and eigen vectors of m. return the number of iterations.
//	eigenvectors are columns of v
DlInt32	jacobi(DlArray::DlMatrix<DlFloat>& m, std::valarray<DlFloat> &eigenVals, 
				DlArray::DlMatrix<DlFloat>& v);

//	compute a single eigen value. return eigenvector in vec.
DlFloat		epower(const DlArray::DlMatrix<DlFloat>& m, DlArray::DlSliceIter<DlFloat>& vec);

//	compute eigen value and eigen vectors. The number of eigen vectors computed
//	is equal to the rows of v. On output eigen vectors in rows of v.
void		nevalpower(DlArray::DlMatrix<DlFloat>& m, DlArray::DlMatrix<DlFloat>& v, 
				std::valarray<DlFloat>& evals);
				
//	reduce the matrix a to tridiagonal form.
void		tred2(DlArray::DlMatrix<DlFloat>& a, std::valarray<DlFloat>& d, 
				std::valarray<DlFloat>& e);

//	compute the eigen values and vectors of the tridiagonal system.
//	eigenvectors are the columns of z. For eigen vectors of original system.
//	v = a . z. Eigenvectors are then in columns of v
void		tqli(std::valarray<DlFloat>& dd, std::valarray<DlFloat>& ee, 
				DlArray::DlMatrix<DlFloat>& z);

#endif
