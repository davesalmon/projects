/*+
 *
 *  NodeLoadController.mm
 *
 *  Copyright © 2003 David C. Salmon. All Rights Reserved.
 *
 *  Handles window controls for editing and assignment of NodeLoads.
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

#import "NodeLoadController.h"
#import "FrameDocument.h"
#import "FrameAction.h"
#import "FrameIO.h"

#include "FrameStructure.h"
#include "NodeLoad.h"
#include "DlString.h"

static NSString* kPasteBoardType = @"dcs.Plane-Frame-NLoads";

@implementation NodeLoadController

//----------------------------------------------------------------------------------------
//  createNodeLoadPanel:
//
//      Create the NodeLoad editor panel.
//
//  createNodeLoadPanel: FrameDocument* doc    -> the document
//
//  returns NodeLoadController*                <- the window controller
//----------------------------------------------------------------------------------------
+ (NodeLoadController*)createNodeLoadPanel: (FrameDocument*) doc
{
    NodeLoadController* theController = nil;

    theController = [[NodeLoadController alloc] initWithDocument: doc];
//    [theController showWindow:theController];
    [theController setShouldCloseDocument:NO];
    [doc addWindowController:theController];
    [theController release];
    return theController;
}

//----------------------------------------------------------------------------------------
//  initWithDocument:
//
//      Initialize with the current document.
//
//  initWithDocument: FrameDocument* doc   -> the document
//
//  returns id                             <- self
//----------------------------------------------------------------------------------------
- (id) initWithDocument: (FrameDocument*) doc
{
	self = [super initWithWindowNibName:@"NodeLoadPanel" owner:self];
	_structure = [doc structure];
	_frameDocument = doc;
	_loads = NEW NodeLoadEnumerator();
	*_loads = _structure->GetNodeLoads();
	return self;
}

//----------------------------------------------------------------------------------------
//  dealloc
//
//      release memory.
//
//  returns nothing
//----------------------------------------------------------------------------------------
- (void) dealloc
{
	delete _loads;
	[super dealloc];
}

//----------------------------------------------------------------------------------------
//  windowTitleForDocumentDisplayName:
//
//      return the title for the window.
//
//  windowTitleForDocumentDisplayName: NSString * displayName  -> 
//
//  returns NSString *                                         <- 
//----------------------------------------------------------------------------------------
- (NSString *)windowTitleForDocumentDisplayName:(NSString *)displayName 
{
	return [displayName stringByAppendingString : @" - Node Loads"];
}

//----------------------------------------------------------------------------------------
//  listChanged
//
//      mark the list as changed.
//
//  returns nothing
//----------------------------------------------------------------------------------------
- (void) listChanged
{
	[_table noteNumberOfRowsChanged];
}

//----------------------------------------------------------------------------------------
//  validSelection
//
//      return true if the current selection is not empty.
//
//  returns bool   <- 
//----------------------------------------------------------------------------------------
- (bool) validSelection {
	int sel = [_table selectedRow];
	return sel >= 0 && sel < _loads->Length();
}

//----------------------------------------------------------------------------------------
//  makeButton
//
//      construct the force/settle button for the panel.
//
//  returns NSButtonCell*  <- 
//----------------------------------------------------------------------------------------
- (NSButtonCell*) makeButton {
	NSButtonCell* button = [[[NSButtonCell alloc] init] autorelease];
	[button setTitle: @"Force"];
	[button setAlternateTitle: @"Settle"];
	[button setImagePosition: NSNoImage];
	[button setControlSize: NSSmallControlSize];
	[button setButtonType: NSToggleButton];
//	[button setBezelStyle: NSRegularSquareBezelStyle];
	[button setFont: [NSFont systemFontOfSize: 9]]; 
	[button setHighlightsBy: NSContentsCellMask];
	[button setTransparent: NO];
	[button setControlTint:NSClearControlTint];
//	[button setBezeled: YES];
	return button;
}

//----------------------------------------------------------------------------------------
//  awakeFromNib
//
//      Handle table initialization.
//
//  returns nothing
//----------------------------------------------------------------------------------------
- (void) awakeFromNib
{
	[_xTypeColumn setDataCell: [self makeButton]];
	[_yTypeColumn setDataCell: [self makeButton]];
	[_tTypeColumn setDataCell: [self makeButton]];
	
	[[_hiliteButton retain] removeFromSuperview];
	
	int diff = [_table rectOfRow:0].size.width - [[_table superview] frame].size.width;
	if (diff > 0) {
		NSRect newFrame = _table.window.frame;
		newFrame.size.width += diff;
		[[_table window] setFrame: newFrame display: YES];
	}

}

//----------------------------------------------------------------------------------------
//  attachClicked:
//
//      Handle click of the attach button.
//
//  attachClicked: id sender   -> the object.
//
//  returns nothing
//----------------------------------------------------------------------------------------
- (IBAction)attachClicked:(id)sender
{
	[[self window] makeFirstResponder: [self window]];

	int selectedRow = [_table selectedRow];
	NodeLoad load = _loads->At(selectedRow);

	// attach the load at the selcted row to all selected nodes.
	[_frameDocument performAction: [FrameAction create: 
		_structure->AssignNodeLoads(*[_frameDocument nodeSelection], load)
		withView: [_frameDocument frameView]]];
	
	[_frameDocument refresh];
}

//----------------------------------------------------------------------------------------
//  hiliteClicked:
//
//      Hanele click of the highlight button.
//
//  hiliteClicked: id sender   -> the object
//
//  returns nothing
//----------------------------------------------------------------------------------------
- (IBAction)hiliteClicked:(id)sender
{
	[_frameDocument refresh];
}

//----------------------------------------------------------------------------------------
//  hiliteLoad
//
//      return the NodeLoad object to hilite. An empty object is returned if there is no
//      hilite.
//
//  returns NodeLoad   <- 
//----------------------------------------------------------------------------------------
- (NodeLoad)hiliteLoad 
{
	if ([self validSelection]) {
		return _loads->At([_table selectedRow]);
	}

	return NodeLoad(0);
}

//----------------------------------------------------------------------------------------
//  removeClicked:
//
//      Handle click of the remve button.
//
//  removeClicked: id sender   -> the object
//
//  returns nothing
//----------------------------------------------------------------------------------------
- (IBAction)removeClicked:(id)sender
{
	NodeLoad empty(0);
	// attach the load at the selcted row to all selected nodes.
	[_frameDocument performAction: [FrameAction create: 
		_structure->AssignNodeLoads(*[_frameDocument nodeSelection], empty)
		withView: [_frameDocument frameView]]];
	[_frameDocument refresh];
}

//----------------------------------------------------------------------------------------
//  refresh
//
//      cause the table to redraw.
//
//  returns nothing
//----------------------------------------------------------------------------------------
- (void) refresh
{
	[_table setNeedsDisplay: YES];
}

//----------------------------------------------------------------------------------------
//  updateButtonState
//
//      update the button state from the selection.
//
//  returns nothing
//----------------------------------------------------------------------------------------
- (void) updateButtonState
{	
	bool hasSelection = [_frameDocument nodeSelection]->Length() != 0;
	bool validRow = [self validSelection];
	BOOL enableAttach = (hasSelection && validRow) ? YES : NO;
	BOOL enableRemove = hasSelection ? YES : NO;
	//	check the button state
	[_attachButton setEnabled: enableAttach];
	[_removeButton setEnabled: enableRemove];
}

//----------------------------------------------------------------------------------------
//  canDelete
//
//      return the selected load if it can be deleted.
//
//  returns NodeLoad
//----------------------------------------------------------------------------------------
- (NodeLoad) canDelete
{
	int sel = [_table selectedRow];
	if (sel >= 0 && sel < _loads->Length()) {
		
		FrameStructure* s = [_frameDocument structure];
		NodeLoad active = s->GetNodeLoads().At(sel);
		
		if (s->GetAssignedNodes(active).Length() == 0)
			return active;
	}
	
	return NodeLoad(0);
}

#pragma mark -
#pragma mark =============== Table Data source ===============
#pragma mark -

//----------------------------------------------------------------------------------------
//  numberOfRowsInTableView:
//
//      return the current number of rows.
//
//  numberOfRowsInTableView: NSTableView * tableView   -> the table
//
//  returns int                                        <- the number of rows
//----------------------------------------------------------------------------------------
- (NSInteger) numberOfRowsInTableView: (NSTableView *)tableView
{
	return _loads->Length() + 1;
}

//----------------------------------------------------------------------------------------
//  tableView:objectValueForTableColumn:row:
//
//      return the value for the given row and col.
//
//  tableView: NSTableView * tableView                     -> the table
//  objectValueForTableColumn: NSTableColumn * tableColumn -> the column
//  row: int row                                           -> the row
//
//  returns id                                             <- the value
//----------------------------------------------------------------------------------------
- (id)            tableView: (NSTableView *)tableView
  objectValueForTableColumn: (NSTableColumn *)tableColumn
						row: (NSInteger)row
{
	int colId = [[tableColumn identifier] intValue];
	if (row < _loads->Length())
	{
		NodeLoad l(_loads->At(row));
		switch(colId) {
		case 1:
		case 2:
		case 3:
				return [NSString stringWithUTF8String: l.GetValue(colId - 1, true).get()];
			break;
		case 4:
		case 5:
		case 6:
			return [NSNumber numberWithInt: l.GetType(colId - 4)];
		default:
			break;
		}
	}
	
	return colId == 0 ? 0 : @"";
}

//----------------------------------------------------------------------------------------
//  tableView:setObjectValue:forTableColumn:row:
//
//      set the value for the given row and col.
//
//  tableView: NSTableView * tableView             -> the table
//  setObjectValue: id object                      -> the new value
//  forTableColumn: NSTableColumn * tableColumn    -> the column
//  row: int row                                   -> the row
//
//  returns nothing
//----------------------------------------------------------------------------------------
- (void)    tableView: (NSTableView *)tableView
	   setObjectValue: (id)object
	   forTableColumn: (NSTableColumn *)tableColumn
				  row: (NSInteger)row
{
	NodeLoad l(0);
	if (row == _loads->Length())
	{
		FrameAction* action = [FrameAction create: _structure->CreateNodeLoad(l) withView: nil];
		[_frameDocument performAction: action];
		*_loads = _structure->GetNodeLoads();
		[_table noteNumberOfRowsChanged];
	}
	else
	{
		l = _loads->At(row);
	}
	
	int colId = [[tableColumn identifier] intValue];
	const char* theValue = 0;
	DlInt32 theDof = -1;
	NodeLoadType theType = NodeLoadUndefined;
	switch(colId) {
			// values X, Y, Theta
	case 1:
	case 2:
	case 3:
		theValue = [object UTF8String];
		theDof = colId - 1;
		// l.SetValue(colId - 1, [object cString]);
		break;
			// Types for X, Y, Theta
	case 4:
	case 5:
	case 6:
		theType = (NodeLoadType)[object intValue];
		theDof = colId - 4;
		//l.SetType(colId - 4, [object intValue]);
		[_table reloadData];
		break;
	}
	
	if (theDof != -1) {
		try {
			FrameAction* action = [FrameAction create: _structure->ChangeNodeLoad(l, theDof, theValue, theType)
								withView: [_frameDocument frameView]];
			[_frameDocument performAction: action];
			[self updateButtonState];
		} catch(DlException& ex) {
			ex.Display();
		}
	}
	
}

#pragma mark -
#pragma mark =============== Table delegate ===============
#pragma mark -

//----------------------------------------------------------------------------------------
//  tableViewSelectionDidChange:
//
//      the table selecton changed.
//
//  tableViewSelectionDidChange: NSNotification * notification -> the notification
//
//  returns nothing
//----------------------------------------------------------------------------------------
- (void)tableViewSelectionDidChange:(NSNotification *)notification
{
	[self updateButtonState];
	[_frameDocument refresh];
}

#pragma mark -
#pragma mark =============== Window delegate ===============
#pragma mark -

//----------------------------------------------------------------------------------------
//  windowDidBecomeKey:
//
//      the window came to the front.
//
//  windowDidBecomeKey: NSNotification * aNotification -> the  notification
//
//  returns nothing
//----------------------------------------------------------------------------------------
- (void) windowDidBecomeKey: (NSNotification *) aNotification 
{
	[self updateButtonState];
	[_frameDocument refresh];
}

//----------------------------------------------------------------------------------------
//  windowDidResignKey:
//
//      handle window resigning key state. Need to refresh the structure window.
//
//  windowDidResignKey: NSNotification * aNotification -> 
//
//  returns nothing
//----------------------------------------------------------------------------------------
- (void) windowDidResignKey: (NSNotification *) aNotification
{
	[_frameDocument refresh];
}

//----------------------------------------------------------------------------------------
//  windowWillClose:
//
//      handle window close.
//
//  windowWillClose: NSNotification * aNotification    ->
//
//  returns nothing
//----------------------------------------------------------------------------------------
- (void) windowWillClose:(NSNotification *) aNotification
{
	[_frameDocument nodeLoadPanelIsClosing];
}

//----------------------------------------------------------------------------------------
//  validateMenuItem:
//
//      return no for handlers declared here. With auto-enabled items, to suppress
//		the document defintion of these items, we must add handlers here and return
//		NO from this method.
//
//  validateMenuItem: NSMenuItem* anItem   -> the item
//
//  returns BOOL                           <- YES if enabled.
//----------------------------------------------------------------------------------------
- (BOOL) validateMenuItem: (NSMenuItem*) anItem
{
	if (anItem.action == @selector(clear:) || anItem.action == @selector(cut:)) {
		return [self canDelete] != nullptr;
	} else if (anItem.action == @selector(paste:)) {
		NSPasteboard* pb = [NSPasteboard generalPasteboard];
		return [pb canReadItemWithDataConformingToTypes:[NSArray arrayWithObject: kPasteBoardType]];
	} else if (anItem.action == @selector(copy:)) {
		return [self validSelection];
	}
	return NO;
}

//----------------------------------------------------------------------------------------
//  cut:
//
//      nothing for cut.
//
//  cut: id sender ->
//
//  returns nothing
//----------------------------------------------------------------------------------------
- (IBAction) cut: (id) sender
{
	NodeLoad deleteMe = [self canDelete];
	if (deleteMe != nullptr) {
		Action* act = [_frameDocument structure]->RemoveNodeLoad(deleteMe);
		[_frameDocument performAction: [FrameAction create: act]];
		
		FrameWriter w(0);
		
		[_frameDocument structure]->SaveNodeLoad(deleteMe, w);
		
		NSData* data = w.GetBytes();
		
		NSPasteboard* pb = [NSPasteboard generalPasteboard];
		
		[pb declareTypes: [NSArray arrayWithObject: kPasteBoardType] owner: nil];
		[pb setData: data forType: kPasteBoardType];
	}
}

//----------------------------------------------------------------------------------------
//  paste:
//
//      nothing for paste.
//
//  paste: id sender   ->
//
//  returns nothing
//----------------------------------------------------------------------------------------
- (IBAction) paste: (id) sender
{
	NSData* data = [[NSPasteboard generalPasteboard] dataForType: kPasteBoardType];
	if (data != nil) {
		// convert to a property and insert into the list.
		FrameReader r(data);
		
		[_frameDocument performAction:
		 	[FrameAction create: [_frameDocument structure]->LoadNodeLoad(r)]];
	}
}

//----------------------------------------------------------------------------------------
//  copy:
//
//      nothing for copy.
//
//  copy: id sender    ->
//
//  returns nothing
//----------------------------------------------------------------------------------------
- (IBAction) copy: (id) sender
{
	int sel = [_table selectedRow];
	if (sel >= 0 && sel < _loads->Length()) {
		
		FrameStructure* s = [_frameDocument structure];
		NodeLoad active = s->GetNodeLoads().At(sel);
		
		FrameWriter w(0);
		
		[_frameDocument structure]->SaveNodeLoad(active, w);
		
		NSData* data = w.GetBytes();
		
		NSPasteboard* pb = [NSPasteboard generalPasteboard];
		
		[pb declareTypes: [NSArray arrayWithObject: kPasteBoardType] owner: nil];
		[pb setData: data forType: kPasteBoardType];
	}
}

//----------------------------------------------------------------------------------------
//  clear:
//
//      nothing for clear.
//
//  clear: id sender   ->
//
//  returns nothing
//----------------------------------------------------------------------------------------
- (IBAction) clear: (id) sender
{
	NodeLoad deleteMe = [self canDelete];
	if (deleteMe != nullptr) {
		Action* act = [_frameDocument structure]->RemoveNodeLoad(deleteMe);
		[_frameDocument performAction: [FrameAction create: act]];
	}
}

@end
