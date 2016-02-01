/*+
 *
 *  main.m
 *
 *  Copyright © 2004 David C. Salmon. All Rights Reserved.
 *
 *  main entry point for Plane-Frame Analysis.
 *
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
-*/

#import <Cocoa/Cocoa.h>

#include "DlException.h"

static DlExceptionResult AlertDisplay(DlExceptionType theType, const char* okButtonText,
		const char* cancelButtonText, const char* leftButtonText,
		const char* errorText, const char* explanation);

//----------------------------------------------------------------------------------------
//  main
//
//      main entry point.
//
//  int argc           -> unused
//  const char *argv[] -> unused
//
//  returns int        <- application response.
//----------------------------------------------------------------------------------------
int
main(int argc, const char *argv[])
{
	SetExceptionDisplay(AlertDisplay);
	SetExceptionSourceType(DlExceptionPList);
	SetExceptionSourceName("Exceptions");

	return NSApplicationMain(argc, argv);
}

//----------------------------------------------------------------------------------------
//  AlertDisplay                                                                   static
//
//      Default DlException alert display method. When the Display method is called
//		from a DlException, this method displays the error.
//
//  DlExceptionType theType        -> the alert type.
//  const char* okButtonText       -> the text for the ok button (null for default)
//  const char* cancelButtonText   -> the text for the cancel button (null for default)
//  const char* leftButtonText     -> the text for the left button (null for default)
//  const char* errorText          -> the text for the title (null for default)
//  const char* explanation        -> the text for the body.
//
//  returns DlExceptionResult      <- the button pressed by the user
//----------------------------------------------------------------------------------------
static DlExceptionResult
AlertDisplay(DlExceptionType theType, const char* okButtonText,
		const char* cancelButtonText, const char* leftButtonText,
		const char* errorText, const char* explanation)
{
	NSString* okText = nil;
	NSString* cancelText = nil;
	NSString* leftText = nil;
	NSString* title = nil;
	NSString* explain = nil;
	NSWindow* theWindow = [NSApp mainWindow];
	
	if (okButtonText)
		okText = [NSString stringWithCString:okButtonText
								   encoding : NSUTF8StringEncoding];
	if (cancelButtonText)
		cancelText = [NSString stringWithCString:cancelButtonText
									   encoding : NSUTF8StringEncoding];
	if (leftButtonText)
		leftText = [NSString stringWithCString:leftButtonText
									 encoding : NSUTF8StringEncoding];
	if (errorText)
		title = [NSString stringWithCString:errorText
								  encoding : NSUTF8StringEncoding];
	if (explanation)
		explain = [NSString stringWithCString:explanation
									encoding : NSUTF8StringEncoding];

	if (theWindow == nil) {
		int resp;
		if (theType == DlExceptionError) {
			resp = NSRunCriticalAlertPanel(title, explain, okText, cancelText, leftText, theWindow);
		} else if (theType == DlExceptionWarning) {
			resp = NSRunAlertPanel(title, explain, okText, cancelText, leftText, theWindow);
		} else if (theType == DlExceptionInformation) {
			resp = NSRunInformationalAlertPanel(title, explain, okText, cancelText, leftText, theWindow);
		}
		
		switch(resp) {
		case NSAlertDefaultReturn: return DlExceptionResultYes;
		case NSAlertAlternateReturn: return DlExceptionResultNo;
		case NSAlertOtherReturn: return DlExceptionResultCancel;
		}
		
	} else {
		if (theType == DlExceptionError) {
			NSBeginCriticalAlertSheet(
				title, okText, cancelText, leftText, theWindow, nil, nil, nil, nil, @"%@", explain);
		} else if (theType == DlExceptionWarning) {
			NSBeginAlertSheet(
				title, okText, cancelText, leftText, theWindow, nil, nil, nil, nil, @"%@", explain);
		} else if (theType == DlExceptionInformation) {
			NSBeginInformationalAlertSheet(
				title, okText, cancelText, leftText, theWindow, nil, nil, nil, nil, @"%@", explain);
		}
	}
		
	return DlExceptionResultNo;
}
