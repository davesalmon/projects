/*+
 *	File:		PropertyImp.cpp
 *
 *	Contains:	Interface for properties
 *	
 *	Properties are an associative array of property names and values. All values
 *	are doubles. Each property object is backed by a type list that defined the 
 *	names and characteristics for each property.
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

#include "DlPlatform.h"
#include "PropertyImp.h"

#include "PropertyTypeList.h"
#include "StrInputStream.h"
#include "StrOutputStream.h"
#include "frame_data.h"
#include "PropertyFactory.h"

//---------------------------------- Class -------------------------------------

//----------------------------------------------------------------------------------------
//  PropertyImp::PropertyImp                                                  constructor
//
//      construct a property implementation.
//
//  const char* title                   -> the title.
//  const PropertyTypeList * propTypes  -> the underlying type list.
//
//  returns nothing
//----------------------------------------------------------------------------------------
PropertyImp::PropertyImp(const char* title, const PropertyTypeList * propTypes)
	: _propTypes(propTypes)
	, _title(title)
{
	_values.resize(propTypes->Length());
	
	for (int i = 0; i < propTypes->Length(); i++) {
		switch (propTypes->GetDataType(i))
		{
			case PropDataFloat:
				SetFloatValue(i, 1.0);
				break;
			case PropDataInt:
				SetIntValue(i, 1);
				break;
			case PropDataBool:
				// TODO: rethink defaults.
				SetBoolValue(i, !propTypes->GetEditable(i));
				break;
		}
	}
}

//----------------------------------------------------------------------------------------
//  PropertyImp::PropertyImp                                                  constructor
//
//      construct a property implementation.
//
//  const PropertyImp& cloneMe -> 
//
//  returns nothing
//----------------------------------------------------------------------------------------
PropertyImp::PropertyImp(const PropertyImp& cloneMe)
	: _propTypes(cloneMe.GetPropertyTypes())
	, _values(cloneMe._values)
	, _title(cloneMe._title)
{
}

//----------------------------------------------------------------------------------------
//  PropertyImp::operator=
//
//      assign a property implementation to this.
//
//  const PropertyImp& cloneMe -> the property to assign
//
//  returns PropertyImp&       <- reference to this.
//----------------------------------------------------------------------------------------
PropertyImp&
PropertyImp::operator=(const PropertyImp& cloneMe)
{
    if (this != &cloneMe)
    {
        _propTypes = cloneMe.GetPropertyTypes();
		_values = cloneMe._values;
		_title = cloneMe._title;
    }
    return *this;
}

//----------------------------------------------------------------------------------------
//  PropertyImp::operator==
//
//      return true if p is equal to this.
//
//  const PropertyImp& p       -> the property to check
//
//  returns bool               <- true if equal.
//----------------------------------------------------------------------------------------
bool
PropertyImp::operator==(const PropertyImp& p) const
{
	if (!(_title == p._title))
		return false;
	
	if (_propTypes->Length() != p._propTypes->Length()) {
		return false;
	}
	
	for (int i = 0; i < _propTypes->Length(); i++) {
		const PropertyType* pType = GetType(i);
		if (pType != p.GetType(i))
			return false;
		
		switch(pType->dataType) {
			case PropDataFloat:
				if (_values[i].floatValue != p._values[i].floatValue)
					return false;
				break;
			case PropDataInt:
				if (_values[i].intValue != p._values[i].intValue)
					return false;
				break;
			case PropDataBool:
				if (_values[i].boolValue != p._values[i].boolValue)
				break;
		}
	}
	
	return true;
}


//----------------------------------------------------------------------------------------
//  PropertyImp::GetIndex                                                          inline
//
//      return the index.
//
//  const char* id     -> the property type id.
//
//  returns DlInt32    <- the index into the defintion list.
//----------------------------------------------------------------------------------------
inline
DlInt32
PropertyImp::GetIndex(const char* id) const
{
	return _propTypes->FindIndex(id);
}

//----------------------------------------------------------------------------------------
//  PropertyImp::GetType                                                           inline
//
//      return the type.
//
//  DlInt32 index                  -> the property index.
//
//  returns const PropertyType*    <- the property type definition.
//----------------------------------------------------------------------------------------
inline
const PropertyType*
PropertyImp::GetType(DlInt32 index) const
{
	return _propTypes->ElementAt(index);
}

//----------------------------------------------------------------------------------------
//  PropertyImp::SetValue
//
//      set a new value.
//
//  const char* name   -> the id.
//  const char* value  -> the value with or without units.
//
//  returns bool       <- true if we could set it.
//----------------------------------------------------------------------------------------
bool
PropertyImp::SetValue(const char* name, const char* value)
{
	if (name == nullptr)
		setTitle(value);
	else {
		DlInt32 index = GetIndex(name);
		if (index == kPropertyNotFound)
			return false;
		
		const PropertyType* pType = GetType(index);
		
		switch(pType->dataType) {
			case PropDataFloat:
				SetFloatValue(index, UnitTable::ParseValue(value, pType->units));
				break;
			case PropDataInt:
				SetIntValue(index, UnitTable::ParseIntValue(value));
				break;
			case PropDataBool:
				SetBoolValue(index, UnitTable::ParseBoolValue(value));
				break;
			default:
				_DlAssert("bad property data type value" == NULL);
				return false;
		}
	}
	return true;
}

//----------------------------------------------------------------------------------------
//  PropertyImp::GetValue
//
//      return the value.
//
//  const char* id     -> the property id.
//  bool wantUnits     -> true to include units.
//
//  returns DlString   <- the value as string.
//----------------------------------------------------------------------------------------
DlString
PropertyImp::GetValue(const char* id, bool wantUnits) const
{
	if (id == nullptr)
		return getTitle();
	
	DlInt32 index = GetIndex(id);
	if (index == kPropertyNotFound)
		return "-";
	
	const PropertyType* dataType = GetType(index);
	
	switch (dataType->dataType) {
		case PropDataFloat:
			return UnitTable::FormatValue(GetFloatValue(index), dataType->units,
										  DlFloatFormat(4, 0, DlFloatFormatType::Fit), wantUnits);
			break;
		case PropDataInt:
			return DlString((long)GetIntValue(index), DlIntFormat(0, DlIntFormatType::Decimal));
		case PropDataBool:
			return DlString(GetBoolValue(index) ? "true" : "false");
		default:
			_DlAssert("bad property data type value" == NULL);
			break;
	}
	return "";	

}

//----------------------------------------------------------------------------------------
//  PropertyImp::GetBoolValue
//
//      return the value as boolean.
//
//  const char* name   -> the type id.
//
//  returns bool       <- the value of that property as bool.
//----------------------------------------------------------------------------------------
bool
PropertyImp::GetBoolValue(const char* name) const
{
	DlInt32 index = GetIndex(name);
	if (index == kPropertyNotFound)
		return false;
	
	return GetBoolValue(index);
}

//----------------------------------------------------------------------------------------
//  PropertyImp::IsEditable
//
//      return true if the property is editable.
//
//  const char* name   -> the property id.
//
//  returns bool       <- true if that property can be changed.
//----------------------------------------------------------------------------------------
bool
PropertyImp::IsEditable(const char* name) const
{
	DlInt32 index = GetIndex(name);
	if (index == kPropertyNotFound)
		return false;
	
	return IsEditable(index);
}

//----------------------------------------------------------------------------------------
//  PropertyImp::CanAssignToElement
//
//      return true if this property can be assigned to elements of the specified type.
//
//  const char* elementType    -> the element type id.
//
//  returns bool               <- true if this property is compatible.
//----------------------------------------------------------------------------------------
bool
PropertyImp::CanAssignToElement(const char* elementType)
{
	return _propTypes->IsAssignableToElement(elementType);
}

//----------------------------------------------------------------------------------------
//  PropertyImp::GetAssociatedElementType
//
//      return the element type that this property is associated with.
//
//  returns const char*    <- the type id.
//----------------------------------------------------------------------------------------
const char*
PropertyImp::GetAssociatedElementType() const
{
	return PropertyFactory::sPropertyFactory.GetElementForPropertyTypes(_propTypes);
}

//----------------------------------------------------------------------------------------
//  PropertyImp::Read
//
//      read the property from the stream.
//
//  StrInputStream& strm   -> the input stream.
//  const frame_data& d    -> the data.
//
//  returns nothing
//----------------------------------------------------------------------------------------
void
PropertyImp::Read(StrInputStream& strm, const frame_data& d)
{
	if (d.GetVersion() >= kFrameVersionPropertyTitles) {
		_title = strm.GetString();
	} else {
		_title = "no title";
	}
	
	auto valPtr = std::begin(_values);
	for (DlSizeT i = 0; i < _propTypes->Length(); i++, valPtr++)
	{
		switch (_propTypes->GetDataType(i)) {
		case PropDataFloat:
			valPtr->floatValue = strm.GetDouble();
			break;
		case PropDataInt:
			valPtr->intValue = strm.GetInt();
			break;
		case PropDataBool:
			valPtr->boolValue = strm.GetBool();
			break;
		default:
			_DlAssert("invalid property type" == NULL);
			break;
		}
	}
}

//----------------------------------------------------------------------------------------
//  PropertyImp::Write
//
//      write the property to the stream.
//
//  StrOutputStream& strm  -> the output stream.
//  const frame_data&      -> the data store.
//
//  returns nothing
//----------------------------------------------------------------------------------------
void 
PropertyImp::Write(StrOutputStream& strm, const frame_data& ) const
{
	strm.PutString(_title.c_str());
	
	auto valPtr = std::begin(_values);
	for (DlSizeT i = 0; i < _propTypes->Length(); i++, valPtr++)
	{
		switch (_propTypes->GetDataType(i)) {
		case PropDataFloat:
			strm.PutDouble(valPtr->floatValue);
			break;
		case PropDataInt:
			strm.PutInt(valPtr->intValue);
			break;
		case PropDataBool:
			strm.PutBool(valPtr->boolValue);
			break;
		default:
			_DlAssert("invalid property type" == NULL);
			break;
		}
	}
}
