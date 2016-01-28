/*+
 *	File:		PropertyList.cpp
 *
 *	Contains:	Interface for Property enumerator
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
#include "PropertyList.h"
#include "PropertyFactory.h"
#include "StrInputStream.h"
#include "StrOutputStream.h"
#include "StrErrCode.h"
#include "StrMessage.h"

//---------------------------------- Class -------------------------------------
//
//	MatchPropertyType
//
//		build a list of properties that have the same names.
//
class MatchPropertyType
{
public:
	MatchPropertyType(const PropertyTypeList * e)
		: itsEnum(e) {}
	
	void operator() (const PropertyImp * p, DlInt32) const
	{
		if (*p==itsEnum)
			PropertyList::GetList(const_cast<PropertyEnumerator&>(itsList))->Add(const_cast<PropertyImp *>(p));
	}
	
	const PropertyTypeList* itsEnum;
	PropertyEnumerator itsList;
private:
	MatchPropertyType(const MatchPropertyType& m);
    MatchPropertyType& operator=(const MatchPropertyType& m);
};

//---------------------------------- Methods -----------------------------------

//------------------------------------------------------------------------------
//	PropertyList::PropertyList				constructor
//
//		construct property enumerator
//
//	size					->	the number of initial elements
//------------------------------------------------------------------------------
PropertyList::PropertyList() 
{
}

DlUInt32
PropertyList::GetListID() const
{
	return PropertyListID;
}

//------------------------------------------------------------------------------
//	PropertyList::findByType
//
//		return a list of properties compatible with elements of type type.
//
//	elementType				->	the element type to match
//	returns					<-	list that matches type
//------------------------------------------------------------------------------
PropertyEnumerator 
PropertyList::FindCompatibleForElement(const char* elementType) const
{
	PropertyEnumerator props;
	Reset();
	while(HasMore()) {
		const PropertyImp* p = Next();
		if (PropertyFactory::sPropertyFactory
				.IsPropertyAssignable(elementType, p->GetPropertyTypes())) {
			props.Add(const_cast<PropertyImp*>(p));
		}
	}
	
	return props;
}

Property
PropertyList::FindPropertyForElement(const char* elementType) const
{
	Reset();
	while(HasMore()) {
		const PropertyImp* p = Next();
		if (p->IsElementType(elementType))
			return Property(const_cast<PropertyImp*>(p));
	}
	
	return FindCompatibleForElement(elementType).At(0);
}

bool
PropertyList::HasPropertyForElement(const char* elementType) const
{
	Reset();
	while(HasMore()) {
		const PropertyImp* p = Next();
		if (p->IsElementType(elementType))
			return true;
	}
	
	return false;
}

PropertyImp*
PropertyList::FindMatchingProperty(const PropertyImp* prop) const
{
	Reset();
	while(HasMore()) {
		const PropertyImp* p = Next();
		if (p == prop || *p == *prop)
			return const_cast<PropertyImp*>(p);
	}

	return nullptr;
}

PropertyImp*
PropertyList::ReadOne(StrInputStream& inp, const frame_data& data) const
{
	DlString elemName(inp.GetString());
	std::auto_ptr<PropertyImp> prop(PropertyFactory::sPropertyFactory.CreateProperty("", elemName));
	if (!prop.get())
		throw DlException("Unknown element type \"%s\".", elemName.get());
	prop->Read(inp, data);
	return prop.release();
}

void
PropertyList::WriteOne(StrOutputStream& out, const frame_data& data, const PropertyImp* p) const
{
	const char* elemName = p->GetAssociatedElementType();
	_DlAssert(elemName != 0);
	out.PutString(elemName);
	p->Write(out, data);
}


void
PropertyList::Read(StrInputStream& inp, DlInt32 count, const frame_data& data)
{
	for (DlInt32 i = 0; i < count; i++) {
		Add(ReadOne(inp, data));
	}
	Reset();
}

void
PropertyList::Write(StrOutputStream & out, const frame_data& data) const
{
	Reset();
	for (DlInt32 i = 0; i < Length(); i++)
	{
		WriteOne(out, data, Next());
	}
}


//	eof