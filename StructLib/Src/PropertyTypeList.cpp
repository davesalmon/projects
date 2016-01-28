/*+
 *	File:		PropertyTypeList.cpp
 *
 *	Contains:	Implementation of lists of propery types. Each property type is an
 *				object containing the names of material properties and their 
 *				attributes. Each element type provides an associated list of
 *				properties suitable for that element. These properties are 
 *				accessed and assigned by their property names.
 *	
 *
 *	Written by:	David C. Salmon
 *
 *	Copyright:	COPYRIGHT (C) 2000 By David C. Salmon.  *WORLDWIDE RIGHTS RESERVED*
 *
 *	To Do:
-*/
/*
 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation; either version 2 of the License, or
 (at your option) any later version.
 
 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.
 
 You should have received a copy of the GNU General Public License along
 with this program; if not, write to the Free Software Foundation, Inc.,
 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

//---------------------------------- Includes ----------------------------------

#include "DlPlatform.h"
#include "PropertyTypeList.h"
#include "PropertyFactory.h"

//#include "ValueIter.h"
#include "DlString.h"
#include <algorithm>

#include "StrInputStream.h"
#include "StrOutputStream.h"
#include "StrErrCode.h"

//---------------------------------- Class -------------------------------------

//---------------------------------- Methods -----------------------------------

class AddMissing {
	
public:
	
	AddMissing(PropertyTypeList& l) : _l(l) {}
	
	void operator() (const PropertyType*p, DlInt32)
	{
		auto curr = _l._propertyIndexes.find(p->name);
		
		if (curr == _l._propertyIndexes.end())
			_l.Add(p);
#if DlDebugging
		else {
			// check to make sure that data types are the same
			_DlAssert(_l.GetDataType(curr->second) == p->dataType);
		}
#endif
	}
	
	PropertyTypeList& _l;
};

//----------------------------------------------------------------------------------------
//  PropertyTypeList::Add
//
//      add property type to the merge list containing all property names.
//
//  const PropertyType* p  -> 
//
//  returns nothing
//----------------------------------------------------------------------------------------
void
PropertyTypeList::Add(const PropertyType* p)
{
	EnumeratorImp<const PropertyType, const PropertyType*>::Add(p);
	_propertyIndexes[p->name] = Length() - 1;
}

//----------------------------------------------------------------------------------------
//  PropertyTypeList::Merge
//
//      mege the given list into this list.
//
//  const PropertyTypeList& l  -> 
//
//  returns nothing
//----------------------------------------------------------------------------------------
void
PropertyTypeList::Merge(const PropertyTypeList& l)
{
	AddMissing a(*this);
	l.Foreach(a);
}

//------------------------------------------------------------------------------
//	PropertyTypeList::findIndex
//
//		return the index of s or -1 if not found. This uses binary search to 
//		locate the index.
//
//	s				->	string to find
//	returns			<-	offset of string
//------------------------------------------------------------------------------
DlInt32 
PropertyTypeList::FindIndex(const char* s) const
{
	if (s) {
		auto val = _propertyIndexes.find(s);
		if (val != _propertyIndexes.end()) {
			return val->second;
		}
	}
	
	return kPropertyNotFound;
}

//----------------------------------------------------------------------------------------
//  PropertyTypeList::GetUnits
//
//      return the units associated with the property at index in the list.
//
//  DlInt32 index      -> 
//
//  returns UnitType   <- 
//----------------------------------------------------------------------------------------
UnitType
PropertyTypeList::GetUnits(DlInt32 index) const
{
	return ElementAt(index)->units;
}

//----------------------------------------------------------------------------------------
//  PropertyTypeList::GetUnits
//
//      return the units associated with the give name.
//
//  const char* name   -> 
//
//  returns UnitType   <- 
//----------------------------------------------------------------------------------------
UnitType
PropertyTypeList::GetUnits(const char* name) const
{
	return ElementAt(FindIndex(name))->units;
}


//----------------------------------------------------------------------------------------
//  PropertyTypeList::GetDataType
//
//      return the data type associated with the property at index in the list.
//
//  DlInt32 index          -> 
//
//  returns PropDataType   <- 
//----------------------------------------------------------------------------------------
PropDataType
PropertyTypeList::GetDataType(DlInt32 index) const
{
	return ElementAt(index)->dataType;
}

//----------------------------------------------------------------------------------------
//  PropertyTypeList::GetDataType
//
//      return the data type associated with the give name.
//
//  const char* name       -> 
//
//  returns PropDataType   <- 
//----------------------------------------------------------------------------------------
PropDataType
PropertyTypeList::GetDataType(const char* name) const
{
	return ElementAt(FindIndex(name))->dataType;
}

//----------------------------------------------------------------------------------------
//  PropertyTypeList::GetEditable
//
//      return editable attribute associated with the property at index in the list.
//
//  DlInt32 index  -> 
//
//  returns bool   <- 
//----------------------------------------------------------------------------------------
bool
PropertyTypeList::GetEditable(DlInt32 index) const
{
	return ElementAt(index)->editable;
}

//----------------------------------------------------------------------------------------
//  PropertyTypeList::GetEditable
//
//      return editable attribute associated with the give name.
//
//  const char* name   -> 
//
//  returns bool       <- 
//----------------------------------------------------------------------------------------
bool
PropertyTypeList::GetEditable(const char* name) const
{
	return ElementAt(FindIndex(name))->editable;
}

//----------------------------------------------------------------------------------------
//  PropertyTypeList::IsAssignableToElement
//
//      return true if this property can be assigned to the specified element type.
//
//  const char* elementId  -> 
//
//  returns bool           <- 
//----------------------------------------------------------------------------------------
bool
PropertyTypeList::IsAssignableToElement(const char* elementId) const
{
	return PropertyFactory::sPropertyFactory.IsPropertyAssignable(elementId, this);
}

//----------------------------------------------------------------------------------------
//  PropertyTypeList::IsAssignableAs
//
//      return true if the this property list is assignment compatible with otherList.
//
//  const PropertyTypeList* otherList  -> 
//
//  returns bool                       <- 
//----------------------------------------------------------------------------------------
bool
PropertyTypeList::IsAssignableAs(const PropertyTypeList* otherList) const
{
	// this is assignable if each of its properties can be used as a replacement for
	// an element in otherList.
	Reset();
	while (HasMore()) {
		// the property to test.
		const PropertyType* prop = Next();
		
		// search this list to find a compatible property for otherList.
		bool found = false;
		otherList->Reset();
		while (otherList->HasMore()) {
			auto p = otherList->Next();
			if (p->IsAssignableWith(prop)) {
				found = true;
				break;
			}
		}
		
		if (!found) {
			printf("%s property not compatible\n", prop->name);
			return false;
		}
	}
	
	return true;
}


//----------------------------------------------------------------------------------------
//  PropertyTypeList::GetListID
//
//      return the identifier for the list.
//
//  returns DlUInt32   <- the identifier.
//----------------------------------------------------------------------------------------
DlUInt32
PropertyTypeList::GetListID() const
{
	return PropertyTypeListID;
}

//	eof
