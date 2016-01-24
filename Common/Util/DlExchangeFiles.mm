//
//  DlExchangeFiles.mm
//
//  Created by David Salmon on 2/12/14.
//  Copyright © 2014-2016 David C. Salmon. All Rights Reserved.
//
//	Define file exchange method that safely exchanges the contents of
//	f1 with f2. If a backup char is specified, create backup.
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

#include "DlPlatform.h"
#include "DlExchangeFiles.h"
#include "DlFileSpec.h"
#include <string>

using namespace std;

void DlExchangeFiles(const DlFileSpec& f1, const DlFileSpec f2, char backupChar)
{
//	string path1 { f1.GetPath() };
//	string path2 { f2.GetPath() };
	
//	printf("path1=%s\npath2=%s\n\n", path1.c_str(), path2.c_str());
	
	@autoreleasepool {
		NSFileManager* mgr = [NSFileManager defaultManager];
		
		NSURL* u1 = [NSURL fileURLWithPath:[NSString stringWithUTF8String:f1.UTF8Path()]];
		NSURL* u2 = [NSURL fileURLWithPath:[NSString stringWithUTF8String:f2.UTF8Path()]];
		
		NSString* backup = nil;
		
		if (backupChar) {
			backup = [NSString stringWithFormat:@"%@",
					  [NSString stringWithUTF8String:f1.GetBackupName(backupChar).c_str()]];
		}
		
		NSURL* resultingURL = nil;
		NSError* resultError = nil;
		
		if (![mgr replaceItemAtURL: u1
					 withItemAtURL: u2
					backupItemName: backup
						   options: NSFileManagerItemReplacementWithoutDeletingBackupItem
				  resultingItemURL: &resultingURL error:&resultError]
			) {
			
			NSString* desc = [resultError localizedFailureReason];
			if (!desc)
				desc = [[[resultError userInfo] objectForKey:NSUnderlyingErrorKey] localizedFailureReason];
			const char * msg = desc ? [desc UTF8String] : "operation failed";
			throw DlException("Failed to write %s because %s", f1.UTF8Path(), msg);
		}
	}
}
