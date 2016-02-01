/*+
 *
 *  ElementInspectorTree.mm
 *
 *  Copyright © 2014 David C. Salmon. All Rights Reserved.
 *
 *  handle the element inspector tree data.
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

#import "ElementInspectorTree.h"

#import "FrameDocument.h"
#import "InspectorControllerBase.h"

#include "FrameStructure.h"
#include "Element.h"
#include "ElementLoad.h"

#include "UnitTable.h"
#include "LoadCaseResults.h"

#pragma mark -
#pragma mark ====== ElementItem declaration
#pragma mark -

@class ElementLeaf;

/* --------------------------------------------------------
 * local class ElementItem
 *
 *	track the nodes in the list
 * --------------------------------------------------------*/
@interface ElementItem: NSObject <InspectorOutlineItem>
{
	DlInt32	_index;
	NSMutableArray* _list;
	FrameDocument* _doc;
}

+ (ElementItem*) createItemForIndex: (DlInt32)index
					 andDocument: (FrameDocument*)doc;

- (instancetype) initForIndex: (DlInt32)index
				  andDocument: (FrameDocument*)doc;

- (void) update;

- (Element) element;

- (ElementItem*) objectAtIndex: (DlInt32) index;

@end

#pragma mark -
#pragma mark ====== ElementLeaf declaration
#pragma mark -

/* --------------------------------------------------------
 * local class NodeLeaf
 *
 *	track the node data elements
 * --------------------------------------------------------*/
@interface ElementLeaf: NSObject <InspectorOutlineItem>
{
	DlInt32	_index;
	DlInt32 _item;
	FrameDocument* _doc;
}

+ (ElementLeaf*) createItemForIndex: (DlInt32)index
						  offset: (DlInt32)item
					 andDocument: (FrameDocument*)doc;

- (instancetype) initForIndex: (DlInt32)index
					   offset: (DlInt32)item
				  andDocument: (FrameDocument*)doc;

@end

#pragma mark -
#pragma mark ====== ElementInspectorTree definition
#pragma mark -

/* --------------------------------------------------------
 *  class ElementInspectorTree
 *
 *	track handle the tree of node data points.
 * --------------------------------------------------------*/
@interface ElementInspectorTree ()
{
	DlInt32 _lastChange;
	bool _analyzed;
	FrameDocument* _doc;
	NSMutableArray* _items;
}
@end

@implementation ElementInspectorTree

+ (ElementInspectorTree*) createWithDocument: (FrameDocument*) doc
{
	return [[[ElementInspectorTree alloc] initWithDocument: doc] autorelease];
}

- (instancetype) initWithDocument: (FrameDocument*) doc
{
	return [super initWithDocument: doc];
}

- (id<InspectorOutlineItem>) createItemForIndex: (DlInt32)index andDocument: (FrameDocument*) doc
{
	return [ElementItem createItemForIndex: index andDocument: doc];
}

- (DlInt32) itemCountForDocument: (FrameDocument*) doc;
{
	FrameStructure* s = [doc structure];
	return s->GetElements().Length();
}

@end

#pragma mark -
#pragma mark =============== ElementItem ===============
#pragma mark -

@implementation ElementItem

//----------------------------------------------------------------------------------------
//  createItemForIndex:andStructure:
//
//      Create an top level item for the outline.
//
//  createItemForIndex: DlInt32 index  -> the element index
//  andStructure: FrameStructure* s    -> the structure
//
//  returns ElementItem*               <- the item.
//----------------------------------------------------------------------------------------
+ (ElementItem*) createItemForIndex : (DlInt32) index
						 andDocument: (FrameDocument*)doc
{
	return [[[ElementItem alloc] initForIndex: index andDocument: doc] autorelease];
}

//----------------------------------------------------------------------------------------
//  initForIndex:andStructure:
//
//      Initialize a top level item. This contains all the sub items.
//
//  initForIndex: DlInt32 index        -> the index
//  andStructure: FrameStructure* s    -> the structure
//
//  returns id                         <- me
//----------------------------------------------------------------------------------------
- (id) initForIndex: (DlInt32) index
		andDocument: (FrameDocument*)doc
{
	self = [super init];
	
	_index = index;
	_doc = doc;
	
	[self update];
	return self;
}

//----------------------------------------------------------------------------------------
//  dealloc
//
//      Release stuff for item.
//
//  returns nothing
//----------------------------------------------------------------------------------------
- (void) dealloc
{
	[_list release];
	[super dealloc];
}

//----------------------------------------------------------------------------------------
//  updateWithStructure:
//
//      update this element item.
//
//  updateWithStructure: FrameStructure* s ->
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
	
	// we need: length, I, E, wl, wa, pre, pre
	//	if the structure has been analyzed, we also
	//	need M0, M1, MMax, V0, V1, A0, A1
	//	Lat D0, Lat D0.5, Lat D1
	//  Ax D0, Ax D1
	DlInt32 count = 5;
	
	FrameStructure* s = [_doc structure];
	ElementEnumerator elems(s->GetElements());
	if (_index >= elems.Length())
		return;
	
	PropertyTypeEnumerator props = s->GetPropertyTypes(elems.At(_index).GetType());
	count += props.Length();
	
	if (s->Analyzed())
		count += elems.At(_index).CountResultTypes();
	
	_list = [NSMutableArray arrayWithCapacity: count];
	[_list retain];
	
	for (DlInt32 i = 0; i < count; i++)
	{
		ElementLeaf* obj = [ElementLeaf createItemForIndex: _index offset: i andDocument: _doc];
		[_list addObject: obj];
		//printf("Added sub item with address %4lx and count %u\n", (unsigned long)obj, [obj retainCount]);
	}
}

- (void) updateDocumentSelection
{
	Element e = [self element];
	[_doc selectElement: e withFlags: NSShiftKeyMask];
}

//----------------------------------------------------------------------------------------
//  objectAtIndex:
//
//      return the sub item for the index
//
//  objectAtIndex: DlInt32 index   -> index
//
//  returns SubItem*               <- the item.
//----------------------------------------------------------------------------------------
- (ElementLeaf*) objectAtIndex: (DlInt32) index
{
	return [_list objectAtIndex: index];
}

//----------------------------------------------------------------------------------------
//  index
//
//      return the index for this item.
//
//  returns DlInt32    <- index
//----------------------------------------------------------------------------------------
- (DlInt32) index
{
	return _index;
}

//----------------------------------------------------------------------------------------
//  count
//
//      return the index for this item.
//
//  returns DlInt32    <- index
//----------------------------------------------------------------------------------------
- (DlInt32) count
{
	return [_list count];
}

//----------------------------------------------------------------------------------------
//  expandable
//
//      return true for expandable
//
//  returns BOOL   <- true
//----------------------------------------------------------------------------------------
- (BOOL) expandable
{
	return YES;
}

//----------------------------------------------------------------------------------------
//  element:
//
//      Fetch the Element wrapper for this item.
//
//  element: FrameStructure* s -> the structure
//
//  returns Element            <- the Element for this item.
//----------------------------------------------------------------------------------------
- (Element) element
{
	return [_doc structure]->GetElements().At(_index);
}

//----------------------------------------------------------------------------------------
//  title:
//
//      return the title for this item.
//
//  title: FrameStructure* s   -> unused
//
//  returns NSString*          <- the title
//----------------------------------------------------------------------------------------
- (NSString*) title
{
	return [NSString stringWithFormat: @"Element #%ld", (long)_index + 1];
}

//----------------------------------------------------------------------------------------
//  value:forLoadCase:
//
//      return the value for this item.
//
//  value: FrameStructure* s   -> unused
//  forLoadCase: LoadCase lc   -> unused
//
//  returns NSString*          <- empty
//----------------------------------------------------------------------------------------
- (NSString*) value
{
	return @"";
}
@end

#pragma mark -
#pragma mark =============== SubItem ===============
#pragma mark -

@implementation ElementLeaf

//----------------------------------------------------------------------------------------
//  createItemForIndex:andOffset:
//
//      Create a sub item for displaying various properties. The offset controls which
//		property will be displayed in the cell.
//
//  createItemForIndex: DlInt32 index  -> the element index
//  andOffset: DlInt32 item            -> the item offset.
//
//  returns SubItem*                   <- item
//----------------------------------------------------------------------------------------
+ (ElementLeaf*) createItemForIndex: (DlInt32)index
							 offset: (DlInt32)item
						andDocument: (FrameDocument*) doc
{
	return [[[ElementLeaf alloc] initForIndex: index offset: item andDocument: doc] autorelease];
}

//----------------------------------------------------------------------------------------
//  initForIndex:andOffset:
//
//      Init.
//
//  initForIndex: DlInt32 index    -> element index
//  andOffset: DlInt32 item        -> offset
//
//  returns id                     <- me
//----------------------------------------------------------------------------------------
- (id) initForIndex: (DlInt32)index
			 offset: (DlInt32)item
		andDocument: (FrameDocument*) doc
{
	self = [super init];
	_index = index;
	_item = item;
	_doc = doc;
	
	return self;
}

//----------------------------------------------------------------------------------------
//  index
//
//      return this element index.
//
//  returns DlInt32    <- the index
//----------------------------------------------------------------------------------------
- (DlInt32) index
{
	return _item;
}

//----------------------------------------------------------------------------------------
//  expandable
//
//      return false. not expandable.
//
//  returns BOOL   <- false
//----------------------------------------------------------------------------------------
- (BOOL) expandable
{
	return NO;
}

//----------------------------------------------------------------------------------------
//  item
//
//      return the item offset
//
//  returns DlInt32    <- offset
//----------------------------------------------------------------------------------------
- (DlInt32) count
{
	return 0;
}

- (void) update
{
}

- (void) updateDocumentSelection
{
}


//----------------------------------------------------------------------------------------
//  title:
//
//      Fetch the title for this offset
//
//  title: FrameStructure* s   -> the structure
//
//  returns NSString*          <- the title
//----------------------------------------------------------------------------------------
- (NSString*) title
{
	FrameStructure* s = [_doc structure];
	
	ElementEnumerator elems(s->GetElements());
	if (_index >= elems.Length())
		return @"No Element";
	
	if (_item == 0) {
		return NSLocalizedString(@"Length", nil);
	} else {
		PropertyTypeEnumerator props = s->GetPropertyTypes(elems.At(_index).GetType());
		int curr = _item - 1;
		if (curr < props.Length()) {
			return NSLocalizedString([NSString stringWithCString: props.At(curr)->name
														encoding:NSUTF8StringEncoding], nil);
		}
		
		curr -= props.Length();
		if (curr < ElementLoadCount) {
			switch(curr) {
				case ElementLoadLateral: return NSLocalizedString(@"Lateral Load", nil);
				case ElementLoadAxial: return NSLocalizedString(@"Axial Load", nil);;
				case ElementLoadPreforce: return NSLocalizedString(@"Pre-Force", nil);;
				case ElementLoadPremoment: return NSLocalizedString(@"Pre-Moment", nil);;
				default: return @"";
			}
		} else {
			Element elem = elems.At(_index);
			curr -= ElementLoadCount;
			if (curr < elem.CountResultTypes()) {
				const PropertyType* resType = elem.GetResultType(curr);
#if _DEBUG
				printf("Name is %s\n", resType->name);
#endif
				return NSLocalizedString([NSString stringWithCString: resType->name
															encoding:NSUTF8StringEncoding], nil);
			}
		}
		return @"N/A";
	}
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
//  value:forLoadCase:
//
//      return the value for this item. The item controls which aspect of the element we
//		are looking at.
//
//  value: FrameStructure* s   -> the structure
//  forLoadCase: LoadCase lc   -> the load case
//
//  returns NSString*          <- the value
//----------------------------------------------------------------------------------------
- (NSString*) value
{
	FrameStructure* s = [_doc structure];
	
	ElementEnumerator elems(s->GetElements());
	if (_index >= elems.Length())
		return @"---";
	
	Element elem = elems.At(_index);
	
	if (_item == 0) {
		return doFormat(elem.Length(), UnitsLength);
	} else {
		PropertyTypeEnumerator props = s->GetPropertyTypes(elems.At(_index).GetType());
		int curr = _item - 1;
		if (curr < props.Length()) {
			Property prop = elem.GetProperty();
			const PropertyType* pType = props.At(_item - 1);
			DlString val(prop.GetValue(pType->name, true));
			return [NSString stringWithCString: val.get()
									  encoding:NSUTF8StringEncoding];
		} else {
			curr -= props.Length();
			if (curr < ElementLoadCount) {
				return [NSString stringWithUTF8String:
						s->GetElementLoadString(ElementLoadType(curr), elem)];
			} else {
				curr -= ElementLoadCount;
				if (curr < elem.CountResultTypes()) {
					const LoadCaseResults* results = s->GetResults();
					if (results) {
						ElementForce frc = results->GetElementForce(_index);
						const PropertyType* resType = elem.GetResultType(curr);
						DlFloat64 val = elem.GetResultValue(curr, frc, *results);
						return doFormat(val, resType->units);
					}
				}
			}
		}
	}
	
	return @"---";
}

@end
