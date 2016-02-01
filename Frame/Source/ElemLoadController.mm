/*+
 *
 *  ElemLoadController.mm
 *
 *  Copyright © 2003 David C. Salmon. All Rights Reserved.
 *
 *  Handles window controls for editing and assignment of element loads.
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

#import "ElemLoadController.h"
#import "FrameDocument.h"
#import "FrameAction.h"
#import "FrameIO.h"

#include "FrameStructure.h"
#include "ElementLoad.h"
#include "DlString.h"


static NSString* kPasteBoardType = @"dcs.Plane-Frame-ELoads";

@implementation ElemLoadController


//----------------------------------------------------------------------------------------
//  createElemLoadPanel:
//
//      Create the ElementLoad editor panel.
//
//  createNodeLoadPanel: FrameDocument* doc    -> the document
//
//  returns ElemLoadController*                <- the window controller
//----------------------------------------------------------------------------------------
+ (ElemLoadController*)createElemLoadPanel: (FrameDocument*) doc
{
    ElemLoadController* theController = nil;

    theController = [[ElemLoadController alloc] initWithDocument: doc];
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
	self = [super initWithWindowNibName:@"ElemLoadPanel" owner:self];
	_structure = [doc structure];
	_frameDocument = doc;
	_loads = NEW ElementLoadEnumerator();
	*_loads = _structure->GetElementLoads();
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
//  awakeFromNib
//
//      build the view.
//
//  returns nothing
//----------------------------------------------------------------------------------------
- (void) awakeFromNib {
	[[_hiliteButton retain] removeFromSuperview];
	
	int diff = [_table rectOfRow:0].size.width - [[_table superview] frame].size.width;
	if (diff > 0) {
		NSRect newFrame = _table.window.frame;
		newFrame.size.width += diff;
		[[_table window] setFrame: newFrame display: YES];
	}
}

//----------------------------------------------------------------------------------------
//  windowTitleForDocumentDisplayName:
//
//      return the document title.
//
//  windowTitleForDocumentDisplayName: NSString * displayName  -> 
//
//  returns NSString *                                         <- 
//----------------------------------------------------------------------------------------
- (NSString *)windowTitleForDocumentDisplayName:(NSString *)displayName 
{
	return [displayName stringByAppendingString : @" - Element Loads"];
}

//----------------------------------------------------------------------------------------
//  listChanged
//
//      handle changes to the list from the structure level. Mainly undo.
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
//      return true if the current selection is valid.
//
//  returns bool   <- 
//----------------------------------------------------------------------------------------
- (bool) validSelection {
	int sel = [_table selectedRow];
	return sel >= 0 && sel < _loads->Length();
}

//----------------------------------------------------------------------------------------
//  attachClicked:
//
//      attach the selected loads to the current element selection.
//
//  attachClicked: id sender   -> 
//
//  returns nothing
//----------------------------------------------------------------------------------------
- (IBAction)attachClicked:(id)sender
{
	[[self window] makeFirstResponder: [self window]];

	int selectedRow = [_table selectedRow];
	ElementLoad load = _loads->At(selectedRow);

	// attach the load at the selcted row to all selected nodes.
	[_frameDocument performAction: [FrameAction create: 
		_structure->AssignElementLoads(*[_frameDocument elementSelection], load)
		withView: [_frameDocument frameView]]];
	[_frameDocument refresh];
}

//----------------------------------------------------------------------------------------
//  hiliteClicked:
//
//      toggle hiliting. unused.
//
//  hiliteClicked: id sender   -> 
//
//  returns nothing
//----------------------------------------------------------------------------------------
- (IBAction)hiliteClicked:(id)sender
{
	[_frameDocument refresh];
}

//----------------------------------------------------------------------------------------
//  removeClicked:
//
//      remove the selected load from any selected elements.
//
//  removeClicked: id sender   -> 
//
//  returns nothing
//----------------------------------------------------------------------------------------
- (IBAction)removeClicked:(id)sender
{
	ElementLoad empty(0);
	// attach the load at the selcted row to all selected nodes.
	[_frameDocument performAction: [FrameAction create: 
		_structure->AssignElementLoads(*[_frameDocument elementSelection], empty)
		withView: [_frameDocument frameView]]];
	[_frameDocument refresh];
}

//----------------------------------------------------------------------------------------
//  hiliteLoad
//
//      return the load to be hilited on the structure.
//
//  returns ElementLoad    <- 
//----------------------------------------------------------------------------------------
- (ElementLoad)hiliteLoad 
{
	return [self validSelection] ? _loads->At([_table selectedRow]) : ElementLoad(0);
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
	bool hasSelection = [_frameDocument elementSelection]->Length() != 0;
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
//  returns ElementLoad
//----------------------------------------------------------------------------------------
- (ElementLoad) canDelete
{
	int sel = [_table selectedRow];
	if (sel >= 0 && sel < _loads->Length()) {

		FrameStructure* s = [_frameDocument structure];
		ElementLoad active = s->GetElementLoads().At(sel);
		
		if (s->GetAssignedElements(active).Length() == 0)
			return active;
	}
	
	return ElementLoad(0);
}

#pragma mark -
#pragma mark =============== Datasource ===============
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
- (NSInteger) numberOfRowsInTableView:(NSTableView *)tableView
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
- (id)             tableView: (NSTableView *)tableView
   objectValueForTableColumn: (NSTableColumn *)tableColumn
						 row: (NSInteger)row
{
	int colId = [[tableColumn identifier] intValue];
	if (row < _loads->Length() && colId > 0)
	{
		ElementLoad l(_loads->At(row));
		return [NSString stringWithCString: l.GetValue((ElementLoadType)(colId - 1), true).get()
								 encoding : NSUTF8StringEncoding];
	}
	return @"";
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
- (void)     tableView: (NSTableView *)tableView
		setObjectValue: (id)object
		forTableColumn: (NSTableColumn *)tableColumn
				   row: (NSInteger)row
{
	ElementLoad l(0);
	if (row == _loads->Length())
	{
		FrameAction* action = [FrameAction create: _structure->CreateElementLoad(l) withView: nil];
		[_frameDocument performAction: action];
		*_loads = _structure->GetElementLoads();
		[_table noteNumberOfRowsChanged];
	}
	else
	{
		l = _loads->At(row);
	}
	
	int colId = [[tableColumn identifier] intValue];
	if (colId > 0) {
		
		try {
			const char* theValue = [object cString];
			ElementLoadType theType = ElementLoadType(colId - 1);
			
			FrameAction* action = [FrameAction create: _structure->ChangeElementLoad(l, theType, theValue)
								withView: [_frameDocument frameView]];
			[_frameDocument performAction: action];
			[self updateButtonState];
		} catch(DlException& ex) {
			ex.Display();
		}
	}
	
}

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
#pragma mark =============== window ===============
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
//      return true if the widow goes inactive.
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
	[_frameDocument elementLoadPanelIsClosing];
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
	ElementLoad deleteMe = [self canDelete];
	if (deleteMe != nullptr) {
		Action* act = [_frameDocument structure]->RemoveElementLoad(deleteMe);
		[_frameDocument performAction: [FrameAction create: act]];
		
		FrameWriter w(0);
		
		[_frameDocument structure]->SaveElementLoad(deleteMe, w);
		
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
		 [FrameAction create: [_frameDocument structure]->LoadElementLoad(r)]];
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
		ElementLoad active = s->GetElementLoads().At(sel);
		
		FrameWriter w(0);
		
		[_frameDocument structure]->SaveElementLoad(active, w);
		
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
	ElementLoad deleteMe = [self canDelete];
	if (deleteMe != nullptr) {
		Action* act = [_frameDocument structure]->RemoveElementLoad(deleteMe);
		[_frameDocument performAction: [FrameAction create: act]];
	}
}

@end
