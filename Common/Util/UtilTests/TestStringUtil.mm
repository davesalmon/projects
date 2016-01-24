//
//  TestStringUtil.m
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
#include "DlStringUtil.h"

#include "DlFileSpec.h"
#include "DlBufferedFileStream.h"
#include "DlStringTokenizer.h"

#include "DlSets.h"

//#include "DlException.h"

#include <vector>

const char * const kTestFolder = "tests/pstrings";

@interface TestStringUtil : XCTestCase

@end

@implementation TestStringUtil

- (void)setUp {
    [super setUp];
//	DlFileSpec::CreateDirectoryPath(kTestFolder);
}

- (void)tearDown {
//	DlFileSpec::PathDelete(kTestFolder, true);
	[super tearDown];
}

- (void)testToPString {
	
	DlFileRef resourceRef;
	
	try
	{
		std::string s1;
		std::string s2;
		std::string s3;
		
		{
			DlFileSpec	textFile(DlFileSpec(kTestFolder), "testStrings.txt", false, false);
			
			DlBufferedFileStream inStream(textFile, DlOpenMode::DlReadOnly);
			
			XCTAssertTrue(inStream.ReadLine(s1));
			XCTAssertTrue(inStream.ReadLine(s2));
			XCTAssertTrue(inStream.ReadLine(s3));
		}
		
		XCTAssertTrue(s1.length() != 0);
		XCTAssertTrue(s2.length() != 0);
		XCTAssertTrue(s3.length() != 0);
		
		//
		//	convert to pstring
		//
		Str255	ps1;
		DlStringToPString(s1, ps1);
		
		Str255	ps2;
		DlStringToPString(s2.c_str(), ps2);
		
		//
		//	check conversions
		//
		XCTAssertTrue(DlPStringToString(ps1) == s1);
		XCTAssertTrue(DlPStringToString(ps2) == s2);
		
		//
		//	check comparisons
		//
		XCTAssertTrue(DlStrICmp(s2.c_str(), s1.c_str()) == 0);
		XCTAssertTrue(DlStrICmp(s2, s1) == 0);
		XCTAssertTrue(DlStrNICmp(s3.c_str(), s1.c_str(), s3.length()) == 0);
		XCTAssertTrue(DlStrNICmp("this is", "this", 4) == 0);
		XCTAssertTrue(DlStrNICmp("this is", "this", 5) != 0);
	}
	catch(const std::exception& ex)
	{
		printf("Exception occurred %s", ex.what());
		XCTFail("File Open Failed");
	}
}

- (void)testTrim {
	//
	//	check trim
	//
	std::string test("  \t\nThis is a string\n\n\t\t  ");
	std::string s4 = DlStringTrimLeft(test);
	XCTAssertTrue(s4 == "This is a string\n\n\t\t  ");
	
	s4 = DlStringTrimRight(test);
	XCTAssertTrue(s4 == "  \t\nThis is a string");
	
	s4 = DlStringTrimBoth(test);
	XCTAssertTrue(s4 == "This is a string");
	
	//	test empty result
	test = "   \t\t\n\n";
	XCTAssertTrue(DlStringTrimLeft(test) == "");
	XCTAssertTrue(DlStringTrimRight(test) == "");
	XCTAssertTrue(DlStringTrimBoth(test) == "");
}

- (void)testSplit {
	std::string s("This\nis\na\ndelimited\nstring\n");
	
	vector<std::string> v;
	int count = DlStringSplit(v, s.c_str(), "\n");
	
	XCTAssertEqual(count, 5);
	
	DlStringTokenizer tok(s.c_str(), "\n");
	
	for (std::string ss : v)
	{
		XCTAssertTrue(tok.hasMoreTokens());
		XCTAssertTrue(tok.nextToken() == ss);
	}
}

@end
