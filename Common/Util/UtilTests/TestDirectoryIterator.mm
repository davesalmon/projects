//
//  TestDirectoryIterator.m
//
//  Copyright © 2014 David C. Salmon. All Rights Reserved.
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
#include "DlDirectoryIterator.h"
#include "DlFileSpec.h"
#include "DlStringUtil.h"

@interface TestDirectoryIterator : XCTestCase

@end

@implementation TestDirectoryIterator

- (void)setUp {
    [super setUp];
	// create a heirarchy in tmp.
	
	try {
		DlFileSpec::CreateDirectoryPath("/tmp/level_1/level_11");
		DlFileSpec::CreateDirectoryPath("/tmp/level_1/level_12");
		DlFileSpec::CreateDirectoryPath("/tmp/level_1/level_13");
		DlFileSpec::CreateDirectoryPath("/tmp/level_1/level_14");
		DlFileSpec::CreateDirectoryPath("/tmp/level_1/level_15");
	} catch(std::exception& ex) {
		printf("Caught exception %s\b", ex.what());
		abort();
	}
}

- (void)tearDown {
	
	DlFileSpec::PathDelete("/tmp/level_1", true);
    // Put teardown code here. This method is called after the invocation of each test method in the class.
    [super tearDown];
}

- (void)testIterator {
	
	try {
		
		DlDirectoryIterator dir(DlFileSpec("/tmp/level_1"));
		
		XCTAssertEqual(dir.GetCount(), 5);
		
		DlFileSpec spec;
		DlInt32 count = 0;
		while(dir.GetNextItem(spec))
		{
			XCTAssertTrue(spec.IsDirectory());
			XCTAssertTrue(DlStrNCmp(spec.GetName().c_str(), "level_1", 7) == 0);

			count++;
		}
		
		XCTAssertEqual(count, 5);
		
	} catch(std::exception & ex) {
		XCTFail("Caught exception %s", ex.what());
	}
	
    // This is an example of a functional test case.
    XCTAssert(YES, @"Pass");
}

//- (void)testPerformanceExample {
//    // This is an example of a performance test case.
//    [self measureBlock:^{
//        // Put the code you want to measure the time of here.
//    }];
//}

@end
