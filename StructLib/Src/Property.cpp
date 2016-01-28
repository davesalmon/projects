/*+
 *	File:		Property.cpp
 *
 *	Contains:	Interface for Property
 *	
 *	Property elements contain the defintions for element properties.
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
#include "Property.h"
#include "StrErrCode.h"
#include "PropertyImp.h"
#include "DlStringUtil.h"
#include "DlAssert.h"
#include "DlString.h"
#include "PropertyFactory.h"

#include <stdlib.h>
//---------------------------------- Class -------------------------------------

//----------------------------------------------------------------------------------------
//  Property::GetValue
//
//      return the value for the specified property.
//
//  const char* name   -> the property name.
//  bool wantUnits     -> the value to set.
//
//	throws int
//
//  returns nothing
//----------------------------------------------------------------------------------------
void
Property::SetValue(const char* name, const char* val)
{
	if (!imp->SetValue(name, val))
		throw DlException("No property found with the name \"%s\".", name);
}

//----------------------------------------------------------------------------------------
//  Property::GetValue
//
//      return the value for the specified property.
//
//  const char* name   -> the property name.
//  bool wantUnits     -> true to add units to the string.
//
//  returns DlString   <- the value
//----------------------------------------------------------------------------------------
DlString
Property::GetValue(const char* name, bool wantUnits) const
{
	return imp->GetValue(name, wantUnits);
}

// return the value as a bool
//----------------------------------------------------------------------------------------
//  Property::GetBoolValue
//
//      return the value as boolean for the specified property.
//
//  const char* name   -> return the value for this property as boolean.
//
//  returns bool       <- value
//----------------------------------------------------------------------------------------
bool
Property::GetBoolValue(const char* name) const
{
	return imp->GetBoolValue(name);
}

//----------------------------------------------------------------------------------------
//  Property::IsEditable
//
//      return if the property specified is editable.
//
//  const char* name   -> the property name
//
//  returns bool       <- true if this property can be changed.
//----------------------------------------------------------------------------------------
bool
Property::IsEditable(const char* name) const
{
	return imp->IsEditable(name);
}

//----------------------------------------------------------------------------------------
//  Property::GetTitle
//
//      return the title for this property object.
//
//  returns const char*    <- the title.
//----------------------------------------------------------------------------------------
const char*
Property::GetTitle() const
{
//	if (imp)
		return imp->getTitle();
//	return "";
}
//----------------------------------------------------------------------------------------
//  Property::SetTitle
//
//      set the title for this property object.
//
//  const char* title  -> the title
//
//  returns nothing
//----------------------------------------------------------------------------------------
void
Property::SetTitle(const char* title)
{
	imp->setTitle(title);
}

//----------------------------------------------------------------------------------------
//  Property::CanAssignToElement
//
//      return true if this property can be assigned to the specied element type.
//
//  const char* elementType    -> the element
//
//  returns bool               <- true if can assign.
//----------------------------------------------------------------------------------------
bool
Property::CanAssignToElement(const char* elementType) const
{
	return imp->CanAssignToElement(elementType);
}

//----------------------------------------------------------------------------------------
//  PropertyType::IsAssignableWith
//
//      return true if an element declaring this property can be assigned using the
//		specified property instead.
//
//  const struct PropertyType* prop    -> the property to assign.
//
//  returns bool                       <- true if prop could be used.
//----------------------------------------------------------------------------------------
bool
PropertyType::IsAssignableWith(const struct PropertyType* prop) const
{
	// return true if an element with this property could use prop
	// instead.

	if (dataType != prop->dataType)
		return false;
	
	if (units != prop->units)
		return false;
	
	// if we can be edit and prop cannot, then prop is assignable. The converse is not true.
	if (editable && !prop->editable)
		return false;
	
	return DlStrCmp(name, prop->name) == 0;
}

//	eof
