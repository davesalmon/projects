//
//  TestSet.m
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

#include "DlSets.h"

@interface TestSet : XCTestCase

@end

#define MYAssertTrue(obj, expression, ...) \
_XCTPrimitiveAssertTrue(obj, expression, @#expression, __VA_ARGS__)

@implementation TestSet

- (void)setUp {
    [super setUp];
    // Put setup code here. This method is called before the invocation of each test method in the class.
}

- (void)tearDown {
    // Put teardown code here. This method is called after the invocation of each test method in the class.
    [super tearDown];
}

static void validateInSet(const DlSet& s, const char* str, const char* what, id obj) {
	for (int i = 0; i < 128; i++) {
		bool isInSet = i != 0 && strchr(str, i) != 0;
		if (DlSetMember(i, s) && !isInSet)
		{
//			syslog(1, "%s test failed with %c", what, i);
			MYAssertTrue(obj, !(DlSetMember(i, s) && !isInSet));
		}
	}
}

static void validateNotInSet(const DlSet& s, const char* str, const char* what, id obj) {
	for (int i = 0; i < 128; i++) {
		bool isInSet = i != 0 && strchr(str, i) != 0;
		if (DlSetMember(i, s) && isInSet)
		{
//			syslog(1, "%s test failed with %c", what, i);
			MYAssertTrue(obj, !(DlSetMember(i, s) && isInSet));
		}
	}
}

- (void)testSets {
	const char* const kSetString = "aeiouAEIO/?";
	const char* const kSetString2 = "aeiouAEIOU/?";
	const char* const kSetString3 = "aeiouEIOU/?";
	const char* const kSetString4 = "aeiouEIO/?";
	const char* const kSetString5 = "U";
	const char* const kSetString6 = "AU";
	DlSet	s = DlSetFromString(kSetString);
	
	validateInSet(s, kSetString, "DlSetMember", self);
	
	DlSet s2 = DlSetNot(s);
	
	validateNotInSet(s2, kSetString, "DlSetNot", self);
	
	DlSetAdd('U', s);
	validateInSet(s, kSetString2, "DlSetAdd", self);
	
	DlSetRemove('A', s);
	validateInSet(s, kSetString3, "DlSetRemove", self);
	
	DlSet s3 = DlSetUnion(DlSetFromString(kSetString), DlSetFromString(kSetString3));
	validateInSet(s3, kSetString2, "DlSetUnion", self);
	
	DlSet s4 = DlSetSect(DlSetFromString(kSetString), DlSetFromString(kSetString3));
	validateInSet(s4, kSetString4, "DlSetSect", self);
	
	DlSet s5 = DlSetNotSect(DlSetFromString(kSetString), DlSetFromString(kSetString3));
	validateNotInSet(s5, kSetString4, "DlSetNotSect", self);
	
	DlSet s6 = DlSetDiff(DlSetFromString(kSetString2), DlSetFromString(kSetString));
	validateInSet(s6, kSetString5, "DlSetDiff #1", self);
	
	DlSet s7 = DlSetDiff(DlSetFromString(kSetString), DlSetFromString(kSetString2));
	validateInSet(s7, kSetString5, "DlSetDiff #2", self);
	
	DlSet s8 = DlSetDiff(DlSetFromString(kSetString3), DlSetFromString(kSetString));
	validateInSet(s8, kSetString6, "DlSetDiff #3", self);
	
	DlSet s9 = DlSetDiff(DlSetFromString(kSetString3), DlSetFromString(kSetString3));
	validateInSet(s9, "", "DlSetDiff #3", self);
	XCTAssertTrue(DlSetIsEmpty(s9));
}

@end
