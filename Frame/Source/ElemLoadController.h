/*+
 *
 *  ElemLoadController.h
 *
 *  Copyright © 2003 David C. Salmon. All Rights Reserved.
 *
 *  Handles window for editing and assignment of element loads.
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

#include "ElementLoadEnumerator.h"
#import "FramePanel.h"

@class FrameDocument;
class FrameStructure;
class ElementLoad;

@interface ElemLoadController : NSWindowController <FramePanel,
									NSTableViewDataSource, NSTableViewDelegate>
{
    IBOutlet NSButton *_attachButton;
    IBOutlet NSTableColumn *_axialColumn;
    IBOutlet NSButton *_hiliteButton;
    IBOutlet NSTableColumn *_lateralColumn;
    IBOutlet NSTableColumn *_preforceColumn;
    IBOutlet NSTableColumn *_premomentColumn;
    IBOutlet NSButton *_removeButton;
    IBOutlet NSTableView *_table;
    
	FrameDocument*		_frameDocument;
	FrameStructure*		_structure;
	ElementLoadEnumerator* _loads;
}

+ (ElemLoadController*)createElemLoadPanel: (FrameDocument*) doc;

- (id) initWithDocument: (FrameDocument*) doc;

- (void) listChanged;
- (bool) validSelection;

- (IBAction)attachClicked:(id)sender;
- (IBAction)hiliteClicked:(id)sender;
- (IBAction)removeClicked:(id)sender;

- (void) refresh;

// return the index of the load to hilite
- (ElementLoad)hiliteLoad;

@end
