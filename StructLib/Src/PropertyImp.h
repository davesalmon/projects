/*+
 *	File:		PropertyImp.h
 *
 *	Contains:	Interface for properties
 *	
 *	Properties are an associative array of property names and values. All values
 *	are doubles.
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

#ifndef _H_PropertyImp
#define _H_PropertyImp

#include "DlAssert.h"
#include "PropertyTypeList.h"
#include "UnitTable.h"
#include <memory>
#include <valarray>

//---------------------------------- Class -------------------------------------

class StrInputStream;
class StrOutputStream;
class frame_data;

typedef union {
	DlFloat32	floatValue;
	DlInt32		intValue;
	DlBoolean	boolValue;
} PropertyValue;

class	PropertyImp
{
	friend class PropertyList;
//	friend class ElementImp;
	friend bool operator==(const PropertyTypeList * e, const PropertyImp & p);
	
public:
	PropertyImp(const char* title, const PropertyTypeList * propNames);
	PropertyImp(const PropertyImp& cloneMe);
    PropertyImp& operator=(const PropertyImp& cloneMe);

	const char* getTitle() const { return _title.c_str(); }
	void setTitle(const char* s) { _title = std::string(s); }
	
	const std::string& getTitleString() const { return _title; }
		
	bool		SetValue(const char* name, const char* value);

	DlString	GetValue(const char* name, bool wantUnits) const;
	
	// return the value as a bool
	bool		GetBoolValue(const char* name) const;
	
	bool		IsEditable(const char* name) const;

	void Read(StrInputStream& strm, const frame_data& data);
	void Write(StrOutputStream& strm, const frame_data& data) const;

	bool	operator==(const PropertyTypeList * e) const;
	bool	operator==(const PropertyImp& p) const;

	bool		CanAssignToElement(const char* elementType);

	DlFloat64	GetFloatValue(DlInt32 index) const;
	DlInt32		GetIntValue(DlInt32 index) const;
	DlBoolean	GetBoolValue(DlInt32 index) const;

	DlBoolean	IsEditable(DlInt32 index) const;
	
	void		SetFloatValue(DlInt32 index, DlFloat64 val);
	void		SetIntValue(DlInt32 index, DlInt32 val);
	void		SetBoolValue(DlInt32 index, DlBoolean val);

	const char* GetAssociatedElementType() const;
	bool		IsElementType(const char* elementType) const;
	
private:
	DlInt32		GetIndex(const char* id) const;
	
	const PropertyType* GetType(DlInt32 index) const;
	

	const PropertyTypeList* GetPropertyTypes() const {  return _propTypes; }

private:
	const PropertyTypeList* 		_propTypes;
	std::valarray<PropertyValue>	_values;
	std::string						_title;
};

inline bool PropertyImp::operator==(const PropertyTypeList * e) const 
{
	return e == _propTypes;
}

inline bool operator==(const PropertyTypeList * e, const PropertyImp & p)
{
	return p._propTypes == e;
}

inline
DlFloat64
PropertyImp::GetFloatValue(DlInt32 index) const
{
	_DlAssert(index >= 0 && index < _propTypes->Length());
	_DlAssert(_propTypes->GetDataType(index) == PropDataFloat);
	
	return _values[index].floatValue;
}

inline
DlInt32
PropertyImp::GetIntValue(DlInt32 index) const
{
	_DlAssert(index >= 0 && index < _propTypes->Length());
	_DlAssert(_propTypes->GetDataType(index) == PropDataInt);
	
	return _values[index].intValue;
}

inline
DlBoolean
PropertyImp::GetBoolValue(DlInt32 index) const
{
	_DlAssert(index >= 0 && index < _propTypes->Length());
	_DlAssert(_propTypes->GetDataType(index) == PropDataBool);
	
	return _values[index].boolValue;
}

inline
DlBoolean
PropertyImp::IsEditable(DlInt32 index) const
{
	_DlAssert(index >= 0 && index < _propTypes->Length());
	return _propTypes->GetEditable(index);
}

inline
void
PropertyImp::SetFloatValue(DlInt32 index, DlFloat64 val)
{
	_DlAssert(index >= 0 && index < _propTypes->Length());
	_DlAssert(_propTypes->GetDataType(index) == PropDataFloat);
	
	_values[index].floatValue = val;
}

inline
void
PropertyImp::SetIntValue(DlInt32 index, DlInt32 val)
{
	_DlAssert(index >= 0 && index < _propTypes->Length());
	_DlAssert(_propTypes->GetDataType(index) == PropDataInt);
	
	_values[index].intValue = val;
}

inline
void
PropertyImp::SetBoolValue(DlInt32 index, DlBoolean val)
{
	_DlAssert(index >= 0 && index < _propTypes->Length());
	_DlAssert(_propTypes->GetDataType(index) == PropDataBool);
	
	_values[index].boolValue = val;
}

inline
bool
PropertyImp::IsElementType(const char* elementType) const
{
	return DlStrCmp(GetAssociatedElementType(), elementType) == 0;
}

#endif

