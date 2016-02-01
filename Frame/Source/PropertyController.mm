/*+
 *
 *  PropertyController.mm
 *
 *  Copyright © 2015 David C. Salmon. All Rights Reserved.
 *
 *  window controller for element property editor.
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

#import "PropertyController.h"
#import "FrameDocument.h"
#import "PropertyListArray.h"
#import "FrameAction.h"
#import "TableColumn.h"

#import "FrameIO.h"

static NSString* kPasteBoardType = @"dcs.Plane-Frame-Property";

@interface PropertyController ()
{
	PropertyListArray*	_listValues;
}
@end

@implementation PropertyController

//----------------------------------------------------------------------------------------
//  createPropertyPanel:
//
//      create a new property panel.
//
//  createPropertyPanel: FrameDocument* doc    -> 
//
//  returns PropertyController*                <- 
//----------------------------------------------------------------------------------------
+ (PropertyController*)createPropertyPanel: (FrameDocument*)doc 
{
    PropertyController* theController = nil;

    theController = [[PropertyController alloc] initWithDocument: doc];
//    [theController showWindow:theController];
    [theController setShouldCloseDocument:NO];
    [doc addWindowController:theController];
    [theController release];
    return theController;
}

//----------------------------------------------------------------------------------------
//  initWithDocument:
//
//      init the panel for the specified document.
//
//  initWithDocument: FrameDocument* doc   -> 
//
//  returns id                             <- 
//----------------------------------------------------------------------------------------
- (id) initWithDocument: (FrameDocument*) doc {
	
	self = [super initWithWindowNibName: @"PropertyPanel"];
	if (self) {
		_doc = doc;
	}
	
	return self;
}

//----------------------------------------------------------------------------------------
//  dealloc
//
//      free memory.
//
//  returns nothing
//----------------------------------------------------------------------------------------
- (void) dealloc {
	[super dealloc];
    [_listValues release];
}

//----------------------------------------------------------------------------------------
//  windowTitleForDocumentDisplayName:
//
//      return the window title.
//
//  windowTitleForDocumentDisplayName: NSString * displayName  -> 
//
//  returns NSString *                                         <- 
//----------------------------------------------------------------------------------------
- (NSString *)windowTitleForDocumentDisplayName:(NSString *)displayName
{
	return [displayName stringByAppendingString : @" - Properties"];
}

//----------------------------------------------------------------------------------------
//  awakeFromNib
//
//      finish initialization.
//
//  returns nothing
//----------------------------------------------------------------------------------------
- (void) awakeFromNib {
	
	_listValues = [[PropertyListArray createWithDocument: _doc] retain];
	
	[TableColumn initializeTable: _table
						withList: [_listValues properties]
					   editiable: YES
						   align: NSLeftTextAlignment];

	[self refresh];
}

//----------------------------------------------------------------------------------------
//  listChanged
//
//      return true if the property list has changed.
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
//      return true if there is a valid selection.
//
//  returns bool   <- 
//----------------------------------------------------------------------------------------
- (bool) validSelection
{
	int sel = [_table selectedRow];
	return sel >= 0 && sel < [_listValues numberOfRows];
}

//----------------------------------------------------------------------------------------
//  attachProperty:
//
//      attach the selected property to the element selection.
//
//  attachProperty: id sender  -> 
//
//  returns nothing
//----------------------------------------------------------------------------------------
- (IBAction) attachProperty:(id)sender
{
	[[self window] makeFirstResponder: [self window]];
	
	[_listValues attachRow: [_table selectedRow]];
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
	bool validRow = [self validSelection];
	BOOL enableAttach = ([_listValues canAttach] && validRow) ? YES : NO;
	//	check the button state
	[_attachButton setEnabled: enableAttach];
}

// return the index of the load to hilite
//----------------------------------------------------------------------------------------
//  selectedRow
//
//      return the selected property row.
//
//  returns int    <- 
//----------------------------------------------------------------------------------------
- (int) selectedRow
{
	return [self validSelection] ? [_table selectedRow] : -1;
}

//----------------------------------------------------------------------------------------
//  selectRow:
//
//      select the specified row.
//
//  selectRow: NSInteger row   -> 
//
//  returns nothing
//----------------------------------------------------------------------------------------
- (void) selectRow: (NSInteger) row
{
	[_table selectRowIndexes:[NSIndexSet indexSetWithIndex: row] byExtendingSelection:NO];
}

//----------------------------------------------------------------------------------------
//  refresh
//
//      refresh the list.
//
//  returns nothing
//----------------------------------------------------------------------------------------
- (void) refresh
{
	[self updateButtonState];
	[_table setNeedsDisplay: YES];
	
	// find the active property and select it.
	Property active = [_doc structure]->GetActiveProperty();
	PropertyEnumerator props = [_doc structure]->GetProperties();
	props.Reset();
	int activeRow = 0;
	while (props.HasMore()) {
		Property next = props.Next();
		if (next == active)
			break;
		
		activeRow++;
	}
	
	[self selectRow: activeRow];

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
- (NSInteger) numberOfRowsInTableView:(NSTableView *)tableView
{
	return [_listValues numberOfRows] + 1; //_loads->Length() + 1;
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
- (id)                 tableView: (NSTableView *)tableView
	   objectValueForTableColumn: (NSTableColumn *)tableColumn
							 row: (NSInteger)row
{
	int colId = [[tableColumn identifier] intValue];
	bool editable = [_listValues canEditItemAtRow: row column:colId];
	NSCell* cell = [tableColumn dataCellForRow: row];
	[cell setEnabled: editable];
	
	return [_listValues getItemAtRow: row column: colId];
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
	int colId = [[tableColumn identifier] intValue];
	if ([_listValues canEditItemAtRow: row column:colId]) {
		try {
			if ([_listValues setItem: object forRow: row column: colId]) {
				[_table noteNumberOfRowsChanged];
			}
			[[_listValues frameDocument] refresh];
		} catch (...) {
			NSBeep();
			[_table needsDisplay];
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
- (void) tableViewSelectionDidChange: (NSNotification *)notification
{
	[self updateButtonState];
	if ([self validSelection]) {
		[_listValues selectRow:[_table selectedRow]];
	}
	[[_listValues frameDocument] refresh];
}

//----------------------------------------------------------------------------------------
//  tableView:shouldEditTableColumn:row:
//
//      return true if the specified column and row can be edited.
//
//  tableView: NSTableView * tableView                 -> 
//  shouldEditTableColumn: NSTableColumn * tableColumn -> 
//  row: NSInteger row                                 -> 
//
//  returns BOOL                                       <- 
//----------------------------------------------------------------------------------------
- (BOOL) tableView: (NSTableView *)tableView shouldEditTableColumn: (NSTableColumn *)tableColumn
			   row: (NSInteger)row
{
	int colId = [[tableColumn identifier] intValue];
	BOOL val = [_listValues canEditItemAtRow: row column: colId];
	
#if DlDebugging
	NSLog(@"Got value %d for shouldEditTableColumn %@", val, [tableColumn identifier]);
#endif
	
	return val;
}

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
	[[_listValues frameDocument] refresh];
}

//----------------------------------------------------------------------------------------
//  windowDidResignKey:
//
//      called when the window is no longer key.
//
//  windowDidResignKey: NSNotification * aNotification -> 
//
//  returns nothing
//----------------------------------------------------------------------------------------
- (void) windowDidResignKey: (NSNotification *) aNotification
{
	[[_listValues frameDocument] refresh];
}

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
	[_doc propertyPanelIsClosing];
}

#pragma mark -
#pragma mark =============== Window delegate ===============
#pragma mark -

//----------------------------------------------------------------------------------------
//  validateMenuItem:
//
//      validate the menu items handled by this controller.
//
//  validateMenuItem: NSMenuItem* anItem   -> 
//
//  returns BOOL                           <- 
//----------------------------------------------------------------------------------------
- (BOOL) validateMenuItem: (NSMenuItem*) anItem
{
	if (anItem.action == @selector(clear:) || anItem.action == @selector(cut:)) {
		return [_listValues canDelete] != nullptr;
	} else if (anItem.action == @selector(paste:)) {
		NSPasteboard* pb = [NSPasteboard generalPasteboard];
		return [pb canReadItemWithDataConformingToTypes:[NSArray arrayWithObject: kPasteBoardType]];
	} else if (anItem.action == @selector(copy:)) {
		return [_doc structure]->GetActiveProperty() != nullptr;
	}
	return NO;
}

//----------------------------------------------------------------------------------------
//  cut:
//
//      cut the selected item.
//
//  cut: id sender -> 
//
//  returns nothing
//----------------------------------------------------------------------------------------
- (IBAction) cut: (id) sender
{
	Property deleteMe = [_listValues canDelete];
	if (deleteMe != nullptr) {
		[_doc performAction: [FrameAction create: [_doc structure]->RemoveProperty(deleteMe)]];
		
		FrameWriter w(0);
		
		[_doc structure]->SaveProperty(deleteMe, w);
		
		NSData* data = w.GetBytes();
		
		NSPasteboard* pb = [NSPasteboard generalPasteboard];
		
		[pb declareTypes: [NSArray arrayWithObject: kPasteBoardType] owner: nil];
		[pb setData: data forType: kPasteBoardType];
	}
}

//----------------------------------------------------------------------------------------
//  paste:
//
//      paste the data into the selected item.
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
		
		[_doc performAction: [FrameAction create: [_doc structure]->LoadProperty(r)]];
	}
}

//----------------------------------------------------------------------------------------
//  copy:
//
//      copy the selected item.
//
//  copy: id sender    -> 
//
//  returns nothing
//----------------------------------------------------------------------------------------
- (IBAction) copy: (id) sender
{
	Property prop = [_doc structure]->GetActiveProperty();
	if (prop) {
		FrameWriter w(0);
		
		[_doc structure]->SaveProperty(prop, w);
		
		NSData* data = w.GetBytes();
		
		NSPasteboard* pb = [NSPasteboard generalPasteboard];
		
		[pb declareTypes: [NSArray arrayWithObject: kPasteBoardType] owner: nil];
		[pb setData: data forType: kPasteBoardType];
	}
}

//----------------------------------------------------------------------------------------
//  clear:
//
//      delete the selected item.
//
//  clear: id sender   -> 
//
//  returns nothing
//----------------------------------------------------------------------------------------
- (IBAction) clear: (id) sender
{
	NSLog(@"Intercepted clear");
	
	Property deleteMe = [_listValues canDelete];
	if (deleteMe != nullptr) {
		[_doc performAction: [FrameAction create: [_doc structure]->RemoveProperty(deleteMe)]];
	} else {
		NSBeep();
	}
}

@end
