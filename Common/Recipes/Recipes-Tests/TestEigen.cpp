//
//  TestEigen.cpp
//  Common_Recipes
//
//  Created by David Salmon on 2/27/14.
//
//

#include "DlPlatform.h"
#include "TestEigen.h"

#include "eigen.h"
#include "matio.h"
#include "DlArray.h"
#include "DlMatrix.h"

#include "TestLUDecomp.h"

#include "gtest/gtest.h"

using namespace DlArray;

const DlMatrix<DlFloat> kMatrix {
	{1, 0, 1, 2},
	{0, 1, 3, 5},
	{1, 3, 4, 1},
	{2, 5, 1, 2}
};

TEST(TestEigen, Jacobi)
{
	DlMatrix<DlFloat> m(kMatrix);
	std::valarray<DlFloat> eigenValues(m.rows());

	DlMatrix<DlFloat> v(m.rows(), m.cols());
	
	DlInt32 nIters = jacobi(m, eigenValues, v);
	
	printf("Converged in %d iterations\neigenvalues: \n", nIters);

	PrintVector(eigenValues);
	
	printf("eigen vectors\n");
	PrintMatrix(v);
	
	printf("\n");
	
	// check the eigen vectors
	checkIdentity(DlMatrixTranspose<DlFloat>(v) * v);
}

TEST(TestEigen, PowerMethod)
{
	DlMatrix<DlFloat> rhs { {0, 0, 0, 0} };
	DlSliceIter<DlFloat> vec(rhs.row(1));
	//DlSliceIter<DlFloat> vec(eVector, {0, kMatrix.rows(), 1});
	
	DlFloat eValue = epower(kMatrix, vec);

	printf("largest is %lf\n", eValue);
	printf("evector is:\n");
	PrintVector(1, vec);
	printf("\n");

	DlMatrix<DlFloat> m(kMatrix);
	DlMatrix<DlFloat> eVectors(4,4);
	std::valarray<DlFloat> eigenValues(kMatrix.rows());

	nevalpower(m, eVectors, eigenValues);

	printf("eigenvalues: \n");
	
	PrintVector(eigenValues);
	
	printf("eigen vectors\n");
	PrintMatrix(DlMatrixTranspose<DlFloat>(eVectors));
	
	printf("\n");

	// check the eigen vectors
	checkIdentity(DlMatrixTranspose<DlFloat>(eVectors) * eVectors);

}

const VectorFormat kBigMathVector = { "{", ", ", "};\n", "", "%.13lf"};
const VectorFormat kBigMathMatrix = { "{", ", ", "},\n", "};\n", "%.13lf"};

TEST(TestEigen, TQLI)
{
	std::valarray<DlFloat> d(4);
	std::valarray<DlFloat> ee(4);
	DlMatrix<DlFloat> m(kMatrix);
	
	tred2(m, d, ee);
	
//	printf("\nQ Matrix\n");
//	PrintMatrix(m);
	
//	printf("\nDiagonal elements\n");
//	PrintVector(d);
	
//	printf("\nOffdiagonal elements\n");
//	PrintVector(ee);
	
	DlMatrix<DlFloat> v(4, 4);
	tqli(d, ee, v);
	
	printf("\nQL QR method\n");
	printf("eigen values\n");
	PrintVector(d);
	
//	printf("eigen vectors of QR system\n");
//	PrintMatrix(v, kBigMathMatrix);
	
	printf("\noriginal eigen vectors\n");
	DlMatrix<DlFloat> mm(m * v);
	PrintMatrix(mm);

	checkIdentity(DlMatrixTranspose<DlFloat>(mm) * mm);

}
