/*+
 *
 *  NodeInspectorController.mm
 *
 *  Copyright © 2014 David C. Salmon. All Rights Reserved.
 *
 *  handle the node inspector window.
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

#import "NodeInspectorController.h"
#import "NodeInspectorTree.h"
#import "FrameDocument.h"

@interface NodeInspectorController ()
{
	IBOutlet NSOutlineView* _outlineView;

	IBOutlet NSTableColumn* _labelColumn;
	IBOutlet NSTableColumn* _valueColumn;
	
	FrameDocument* _frameDocument;
	NodeInspectorTree* _items;
	bool _updatingSelection;
}

@end

@implementation NodeInspectorController

+ (NodeInspectorController*)createNodePanel: (FrameDocument*) doc
{
	NodeInspectorController* theController = nil;
	
	theController = [[NodeInspectorController alloc] initWithDocument: doc];
	//    [theController showWindow:theController];
	[theController setShouldCloseDocument:NO];
	[doc addWindowController:theController];
	[theController release];
	return theController;
}

//----------------------------------------------------------------------------------------
//  initWithDocument:
//
//      Initialize the panel controller.
//
//  initWithDocument: FrameDocument* doc   -> document
//
//  returns instancetype                   <- self
//----------------------------------------------------------------------------------------
- (instancetype) initWithDocument: (FrameDocument*) doc
{
	self = [super initWithWindowNibName: @"NodeInspector"];
	if (self) {
		_frameDocument = doc;
		_items = 0;
	}
	
	return self;
}

//----------------------------------------------------------------------------------------
//  dealloc
//
//      Free the panel controller.
//
//  returns nothing
//----------------------------------------------------------------------------------------
- (void) dealloc
{
	[_items release];
	[super dealloc];
}

//----------------------------------------------------------------------------------------
//  buildList
//
//      build or rebuild the element list.
//
//  returns nothing
//----------------------------------------------------------------------------------------
- (void) buildList
{
	[_items updateTree: _outlineView];
	
#if DlDebugging
	NSLog(@"Setting node items to super with count %ld", [_items count]);
#endif
	[super setItems: _items];
}

//----------------------------------------------------------------------------------------
//  updateSelection
//
//      Update the selection shown in the inspector to correspond to the structure view.
//
// updateSelection: bool fromFrameDocument    -> true if the update comes from the doc.
//
//  returns nothing
//----------------------------------------------------------------------------------------
- (void) updateSelection: (bool) fromFrameDocument
{
	if (_items != nil)
	{
		[self setUpdatingSelection: fromFrameDocument];
		NSMutableIndexSet* selected = [NSMutableIndexSet indexSet];
		for (DlUInt32 i = 0; i < [_items count]; i++)
		{
			id<InspectorOutlineItem> item = [_items objectAtIndex: i];
			if ([_frameDocument nodeSelected: [item index]])
			{
				[selected addIndex: [_outlineView rowForItem: item]];
			}
		}
		[_outlineView selectRowIndexes:selected byExtendingSelection:FALSE];
	}
}

//----------------------------------------------------------------------------------------
//  refresh
//
//      refresh the display.
//
//  returns nothing
//----------------------------------------------------------------------------------------
- (void) refresh
{
//	NSLog(@"Got node list refresh");
	[_outlineView setNeedsDisplay: YES];
}

#pragma mark -
#pragma mark =============== Notifications ===============
#pragma mark -

//----------------------------------------------------------------------------------------
//  windowWillClose:
//
//      handle window closing.
//
//  windowWillClose: NSNotification * aNotification    ->
//
//  returns nothing
//----------------------------------------------------------------------------------------
- (void) windowWillClose:(NSNotification*) aNotification
{
	[_frameDocument nodeInspectorIsClosing];
}

//----------------------------------------------------------------------------------------
//  windowFrameAutosaveName
//
//      return the autosave name for this window.
//
//  returns NSString*  <- the autosave name
//----------------------------------------------------------------------------------------
- (NSString*) windowFrameAutosaveName
{
	return @"NodeInspector";
}

//----------------------------------------------------------------------------------------
//  windowDidLoad
//
//      handle the window controller initialization.
//
//  returns nothing
//----------------------------------------------------------------------------------------
- (void)windowDidLoad {
    [super windowDidLoad];
    
    // Implement this method to handle any initialization after your window controller's window has
	// been loaded from its nib file.
	_items = [[NodeInspectorTree createWithDocument: _frameDocument] retain];
	
	[super setOutlineView: _outlineView];
	[super setLabelColumn: _labelColumn];
	[self buildList];
}

@end
