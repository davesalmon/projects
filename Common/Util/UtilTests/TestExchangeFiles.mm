//
//  TestExchangeFiles.m
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

@interface TestExchangeFiles : XCTestCase

@end

const char * const kTestFolder = "/tmp/Util-tests/filespec";

@implementation TestExchangeFiles

- (void)setUp {
    [super setUp];
	DlFileSpec::CreateDirectoryPath(kTestFolder);
}

- (void)tearDown {
	DlFileSpec::PathDelete(kTestFolder, true);
	
    [super tearDown];
}

- (void)testExchange {
	try {
		// create the base file
		DlFileSpec theFile("/tmp/Util-tests/filespec/writeme.txt", true);
		
		// open a temporary file in the same directory.
		DlDataFile fs(theFile.OpenTemporary());
		
		// check directory.
		XCTAssertTrue(fs.GetSpec().GetParent() == theFile.GetParent());
		
		// reopen it?
		fs.OpenFile(DlOpenMode::DlAppend);
		
		for (int i = 0; i < 10; i++) {
			fs.Write(&i, sizeof(i));
		}
		
		fs.CloseFile();
		
		// check the file. There should be 10 ints in it.
		XCTAssertEqual(fs.GetSpec().GetFileSize(), 10*sizeof(int));
		
		// exchange with original with backup
		theFile.Exchange(fs.GetSpec(), '~');
		
		DlFileSpec backupFile(theFile.GetParent(),
							  theFile.GetBackupName('~').c_str(), false);
		
		// we expect the backup to exist.
		XCTAssertTrue(backupFile.Exists());
		
		// and delete it.
		if (backupFile.Exists())
			backupFile.Delete();
		
		// specify another file.
		
		DlFileSpec otherFile(DlFileSpec::CreateDirectoryPath("/tmp/filespec"), "writeagain.txt", true);
		
		// exchange the original with that one.
		// This calls DlExchangeFiles
		otherFile.Exchange(theFile, 0);
		
		XCTAssertTrue(otherFile.Exists());
		otherFile.Delete();
		
		if (otherFile.GetParent().Exists(otherFile.GetBackupName('~').c_str()))
		{
			otherFile.GetParent().Delete(otherFile.GetBackupName('~').c_str());
			XCTAssertFalse("Other file back up exists!!");
		}
		
	} catch (DlException& ex) {
		printf("dl - error: %s\n", ex.what());
		XCTAssertFalse("failed to exchange");
	} catch (std::exception& ex) {
		printf("error: %s\n", ex.what());
		XCTAssertFalse("stdexcpt in exchange");
	}
}


@end
