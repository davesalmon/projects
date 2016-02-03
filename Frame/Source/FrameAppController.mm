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
#import "WorkspaceData.h"
#import "FrameGrid.h"
#import "FrameColor.h"
#import "FrameDocument.h"

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

NSString* kDefaultWorldLeft = @"WorldLeft";
NSString* kDefaultWorldBottom = @"WorldBottom";
NSString* kDefaultWorldRight = @"WorldRight";
NSString* kDefaultWorldTop = @"WorldTop";

NSString* kDefaultGridSnap = @"GridSnap";
NSString* kDefaultGridSpacing = @"GridSpacing";

NSString* kDefaultScale = @"Scale";

NSString* kDefaultUnits = @"Units";

+ (void) ensureUserDefaults
{
	// if there are no keys in the app, then add them
	if ([[NSUserDefaults standardUserDefaults] objectForKey: kDefaultWorldLeft] == nil) {
	
		NSString *setupPath = [[NSBundle mainBundle] pathForResource:@"setup" ofType:@"plist"];
		NSDictionary *dict = [[NSDictionary alloc] initWithContentsOfFile: setupPath];
		
		// just copy in all the keys in setup
		for (id key in dict) {
			[[NSUserDefaults standardUserDefaults] setObject: [dict objectForKey: key]
													  forKey: key];
		}
	}
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
	[FrameColor update];
	
	for (FrameDocument* doc in [[NSDocumentController sharedDocumentController] documents]) {
		[doc refresh];
	}
	
#if _DEBUG
//	printf("forget leaks\n");
	DebugNewForgetLeaks();
#endif
}

- (WorldRect) getDefaultWorld
{
	NSUserDefaults* defs = [NSUserDefaults standardUserDefaults];
	WorldRect wr {
		[defs doubleForKey: kDefaultWorldLeft],
		[defs doubleForKey: kDefaultWorldBottom],
		[defs doubleForKey: kDefaultWorldRight],
		[defs doubleForKey: kDefaultWorldTop] };
	
	if (wr.left() == wr.right()) {
		wr.left() = -1;
		wr.right() = 39;
	} else if (wr.left() > wr.right()) {
		double t = wr.left();
		wr.left() = wr.right();
		wr.right() = t;
	}
	
	if (wr.bottom() == wr.top()) {
		wr.bottom() = -1;
		wr.top() = 39;
	} else if (wr.bottom() > wr.top()) {
		double t = wr.bottom();
		wr.bottom() = wr.top();
		wr.top() = t;
	}

	return wr;
}

- (double) getDefaultScale
{
	double s = [[NSUserDefaults standardUserDefaults] doubleForKey: kDefaultScale];
	return s > 0 ? s : 40.0;
}

- (FrameGrid*) getDefaultGrid
{
	NSUserDefaults* defs = [NSUserDefaults standardUserDefaults];
	return [FrameGrid createWithSpacing: [defs doubleForKey: kDefaultGridSpacing]
									 on: [defs boolForKey: kDefaultGridSnap]];
}

- (int) getDefaultUnits
{
	return [[NSUserDefaults standardUserDefaults] integerForKey: kDefaultUnits];
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
