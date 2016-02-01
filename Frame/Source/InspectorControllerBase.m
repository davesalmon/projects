/*+
 *
 *  InspectorControllerBase.m
 *
 *  Copyright © 2015 David C. Salmon. All Rights Reserved.
 *
 *  Base class implementing inspector controller logic for node and element inspectors.
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

#import "InspectorControllerBase.h"
#import "InspectorTree.h"

#import "FrameDocument.h"

#include <memory>

@interface InspectorControllerBase ()
{
	NSTableColumn *_labelColumn;
	NSOutlineView *_outlineView;
	InspectorTree* _items;
	
	bool _updatingSelection;
}

@end

@implementation InspectorControllerBase

//----------------------------------------------------------------------------------------
//  setOutlineView:
//
//      set the outline view used by this class.
//
//  setOutlineView: NSOutlineView* view    -> 
//
//  returns nothing
//----------------------------------------------------------------------------------------
- (void) setOutlineView: (NSOutlineView*) view
{
	_outlineView = view;
}
//----------------------------------------------------------------------------------------
//  setLabelColumn:
//
//      set the label column for this class.
//
//  setLabelColumn: NSTableColumn* col -> 
//
//  returns nothing
//----------------------------------------------------------------------------------------
- (void) setLabelColumn: (NSTableColumn*) col
{
	_labelColumn = col;
}

//----------------------------------------------------------------------------------------
//  setItems:
//
//      set the items to display.
//
//  setItems: InspectorTree* items -> 
//
//  returns nothing
//----------------------------------------------------------------------------------------
- (void) setItems: (InspectorTree*)items
{
#if DlDebugging
	NSLog(@"updating items with count %ld", [items count]);
#endif
	_items = items;
	[_outlineView reloadData];
}

//----------------------------------------------------------------------------------------
//  setUpdatingSelection:
//
//      set true if the selection is being updated. This prevent recursion when the list
//      selections are synched with the main window.
//
//  setUpdatingSelection: bool updating    -> 
//
//  returns nothing
//----------------------------------------------------------------------------------------
- (void) setUpdatingSelection: (bool) updating
{
	_updatingSelection = updating;
}

//----------------------------------------------------------------------------------------
//  updatingSelection
//
//      return true if the selection is being updated from the document.
//
//  returns bool   <- 
//----------------------------------------------------------------------------------------
- (bool) updatingSelection
{
	return _updatingSelection;
}

//----------------------------------------------------------------------------------------
//  updateSelection:
//
//      update the selected items. placeholder for subclass. subclass must override.
//
//  updateSelection: bool fromFrameDocument    -> the document
//
//  returns nothing
//----------------------------------------------------------------------------------------
- (void) updateSelection: (bool)fromFrameDocument
{
}

//----------------------------------------------------------------------------------------
//  buildList
//
//      placeholder for subclass. subclass must override.
//
//  returns nothing
//----------------------------------------------------------------------------------------
- (void) buildList
{
}

#pragma mark -
#pragma mark ========== data source methods ============
#pragma mark -

//----------------------------------------------------------------------------------------
//  outlineView:numberOfChildrenOfItem:
//
//      Return the number of children for the specified item.
//
//  outlineView: NSOutlineView * outlineView   -> view
//  numberOfChildrenOfItem: id item            -> item
//
//  returns int                                <- the number of children
//----------------------------------------------------------------------------------------
- (NSInteger)outlineView: (NSOutlineView *)outlineView numberOfChildrenOfItem: (id)item
{
	if (item == nil)
	{
#if DlDebugging
		NSLog(@"Root node has %lu items\n", [_items count]);
#endif
		return [_items count];
	}
	else
	{
#if DlDebugging
		NSLog(@"sub node has %lu items\n", [(NSArray*)item count]);
#endif
		return [(NSArray*)item count];
	}
}

//----------------------------------------------------------------------------------------
//  outlineView:child:ofItem:
//
//      Return the object that is the index child of the specified item. The root item
//		is nil.
//
//  outlineView: NSOutlineView * outlineView   -> the view
//  child: int index                           -> the index
//  ofItem: id item                            -> the item
//
//  returns id                                 <- the object to associate.
//----------------------------------------------------------------------------------------
- (id)outlineView: (NSOutlineView *)outlineView child: (NSInteger)index ofItem: (id)item
{
	id val = nil;
	
	if (item == nil)
	{
		//printf("child of top level item\n");
		val = [_items objectAtIndex: index];
	}
	else
	{
		//printf("child  sub item of %s (%u)\n", [[[item class] description] cString], [item retainCount]);
		val = [item objectAtIndex: index];
	}
	
	//printf("returning item is %s (%u)\n", [[[val class] description] cString], [val retainCount]);
	return val;
	
}

//----------------------------------------------------------------------------------------
//  outlineView:isItemExpandable:
//
//      Return true if the given item is expandable.
//
//  outlineView: NSOutlineView * outlineView   -> the view
//  isItemExpandable: id item                  -> the item
//
//  returns BOOL                               <- true if expandable.
//----------------------------------------------------------------------------------------
- (BOOL)outlineView: (NSOutlineView *)outlineView isItemExpandable: (id)item
{
	return [(id<InspectorOutlineItem>)item expandable];
}

//----------------------------------------------------------------------------------------
//  outlineView:objectValueForTableColumn:byItem:
//
//      Return the value for the specified colum and item.
//
//  outlineView: NSOutlineView * outlineView               -> view
//  objectValueForTableColumn: NSTableColumn * tableColumn -> the column
//  byItem: id item                                        -> the item
//
//  returns id                                             <- the value to display
//----------------------------------------------------------------------------------------
- (id)          outlineView: (NSOutlineView *)outlineView
  objectValueForTableColumn: (NSTableColumn *)tableColumn
					 byItem: (id)item
{
	if (tableColumn == _labelColumn)
	{
		//printf("label for item with address %4lx (%u)\n", (unsigned long)item, [item retainCount]);
		return [item title];
	}
	else
	{
		//printf("value for item with address %4lx (%u)\n", (unsigned long)item, [item retainCount]);
		return [item value];
	}
}

#pragma mark -
#pragma mark ========== delegate methods ============
#pragma mark -

//----------------------------------------------------------------------------------------
//  outlineView:shouldEditTableColumn:item:
//
//      Return true if the item is editable. Always false.
//
//  outlineView: NSOutlineView * outlineView           -> view
//  shouldEditTableColumn: NSTableColumn * tableColumn -> column
//  item: id item                                      -> item
//
//  returns BOOL                                       <- false
//----------------------------------------------------------------------------------------
- (BOOL)     outlineView: (NSOutlineView *)outlineView
   shouldEditTableColumn: (NSTableColumn *)tableColumn
					item: (id)item
{
	return NO;
}

//----------------------------------------------------------------------------------------
//  outlineView:shouldSelectItem:
//
//      Return true if the item can be selected. Here only expandable items are selectable.
//
//  outlineView: NSOutlineView * outlineView   -> view
//  shouldSelectItem: id item                  -> item
//
//  returns BOOL                               <- true if top level.
//----------------------------------------------------------------------------------------
- (BOOL)outlineView: (NSOutlineView*)outlineView shouldSelectItem: (id)item
{
	return [item expandable];
}

//----------------------------------------------------------------------------------------
//  outlineView:shouldSelectTableColumn:
//
//      return true if the column should be selected. Always false.
//
//  outlineView: NSOutlineView * outlineView               -> view
//  shouldSelectTableColumn: NSTableColumn * tableColumn   -> column
//
//  returns BOOL                                           <- false
//----------------------------------------------------------------------------------------
- (BOOL)outlineView: (NSOutlineView*)ov shouldSelectTableColumn: (NSTableColumn*)tc
{
	return NO;
}

//----------------------------------------------------------------------------------------
//  outlineView:willDisplayCell:forTableColumn:item:
//
//      The specified cell is about to display. Modify the style appropriately.
//
//  outlineView: NSOutlineView * outlineView       -> the view
//  willDisplayCell: id cell                       -> the cell
//  forTableColumn: NSTableColumn * tableColumn    -> the column
//  item: id item                                  -> the item
//
//  returns nothing
//----------------------------------------------------------------------------------------
- (void)outlineView: (NSOutlineView*)outlineView
	willDisplayCell: (id)cell
	 forTableColumn: (NSTableColumn*)tableColumn
			   item: (id)item
{
	if ([item expandable])
	{
		[cell setFont: [NSFont boldSystemFontOfSize: 12]];
		[cell setAlignment: NSLeftTextAlignment];
	}
	else
	{
		if (tableColumn == _labelColumn)
		{
			[cell setFont: [NSFont boldSystemFontOfSize: 11]];
			[cell setAlignment: NSRightTextAlignment];
		}
		else
		{
			[cell setFont: [NSFont userFontOfSize: 12]];
			[cell setAlignment: NSLeftTextAlignment];
		}
	}
}

#pragma mark -
#pragma mark =============== Notifications ===============
#pragma mark -

//----------------------------------------------------------------------------------------
//  outlineViewSelectionDidChange:
//
//      The selection changed. Update the document selection.
//
//  outlineViewSelectionDidChange: NSNotification * notification   -> unused
//
//  returns nothing
//----------------------------------------------------------------------------------------
- (void)outlineViewSelectionDidChange:(NSNotification*)notification
{
	if (!_updatingSelection) {
		NSIndexSet* sel = [_outlineView selectedRowIndexes];
		unsigned int count = [sel count];
		[[_items frameDocument] clearSelections];
		if (count > 0)
		{
			std::unique_ptr<NSUInteger> values((NSUInteger*)OPERATORNEW(count * sizeof(NSUInteger)));
			NSUInteger* ptr = values.get();
			
			[sel getIndexes: values.get() maxCount: count inIndexRange: nil];
			
			for (int i = 0; i < count; i++)
			{
				unsigned int row = ptr[i];
				
				// now we need to find the object at row
				id<InspectorOutlineItem> obj = [_outlineView itemAtRow: row];
				[obj updateDocumentSelection];
			}
		}
	}
	_updatingSelection = false;
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
- (void) windowDidBecomeKey: (NSNotification*) aNotification
{
	if ([_items treeNeedsUpdate])
		[self buildList];
	
	[self updateSelection: false];
}

@end
