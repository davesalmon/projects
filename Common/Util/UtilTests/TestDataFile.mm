//
//  TestDataFile.m
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
#include "DlDataFile.h"

@interface TestDataFile : XCTestCase

@end

const std::string kStringToWrite("this is a bunch of chars");

@implementation TestDataFile

- (void)setUp {
    [super setUp];
    // Put setup code here. This method is called before the invocation of each test method in the class.
}

- (void)tearDown {
    // Put teardown code here. This method is called after the invocation of each test method in the class.
    [super tearDown];
}

- (void)testFileIO {
	// test read/write
	try {
		DlDataFile f(DlFileSpec("/tmp/test.log"));
		
		f.OpenFile(DlOpenMode::DlReadWrite);
		
		XCTAssertTrue(f.GetSpec().Exists());
		
		DlUInt32 nChars = f.Write(kStringToWrite.c_str(), (DlUInt32)kStringToWrite.length());
		
		XCTAssertEqual(f.Tell(), nChars);
		
		f.Seek(0, DlFromStart);
		
		XCTAssertEqual(f.Tell(), 0);
		
		char buf[256];
		
		DlUInt32 rChars = f.Read(buf, sizeof(buf));
		
		XCTAssertEqual(rChars, nChars);
		XCTAssertTrue(f.AtEOF());
		
		XCTAssertEqual(f.Tell(), nChars);

		buf[rChars] = 0;
		XCTAssertTrue(kStringToWrite == buf);
		
	} catch(std::exception& ex) {
		XCTFail("Got DlDataFile exception %s", ex.what());
	}
	
	try {
		DlDataFile f(DlFileSpec("/tmp/test.log"));
		
		f.OpenFile(DlOpenMode::DlReadOnly);
		
		f.Seek(0, DlFromEnd);
		XCTAssertTrue(f.AtEOF());
		
		long ofs = kStringToWrite.length()/2;
		f.Seek(ofs, DlFromStart);
		f.Seek(-ofs, DlFromCurrent);
		
		XCTAssertEqual(f.Tell(), 0);

		f.Seek(0, DlFromStart);
		
		XCTAssertEqual(f.Tell(), 0);
		
		char buf[256];
		
		DlUInt32 rChars = f.Read(buf, sizeof(buf));
		
		XCTAssertEqual(rChars, kStringToWrite.length());
		
		XCTAssertEqual(f.Tell(), rChars);
		
		buf[rChars] = 0;
		XCTAssertTrue(kStringToWrite == buf);
		
	} catch(std::exception& ex) {
		XCTFail("Got DlDataFile exception %s", ex.what());
	}

}

//- (void)testPerformanceExample {
//    // This is an example of a performance test case.
//    [self measureBlock:^{
//        // Put the code you want to measure the time of here.
//    }];
//}

@end
