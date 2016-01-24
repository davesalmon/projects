//
//  TestSizeCheck.m
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
#include "DlTypes.h"

#include <typeinfo>

template <typename T>
void printTypeSize(const char * actName)
{
	printf("%s(%s) is %ld bytes\n", actName, typeid(T).name(), sizeof(T));
}

#define PrintTypeSize(type) printTypeSize<type>(#type)

@interface TestSizeCheck : XCTestCase

@end

@implementation TestSizeCheck

- (void)setUp {
    [super setUp];
    // Put setup code here. This method is called before the invocation of each test method in the class.
}

- (void)tearDown {
    // Put teardown code here. This method is called after the invocation of each test method in the class.
    [super tearDown];
}

- (void)testSizeCheck {
	
	PrintTypeSize(DlInt32);
	PrintTypeSize(DlInt64);
	PrintTypeSize(DlFloat);
	PrintTypeSize(DlFloat32);
	PrintTypeSize(DlFloat64);
	PrintTypeSize(double);
	PrintTypeSize(long double);
	PrintTypeSize(int);
	PrintTypeSize(long);
	PrintTypeSize(long long);
}

@end
