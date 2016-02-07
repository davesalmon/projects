/*+
 *
 *  WorkspaceController.m
 *
 *  Copyright © 2005 David C. Salmon. All Rights Reserved.
 *
 *  Implement functions for the workspace editor.
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

#import "WorkspaceController.h"
#import "FrameDocument.h"
#import "WorkspaceData.h"
#import "UnitsFormatter.h"

//#include "UnitTable.h"
#include "WorldRect.h"
#include "graphics.h"

#define MIN_SCREEN_SIZE	200

@implementation WorkspaceController

+ (WorkspaceController*) createWorkspacePanel: (FrameDocument*) doc
{
	WorkspaceController* theController = [[WorkspaceController alloc] initWithDocument:doc];
    
    [theController setShouldCloseDocument:NO];
    [doc addWindowController:theController];
    [theController release];

    return theController;
}

- (id) initWithDocument: (FrameDocument*) doc
{
	self = [super initWithWindowNibName:@"Workspace"];
	_frameDocument = doc;
    return self;
}

- (IBAction)doUpdate:(id)sender
{
	[[self window] makeFirstResponder: [self window]];

	WorkspaceData* data = [_frameDocument workspace];
    
    [_errorText setStringValue:@""];
    
    // get the limiting scale factors
    
    try
    {
        WorldRect wr(
            [[_worldForm cellWithTag: 0] doubleValue],
            [[_worldForm cellWithTag: 1] doubleValue],
            [[_worldForm cellWithTag: 2] doubleValue],
            [[_worldForm cellWithTag: 3] doubleValue]);

        WorldPoint limits(graphics::getScaleLimits(
        		DlScreenPoint2d(MIN_SCREEN_SIZE, MIN_SCREEN_SIZE), wr));
    
    	DlFloat64 scaleFactor = 100.0 / [_scale doubleValue];
        
        // check the scale
        if (scaleFactor < limits.x() || scaleFactor > limits.y())
        {
			[_scale setDoubleValue: 100.0 / [data scale]];

        	double sfLow = 100.0 / limits.y();
            double sfHigh = 100.0 / limits.x();
        	[_errorText setStringValue:[NSString stringWithFormat:
						@"Scale must be between %#6.2g and %-#6.2g", sfLow, sfHigh]];
			return;
        }

		// get the current screen rect. and check if user attempts to make the workspace
		//	smaller than the visible part of the current view. In this case, set the bounds to
		//	be equal to the window size at the new scale.
		auto screenSize = [_frameDocument visibleSize];

		WorldPoint newSize { wr.width(), wr.height() };
		newSize.x() *= scaleFactor;
		newSize.y() *= scaleFactor;

		if (newSize.x() < screenSize.width || newSize.y() < screenSize.height) {

			screenSize.width /= scaleFactor;
			screenSize.height /= scaleFactor;

			wr = [data worldRect];

			wr.right() = wr.left() + screenSize.width;
			wr.top() = wr.bottom() + screenSize.height;

			[[_worldForm cellWithTag: 0] setDoubleValue: wr.left()];
			[[_worldForm cellWithTag: 1] setDoubleValue: wr.bottom()];
			[[_worldForm cellWithTag: 2] setDoubleValue: wr.right()];
			[[_worldForm cellWithTag: 3] setDoubleValue: wr.top()];

        	[_errorText setStringValue: @"world may not be smaller than window."];

			return;
		}

        {
            WorldPoint spacing(
                [[_gridForm cellWithTag: 0] doubleValue],
                [[_gridForm cellWithTag: 1] doubleValue]);
            bool snapOn = [_gridSnap state] == NSOnState;
            bool visible = [_gridVisible state] == NSOnState;
            
            [data setWorldRect:wr];
            [data setGridSpacing:spacing];
            [data setGridSnapOn:snapOn];
            [data setGridVisible:visible];
            [data setScale: scaleFactor];
            
            [_frameDocument updateWorkspace: data];
        }
    }
    catch(...)
    {
    	// what here?
        [_errorText setStringValue:@"Update failed -- Invalid units!"];
    }
}

/** private and utility methods **/

- (void)windowDidLoad {
	
	[super windowDidLoad];

	[self updateData];
	[[self window] makeFirstResponder: _scale];
//	[[self window] makeFirstResponder: nil];
//	[self refresh];
}

//----------------------------------------------------------------------------------------
//  updateData
//
//      update the date from the document.
//
//  returns nothing
//----------------------------------------------------------------------------------------
- (void) updateData
{
	// fill in the dimensions form.
    WorkspaceData* data = [_frameDocument workspace];
    const WorldRect& wr = [data worldRect];
    UnitsFormatter* fmt = [UnitsFormatter 
    	formatterWithDocument: _frameDocument
								 units: UnitsLength
                             precision: 2
                            showingTag: true];
    
	[_scale setFormatter: fmt];
    [_scale setDoubleValue: 100.0 / [data scale]];
	
	[[_worldForm cellWithTag: 0] setFormatter: fmt];
    [[_worldForm cellWithTag: 0] setDoubleValue: wr.left()];

	[[_worldForm cellWithTag: 1] setFormatter: fmt];
    [[_worldForm cellWithTag: 1] setDoubleValue: wr.bottom()];

	[[_worldForm cellWithTag: 2] setFormatter: fmt];
    [[_worldForm cellWithTag: 2] setDoubleValue: wr.right()];

	[[_worldForm cellWithTag: 3] setFormatter: fmt];
    [[_worldForm cellWithTag: 3] setDoubleValue: wr.top()];
	
    WorldPoint spacing = [data gridSpacing];
    
	[[_gridForm cellWithTag: 0] setFormatter: fmt];
    [[_gridForm cellWithTag: 0] setDoubleValue: spacing.x()];

	[[_gridForm cellWithTag: 1] setFormatter: fmt];
    [[_gridForm cellWithTag: 1] setDoubleValue: spacing.y()];
	
    // fill in the check boxes.
    [_gridSnap setState: [data gridSnapOn] ? NSOnState : NSOffState];
    [_gridVisible setState: [data gridVisible] ? NSOnState : NSOffState];
        
    [self refresh];
}

- (void) refresh
{
	[[[self window] contentView] setNeedsDisplay: YES];
}

//----------------------------------------------------------------------------------------
//  windowDidBecomeKey:
//
//      the window came to the front. enable controls.
//
//  windowDidBecomeKey: NSNotification * aNotification -> the  notification
//
//  returns nothing
//----------------------------------------------------------------------------------------
- (void) windowDidBecomeKey: (NSNotification *) aNotification 
{
	[self updateData];
    [_worldForm setEnabled: YES];
 	[_gridForm setEnabled: YES];
    [_gridSnap setEnabled: YES];
    [_gridVisible setEnabled: YES];
    [_scale setEnabled: YES];
}

//----------------------------------------------------------------------------------------
//  windowDidResignKey:
//
//      The window is no longer key. disable controls.
//
//  windowDidResignKey: NSNotification * aNotification -> 
//
//  returns nothing
//----------------------------------------------------------------------------------------
- (void)windowDidResignKey:(NSNotification *)aNotification
{
	// clear the selected item
 	[_worldForm setEnabled: NO];
 	[_gridForm setEnabled: NO];
    [_gridSnap setEnabled: NO];
    [_gridVisible setEnabled: NO];
    [_scale setEnabled: NO];
}

@end
