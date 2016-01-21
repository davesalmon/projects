//
//  TestLUDecomp.cpp
//  Common_Recipes
//
//  Created by David Salmon on 2/26/14.
//
//

#include "DlPlatform.h"
#include "DlArray.h"
#include "DlMatrix.h"

#include "TestLUDecomp.h"

#include "matio.h"
#include "matrix.h"

#include "gtest/gtest.h"

using namespace DlArray;

DlMatrix<DlFloat>	kOriginalMatrix {
	{1, 2, 3, 4, 5},
	{2, 3, 1, 4, 4},
	{6, 2, 1, 2, 3},
	{8, 1, 2, 6, 9},
	{9, 3, 1, 3, 4}
};

void checkIdentity(const DlMatrix<DlFloat> m)
{
	for (auto i = 1; i <= m.rows(); i++)
	{
		for (auto j = 1; j <= m.cols(); j++)
		{
			if (i == j)
				ASSERT_NEAR(m[i][j], 1.0, 1e-8);
			else
				ASSERT_NEAR(m[i][j], 0.0, 1e-8);
		}
	}
}


TEST(TestLUDecomp, NoPivot)
{
	DlMatrix<DlFloat> m(kOriginalMatrix);
	
	try {
		ludcmpnp(m);
	} catch (std::exception& ex) {
		printf("failed with error %s\n", ex.what());
		ASSERT_FALSE(ex.what());
	}
	
	try {
		
		DlMatrix<DlFloat> inv(m.rows(), m.cols());

		for (auto i = 0; i < m.cols(); i++)
		{
			std::valarray<DlFloat> rhs(0.0, m.rows());
			rhs[i] = 1;
			
//			PrintVector(0, rhs);
			
			lubksbnp(m, rhs);
			
//			PrintVector(0, rhs);

			for (auto j = 0; j < m.rows(); j++)
				inv[j+1][i+1] = rhs[j];
		}
		
		printf("inverse\n");
		PrintMatrix(inv);

		DlMatrix<DlFloat> prod(kOriginalMatrix * inv);
		
		printf("product\n");
		PrintMatrix(prod);
		
		checkIdentity(prod);
				
	} catch (std::exception& ex) {
		printf("failed with error %s\n", ex.what());
		ASSERT_FALSE(ex.what());
	}
}

// check the matrix inverter. This uses pivots.
TEST(TestLUDecomp, Invert)
{
	DlMatrix<DlFloat> m(kOriginalMatrix);
	
	DlMatrix<DlFloat> inv(m.rows(), m.cols());
	
	PrintMatrix(m);
	printf("original\n");
	
	invert_matrix_lu(m, inv);
	
	PrintMatrix(inv);
	printf("inv\n");
	
	DlMatrix<DlFloat> prod(kOriginalMatrix * inv);
	
	DlFloat tmp {
		1 * inv[1][1] +
		2 * inv[2][1] +
		3 * inv[3][1] +
		4 * inv[4][1] +
		5 * inv[5][1] };
	
	printf("%f\n", tmp);
	
	printf("product\n");
	PrintMatrix(prod);
	
	checkIdentity(prod);
}

