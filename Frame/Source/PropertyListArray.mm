/*+
 *
 *  PropertyListArray.mm
 *
 *  Copyright © 2005 David C. Salmon. All Rights Reserved.
 *
 *  Implement data access class for property list.
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

#import "PropertyListArray.h"
#import "FrameDocument.h"
#import "FrameAction.h"
#import "TableColumn.h"

#include "Property.h"
#include "Element.h"
#include "DlString.h"

#include "FrameStructure.h"

@implementation PropertyListArray 

//----------------------------------------------------------------------------------------
//  createWithDocument:
//
//      create the property list for the specified document.
//
//  createWithDocument: FrameDocument* doc -> 
//
//  returns id<ListAccessor>               <- 
//----------------------------------------------------------------------------------------
+ (PropertyListArray*)createWithDocument: (FrameDocument*)doc
{
	return [[[PropertyListArray alloc] initWithDocument: doc] autorelease];
}

//----------------------------------------------------------------------------------------
//  initWithDocument:
//
//      initialize the property list for the specified document.
//
//  initWithDocument: FrameDocument* doc   -> 
//
//  returns id                             <- 
//----------------------------------------------------------------------------------------
- (id) initWithDocument: (FrameDocument*)doc 
{
	_frameDocument = doc;
	FrameStructure* s = [doc structure];
	_props = NEW PropertyEnumerator(s->GetProperties());

	return self;
}

//----------------------------------------------------------------------------------------
//  dealloc
//
//      nuke me.
//
//  returns nothing
//----------------------------------------------------------------------------------------
- (void) dealloc 
{
	delete _props;
    [super dealloc];
}

//----------------------------------------------------------------------------------------
//  numberOfRows
//
//      return the number of rows in the list.
//
//  returns int    <- 
//----------------------------------------------------------------------------------------
- (int) numberOfRows 
{
	return _props->Length();
}

//----------------------------------------------------------------------------------------
//  properties
//
//      return the property types for this panel.
//
//  returns PropertyTypeEnumerator <- 
//----------------------------------------------------------------------------------------
- (PropertyTypeEnumerator) properties
{
	return [_frameDocument structure]->GetMergedPropertyTypes();
}

//----------------------------------------------------------------------------------------
//  getItemAtRow:column:
//
//      return the item at the specified location in the table.
//
//  getItemAtRow: int row  -> 
//  column: int col        -> 
//
//  returns id             <- 
//----------------------------------------------------------------------------------------
- (id) getItemAtRow: (int)row column: (int)col
{
	if (row < _props->Length()) {
				
		Property prop(_props->At(row));
		
		if(col == 0) {
			return [NSString stringWithUTF8String: prop.GetTitle()];
		} else {
			const PropertyType* propType = [self properties].At(col-1);

			if (propType->dataType == PropDataBool)
				return [NSNumber numberWithBool: prop.GetBoolValue(propType->name)];
			else
				return [NSString stringWithCString: prop.GetValue(propType->name, true)
										 encoding : NSUTF8StringEncoding];
		}
	}
	return @"";
}

//----------------------------------------------------------------------------------------
//  setItem:forRow:column:
//
//      set the value for the item at the specified location in the table.
//
//  setItem: id item   -> 
//  forRow: int row    -> 
//  column: int col    -> 
//
//  returns bool       <- 
//----------------------------------------------------------------------------------------
- (bool) setItem: (id)item forRow: (int)row column:(int)col
{
	if (row < _props->Length()) {
		FrameStructure* s = [_frameDocument structure];
		Action* act;

		Property prop(_props->At(row));
		
		if (col == 0) {
			act = s->ChangeProperty(prop, nullptr, [item UTF8String]);
		} else {
			const PropertyType* propType = [self properties].At(col-1);
#if DlDebugging
			NSLog(@"setting property %s to %@", propType->name, item);
#endif
			if (propType->dataType == PropDataBool) {
				act = s->ChangeProperty(prop, propType->name, [[item stringValue] UTF8String]);
			} else {
				act = s->ChangeProperty(prop, propType->name, [item UTF8String]);
			}
		}
		[_frameDocument performAction : [FrameAction create: act]];
		return false;
	} else if (row == _props->Length()) {
		[self createElementWithValue: item forColumn: col];
		[self selectRow:row];
		return true;
	} else {
		NSAssert(false, @"Invalid table row");
		return false;
	}
}

//----------------------------------------------------------------------------------------
//  canEditItemAtRow:column:
//
//      return true if the specified item is editable.
//
//  canEditItemAtRow: int row  -> 
//  column: int col            -> 
//
//  returns bool               <- 
//----------------------------------------------------------------------------------------
- (bool) canEditItemAtRow: (int)row column: (int)col
{
	if (row < _props->Length() && col > 0) {
		
		Property prop(_props->At(row));
		const PropertyType* propType = [self properties].At(col-1);
		return prop.IsEditable(propType->name);
	}
	
	return true;
}

//----------------------------------------------------------------------------------------
//  attachRow:
//
//      attach the specified row to the current selection.
//
//  attachRow: int row -> 
//
//  returns nothing
//----------------------------------------------------------------------------------------
- (void) attachRow: (int)row 
{
	FrameStructure* s = [_frameDocument structure];
	ElementEnumerator* sel = [_frameDocument elementSelection];
	Property prop = _props->At(row);
	
	[_frameDocument performAction :
		[FrameAction create: s->AssignProperties(*sel, prop)
				   withView: [_frameDocument frameView]]];
}


//----------------------------------------------------------------------------------------
//  detach
//
//      detach the specified row from the current selection.
//
//  returns nothing
//----------------------------------------------------------------------------------------
- (void) detach {
	FrameStructure* s = [_frameDocument structure];
	ElementEnumerator* sel = [_frameDocument elementSelection];
	Property prop;
	
	[_frameDocument performAction :
		[FrameAction create: s->AssignProperties(*sel, prop)
				   withView: [_frameDocument frameView]]];
}

//----------------------------------------------------------------------------------------
//  selectRow:
//
//      select the specified row.
//
//  selectRow: int row -> 
//
//  returns nothing
//----------------------------------------------------------------------------------------
- (void) selectRow: (int)row 
{
	// the selected row has changed.
	[_frameDocument setActiveProperty: _props->At(row)];
}

//----------------------------------------------------------------------------------------
//  hasSelection
//
//      return true if there is a selection.
//
//  returns bool   <- 
//----------------------------------------------------------------------------------------
- (bool) hasSelection 
{
	return [_frameDocument elementSelection]->Length() != 0;	
}

//----------------------------------------------------------------------------------------
//  canAttach
//
//      return true if the current property can be attached to any element in the
//      selection.
//
//  returns bool   <- 
//----------------------------------------------------------------------------------------
- (bool) canAttach
{
	// look through the elements in the selection and return true if
	// there is at least one element that can attach the active
	// property and does not already have the active property assigned.
	
	Property prop([_frameDocument structure]->GetActiveProperty());
	
	if (prop) {
	
		ElementEnumerator* elems([_frameDocument elementSelection]);
		for (int i = 0; i < elems->Length(); i++) {
			Element e(elems->At(i));
			
			if (!(e.GetProperty() == prop))
				if (e.CanAssignProperty(prop)) {
					return true;
			}
		}
	}
	
	return false;
}

//----------------------------------------------------------------------------------------
//  canDelete
//
//      return the current property if it can be deleted.
//
//  returns Property   <-
//----------------------------------------------------------------------------------------
- (Property) canDelete
{
	FrameStructure* s = [_frameDocument structure];
	
	if (s->GetProperties().Length() > 1) {
		
		Property active = s->GetActiveProperty();
		ElementEnumerator elems(s->GetAssignedElements(active));
		
		if (elems.Length() == 0)
			return active;
	}

	return Property();
}

//----------------------------------------------------------------------------------------
//  propertyListChanged
//
//      the propety list changed.
//
//  returns nothing
//----------------------------------------------------------------------------------------
- (void) propertyListChanged
{
	// the element type has changed.
	delete _props;
	_props = NEW PropertyEnumerator([_frameDocument structure]->GetProperties());
}

//----------------------------------------------------------------------------------------
//  createElementWithValue:forColumn:
//
//      create a new row.
//
//  createElementWithValue: id item    -> 
//  forColumn: int col                 -> 
//
//  returns nothing
//----------------------------------------------------------------------------------------
- (void) createElementWithValue: (id) item forColumn: (int) col 
{
	FrameStructure* s = [_frameDocument structure];
	Property theProp;
	
	Action* action = s->CreateProperty([NSLocalizedString(@"default", nil) UTF8String], theProp);
	
	if (col == 0) {
		theProp.SetTitle([item UTF8String]);
	} else {
		
		try {
		
			const PropertyType* propType = [self properties].At(col-1);
	#if DlDebugging
			NSLog(@"setting property %s to %@", propType->name, item);
	#endif
			if (propType->dataType == PropDataBool) {
				theProp.SetValue(propType->name, [[item stringValue] UTF8String]);
			} else {
				theProp.SetValue(propType->name, [item UTF8String]);
			}
			
		} catch(DlException& ex) {
			ex.Display();
		}
	}

	[_frameDocument performAction: [FrameAction create: action]];
	
// 	[self propertyListChanged];
}

- (FrameDocument*) frameDocument
{
	return _frameDocument;
}

@end

// eof 

