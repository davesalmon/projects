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
#import "PreferencesPanelController.h"

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
	if ([PreferencesPanelController runDialog])
		;
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
NSString* kDefaultGridVisible = @"GridVisible";
NSString* kDefaultGridSpacing = @"GridSpacing";

NSString* kDefaultScale = @"Scale";

NSString* kDefaultUnits = @"Units";

//----------------------------------------------------------------------------------------
//  ensureUserDefaults
//
//      make sure the defaults have been set.
//
//  returns nothing
//----------------------------------------------------------------------------------------
+ (void) ensureUserDefaults
{
	// if there are no keys in the app, then add them
	if ([[NSUserDefaults standardUserDefaults] objectForKey: kDefaultWorldLeft] == nil) {
	
		
		
		NSString *setupPath = [[NSBundle mainBundle] pathForResource:@"setup" ofType:@"plist"];
		NSDictionary *dict = [[NSDictionary alloc] initWithContentsOfFile: setupPath];
		
		// register the setup values as defaults.
		[[NSUserDefaults standardUserDefaults] registerDefaults: dict];
#if 0
		// just copy in all the keys in setup
		for (id key in dict) {
			[[NSUserDefaults standardUserDefaults] setObject: [dict objectForKey: key]
													  forKey: key];
		}
#endif
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
//	[FrameColor update];
	
	for (FrameDocument* doc in [[NSDocumentController sharedDocumentController] documents]) {
		[doc refresh];
	}
	
#if _DEBUG
//	printf("forget leaks\n");
	DebugNewForgetLeaks();
#endif
}

//----------------------------------------------------------------------------------------
//  getDefaultWorld
//
//      return the default world rectangle.
//
//  returns WorldRect  <- 
//----------------------------------------------------------------------------------------
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

//----------------------------------------------------------------------------------------
//  getDefaultScale
//
//      return the default world scale.
//
//  returns double <- 
//----------------------------------------------------------------------------------------
- (double) getDefaultScale
{
	double s = [[NSUserDefaults standardUserDefaults] doubleForKey: kDefaultScale];
	return s > 0.0 ? s : 40.0;
}

//----------------------------------------------------------------------------------------
//  getDefaultGrid
//
//      return the default grid settings.
//
//  returns FrameGrid* <- 
//----------------------------------------------------------------------------------------
- (FrameGrid*) getDefaultGrid
{
	NSUserDefaults* defs = [NSUserDefaults standardUserDefaults];
	return [FrameGrid createWithSpacing: [defs doubleForKey: kDefaultGridSpacing]
										 gridOn: [defs boolForKey: kDefaultGridSnap]
									 andVisible: [defs boolForKey: kDefaultGridVisible]];
}

//----------------------------------------------------------------------------------------
//  getDefaultUnits
//
//      return the default units.
//
//  returns int    <- 
//----------------------------------------------------------------------------------------
- (int) getDefaultUnits
{
	return [[NSUserDefaults standardUserDefaults] integerForKey: kDefaultUnits];
}

//----------------------------------------------------------------------------------------
//  setDefaultWorld:
//
//      set the default world rectangle.
//
//  setDefaultWorld: const WorldRect& wr   -> 
//
//  returns nothing
//----------------------------------------------------------------------------------------
- (void) setDefaultWorld : (const WorldRect&) wr
{
	WorldRect oldWr = [self getDefaultWorld];
	if (!(oldWr == wr)) {
		NSUserDefaults* defs = [NSUserDefaults standardUserDefaults];
		
		[defs setDouble: wr.left() forKey: kDefaultWorldLeft];
		[defs setDouble: wr.bottom() forKey: kDefaultWorldBottom];
		[defs setDouble: wr.right() forKey: kDefaultWorldRight];
		[defs setDouble: wr.top() forKey: kDefaultWorldTop];
	}
}

//----------------------------------------------------------------------------------------
//  setDefaultScale:
//
//      set the default world scale.
//
//  setDefaultScale: double scale  -> 
//
//  returns nothing
//----------------------------------------------------------------------------------------
- (void) setDefaultScale : (double) scale
{
	NSUserDefaults* defs = [NSUserDefaults standardUserDefaults];
	[defs setDouble: scale forKey: kDefaultScale];
}

//----------------------------------------------------------------------------------------
//  setDefaultUnits:
//
//      set the defalut units.
//
//  setDefaultUnits: int units -> 
//
//  returns nothing
//----------------------------------------------------------------------------------------
- (void) setDefaultUnits : (int) units
{
	[[NSUserDefaults standardUserDefaults] setInteger: units forKey: kDefaultUnits];
}

//----------------------------------------------------------------------------------------
//  setDefaultGrid:
//
//      set the default grid setting.
//
//  setDefaultGrid: FrameGrid* grid    -> 
//
//  returns nothing
//----------------------------------------------------------------------------------------
- (void) setDefaultGrid : (FrameGrid*) grid
{
	[grid isEqual: [self getDefaultGrid]];
	NSUserDefaults* defs = [NSUserDefaults standardUserDefaults];
	[defs setDouble: [grid spacing].x() forKey: kDefaultGridSpacing];
	[defs setBool: [grid snapOn] forKey: kDefaultGridSnap];
	[defs setBool: [grid gridOn] forKey: kDefaultGridVisible];
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
