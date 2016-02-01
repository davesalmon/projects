/*+
 *
 *  InspectorTree.m
 *
 *  Copyright © 2015 David C. Salmon. All Rights Reserved.
 *
 *  Base class for the node and element inspector trees. Handles all tree management
 *  except item creation and document item counts.
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

#import "InspectorTree.h"
#import "FrameDocument.h"
#include "FrameStructure.h"

@interface InspectorTree ()
{
	DlInt32 _lastChange;
	bool _analyzed;
	FrameDocument* _doc;
	NSMutableArray* _items;
}
@end

@implementation InspectorTree

//----------------------------------------------------------------------------------------
//  initWithDocument:
//
//      initialize the tree using the specified document.
//
//  initWithDocument: FrameDocument* doc   -> the document
//
//  returns instancetype                   <- the instance
//----------------------------------------------------------------------------------------
- (instancetype) initWithDocument: (FrameDocument*) doc
{
	self = [super init];
	if (self ) {
		_lastChange = [doc changeCount] - 1;
		_analyzed = false;
		_doc = doc;
		_items = nil;
		[self updateTree: nil];
	}
	
	return self;
}

//----------------------------------------------------------------------------------------
//  updateTree:
//
//      update the tree for the specified view (or nil).
//
//  updateTree: NSOutlineView* view    -> the view
//
//  returns nothing
//----------------------------------------------------------------------------------------
- (void) updateTree: (NSOutlineView*) view
{
	NSMutableArray* expandedItems = nil;
	
	if ([self treeNeedsUpdate]) {
		
		if (view) {
			// build the list of expanded items and remove any items
			// that may have been removed from the list.
			if ([_items count] > 0) {
				expandedItems = [NSMutableArray array];
				
				// remove any missing items
				while ([_items count] > 0)
					[_items removeObjectAtIndex: [_items count] - 1];
				
				for (int i = 0; i < [_items count]; i++) {
					id item = [_items objectAtIndex: i];
					if ([view isItemExpanded: item])
						[expandedItems addObject: item];
				}
			}
		}
		
		// get the item count from the sub-class.
		DlInt32 count = [self itemCountForDocument: _doc];
		
		if (_items == nil) {
			_items = [[NSMutableArray arrayWithCapacity: count] retain];
		}
		
		// build the new list or update existing.
		
		@autoreleasepool {
			DlInt32 currentCount = [_items count];
			
			for(DlInt32 i = 0; i < count; i++) {
				if (i < currentCount) {
					[[self objectAtIndex: i] update];
				} else {
					id<InspectorOutlineItem> obj = [self createItemForIndex: i andDocument: _doc];
					[_items addObject: obj];
				}
			}
		}
		
		if (view) {
			[view reloadData];
			if (expandedItems) {
				for (int i = 0; i < [expandedItems count]; i++) {
					[view expandItem: [expandedItems objectAtIndex: i]];
				}
			}
		}
		
		_lastChange = [_doc changeCount];
		_analyzed = [_doc structure]->Analyzed();
	}
}

//----------------------------------------------------------------------------------------
//  objectAtIndex:
//
//      return the tree object at the specified index.
//
//  objectAtIndex: NSUInteger index    -> 
//
//  returns id<InspectorOutlineItem>   <- 
//----------------------------------------------------------------------------------------
- (id<InspectorOutlineItem>) objectAtIndex:(NSUInteger)index
{
	return [_items objectAtIndex: index];
}

//----------------------------------------------------------------------------------------
//  count
//
//      return the number of top level nodes.
//
//  returns NSUInteger <- 
//----------------------------------------------------------------------------------------
- (NSUInteger) count
{
	return [_items count];
}

//----------------------------------------------------------------------------------------
//  treeNeedsUpdate
//
//      return true if the tree is out-of-date.
//
//  returns bool   <- 
//----------------------------------------------------------------------------------------
- (bool) treeNeedsUpdate
{
	return _lastChange != [_doc changeCount] || _analyzed != [_doc structure]->Analyzed();
}

//----------------------------------------------------------------------------------------
//  frameDocument
//
//      return the associated document.
//
//  returns FrameDocument* <- 
//----------------------------------------------------------------------------------------
- (FrameDocument*) frameDocument
{
	return _doc;
}

//----------------------------------------------------------------------------------------
//  createItemForIndex:andDocument:
//
//      create an item for the specified index.
//
//		subclasses must override.
//
//  createItemForIndex: DlInt32 index  -> the index.
//  andDocument: FrameDocument* doc    -> the document.
//
//  returns id<InspectorOutlineItem>   <- the new item.
//----------------------------------------------------------------------------------------
- (id<InspectorOutlineItem>) createItemForIndex: (DlInt32)index andDocument: (FrameDocument*) doc
{
	[self doesNotRecognizeSelector: _cmd];
	return nil;
}

//----------------------------------------------------------------------------------------
//  itemCountForDocument:
//
//      return the item count.
//
//		subclasses must override.
//
//  itemCountForDocument: FrameDocument* doc   -> the document
//
//  returns DlInt32                            <- the count
//----------------------------------------------------------------------------------------
- (DlInt32) itemCountForDocument: (FrameDocument*) doc
{
	[self doesNotRecognizeSelector: _cmd];
	return 0;
}

@end

// eof

