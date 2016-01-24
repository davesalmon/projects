//
//  TestStrings.m
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

#include "DlString.h"
//#include "DlFileSpec.h"
//#include "DlStringUtil.h"
//#include "DlException.h"
//#include "DlSets.h"
//#include "DlStringTokenizer.h"
//#include "DlBufferedFileStream.h"

//#include <vector>

#include <iostream>

@interface TestString : XCTestCase

@end

@implementation TestString

- (void)setUp {
    [super setUp];
    // Put setup code here. This method is called before the invocation of each test method in the class.
}

- (void)tearDown {
    // Put teardown code here. This method is called after the invocation of each test method in the class.
    [super tearDown];
}

- (void)testDlString {
	DlString s;
	
	DlFloatFormat f { s.GetFloatingFormat() };
	//	f.
	
	s.Assign(12.0, f);
	XCTAssertEqual(s.getFloat(), 12.0);
	XCTAssertTrue(s.get() == std::string("12.0000"));
	printf("%s\n", s.get());
	
	f.setprecision(8);
	s.Assign(12.0, f);
	XCTAssertTrue(s.get() == std::string("12.00000000"));
	
	
	string ss(to_string(12.0));
	cout << ss << endl;
	cout << 12.0 << endl;
	
	cout << s << endl;
	
	try {
		DlStkPStr<100> fixedS;
		
		fixedS.Assign("This is a very long string (");
		fixedS.Append(12.0);
		fixedS.Append(')');
		fixedS.Append(" Hope that overflow works.");
		fixedS.Append(" Hope that overflow works.");
		fixedS.Append(" Hope that overflow works.");
		
		cout << (DlString&)fixedS;
		
		XCTFail("Fixed size string should fail.");
	} catch (const std::exception& ex) {
		printf("got expected exception %s", ex.what());
	}
}


@end
