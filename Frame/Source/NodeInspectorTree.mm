/*+
 *
 *  NodeInspectorTree.mm
 *
 *  Copyright © 2014 David C. Salmon. All Rights Reserved.
 *
 *  handle the node inspector tree data.
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

#import "NodeInspectorTree.h"
#import "FrameDocument.h"
#import "InspectorControllerBase.h"

#include "FrameStructure.h"
#include "NodeEnumerator.h"
#include "Node.h"
#include "LoadCaseResults.h"

#pragma mark -
#pragma mark ====== NodeItem declaration
#pragma mark -

@class NodeLeaf;

/* --------------------------------------------------------
 * local class NodeItem
 *
 *	track the nodes in the list
 * --------------------------------------------------------*/
@interface NodeItem: NSObject <InspectorOutlineItem>
{
	DlInt32	_index;
	NSMutableArray* _list;
	FrameDocument* _doc;
}

+ (NodeItem*) createItemForIndex: (DlInt32)index
					 andDocument: (FrameDocument*)doc;

- (instancetype) initForIndex: (DlInt32)index
				  andDocument: (FrameDocument*)doc;

- (void) update;

- (Node) node;

- (NodeLeaf*) objectAtIndex: (DlInt32) index;

@end

#pragma mark -
#pragma mark ====== NodeLeaf declaration
#pragma mark -

/* --------------------------------------------------------
 * local class NodeLeaf
 *
 *	track the node data elements
 * --------------------------------------------------------*/
@interface NodeLeaf: NSObject <InspectorOutlineItem>
{
	DlInt32	_index;
	DlInt32 _item;
	FrameDocument* _doc;
}

+ (NodeLeaf*) createItemForIndex: (DlInt32)index
						  offset: (DlInt32)item
					 andDocument: (FrameDocument*)doc;

- (instancetype) initForIndex: (DlInt32)index
					   offset: (DlInt32)item
				  andDocument: (FrameDocument*)doc;

@end

#pragma mark -
#pragma mark ====== NodeInspectorTree definition
#pragma mark -

/* --------------------------------------------------------
 *  class NodeInspectorTree
 *
 *	track handle the tree of node data points.
 * --------------------------------------------------------*/
@implementation NodeInspectorTree

//----------------------------------------------------------------------------------------
//  createWithDocument:
//
//      create the tree.
//
//  createWithDocument: FrameDocument* doc -> 
//
//  returns NodeInspectorTree*             <- 
//----------------------------------------------------------------------------------------
+ (NodeInspectorTree*) createWithDocument: (FrameDocument*) doc
{
	return [[[NodeInspectorTree alloc] initWithDocument: doc] autorelease];
}

//----------------------------------------------------------------------------------------
//  initWithDocument:
//
//      initialize the tree.
//
//  initWithDocument: FrameDocument* doc   -> 
//
//  returns instancetype                   <- 
//----------------------------------------------------------------------------------------
- (instancetype) initWithDocument: (FrameDocument*) doc
{
	return [super initWithDocument: doc];
}

//----------------------------------------------------------------------------------------
//  createItemForIndex:andDocument:
//
//      create an item for the node tree.
//
//  createItemForIndex: DlInt32 index  -> 
//  andDocument: FrameDocument* doc    -> 
//
//  returns id<InspectorOutlineItem>   <- 
//----------------------------------------------------------------------------------------
- (id<InspectorOutlineItem>) createItemForIndex: (DlInt32)index andDocument: (FrameDocument*) doc
{
	return [NodeItem createItemForIndex: index andDocument: doc];
}

- (DlInt32) itemCountForDocument: (FrameDocument*) doc;
{
	FrameStructure* s = [doc structure];
	return s->GetNodes().Length();
}

@end

#pragma mark -
#pragma mark ====== NodeItem definition
#pragma mark -

@implementation NodeItem

//----------------------------------------------------------------------------------------
//  createItemForIndex:andDocument:
//
//      create a NodeItem.
//
//  createItemForIndex: DlInt32 index  -> 
//  andDocument: FrameDocument* doc    -> 
//
//  returns NodeItem*                  <- 
//----------------------------------------------------------------------------------------
+ (NodeItem*) createItemForIndex: (DlInt32)index
					 andDocument: (FrameDocument*)doc
{
	return [[[NodeItem alloc] initForIndex: index andDocument: doc] autorelease];
}

//----------------------------------------------------------------------------------------
//  initForIndex:andDocument:
//
//      initialize a node item.
//
//  initForIndex: DlInt32 index        -> 
//  andDocument: FrameDocument* doc    -> 
//
//  returns instancetype               <- 
//----------------------------------------------------------------------------------------
- (instancetype) initForIndex: (DlInt32)index
				  andDocument: (FrameDocument*)doc
{
	self = [super init];
	if (self) {
		// init me
		_index = index;
		_doc = doc;
		[self update];
	}
	
	return self;
}

//----------------------------------------------------------------------------------------
//  update
//
//      update a NodeItem
//
//  returns nothing
//----------------------------------------------------------------------------------------
- (void) update
{
	if (_list)
	{
		[_list release];
		_list = nil;
	}
	
	// we need: x, y, fx, fy, ftheta or settle in each
	// if analyzed, we need dx, dy, dtheta or rx, ry, rtheta.
	DlInt32 count = 5;
	
	FrameStructure* s = [_doc structure];
	NodeEnumerator nodes(s->GetNodes());
	if (_index >= nodes.Length())
		return;
	
	if (s->Analyzed())
		count += DOF_PER_NODE; // 3 DOF
	
	_list = [[NSMutableArray arrayWithCapacity: count] retain];
	
	for (DlInt32 i = 0; i < count; i++)
	{
		NodeLeaf* obj = [NodeLeaf createItemForIndex: _index offset: i andDocument: _doc];
		[_list addObject: obj];
	}
}

//----------------------------------------------------------------------------------------
//  node
//
//      return the associated node.
//
//  returns Node   <- 
//----------------------------------------------------------------------------------------
- (Node) node
{
	return [_doc structure]->GetNodes().At(_index);
}

//----------------------------------------------------------------------------------------
//  objectAtIndex:
//
//      return the sub-item at the specified index.
//
//  objectAtIndex: DlInt32 index   -> 
//
//  returns NodeLeaf*              <- 
//----------------------------------------------------------------------------------------
- (NodeLeaf*) objectAtIndex: (DlInt32) index
{
	return [_list objectAtIndex: index];
}

#pragma mark -
#pragma mark ====== NodeItem NodeOutlineItem implementation
#pragma mark -

//----------------------------------------------------------------------------------------
//  index
//
//      return the index of this item.
//
//  returns DlInt32    <- 
//----------------------------------------------------------------------------------------
- (DlInt32) index
{
	return _index;
}

//----------------------------------------------------------------------------------------
//  count
//
//      return the number of items in the list.
//
//  returns DlInt32    <- the item count.
//----------------------------------------------------------------------------------------
- (DlInt32) count
{
	return [_list count];
}

//----------------------------------------------------------------------------------------
//  expandable
//
//      return true if this one is expandable.
//
//  returns BOOL   <- 
//----------------------------------------------------------------------------------------
- (BOOL) expandable
{
	return YES;
}

//----------------------------------------------------------------------------------------
//  title
//
//      return the title for this item.
//
//  returns NSString*  <- 
//----------------------------------------------------------------------------------------
- (NSString*) title
{
	return [NSString stringWithFormat: @"Node #%ld", (long)_index + 1];
}

//----------------------------------------------------------------------------------------
//  value
//
//      reutn the vlaue for this item.
//
//  returns NSString*  <- 
//----------------------------------------------------------------------------------------
- (NSString*) value
{
	return @"";
}

//----------------------------------------------------------------------------------------
//  updateDocumentSelection
//
//      update the document selection for this item.
//
//  returns nothing
//----------------------------------------------------------------------------------------
- (void) updateDocumentSelection
{
	Node n = [self node];
	[_doc selectNode: n withFlags: NSShiftKeyMask];
}

@end

#pragma mark -
#pragma mark ====== NodeLeaf definition
#pragma mark -

@implementation NodeLeaf

//----------------------------------------------------------------------------------------
//  createItemForIndex:offset:andDocument:
//
//      create a NodeLeaf.
//
//  createItemForIndex: DlInt32 index  -> 
//  offset: DlInt32 item               -> 
//  andDocument: FrameDocument* doc    -> 
//
//  returns NodeLeaf*                  <- 
//----------------------------------------------------------------------------------------
+ (NodeLeaf*) createItemForIndex: (DlInt32)index
						  offset: (DlInt32)item
					 andDocument: (FrameDocument*)doc
{
	return [[[NodeLeaf alloc] initForIndex: index offset: item andDocument: doc] autorelease];
}

//----------------------------------------------------------------------------------------
//  initForIndex:offset:andDocument:
//
//      initialize a NodeLeaf.
//
//  initForIndex: DlInt32 index        -> 
//  offset: DlInt32 item               -> 
//  andDocument: FrameDocument* doc    -> 
//
//  returns instancetype               <- 
//----------------------------------------------------------------------------------------
- (instancetype) initForIndex: (DlInt32)index
					   offset: (DlInt32)item
				  andDocument: (FrameDocument*)doc
{
	self = [super init];
	if (self) {
		_index = index; 	// the node index.
		_item = item;		// the item offset.
		_doc = doc;
	}
	return self;
}

//----------------------------------------------------------------------------------------
//  update
//
//      update the NodeLeaf.
//
//  returns nothing
//----------------------------------------------------------------------------------------
- (void) update {}

//----------------------------------------------------------------------------------------
//  updateDocumentSelection
//
//      update selection for this item.
//
//  returns nothing
//----------------------------------------------------------------------------------------
- (void) updateDocumentSelection {}

#pragma mark -
#pragma mark ====== NodLeaf NodeOutlineItem implementation
#pragma mark -

//----------------------------------------------------------------------------------------
//  index
//
//      return the offset of this item.
//
//  returns DlInt32    <- 
//----------------------------------------------------------------------------------------
- (DlInt32) index
{
	return _item;
}

//----------------------------------------------------------------------------------------
//  count
//
//      return the number of sub-items.
//
//  returns DlInt32    <- the number of sub-items.
//----------------------------------------------------------------------------------------
- (DlInt32) count
{
	return 0;
}

//----------------------------------------------------------------------------------------
//  expandable
//
//      return false.
//
//  returns BOOL   <- 
//----------------------------------------------------------------------------------------
- (BOOL) expandable
{
	return NO;
}

//----------------------------------------------------------------------------------------
//  title
//
//      return the title for this item.
//
//  returns NSString*  <- 
//----------------------------------------------------------------------------------------
- (NSString*) title
{
	FrameStructure* s = [_doc structure];
	NodeEnumerator nodes(s->GetNodes());
	
	if (_index >= nodes.Length())
		return @"No Node";
	
	Node theNode(nodes.At(_index));
	
	const char* theString = "";
	
	switch(_item) {
			// node coordinates
		case 0:
			theString = "X Coordinate";
			break;
		case 1:
			theString = "Y Coordinate";
			break;
			
		case 2:
		case 3:
		case 4:
			// applied load
			theString = theNode.GetLoadTypeForDOF(_item - 2)->name;
			break;
			
		case 5:
		case 6:
		case 7:
			theString = theNode.GetReactionTypeForDOF(_item - 5)->name;
			break;
	}
	
	return NSLocalizedString([NSString stringWithUTF8String: theString], nil);

	// fix me, get the titles of each element
	return @"";
}

//----------------------------------------------------------------------------------------
//  doFormat
//
//      format the value using the units.
//
//  DlFloat64 value    -> the value
//  UnitType units     -> the unit type.
//
//  returns NSString*  <- the value to display.
//----------------------------------------------------------------------------------------
static NSString*
doFormat(DlFloat64 value, UnitType units)
{
	return [NSString stringWithCString: UnitTable::FormatValue(value, units, DlFloatFormat()).get()
							  encoding:NSUTF8StringEncoding];
}

//----------------------------------------------------------------------------------------
//  value
//
//      return the value for this item.
//
//  returns NSString*  <- 
//----------------------------------------------------------------------------------------
- (NSString*) value
{
	// we need: x, y, fx, fy, ftheta or settle in each
	// if analyzed, we need dx, dy, dtheta or rx, ry, rtheta.

	FrameStructure* s = [_doc structure];
	NodeEnumerator nodes(s->GetNodes());
	
	if (_index >= nodes.Length())
		return @"No Node";
	
	Node theNode(nodes.At(_index));

	switch(_item) {
		case 0:
		case 1:
			// node coordinates
		{
			return [NSString stringWithUTF8String: theNode.GetCoord(_item, true)];
		} break;
			
		case 2:
		case 3:
		case 4:
			// applied load
		{
			return [NSString stringWithUTF8String:
					s->GetNodeLoadString(_item - 2, theNode)];
		} break;
			
		case 5:
		case 6:
		case 7:
			// displacement or reactions
		{
			if (s->Analyzed()) {
				int dof = _item - 5;
				const LoadCaseResults* res = s->GetResults();
				DlInt32 eqNum = theNode.GetEquationNumber(dof);
				DlFloat64 val;
				
				if (eqNum > 0) {
					val = res->GetDisplacements()[eqNum - 1];
					
				} else {
					// reaction value
					val = res->GetReactions()[-eqNum - 1];
				}
				
				const PropertyType* type = theNode.GetReactionTypeForDOF(dof);
				
				return doFormat(val, type->units);
			}
		} break;
	}
	
	return @"--";
}

@end
