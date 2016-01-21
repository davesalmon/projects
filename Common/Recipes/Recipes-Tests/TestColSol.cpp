//
//  TestColSol.cpp
//  Common_Recipes
//
//  Created by David Salmon on 2/28/14.
//
//

#include "DlPlatform.h"
#include "TestColSol.h"

#include "colsol.h"
#include "matio.h"

#include <valarray>
#include <vector>

#include "DlMatrix.h"
#include "TestLUDecomp.h"

#include "gtest/gtest.h"

using namespace DlArray;

const DlInt32 diag[] = {1, 2, 4, 5};
const DlFloat64 matrix[] = {2.0, 3.0, 1.0, 3.0};
const DlMatrix<DlFloat64> kSmallMatrix {{2, 1, 0}, {1, 3, 0 }, {0, 0, 3}};

// 2  1  0
//    3  0
//       3

const DlFloat64 matrix2[] = { 2.0, 2.0, 2.0, 2.0, 2.0, 0.0, 1.0, 1.0, 1.0};
const DlInt32	diag2[] = { 1, 2, 3, 4, 5, 10 };

// 2  0  0  0  1
//    2  0  0  1
//       2  0  1
//          2  0
//             2

const DlMatrix<DlFloat64> kLargerMatrix {
	{2, 0, 0, 0, 1},
	{0, 2, 0, 0, 1},
	{0, 0, 2, 0, 1},
	{0, 0, 0, 2, 0},
	{1, 1, 1, 0, 2},
};


static int solveN(DlInt32 n, std::valarray<DlInt32>& maxa,
				  std::valarray<DlFloat64>& a, const DlMatrix<DlFloat64>& m, bool combined)
{
	DlMatrix<DlFloat64> inv(n, n);
	
	printf("\n");
	
	try {
		
		if (combined)
			colsol(true, n, a, maxa, nullptr, false);
		else
			colsolDecomp(n, a, maxa, false);
		
		for (DlInt32 i = 0; i < n; i++) {
			std::valarray<DlFloat64> rhs(0.0, n);
			rhs[i] = 1;

			if (combined)
				colsol(false, n, a, maxa, &rhs, false);
			else
				colsolBackSub(n, a, maxa, &rhs);
			for (auto j = 0; j < n; j++)
				inv[j+1][i+1] = rhs[j];
		}
		
		printf("the inverse is:\n");
		PrintMatrix(inv);
		printf("m * inv is\n");
		
		DlMatrix<DlFloat64> prod(m * inv);
		
		PrintMatrix(prod);
		
		checkIdentity(prod);

		printf("\n");

		
	} catch(const EqSolveFailure& fail) {
		fprintf(stderr, "Failed at eq %d\n", fail.getEquation());
		return false;
	}
	
	return true;
}


TEST(TestColSol, invert)
{
	{
		DlInt32 n = DlArrayElements(diag) - 1;
		std::valarray<DlFloat64>	a(matrix, DlArrayElements(matrix));
		std::valarray<DlInt32>		maxa(diag, DlArrayElements(diag));
		solveN(n, maxa, a, kSmallMatrix, true);
	}
	
	{
		DlInt32 n = DlArrayElements(diag2) - 1;
		std::valarray<DlFloat64>	a(matrix2, DlArrayElements(matrix2));
		std::valarray<DlInt32>		maxa(diag2, DlArrayElements(diag2));
		solveN(n, maxa, a, kLargerMatrix, true);
	}

	{
		DlInt32 n = DlArrayElements(diag) - 1;
		std::valarray<DlFloat64>	a(matrix, DlArrayElements(matrix));
		std::valarray<DlInt32>		maxa(diag, DlArrayElements(diag));
		solveN(n, maxa, a, kSmallMatrix, false);
	}
	
	{
		DlInt32 n = DlArrayElements(diag2) - 1;
		std::valarray<DlFloat64>	a(matrix2, DlArrayElements(matrix2));
		std::valarray<DlInt32>		maxa(diag2, DlArrayElements(diag2));
		solveN(n, maxa, a, kLargerMatrix, false);
	}

}
