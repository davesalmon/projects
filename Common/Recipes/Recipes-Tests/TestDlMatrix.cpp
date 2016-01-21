//
//  TestDlMatrix.cpp
//  Common_Recipes
//
//  Created by David Salmon on 2/26/14.
//
//

#include "DlPlatform.h"
#include "TestDlMatrix.h"
#include "DlArray.h"
#include "DlMatrix.h"

#include "matio.h"

#include "gtest/gtest.h"

using namespace DlArray;

const std::valarray<DlFloat> kFloatArray { 1.0, 2.0, 3.0, 4.0, 5.0 };

const std::valarray<DlFloat> kFloatMatrix {
	11, 12, 13, 14,
	21, 22, 23, 24,
	31, 32, 33, 34,
	41, 42, 43, 44,
	51, 52, 53, 54
};

const DlMatrix<DlFloat> kTestMatrix {
	{11, 12, 13, 14, 15, 16},
	{21, 22, 23, 24, 25, 26},
	{31, 32, 33, 34, 35, 36},
	{41, 42, 43, 44, 45, 46},
	{51, 52, 53, 54, 55, 56},
	{61, 62, 63, 64, 65, 66},
};

template <typename T, DlUInt32 OFFSET>
inline
bool operator==(const DlMatrix<T,OFFSET>& a, const DlMatrix<T, OFFSET>& b)
{
	if (a.rows() != b.rows())
		return false;
	
	if (a.cols() != b.cols())
		return false;
	
	for (auto i = 0; i < a.rows(); i++)
	{
		auto arow = a[i+OFFSET];
		auto brow = b[i+OFFSET];
		for (auto j = 0; j < a.cols(); j++)
		{
			if (arow[j+OFFSET] != brow[j+OFFSET])
				return false;
		}
	}
	
	return true;
}

template <typename T, DlUInt32 OFFSET>
void
printMatrix(const DlMatrix<T, OFFSET>&m)
{
	for (auto r = 1; r <= m.rows(); r++)
	{
		auto row(m.row(r));
		for (auto c = 1; c <= m.cols(); c++)
			printf("%10.6g", row[c]);
		printf("\n");
	}
}

template <typename T, DlUInt32 OFFSET>
void
printMatrixTranspose(const DlMatrix<T, OFFSET>&m)
{
	for (auto c = 1; c <= m.cols(); c++)
	{
		auto col(m.col(c));
		for (auto r = 1; r <= m.rows(); r++)
			printf("%10.6g", col[r]);
		printf("\n");
	}
}

TEST(TestDlMatrix, Copy)
{
	DlMatrix<DlFloat> m(kTestMatrix);
	ASSERT_TRUE(m == kTestMatrix);
	
	DlMatrix<DlFloat> copy = (m = kTestMatrix);
	ASSERT_TRUE(copy == kTestMatrix);

	DlMatrix<DlFloat> move = std::move(m);
	ASSERT_TRUE(move == kTestMatrix);
	ASSERT_FALSE(m == kTestMatrix);
}

TEST(TestDlMatrix, BasicMatrix)
{
	DlMatrix<DlFloat> m(kFloatMatrix, 5, 4);
	for (auto r = 1; r <= m.rows(); r++)
	{
		for (auto c = 1; c <= m.cols(); c++)
			printf("%10.6g", m[r][c]);
		printf("\n");
	}
	printf("\n");
			
	printMatrix(m);

	printf("\n");

	printMatrixTranspose(m);
	
	printf("\n");

}

const DlMatrix<DlFloat> kFirst {
	{ 1, 2, 3, 4, 5 },
	{ 2, 3, 4, 5, 6 },
};

TEST(TestDlMatrix, Multiply)
{
	ASSERT_EQ(kFirst.rows(), 2);
	ASSERT_EQ(kFirst.cols(), 5);
	
	DlMatrix<DlFloat> prod(kFirst * DlMatrixTranspose<DlFloat>(kFirst));
	
	ASSERT_EQ(prod.rows(), 2);
	ASSERT_EQ(prod.cols(), 2);

	ASSERT_EQ(prod[1][1], 1*1 + 2*2 + 3*3 + 4*4 + 5*5);
	ASSERT_EQ(prod[1][2], 1*2+ 2*3 + 3*4 + 4*5 + 5*6);
	ASSERT_EQ(prod[2][1], prod[1][2]);
	
	ASSERT_EQ(prod[2][2], 2*2 + 3*3 + 4*4 + 5*5 + 6*6);
	
	printMatrix(prod);
	printf("\n");

	DlMatrix<DlFloat> prod2(DlMatrixTranspose<DlFloat>(kFirst) * kFirst);

	ASSERT_EQ(prod2.rows(), 5);
	ASSERT_EQ(prod2.cols(), 5);

	ASSERT_EQ(prod2[1][1], 1*1 + 2*2);
	ASSERT_EQ(prod2[1][2], 1*2 + 2*3);
	ASSERT_EQ(prod2[1][3], 1*3 + 2*4);
	ASSERT_EQ(prod2[1][4], 1*4 + 2*5);
	ASSERT_EQ(prod2[1][5], 1*5 + 2*6);
	
	ASSERT_EQ(prod2[2][1], 2*1 + 3*2);
	ASSERT_EQ(prod2[2][2], 2*2 + 3*3);
	ASSERT_EQ(prod2[2][3], 2*3 + 3*4);
	ASSERT_EQ(prod2[2][4], 2*4 + 3*5);
	ASSERT_EQ(prod2[2][5], 2*5 + 3*6);

	ASSERT_EQ(prod2[3][1], 3*1 + 4*2);
	ASSERT_EQ(prod2[3][2], 3*2 + 4*3);
	ASSERT_EQ(prod2[3][3], 3*3 + 4*4);
	ASSERT_EQ(prod2[3][4], 3*4 + 4*5);
	ASSERT_EQ(prod2[3][5], 3*5 + 4*6);

	ASSERT_EQ(prod2[4][1], 4*1 + 5*2);
	ASSERT_EQ(prod2[4][2], 4*2 + 5*3);
	ASSERT_EQ(prod2[4][3], 4*3 + 5*4);
	ASSERT_EQ(prod2[4][4], 4*4 + 5*5);
	ASSERT_EQ(prod2[4][5], 4*5 + 5*6);

	ASSERT_EQ(prod2[5][1], 5*1 + 6*2);
	ASSERT_EQ(prod2[5][2], 5*2 + 6*3);
	ASSERT_EQ(prod2[5][3], 5*3 + 6*4);
	ASSERT_EQ(prod2[5][4], 5*4 + 6*5);
	ASSERT_EQ(prod2[5][5], 5*5 + 6*6);

	printMatrix(prod2);
	printf("\n");

	// and set some values
	for (auto i = 1; i <= prod2.rows(); i++)
		for (auto j = 1; j <= prod2.cols(); j++)
			prod2[i][j] = i*j;
	
	printMatrix(prod2);
	printf("\n");
	
}

TEST(TestDlMatrix, Matio)
{
	PrintMatrix(kFirst);
	printf("\n");
	PrintMatrix(DlMatrix<DlFloat>(kFloatMatrix, 5, 4));
	printf("\n");
	PrintMatrix(DlMatrixTranspose<DlFloat>(DlMatrix<DlFloat>(kFloatMatrix, 5, 4)));
	printf("\n");
	PrintMatrix(DlMatrix<DlFloat>(kFloatMatrix, 4, 5));

	printf("\n");

}