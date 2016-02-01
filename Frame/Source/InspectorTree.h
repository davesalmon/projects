/*+
 *
 *  InspectorTree.h
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

#import <Foundation/Foundation.h>

@class FrameDocument;

/* --------------------------------------------------------
 * protocol OutlineItem
 *
 *	define methods for all outline items.
 * --------------------------------------------------------*/
@protocol InspectorOutlineItem
- (DlInt32) index;		// return the index of the item in the list.
- (DlInt32) count;		// return the number of sub-items
- (BOOL) expandable;	// return true if expandable

- (NSString*) title;	// return the title column string.
- (NSString*) value;	// return the value column string.

- (void) update;		// update the item.

- (void) updateDocumentSelection;	// select corresponding element in document.
@end


/* --------------------------------------------------------
 * InspectorTree class
 *
 *	shared subclass to manage the inspector tree for both
 *	Node and Element objects.
 *
 *  sub-classes must override.
 * --------------------------------------------------------*/
@interface InspectorTree : NSObject

// initialize
- (instancetype) initWithDocument: (FrameDocument*) doc;

// update the tree.
- (void) updateTree: (NSOutlineView*) view;

// return the top level outline item for the specified index.
- (id<InspectorOutlineItem>) objectAtIndex:(NSUInteger)index;

// return the number of top level outline items
- (NSUInteger) count;

// return true if the tree is out-of-date
- (bool) treeNeedsUpdate;

// create a tree item. Sub-classes must implement.
- (id<InspectorOutlineItem>) createItemForIndex: (DlInt32)index andDocument: (FrameDocument*) doc;

// return the item count from the document. Sub-classes must implement.
- (DlInt32) itemCountForDocument: (FrameDocument*) doc;

// return the document.
- (FrameDocument*) frameDocument;

@end
