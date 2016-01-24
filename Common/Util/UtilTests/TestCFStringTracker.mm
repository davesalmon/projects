/*+
 *
 *  TestCFStringTracker.mm
 *
 *  Copyright © 2014 David C. Salmon. All Rights Reserved.
 *
 *  test the string tracker code.
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

#include "DlStringUtil.h"
#include "CFStringTracker.h"

#include <string>

@interface TestCFStringTracker : XCTestCase

@end

@implementation TestCFStringTracker

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
//  testCString
//
//      test c-string tracking.
//
//  returns nothing
//----------------------------------------------------------------------------------------
- (void)testCString {
	CFStringTracker bndlKey("this is a test");
	XCTAssert([@"this is a test" compare: (NSString*)bndlKey.get()] == NSOrderedSame);
}

//----------------------------------------------------------------------------------------
//  testWString
//
//      test w-string tracking.
//
//  returns nothing
//----------------------------------------------------------------------------------------
- (void)testWString {
	NSString* s = @"this \u2193 is a test";
	
	CFStringTracker bndlKey(L"this \u2193 is a test");
	
	XCTAssert([s compare: (NSString*)bndlKey.get()] == NSOrderedSame);
	[s release];
}

//----------------------------------------------------------------------------------------
//  testUString
//
//      test u-string tracking.
//
//  returns nothing
//----------------------------------------------------------------------------------------
- (void)testUString {
	NSString* s = @"this \u2193 is a test";
	unistring us((const UniChar*)u"this \u2193 is a test");
	
	CFStringTracker bndlKey(us.c_str(), us.length());
	
	XCTAssert([s compare: (NSString*)bndlKey.get()] == NSOrderedSame);
	[s release];
}

//----------------------------------------------------------------------------------------
//  testGetBundleString
//
//      test getting strings from a bundle resource.
//
//  returns nothing
//----------------------------------------------------------------------------------------
- (void)testBundleString {
	
	// for the test app, we need to get strings from the test bundle, not the main one.
	CFBundleRef b(CFBundleGetBundleWithIdentifier((CFStringRef)@"dcs.UtilTests"));
	
	if (b)
	{
		CFStringTracker value((CFStringRef)@"astring", (CFStringRef)@"UtilTest", b);

		XCTAssert([@"this is a string" compare: (NSString*)value.get()] == NSOrderedSame);
	} else {
		XCTAssert(NO, @"failed to get bundle");
	}
}

@end
