/*+
 *	File:		Property.h
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

#ifndef _H_Property
#define _H_Property

#include <string>
#include "UnitTable.h"
#include "PropertyTypeEnumerator.h"

//---------------------------------- Class -------------------------------------

enum {
	PropDataFloat,
	PropDataInt,
	PropDataBool
	};
typedef DlInt32 PropDataType;

typedef struct PropertyType {
	const char* 	name;		//	property name (e.g., "Modulus")
	UnitType		units;		//	associated units
	PropDataType	dataType;	//	associated data type
	bool			editable;	//	true if editable
	
	// return true if an element with this property could use prop
	// instead.
	bool IsAssignableWith(const struct PropertyType* prop) const;
	
} PropertyType;

const int kPropertyNotFound = -1;

class PropertyImp;
class Property 
{
public:

	Property() : imp(0) {}
	Property(PropertyImp* prop) : imp(prop) {}

	operator PropertyImp* () const { return imp; }
	bool 		Empty() const { return imp == 0; }

	//	set the value as a string.
	void		SetValue(const char* name, const char* val);
	
	//	get the value as a string
	DlString	GetValue(const char* name, bool wantUnits) const;
	
	// return the value as a bool
	bool		GetBoolValue(const char* id) const;
	
	const char*	GetTitle() const;
	void		SetTitle(const char* title);
	
	bool 		IsEditable(const char* name) const;
	//PropertyTypeEnumerator GetPropertyNames() const;

	//	return the element type for this property
	//const char* GetElementType() const;
    
	bool		CanAssignToElement(const char* elementType) const;
	
    bool operator==(const Property& prop) const;

private:
	
	PropertyImp* imp;

};

inline bool
Property::operator==(const Property& prop) const
{
	return prop.imp == imp;
}

#endif

//	eof
