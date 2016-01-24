//
//  TestFileSpec.m
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
#include "DlFileSpec.h"
#include "DlDataFile.h"
#include "DlAppSupportFiles.h"

const char * const kTestFolder = "/tmp/Util-tests/filespec";

@interface TestFileSpec : XCTestCase

@end

@implementation TestFileSpec

- (void)setUp {
    [super setUp];
	DlFileSpec::CreateDirectoryPath(kTestFolder);
}

- (void)tearDown {
	DlFileSpec::PathDelete(kTestFolder, true);
    [super tearDown];
}

- (void)testConstructors {
	DlFileSpec theSpec(kTestFolder);
	XCTAssertTrue(theSpec.Exists());
	XCTAssertTrue(theSpec.IsDirectory());
	
	
	if (!theSpec.Exists())
	{
		exit(1);
	}
	
	// no create on the folder. This should throw.
	try {
		DlFileSpec(theSpec, "SubFolder", false, true);
		XCTFail("Failed to throw required exception");
	} catch(DlException & expected) {
		// nothing
	} catch(...) {
		XCTFail("threw incorrect exception type.");
	}
	
	//	Create subfolder
	DlFileSpec createSubFolder(theSpec, "SubFolder", true, true);
	
	XCTAssertTrue(createSubFolder.Exists());
	XCTAssertTrue(createSubFolder.IsDirectory());
	
	DlFileSpec createSubFile(createSubFolder, "MyFile", true, false);
	
	XCTAssertTrue(createSubFile.Exists());
	XCTAssertTrue(!createSubFile.IsDirectory());
	XCTAssertTrue(createSubFile.IsFile());
	
	createSubFile.Delete();
	XCTAssertFalse(createSubFile.Exists());
	
	createSubFolder.Delete();
	XCTAssertFalse(createSubFolder.Exists());
	
	DlFileSpec test(createSubFolder);
	XCTAssertTrue(test == createSubFolder);
	
	DlFileSpec test2 { createSubFolder };
	XCTAssertTrue(test2 == createSubFolder);
	
	DlFileSpec par(createSubFolder.GetParent());
	XCTAssertTrue(DlFileSpec(par, createSubFolder.GetName().c_str()) == createSubFolder);
	
}

- (void)testOpenFile {
	try
	{
		DlFileSpec theSpec(kTestFolder);

		DlFileSpec	aFile(theSpec, "testFile", true);
		DlFileRef ref = aFile.Open(DlOpenMode::DlWriteOnly);
		DlFileSpec openSpec(ref);
		XCTAssertTrue(openSpec == aFile);
	}
	catch(std::exception& ex)
	{
		printf("testOpen exception %s", ex.what());
		XCTFail("testOpen failed");
	}

}

- (void)testPaths {
	XCTAssertTrue(DlFileSpec("./tests/filespec/TestFile/what") == DlFileSpec("tests/filespec/TestFile/what"));
	XCTAssertTrue(DlFileSpec("/tests/./filespec/../TestFile/what") == DlFileSpec("/tests/TestFile/what/."));
}

- (void)testAttributes {
	DlFileSpec baseDir(kTestFolder);
	
	DlFileSpec theFile(baseDir, "writeme.txt");
	
	//	printf("file is %s\n with temp %s\n\n", theFile.UTF8Path(), theTemporary.UTF8Path());
	
	// write some data into the file
	DlDataFile fs(theFile);
	
	fs.OpenFile(DlOpenMode::DlWriteOnly);
	
	int totalSize = 0;
	for (int i = 0; i < 10; i++) {
		fs.Write(&i, sizeof(i));
		totalSize += sizeof(i);
	}
	
	fs.CloseFile();
	
	XCTAssertEqual(theFile.GetFileSize(), totalSize);
	
	theFile.Delete();
}

- (void)testMoveRename {
	
	DlFileSpec baseDir(kTestFolder);
	
	DlFileSpec target(baseDir, "testme.txt");
	if (target.Exists())
		target.Delete();
	
	DlFileSpec theFile(baseDir, "writeme.txt", true);
	theFile.Rename(target.GetName().c_str());
	XCTAssertTrue(target.Exists());
	XCTAssertTrue(theFile == target);
	
	target = DlFileSpec(baseDir, "SubDir/testme.txt");
	if (target.Exists())
		target.Delete();
	
	if (target.GetParent().Exists())
		target.GetParent().Delete();
	
	DlFileSpec theDir(baseDir, "SubDir", true, true);
	theFile.Move(theDir);
	
	XCTAssertTrue(target.Exists());
	XCTAssertTrue(theFile == target);
	
	theFile.Delete();
}

@end
