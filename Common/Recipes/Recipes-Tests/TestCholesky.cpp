/*
 *  choleskyTest.cpp
 *  Common_Recipes
 *
 *  Created by David Salmon on 12/27/05.
 *  Copyright 2005 __MyCompanyName__. All rights reserved.
 *
 */

#include "DlPlatform.h"
#include "TestCholesky.h"
#include "matrix.h"
#include <memory>
#include <syslog.h>
#include <string>

#include <valarray>

#include "gtest/gtest.h"

const DlInt32 kMatrixSize = 4;
const DlInt32 kArraySize = (kMatrixSize) * (kMatrixSize + 1) / 2;

const DlFloat	kMatrix[kArraySize] = {
	12.0,
    4.0, 12.0,
    2.0,  8.0,  9.0,
    0.0,  1.0,  2.0, 4.0 
};

const DlFloat kMatrixResults[kArraySize] = {
    0.0948492462311558, 
    -0.04271356783919598, 0.2311557788944723, 
    0.01633165829145729, -0.2060301507537688, 0.314070351758794, 
    0.002512562814070352, 0.04522613065326634, -0.1055276381909548, 0.2914572864321608
};

const std::valarray<DlFloat> kValMatrix {
	12.0,
    4.0, 12.0,
    2.0,  8.0,  9.0,
    0.0,  1.0,  2.0, 4.0
};

static void printVec(const DlFloat* vec, DlInt32 n)
{
    std::string outputLine;
	for (DlInt32 j = 0; j < n; j++)
    {
        char buf[64];
        snprintf(buf, sizeof(buf), j != 0 ? "  %10.6g" : "%10.6g", vec[j]);
        outputLine.append(buf);
	}
    
	printf("%s\n", outputLine.c_str());
}

void printVec(const std::valarray<DlFloat> &vec)
{
    std::string outputLine;
	for (DlInt32 j = 0; j < vec.size(); j++)
    {
        char buf[64];
        snprintf(buf, sizeof(buf), j != 0 ? "  %10.6g" : "%10.6g", vec[j]);
        outputLine.append(buf);
	}
    
	printf("%s\n", outputLine.c_str());
}


inline DlInt32 compIndex(DlInt32 i, DlInt32 j)
{
	DlInt32 r;
	DlInt32 c;
	if (i > j) {
		r = i; c = j;
	} else {
		r = j, c = i;
	}
	
	return c + r * (r + 1) / 2;
}

static void printMatrix(const DlFloat* m, DlInt32 n)
{
	for (DlInt32 i = 0; i < n; i++) {
        std::string outputLine;
        
		for (DlInt32 j = 0; j < n; j++) {
            char buf[64];
            
			DlInt32 index = compIndex(i, j);
			snprintf(buf, sizeof(buf), j != 0 ? "  %10.6g" : "%10.6g", m[index]);
            outputLine.append(buf);
		}
		printf("%s\n", outputLine.c_str());
	}
}

TEST(Cholesky, pointers)
{
    std::unique_ptr<DlFloat[]>	m(new DlFloat[kArraySize]);
	memcpy(m.get(), kMatrix, sizeof(kMatrix));
	
	printMatrix(m.get(), kMatrixSize);
	
	try {
		EXPECT_NO_THROW(::cholesky(m.get(), kMatrixSize));
	} catch(EqSolveFailure& ex) {
		syslog(1, "cholesky failed at equation %d with error %ld\n",
                ex.getEquation(), (long)ex.getReason());
		//EXPECT_FALSE("failed" == 0);
	}
	
	printf("\n");
	
	std::unique_ptr<DlFloat[]>	rhs(new(DlFloat[kMatrixSize]));
	
	for (DlInt32 i = 0; i < kMatrixSize; i++) {
		for (DlInt32 j = 0; j < kMatrixSize; j++) {
			rhs.get()[j] = i == j ? 1.0 : 0.0;
		}

		::cholesbs(m.get(), kMatrixSize, rhs.get());
		
        for (DlInt32 j = 0; j < kMatrixSize; j++)
        {
            DlInt32 index = compIndex(i, j);
        	EXPECT_TRUE(fabs(kMatrixResults[index] - rhs.get()[j]) < 1.0e-15);
        }
        
		printVec(rhs.get(), kMatrixSize);
	}
}

TEST(Cholesky, valarray)
{
	std::valarray<DlFloat> m(kValMatrix);
	try {
		EXPECT_NO_THROW(::cholesky(m));
	} catch(EqSolveFailure& ex) {
		syslog(1, "cholesky failed at equation %d with error %ld\n",
			   ex.getEquation(), (long)ex.getReason());
		//EXPECT_FALSE("failed" == 0);
	}

	std::valarray<DlFloat>	rhs(kMatrixSize);
	
	printf("size is %ld\n", rhs.size());
		
	for (DlInt32 i = 0; i < kMatrixSize; i++) {
		for (DlInt32 j = 0; j < kMatrixSize; j++) {
			rhs[j] = i == j ? 1.0 : 0.0;
		}
		
//		printVec(rhs);

		::cholesbs(m, rhs);
		
        for (DlInt32 j = 0; j < kMatrixSize; j++)
        {
            DlInt32 index = compIndex(i, j);
        	EXPECT_TRUE(fabs(kMatrixResults[index] - rhs[j]) < 1.0e-15);
        }
        
		printVec(rhs);
	}

}


//TestCholesky choleskyTest(TEST_INVOCATION(TestCholesky, test));
