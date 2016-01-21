/*+
 *
 *  matrix.h
 *
 *  Copyright © 2003 David C. Salmon. All Rights Reserved.
 *
 *  Contains definitions for matrix solving functions.
 *
-*/
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

#ifndef	_H_matrix
#define	_H_matrix

//#include	"recipes.h"
#include <valarray>
#include "DlMatrix.h"

/*	Define Prototypes 
	*/

#define	TINY		1.0e-20

//	define a progress callback class
class EqSolveProgress {
public:
	enum Stage {
		Initializing,
		Decomposing,
		BackSubstituting,
		Done
	};

    virtual ~EqSolveProgress() = 0;
    
	//	called for transitions
	virtual bool SetStage(Stage stage) = 0;
	//	return false to cancel analysis
	virtual bool Processing(DlInt32 i, DlInt32 n) = 0;
}; 

class EqSolveFailure
{
public:
	typedef enum {
		Singular,
		NonPositiveDefinite,
		UserCancelled,
		NeedPivot
	} Reason;
	
	EqSolveFailure(Reason reason, DlInt32 eq) 
		: _reason(reason), _equation(eq) {}
	
	Reason	getReason() const { return _reason; }
	DlInt32	getEquation() const { return _equation; }

private:
	Reason	_reason;
	DlInt32	_equation;
};

/*	for pivoting */

//	m can have any start
//	scale and index are zero based.
void order(const DlArray::DlMatrix<DlFloat, 1>& m, std::valarray<DlFloat>& scale,
				std::valarray<DlInt32>& index);

//	swap rows for best pivot. return true if swapped.
bool pivot(const DlArray::DlMatrix<DlFloat, 1>& m, std::valarray<DlFloat>& scale,
			std::valarray<DlInt32>& index, DlInt32 actCol);

//	use index to unscramble the results.
void unscramble(std::valarray<DlFloat>& results, const std::valarray<DlInt32>& indx);

void unscramble(DlArray::DlSliceIter<DlFloat, 1>& results, const std::valarray<DlInt32>& indx);

void unscramble(DlArray::DlMatrix<DlFloat, 1>& m, const std::valarray<DlInt32>& indx);

/*	for solving */

// decompose lower diagonal symmetric matrix
void	cholesky(DlFloat *lowerDiag, DlInt32 mSize, EqSolveProgress* progress=NULL);
// and back-substitute same
void	cholesbs(const DlFloat *decomp, DlInt32 mSize, DlFloat *rhs);

// same as above, but use valarray for storage.
void	cholesky(std::valarray<DlFloat>& lowerDiag, EqSolveProgress* progress=NULL);
void	cholesbs(const std::valarray<DlFloat>& decomp, std::valarray<DlFloat>& rhs);

// decompse matrix using LU deomposition. indx is returned as the zero-based order array.
void	ludcmp(DlArray::DlMatrix<DlFloat, 1>& m, std::valarray<DlInt32>& indx,
				DlFloat& d);
// back substitute the specified rhs vector getting back the results in rhs. use
//	unscramble (above) to put the results back in order.
void	lubksb(const DlArray::DlMatrix<DlFloat, 1>& a, std::valarray<DlInt32>& indx,
				std::valarray<DlFloat>& rhs);

// invert the specified matrix.
void	invert_matrix_lu(DlArray::DlMatrix<DlFloat>& a, 
				DlArray::DlMatrix<DlFloat>& a_inv);

// perform LU decomp without pivoting.
void	ludcmpnp(DlArray::DlMatrix<DlFloat, 1>& m);
void	lubksbnp(const DlArray::DlMatrix<DlFloat, 1>& m, std::valarray<DlFloat>& rhs);

// perform gauss elimination using the matrix and rhs vectors.
void	gaussElim(DlArray::DlMatrix<DlFloat, 1>& m, DlArray::DlMatrix<DlFloat, 1>& x,
			std::valarray<DlInt32>& ord);

// perform singular value decomposition on the specified matrix.
void	svdcmp(DlArray::DlMatrix<DlFloat, 1>& m, std::valarray<DlFloat>& w,
			DlArray::DlMatrix<DlFloat, 1>& v);


inline EqSolveProgress::~EqSolveProgress()
{
}

#endif
