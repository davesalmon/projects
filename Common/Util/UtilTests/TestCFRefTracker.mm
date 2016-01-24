/*+
 *
 *  TestCFRefTracker.mm
 *
 *  Copyright © 2014 David C. Salmon. All Rights Reserved.
 *
 *  test the basic reference tracker.
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

#include "DlPlatform.h"
#import <Cocoa/Cocoa.h>
#import <XCTest/XCTest.h>

#include "CFRefTracker.h"

@interface TestCFRefTracker : XCTestCase

@end

@implementation TestCFRefTracker

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
}

//----------------------------------------------------------------------------------------
//  testTracker
//
//      test the tracker for retain/release.
//
//  returns nothing
//----------------------------------------------------------------------------------------
- (void)testTracker {
	
	CFStringRef test = CFStringCreateWithCString(kCFAllocatorDefault,
												 "this is a string", kCFStringEncodingUTF8);
	CFRetain(test);
	{
		XCTAssert(CFGetRetainCount(test) == 2);
		CFRefTracker<CFStringRef> ref(test);
		
		{
			CFRefTracker<CFStringRef> test2(ref);
			XCTAssert(CFGetRetainCount(test) == 3);
		}
		XCTAssert(CFGetRetainCount(test) == 2);
	}
	
	XCTAssert(CFGetRetainCount(test) == 1);
	
	CFRelease(test);
}

//- (void)testPerformanceExample {
//    // This is an example of a performance test case.
//    [self measureBlock:^{
//        // Put the code you want to measure the time of here.
//    }];
//}

@end
