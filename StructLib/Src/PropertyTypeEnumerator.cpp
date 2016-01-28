/*+
 *	File:		PropertyTypeEnumerator.cpp
 *
 *	Contains:	Interface for string lists
 *	
 *	List of strings.
 *
 *	Written by:	David C. Salmon
 *
 *	Copyright:	COPYRIGHT (C) 2000 By David C. Salmon.  *WORLDWIDE RIGHTS RESERVED*
 *
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
#include "PropertyTypeEnumerator.h"
#include "PropertyTypeList.h"
//#include "Enumerator.tem"

//---------------------------------- Methods -----------------------------------

//----------------------------------------------------------------------------------------
//  PropertyTypeEnumerator::PropertyTypeEnumerator                            constructor
//
//      Construct empty property type enumerator.
//
//  returns nothing
//----------------------------------------------------------------------------------------
PropertyTypeEnumerator::PropertyTypeEnumerator()
	: itsList(NEW PropertyTypeList {})
{
	itsList->IncReference();
}

//----------------------------------------------------------------------------------------
//  PropertyTypeEnumerator::PropertyTypeEnumerator                            constructor
//
//      Construct property type enumerator using imp.
//
//  const PropertyTypeList* imp    -> the list implementation.
//
//  returns nothing
//----------------------------------------------------------------------------------------
PropertyTypeEnumerator::PropertyTypeEnumerator(const PropertyTypeList* imp)
	: itsList(const_cast<PropertyTypeList*>(imp))
{
	itsList->IncReference();
}

//----------------------------------------------------------------------------------------
//  PropertyTypeEnumerator::PropertyTypeEnumerator                            constructor
//
//      Construct property type enumerator from another one.
//
//  const PropertyTypeEnumerator& e    -> the enumerator.
//
//  returns nothing
//----------------------------------------------------------------------------------------
PropertyTypeEnumerator::PropertyTypeEnumerator(const PropertyTypeEnumerator& e)
	: itsList(e.itsList)
{
	itsList->IncReference();
}

//----------------------------------------------------------------------------------------
//  PropertyTypeEnumerator::operator=
//
//      Assign property type enumerator from another one.
//
//  const PropertyTypeEnumerator& e    -> the enumerator.
//
//  returns nothing
//----------------------------------------------------------------------------------------
PropertyTypeEnumerator&
PropertyTypeEnumerator::operator=(const PropertyTypeEnumerator& e)
{
	if (this != &e)
	{
		itsList->DecReference(itsList);
		itsList = e.itsList;
		itsList->IncReference();
	}
	return *this;
}

//----------------------------------------------------------------------------------------
//  PropertyTypeEnumerator::~PropertyTypeEnumerator                            destructor
//
//      Destructor.
//
//  returns nothing
//----------------------------------------------------------------------------------------
PropertyTypeEnumerator::~PropertyTypeEnumerator()
{
	itsList->DecReference(itsList);
}

//----------------------------------------------------------------------------------------
//  PropertyTypeEnumerator::HasMore
//
//      true if there are more elements in list.
//
//  returns bool   <- 
//----------------------------------------------------------------------------------------
bool
PropertyTypeEnumerator::HasMore() const
{
	return itsList->HasMore();
}

//----------------------------------------------------------------------------------------
//  PropertyTypeEnumerator::Next
//
//      return the next element.
//
//  returns const PropertyType*    <- 
//----------------------------------------------------------------------------------------
const PropertyType*	
PropertyTypeEnumerator::Next() const
{
	return itsList->Next();
}

//----------------------------------------------------------------------------------------
//  PropertyTypeEnumerator::Reset
//
//      reset the iterator to start.
//
//  returns nothing
//----------------------------------------------------------------------------------------
void
PropertyTypeEnumerator::Reset() const
{
	itsList->Reset();
}

//----------------------------------------------------------------------------------------
//  PropertyTypeEnumerator::Length
//
//      return the number of elements.
//
//  returns DlInt32    <- the number of elements.
//----------------------------------------------------------------------------------------
DlInt32
PropertyTypeEnumerator::Length() const
{
	return itsList->Length();
}

//----------------------------------------------------------------------------------------
//  PropertyTypeEnumerator::At
//
//      return the element at the location.
//
//  DlInt32 index                  -> the location.
//
//  returns const PropertyType*    <- the value.
//----------------------------------------------------------------------------------------
const PropertyType*	
PropertyTypeEnumerator::At(DlInt32 index) const
{
	return itsList->ElementAt(index);	
}

//----------------------------------------------------------------------------------------
//  PropertyTypeEnumerator::IndexOf
//
//      return the index of the matching element.
//
//  const PropertyType* prop   -> the property type to match.
//
//  returns DlInt32            <- the index of the matching property.
//----------------------------------------------------------------------------------------
DlInt32
PropertyTypeEnumerator::IndexOf(const PropertyType* prop) const
{
	return itsList->IndexOf(prop);
}
