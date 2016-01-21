//
//  TestDlArray.cpp
//  Common_Recipes
//
//  Created by David Salmon on 2/25/14.
//
//

#include "DlPlatform.h"
#include "TestCholesky.h"
#include "TestDlArray.h"

#include "DlArray.h"

#include "gtest/gtest.h"

using namespace DlArray;

const std::valarray<DlFloat> kFloatArray { 1.0, 2.0, 3.0, 4.0, 5.0 };

const std::valarray<DlFloat> kFloatMatrix {
	11, 12, 13, 14,
	21, 22, 23, 24,
	31, 32, 33, 34,
	41, 42, 43, 44 };

template <typename DlVector>
static void printVector(const DlVector& v, bool oneBased=false)
{
	std::string outputLine;
	
	DlUInt32 start = oneBased ? 1 : 0;
	DlUInt32 end = oneBased ? v.size() + 1 : v.size();
	
	for (auto j = start; j < end; j++)
    {
        char buf[64];
        snprintf(buf, sizeof(buf), j != start ? "  %10.6g" : "%10.6g", v[j]);
        outputLine.append(buf);
	}
    
	printf("%s\n", outputLine.c_str());
}

// test the const slice object
TEST(TestDlArray, ConstSlice)
{
	// one-based slice
	for (int i = 0 ; i < 4; i++)
	{
		DlSliceConstIter<DlFloat> cols(kFloatMatrix, slice(i, 4, 4));
		printVector(cols, true);
		ASSERT_EQ(cols[1], kFloatMatrix[i]);
		ASSERT_EQ(cols[2], kFloatMatrix[i + 4]);
		ASSERT_EQ(cols[3], kFloatMatrix[i + 8]);
		ASSERT_EQ(cols[4], kFloatMatrix[i + 12]);
	}
	
	printf("\n");
	
	// 0 based slice
	for (int i = 0 ; i < 4; i++)
	{
		DlSliceConstIter<DlFloat, 0> cols(kFloatMatrix, slice(i, 4, 4));
		printVector(cols, false);
		ASSERT_EQ(cols[0], kFloatMatrix[i]);
		ASSERT_EQ(cols[1], kFloatMatrix[i + 4]);
		ASSERT_EQ(cols[2], kFloatMatrix[i + 8]);
		ASSERT_EQ(cols[3], kFloatMatrix[i + 12]);
	}
}

// test the non-const slice
TEST(TestDlArray, Slice)
{
	std::valarray<DlFloat> test(kFloatMatrix);

	for (int i = 0 ; i < 4; i++)
	{
		DlSliceIter<DlFloat> cols(test, slice(i, 4, 4));
		printVector(cols, true);
		ASSERT_EQ(cols[1], kFloatMatrix[i]);
		ASSERT_EQ(cols[2], kFloatMatrix[i + 4]);
		ASSERT_EQ(cols[3], kFloatMatrix[i + 8]);
		ASSERT_EQ(cols[4], kFloatMatrix[i + 12]);
	}
	
	printf("\n");
	
	for (int i = 0 ; i < 4; i++)
	{
		DlSliceIter<DlFloat, 0> cols(test, slice(i, 4, 4));
		printVector(cols, false);
		ASSERT_EQ(cols[0], kFloatMatrix[i]);
		ASSERT_EQ(cols[1], kFloatMatrix[i + 4]);
		ASSERT_EQ(cols[2], kFloatMatrix[i + 8]);
		ASSERT_EQ(cols[3], kFloatMatrix[i + 12]);
	}
	
}

// test the array iterator
TEST(TestDlArray, Iterator)
{
	std::valarray<DlFloat> test(kFloatArray);
	
	DlArray::DlArrayIter<DlFloat> iter(test);

	printVector(iter);

	for (DlFloat x : kFloatArray)
	{
		ASSERT_EQ(x, *iter);
		*iter += 1;
		++iter;
	}

	iter -= static_cast<DlInt32>(iter.size());
	
	printVector(iter);

	for (DlFloat x : kFloatArray)
	{
		ASSERT_EQ(x+1, *iter);
		*iter -= 1;
		++iter;
	}

	iter -= static_cast<DlInt32>(iter.size());

	printVector(iter);

	for (DlFloat x : kFloatArray)
	{
		ASSERT_EQ(x, *iter);
		++iter;
	}
}

// and the one-based iterator.
TEST(TestDlArray, DlOneBasedIter)
{
	std::valarray<DlFloat> test(kFloatArray);
	
	DlOneBasedIter<DlFloat> iter(test);
	
	printVector(iter, true);
	
	DlInt32 ofs = 1;
	for (DlFloat x : kFloatArray)
	{
		ASSERT_EQ(x, iter[ofs++]);
	}
}