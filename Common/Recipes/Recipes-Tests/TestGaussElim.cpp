//
//  TestGaussElim.cpp
//  Common_Recipes
//
//  Created by David Salmon on 3/1/14.
//
//

#include "TestGaussElim.h"

#include "DlMatrix.h"
#include "matio.h"
#include "matrix.h"

#include "TestLUDecomp.h"

#include "gtest/gtest.h"

using namespace DlArray;
using namespace std;

const DlMatrix<DlFloat> kTestMat {
	{3, 0, 6, 0},
	{2, 2, 0, 1},
	{1, 0, 1, 0},
	{0, 1, 0, 1}};

const DlMatrix<DlFloat> kTestRhs {
	{1, 0, 0, 0},
	{0, 1, 0, 0},
	{0, 0, 1, 0},
	{0, 0, 0, 1} };


TEST(TestGaussElim, solve)
{
	DlMatrix<DlFloat> m(kTestMat);
	DlMatrix<DlFloat> x(kTestRhs);
	valarray<DlInt32> ord;
	
	try {
		
		printf("input matrix:\n");
		PrintMatrix(m);
		
		printf("\ninput rhs:\n");
		PrintMatrix(x);
		
		gaussElim(m, x, ord);
		
		printf("\nafter elim:\n");
		PrintMatrix(m);
		
		printf("\nresults:\n");
		PrintMatrix(x);
		
		printf("\nunscrambled:\n");
		PrintMatrix(x, ord);
		
		printf("\nunscrambled:\n");
		unscramble(x, ord);
		PrintMatrix(x);

		DlMatrix<DlFloat> prod(DlMatrix<DlFloat>(kTestMat) * x);
		printf("\nm*x:\n");
		PrintMatrix(prod);

		checkIdentity(prod);
		
		printf("\n");
		for (auto i = 0; i < ord.size(); ++i)
			printf(i == 0 ? "%d" : "\t%d", ord[i]);
		printf("\n");
		
	} catch(EqSolveFailure& fail) {
		printf("gausElim failed at equation %d\n", fail.getEquation());
	}
	
}
