//
//  TestStringStream.m
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
#include "DlStrStream.h"

#include "DlStringUtil.h"

#include <string>

const std::string kFileLine("This is a line in the file");
const size_t kFileLineSize = kFileLine.length() + 1;
const size_t kNumWriteLines = 5;

@interface TestStringStream : XCTestCase

@end

@implementation TestStringStream

- (void)setUp {
    [super setUp];
    // Put setup code here. This method is called before the invocation of each test method in the class.
}

- (void)tearDown {
    // Put teardown code here. This method is called after the invocation of each test method in the class.
    [super tearDown];
}

- (void)testReadWrite {
	try {
		DlStrStream s;
		
		DlUInt32 count = 0;
		for (auto i = 0; i < kNumWriteLines; i++) {
			s.WriteBytes(kFileLine.data(), (DlUInt32)kFileLine.length());
			s.WriteBytes("\n", 1);
			count += kFileLine.length() + 1;
		}
		
		XCTAssertEqual(s.Buffer().length(), count);
		
		s.Seek(0, DlFromStart);
		
		XCTAssertEqual(s.Tell(), 0);
		
		char buf[256];
		
		DlUInt32 nRead = s.ReadBytes(buf, sizeof(buf));
		XCTAssertEqual(nRead, kNumWriteLines * kFileLineSize);
		XCTAssertTrue(DlStrNCmp(kFileLine.c_str(), buf, kFileLine.length())==0);
		
	} catch(std::exception& ex) {
		XCTFail("Caught exception %s", ex.what());
	}
}

@end
