/*+
 *
 *  DlAppSupportFiles.mm
 *
 *  Copyright © 2014-2016 David C. Salmon. All Rights Reserved.
 *
 *  utilities to create and remove folders and files for general application use.
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

#include "DlPlatform.h"
#include "DlAppSupportFiles.h"
#include "DlStringUtil.h"
#include "DlException.h"

//----------------------------------------------------------------------------------------
//  throwNSError
//
//      utility method to package an NSError as a DlException.
//
//  NSURL* url                         -> the url
//  NSError* err                       -> the error
//  const char* def                    -> the default message if one is not found in err
//
//  returns never
//----------------------------------------------------------------------------------------
[[noreturn]] static void
throwNSError(NSURL* url, NSError* err, const char* def)
{
	NSString* desc = [err localizedFailureReason];
	if (!desc)
		desc = [[[err userInfo] objectForKey:NSUnderlyingErrorKey] localizedFailureReason];
	const char * msg = desc ? [desc UTF8String] : def;
	throw DlException("Failed to create %s because %s",
					  [url fileSystemRepresentation], msg);
}

@interface AppSupportFiles : NSObject

@end

@implementation AppSupportFiles

+ (NSString*) getMyBundleId
{
	return [[NSBundle bundleForClass: [self class]] bundleIdentifier];
}

@end

//----------------------------------------------------------------------------------------
//  DlGetApplicationSupportFolder
//
//      return the application support subfolder specified.
//
//  const char* subFolder  -> 
//  bool create) throw(    -> 
//
//  returns std::string    <- 
//----------------------------------------------------------------------------------------
std::string
DlGetApplicationSupportFolder(const char* subFolder, bool create)
{
	@autoreleasepool {
		NSFileManager *fileManager = [NSFileManager defaultManager];
		NSURL *theURL = [[fileManager URLsForDirectory:NSApplicationSupportDirectory
													inDomains:NSUserDomainMask] lastObject];
		
		if (DlStrCmp(subFolder, kBundleIdFolder) == 0) {
//			NSRunningApplication* app = [NSRunningApplication currentApplication];
//			theURL = [theURL URLByAppendingPathComponent:[app bundleIdentifier]];
			theURL = [theURL URLByAppendingPathComponent:[AppSupportFiles getMyBundleId]];
		} else if (subFolder) {
			theURL = [theURL URLByAppendingPathComponent:[NSString stringWithUTF8String: subFolder]];
		}
		
		if (create && ![fileManager fileExistsAtPath: [theURL path]]) {
			
			NSError* error = nil;
			
			if (![fileManager createDirectoryAtPath:[theURL path]
						withIntermediateDirectories:YES
										 attributes:nil
											  error:&error]) {
				throwNSError(theURL, error, "unknown failure");
			}
        }
		
		return std::string([theURL fileSystemRepresentation]);
	}
}

//----------------------------------------------------------------------------------------
//  DlRemoveFolderFromApplicationSupport
//
//      remove the specified subfolder from the application support directory.
//
//  const char* subFolder) throw(  -> 
//
//  returns nothing
//----------------------------------------------------------------------------------------
void
DlRemoveFolderFromApplicationSupport(const char* subFolder)
{
	if (subFolder && subFolder[0]) {
		@autoreleasepool {
			NSFileManager *fileManager = [NSFileManager defaultManager];
			NSURL *theURL = [[fileManager URLsForDirectory:NSApplicationSupportDirectory
												 inDomains:NSUserDomainMask] lastObject];
			
			if (DlStrCmp(subFolder, kBundleIdFolder) == 0) {
//				NSRunningApplication* app = [NSRunningApplication currentApplication];
//				theURL = [theURL URLByAppendingPathComponent:[app bundleIdentifier]];
				theURL = [theURL URLByAppendingPathComponent:[AppSupportFiles getMyBundleId]];
			} else if (subFolder) {
				theURL = [theURL URLByAppendingPathComponent:[NSString stringWithUTF8String: subFolder]];
			}
			
			if ([fileManager fileExistsAtPath: [theURL path]]) {
				NSError* error = nil;
				if (![fileManager removeItemAtURL:theURL error:&error]) {
					throwNSError(theURL, error, "unknown failure");
				}
			}
		}
	}
}

//----------------------------------------------------------------------------------------
//  DlGetApplicationLogPath
//
//      return a base log path for the application. If appendingDate is true, the
//		date is appended to the base name.
//
//		Example: for an application with the id dcs.App, the log file is located at
//		~/Library/Logs/dcs.App/App.log
//
//		if the date is appended you get
//		~/Library/Logs/dcs.App/App-2014-03-20.log
//
//  const char* defaultName  -> the default name if one cannot be constructed.
//  bool appendingDate       -> true if the date should be appended.
//
//  returns std::string      <- the path.
//----------------------------------------------------------------------------------------
std::string
DlGetApplicationLogPath(const char* defaultName, bool appendingDate)
{
	@autoreleasepool {
		NSFileManager *fileManager = [NSFileManager defaultManager];
		NSURL *theURL = [[fileManager URLsForDirectory:NSLibraryDirectory
											 inDomains:NSUserDomainMask] lastObject];
		
		theURL = [theURL URLByAppendingPathComponent:@"Logs"];
		
		
//		NSRunningApplication* app = [NSRunningApplication currentApplication];
		NSString* bundleId = [AppSupportFiles getMyBundleId];
		
//		if (!bundleId)
//			bundleId = [app localizedName];

		if (!bundleId && defaultName)
			bundleId = [NSString stringWithUTF8String:defaultName];
		
		if (!bundleId)
			throw DlException("failed to get identifier for this application");
		
		// this is the directory
		theURL = [theURL URLByAppendingPathComponent:bundleId];
		
		if (![fileManager fileExistsAtPath: [theURL path]]) {
			
			NSError* error = nil;
			
			if (![fileManager createDirectoryAtPath:[theURL path]
						withIntermediateDirectories:YES
										 attributes:nil
											  error:&error]) {
				throwNSError(theURL, error, "unknown failure");
			}
        }
		
		// now create the log file.
		NSString* baseName = [[bundleId componentsSeparatedByString: @"."] lastObject];
		
		if (appendingDate)
		{
			NSDateFormatter *dateFormatter = [[[NSDateFormatter alloc] init] autorelease];
			[dateFormatter setDateFormat:@"-yyyy-MM-dd"];

			NSString* dateString = [dateFormatter stringFromDate:[NSDate date]];
			baseName = [baseName stringByAppendingString:dateString];
		}
		
		baseName = [baseName stringByAppendingPathExtension: @"log"];
		
		theURL = [theURL URLByAppendingPathComponent: baseName];
		
		return std::string([theURL fileSystemRepresentation]);
	}
}

// return the bundle id for the module containing this code.
std::string
DlGetBundleId()
{
	return std::string([[AppSupportFiles getMyBundleId] UTF8String]);
}

