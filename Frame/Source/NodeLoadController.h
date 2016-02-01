/*+
 *
 *  NodeLoadController.h
 *
 *  Copyright © 2003 David C. Salmon. All Rights Reserved.
 *
 *  Interface for editing and assignment of NodeLoads.
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
#include "NodeLoadEnumerator.h"
#import "FramePanel.h"

#include "StrDefines.h"

@class FrameDocument;
class FrameStructure;
class NodeLoad;

@interface NodeLoadController : NSWindowController <FramePanel,
									NSTableViewDataSource, NSTableViewDelegate>
{
    IBOutlet NSButton*		_attachButton;
    IBOutlet NSButton*		_hiliteButton;
    IBOutlet NSButton*		_removeButton;
	
    IBOutlet NSTableView*	_table;
	
    IBOutlet NSTableColumn* _xTypeColumn;
    IBOutlet NSTableColumn* _yTypeColumn;
    IBOutlet NSTableColumn* _tTypeColumn;
    IBOutlet NSTableColumn* _xColumn;
    IBOutlet NSTableColumn* _yColumn;
    IBOutlet NSTableColumn* _tColumn;

	FrameDocument*		_frameDocument;
	FrameStructure*		_structure;
	NodeLoadEnumerator* _loads;
}

+ (NodeLoadController*)createNodeLoadPanel: (FrameDocument*) doc;
- (id) initWithDocument: (FrameDocument*) doc;

- (void) listChanged;
- (bool) validSelection;

- (IBAction)attachClicked:(id)sender;
- (IBAction)hiliteClicked:(id)sender;
- (IBAction)removeClicked:(id)sender;

- (void) refresh;

// return the index of the load to hilite
- (NodeLoad)hiliteLoad;

@end
