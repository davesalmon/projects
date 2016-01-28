/*+
 *	File:		PropertyTypeList.h
 *
 *	Contains:	Interface for property type lists
 *	
 *	List of string/unit pairs.
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

#ifndef _H_PropertyTypeList
#define _H_PropertyTypeList

//---------------------------------- Includes ----------------------------------

#include "EnumeratorImp.h"
#include "DlString.h"
#include "UnitTable.h"
#include "Property.h"

//---------------------------------- Class -------------------------------------


class AddMissing;

//	PropetyType is defined in UnitTable.h

class	PropertyTypeList : public EnumeratorImp<const PropertyType, const PropertyType*>
{
public:
	
//	explicit PropertyTypeList() {}
	PropertyTypeList(const PropertyType* s, DlInt32 len) 
	{
		for (DlInt32 i = 0; i < len; i++)
			Add(s + i);
	}
	
	PropertyTypeList(std::initializer_list<const PropertyType*> init)
	{
		for (const auto& o : init)
			Add(o);
	}
	
	void				Add(const PropertyType* p);
	
	// merge the specified list into this one. Used by PropertyFactory to build the
	//	master property types list.
	void				Merge(const PropertyTypeList& l);
	
	//	lookup index from name
	DlInt32				FindIndex(const char* s) const;
	
	//	get unit type and data type from index
	UnitType			GetUnits(DlInt32 index) const;
	UnitType			GetUnits(const char* name) const;
	
	PropDataType		GetDataType(DlInt32 index) const;
	PropDataType		GetDataType(const char* name) const;

	bool				GetEditable(DlInt32 index) const;
	bool				GetEditable(const char* name) const;
	
	bool				IsAssignableToElement(const char* elementId) const;
	
	// test to see if this list could be assigned to an element using
	//	other list.
	bool				IsAssignableAs(const PropertyTypeList* otherList) const;
	
	virtual 			DlUInt32 GetListID() const;

protected:
//	void 				Add(const PropertyType*s);
//	void 				Remove(const DlString& s);

//	void				Read(StrInputStream& inp, DlInt32 count, const frame_data & data);
//	void 				Write(StrOutputStream& out, const frame_data & data) const;

private:

	
	friend class AddMissing;
	
//	BaseConstIter		find(const char* s) const;
//	BaseIter			find(const char* s);
	
	using PropertyTypeMap = std::map<const char*, DlUInt32, DlCStringKeyLess>;
	
	PropertyTypeMap _propertyIndexes;
};



#endif
