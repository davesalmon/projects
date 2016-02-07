/*+
 *
 *  WorkspaceController.h
 *
 *  Copyright © 2005 David C. Salmon. All Rights Reserved.
 *
 *  Functions for the workspace editor.
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
#import "FramePanel.h"

@class FrameDocument;

@interface WorkspaceController : NSWindowController <FramePanel>
{
    IBOutlet NSMatrix *_gridForm;
    IBOutlet NSButton *_gridSnap;
    IBOutlet NSButton *_gridVisible;
    IBOutlet NSMatrix *_worldForm;
    IBOutlet NSTextField*_errorText;
    IBOutlet NSTextField*_scale;
    
    FrameDocument* _frameDocument;
}

+ (WorkspaceController*) createWorkspacePanel: (FrameDocument*) doc;

- (id) initWithDocument: (FrameDocument*) doc;

- (void) updateData;

- (void) refresh;

- (IBAction)doUpdate:(id)sender;

- (void) windowDidBecomeKey: (NSNotification *) aNotification;

@end
