/*+
 *
 *  FrameAppController.mm
 *
 *  Copyright © 2004 David C. Salmon. All Rights Reserved.
 *
 *  Handle application level events and shutdown.
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

#import "FrameAppController.h"
#import "FrameClipboard.h"

@implementation FrameAppController

//----------------------------------------------------------------------------------------
//  instance
//
//      return an instance of this object.
//
//  returns FrameAppController*    <- this object.
//----------------------------------------------------------------------------------------
+ (FrameAppController*) instance {
	return (FrameAppController*)[NSApp delegate];
}


//----------------------------------------------------------------------------------------
//  handlePreferences:
//
//      handle the preferences panel (unused)
//
//  handlePreferences: id sender   -> 
//
//  returns nothing
//----------------------------------------------------------------------------------------
- (IBAction)handlePreferences:(id)sender
{
	printf("preferences\n");
	// no preferences at this time.
}

//----------------------------------------------------------------------------------------
//  showAboutBox:
//
//      show the about box (unused).
//
//  showAboutBox: id sender    -> 
//
//  returns nothing
//----------------------------------------------------------------------------------------
- (IBAction)showAboutBox:(id)sender
{
	// this is never called -- using standard about box method.
	[NSApp orderFrontStandardAboutPanel:sender];
}

//----------------------------------------------------------------------------------------
//  applicationDidFinishLaunching:
//
//      called when the application is done launching.
//
//  applicationDidFinishLaunching: NSNotification * aNotification  -> 
//
//  returns nothing
//----------------------------------------------------------------------------------------
- (void)applicationDidFinishLaunching:(NSNotification *)aNotification
{
#if _DEBUG
//	printf("forget leaks\n");
	DebugNewForgetLeaks();
#endif
}

//----------------------------------------------------------------------------------------
//  applicationWillTerminate:
//
//      called when the application is terminating.
//
//  applicationWillTerminate: NSNotification * aNotification   -> 
//
//  returns nothing
//----------------------------------------------------------------------------------------
- (void) applicationWillTerminate: (NSNotification *) aNotification 
{
	[FrameClipboard deleteClipboard];
#if _DEBUG
	DebugNewValidateAllBlocks();
	DebugNewReportLeaks("/tmp/leaks-Plane-Frame.log");
#endif
}

@end
