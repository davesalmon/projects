/*+
 *
 *  TestBufferedStreams.mm
 *
 *  Copyright © 2014 David C. Salmon. All Rights Reserved.
 *
 *  unit tests for buffered stream code.
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

#import <Cocoa/Cocoa.h>
#import <XCTest/XCTest.h>

#include "DlPlatform.h"
#include "DlBufferedFileStream.h"
#include "DlBufferedStrStream.h"

#include <vector>
#include <exception>
#include <syslog.h>

using namespace std;

const std::vector<const char*>  testLines = {
	"This is the test file.",
	"",
	"Line 3",
	"",
	"Line 5",
	"",
	"this is the last line. Line 7. No return."
};

const char* const kExceptionValue = "this is the exception";

@interface TestBufferedStreams : XCTestCase

@end

@implementation TestBufferedStreams

//----------------------------------------------------------------------------------------
//  setUp
//
//      
//
//  returns nothing
//----------------------------------------------------------------------------------------
- (void)setUp {
    [super setUp];
	DebugNewForgetLeaks();
}

//----------------------------------------------------------------------------------------
//  tearDown
//
//      
//
//  returns nothing
//----------------------------------------------------------------------------------------
- (void)tearDown {
	DebugNewReportLeaks("/tmp/TestBufferedStreams-leaks.log");
    [super tearDown];
}

//----------------------------------------------------------------------------------------
//  runTestStream:
//
//      run the test stream, both string and file.
//
//  runTestStream: DlBufferedStream& buffer    -> 
//
//  returns nothing
//----------------------------------------------------------------------------------------
- (void)runTestStream:(DlBufferedStream&) buffer
{
	int writeLineCount = 0;
	for (auto str : testLines) {
		buffer.WriteLine(str);
		if (strlen(str))
			writeLineCount++;
	}
	
	for (auto i = 0; i < 10; i++) {
		buffer.Write(i);
		buffer.WriteChar('\n');
		writeLineCount++;
	}
	
	//		printf("value is %s\n", buffer.Buffer());
	
	buffer.Seek(0, DlFromStart);
	
	std::string line;
	int ofs = 0;
	while (buffer.ReadLine(line)) {
		//			printf("got line %s\n", line.c_str());
		if (ofs < testLines.size())
			XCTAssertEqual(line, testLines[ofs]);
		else
			XCTAssertEqual(atoi(line.c_str()), ofs - testLines.size());
		ofs++;
	}
	
	//		printf("%s\n", line.c_str());
	
	buffer.Seek(0, DlFromStart);
	
	int lineCount = 0;
	while(buffer.ReadNextLine(line))
		lineCount++;
	
	XCTAssertEqual(lineCount, writeLineCount);
}

//----------------------------------------------------------------------------------------
//  testExcept
//
//      test exception throw.
//
//  returns nothing
//----------------------------------------------------------------------------------------
- (void)testExcept {
	
	try {
		throw DlException(kExceptionValue);
		XCTAssertFalse("We cannot be here");
	} catch (const std::exception& ex) {
		XCTAssertTrue(strcmp(ex.what(), kExceptionValue) == 0);
	}
}


//----------------------------------------------------------------------------------------
//  testStringStream
//
//      test string stream execution.
//
//  returns nothing
//----------------------------------------------------------------------------------------
- (void)testStringStream {
	try {
		DlBufferedStrStream	buffer;
		
		[self runTestStream: buffer];
		
	} catch (const std::exception& ex) {
		printf("DlBufferedStrStream failed -- %s\n", ex.what());
		XCTAssertFalse("DlBufferedStrStream test failed");
	}
}

//----------------------------------------------------------------------------------------
//  testFileStream
//
//      test file stream execution.
//
//  returns nothing
//----------------------------------------------------------------------------------------
- (void)testFileStream {
	try {
		DlFileSpec folder(DlFileSpec::CreateDirectoryPath("/tmp/bufferedTest"));
		
		DlBufferedFileStream buffer(DlFileSpec("/tmp/bufferedTest/testIn.txt"), DlOpenMode::DlReadWrite);
		[self runTestStream: buffer];
		
	} catch (const std::exception& ex) {
		printf("DlBufferedFileStream failed -- %s\n", ex.what());
		XCTAssertFalse("DlBufferedFileStream test failed");
	}
	
	try {
		printf("creating file\n");
		
		DlFileSpec folder(DlFileSpec::CreateDirectoryPath("/tmp/bufferedTest"));
		
		DlFileSpec	spec("/tmp/bufferedTest/testIn.txt", true);
		
		{	//	create the test file
			printf("buffered: writing file\n");
			DlBufferedFileStream s1(spec, DlOpenMode::DlWriteOnly);
			
			for (auto testLine : testLines) {
				//printf("writing line \"%s\"\n", testLine);
				s1.WriteLine(testLine);
			}
			printf("buffered: close file\n");
		}
		
		string str;
		size_t count = 0;
		
		printf("reopening file\n");
		DlBufferedFileStream s(spec, DlOpenMode::DlReadOnly);
		DlBufferedStrStream s2;
		while (s.ReadLine(str)) {
			
			XCTAssertTrue(count < testLines.size());
			XCTAssertTrue(str == testLines[count]);
			s2.WriteLine(str);
			
			count++;
		}
		
		XCTAssertTrue(count == testLines.size());
		
		s2.Flush();
		
		for (size_t i = 0; i < count; i++) {
			XCTAssertTrue(s2.ReadLine(str));
			XCTAssertTrue(str == testLines[i]);
		}
		
		// and make sure we cant create directory
		try
		{
			DlFileSpec	spec("/tmp/bufferedTest/testIn.txt/tmp.log", true);
			XCTAssertTrue("Should not be able to create file" == 0);
		}
		catch(const std::exception& ex)
		{
			printf("got expected exception: %s\n", ex.what());
		}
		
		DlFileSpec::PathDelete(folder.UTF8Path(), true);
		
		return;
		
	} catch(const exception& ex) {
		printf("buffered: %s\n", ex.what());
	} catch(...) {
		printf("buffered: Unknown exception\n");
	}
	
	XCTAssertFalse("Exception occurred");
}

@end
