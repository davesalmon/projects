//
//  TestAppSupportFiles.m
//
//  Copyright © 2014 David C. Salmon. All Rights Reserved.
//
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
#include "DlAppSupportFiles.h"
#include "DlFileSpec.h"
#include "DlStringUtil.h"

@interface TestAppSupportFiles : XCTestCase

@end

@implementation TestAppSupportFiles

//----------------------------------------------------------------------------------------
//  setUp
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
//  returns nothing
//----------------------------------------------------------------------------------------
- (void)tearDown {
    // Put teardown code here. This method is called after the invocation of each test method in the class.
    [super tearDown];
}

//----------------------------------------------------------------------------------------
//  testGetApplicationSupportFolder
//
//      test creating folder in app support location.
//
//  returns nothing
//----------------------------------------------------------------------------------------
- (void)testGetApplicationSupportFolder {
	
	std::string path = DlGetApplicationSupportFolder(kBundleIdFolder);
	
	XCTAssert(DlEndsWith(path.c_str(), DlGetBundleId().c_str()));
	
	DlFileSpec spec(path.c_str());

	XCTAssert(spec.Exists());
	XCTAssert(spec.IsDirectory());
	
	if (spec.IsDirectory()) {
		DlRemoveFolderFromApplicationSupport("test");
	} else {
		XCTFail("Directory \"%s\" not found", path.c_str());
	}
}

//----------------------------------------------------------------------------------------
//  testGetApplicationLogPath
//
//      test crating the path to an application log file.
//
//  returns nothing
//----------------------------------------------------------------------------------------
- (void)testGetApplicationLogPath {
	
	std::string path = DlGetApplicationLogPath("test");
	
	XCTAssert(path.length() > 0, "path found is \"%s\"", path.c_str());
	
	XCTAssert(strstr(path.c_str(), "Library/Logs") != 0);
	
//	DlFileSpec spec(path.c_str());
}

//----------------------------------------------------------------------------------------
//  testGetBundleId
//
//      test getting the bundleid.
//
//  returns nothing
//----------------------------------------------------------------------------------------
- (void) testGetBundleId {
	printf("Bundle id is %s\n", DlGetBundleId().c_str());
	XCTAssert("dcs.UtilTests" == DlGetBundleId());
}

@end
