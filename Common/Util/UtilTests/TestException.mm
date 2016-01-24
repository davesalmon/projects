//
//  TexstException.m
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
#include "DlException.h"
#include "DlSimpException.h"
#include "DlStringUtil.h"

@interface TestException : XCTestCase

@end

@implementation TestException

- (void)setUp {
    [super setUp];
    // Put setup code here. This method is called before the invocation of each test method in the class.
}

- (void)tearDown {
    // Put teardown code here. This method is called after the invocation of each test method in the class.
    [super tearDown];
}

- (void)testExcept {
	
	int count = 0;
	
	while(true)  {
		
		// fprintf(stderr, "count is %d\n", count);
		
		try {
			if (count == 0) {
				count++;
				throw DlException("DlException: %s %d", "test", 11);
			}
			
			if (count == 1) {
				count++;
				throw DlSimpException("DlSimpException: %s %d", "test", 11);
			}
			
			if (count == 2) {
				count ++;
				throw DlException("quit");
			}
			
			XCTAssertTrue("failed to throw exception");
			
		} catch(const std::exception& ex) {
			
			if (count == 1)
				XCTAssertTrue(DlStrCmp(ex.what(), "DlException: test 11") == 0);
			else if (count == 2)
				XCTAssertTrue(DlStrCmp(ex.what(), "DlSimpException: test 11") == 0);
			else
			{
				XCTAssertTrue(DlStrCmp(ex.what(), "quit") == 0);
				break;
			}
			
		} catch(...) {
			XCTAssertTrue("Unknown exception" == 0);
		}
	}
}


@end
