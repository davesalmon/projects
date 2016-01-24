//
//  TestUnicode.m
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
#include "DlUnicode.h"

@interface TestUnicode : XCTestCase

@end

@implementation TestUnicode

- (void)setUp {
    [super setUp];
    // Put setup code here. This method is called before the invocation of each test method in the class.
}

- (void)tearDown {
    // Put teardown code here. This method is called after the invocation of each test method in the class.
    [super tearDown];
}

- (void)testUnicode {
	
	const wchar_t* const kUniString { L"\u2155\u2154\u0498A" };
	const char* const kUTF8String { "\xe2\x85\x95\xe2\x85\x94\xd2\x98" "A" };
	
	XCTAssertTrue(DlUTF8Len(kUTF8String[0]) == 3);
	XCTAssertTrue(DlUTF8Len(kUTF8String[3]) == 3);
	XCTAssertTrue(DlUTF8Len(kUTF8String[6]) == 2);
	XCTAssertTrue(DlUTF8Len(kUTF8String[8]) == 1);
	XCTAssertTrue(DlUTF8Len(kUTF8String[9]) == 1);
	
	XCTAssertTrue(DlUTF8StrLen(kUTF8String) == 4);
	
	unistring s1(DlWideToUnicode(kUniString));
	unistring s2(DlUTF8ToUnicode(kUTF8String));
	XCTAssertTrue(s1 == s2);
}

@end
