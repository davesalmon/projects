//
//  TestBinaryStream.m
//
//  Copyright © 2014 David C. Salmon. All Rights Reserved.
//
//
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

#import <Cocoa/Cocoa.h>
#import <XCTest/XCTest.h>

#include "DlPlatform.h"
#include "DlBinaryStream.h"
#include "DlStrStream.h"
#include "DebugNew.h"

#include <cmath>

@interface TestBinaryStream : XCTestCase
{
}

@end

const int INT_COUNT = 27;
const int DOUBLE_COUNT = 11;
const int FLOAT_COUNT = 13;

@implementation TestBinaryStream

//----------------------------------------------------------------------------------------
//  setUp
//
//      nothing
//
//  returns nothing
//----------------------------------------------------------------------------------------
- (void)setUp {
    [super setUp];
    // Put setup code here. This method is called before the invocation of each test method in the class.

	DebugNewForgetLeaks();
}

//----------------------------------------------------------------------------------------
//  tearDown
//
//      nothing
//
//  returns nothing
//----------------------------------------------------------------------------------------
- (void)tearDown {
    // Put teardown code here. This method is called after the invocation of each test method in the class.
    [super tearDown];
	
	DebugNewReportLeaks("/tmp/TestBinaryStream-leaks.log");
}

//----------------------------------------------------------------------------------------
//  testReadWriteLE
//
//      test reading and writing little-endian.
//
//  returns nothing
//----------------------------------------------------------------------------------------
- (void)testReadWriteLE {

	DlStrStream s;

	DlBinaryStream bs(s);
	
	for (DlInt32 i = 0; i < INT_COUNT; i++) {
		bs.WriteLE(i);
	}
	
	for (auto i = 0; i < DOUBLE_COUNT; i++) {
		double d = sqrt((double)i);
		bs.WriteLE(d);
	}

	for (auto i = 0; i < FLOAT_COUNT; i++) {
		float d = sqrt((float)i);
		bs.WriteLE(d);
	}

	bs.Seek(0, DlFromStart);
	
	for (DlInt32 i = 0; i < INT_COUNT; i++) {
		DlInt32 n;
		bs.ReadLE(n);
		
		XCTAssertEqual(n, i);
	}
	
	for (auto i = 0; i < DOUBLE_COUNT; i++) {
		double d;
		bs.ReadLE(d);
		XCTAssertEqual(sqrt((double)i), d);
	}
	
	for (auto i = 0; i < FLOAT_COUNT; i++) {
		float d;
		bs.ReadLE(d);
		float t = sqrt((float)i);
		XCTAssertEqual(t, d);
	}
}

//----------------------------------------------------------------------------------------
//  testReadWriteBE
//
//      test reading and writing big-endian.
//
//  returns nothing
//----------------------------------------------------------------------------------------
- (void)testReadWriteBE {
	
	DlStrStream s;

	DlBinaryStream bs(s);
	
	for (DlInt32 i = 0; i < INT_COUNT; i++) {
		bs.WriteBE(i);
	}
	
	for (auto i = 0; i < DOUBLE_COUNT; i++) {
		double d = sqrt((double)i);
		bs.WriteBE(d);
	}
	
	for (auto i = 0; i < FLOAT_COUNT; i++) {
		float d = sqrt((float)i);
		bs.WriteBE(d);
	}

	bs.Seek(0, DlFromStart);
	
	for (DlInt32 i = 0; i < INT_COUNT; i++) {
		DlInt32 n;
		bs.ReadBE(n);
		XCTAssertEqual(n, i);
	}
	
	for (auto i = 0; i < DOUBLE_COUNT; i++) {
		double d;
		bs.ReadBE(d);
		XCTAssertEqual(sqrt((double)i), d);
	}
	
	for (auto i = 0; i < FLOAT_COUNT; i++) {
		float d;
		bs.ReadBE(d);
		float t = sqrt((float)i);
		XCTAssertEqual(t, d);
	}
}

@end
