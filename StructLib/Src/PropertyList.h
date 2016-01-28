/*+
 *	File:		PropertyList.h
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

#ifndef _H_PropertyList
#define _H_PropertyList

//---------------------------------- Includes ----------------------------------

#include "EnumeratorImp.h"
#include "PropertyImp.h"
#include "PropertyEnumerator.h"

class	frame_data;
class	StrInputStream;
class	StrOutputStream;
class	Property;

//---------------------------------- Class -------------------------------------

class	PropertyList : public EnumeratorImp<PropertyImp, Property>
{
public:
	PropertyList();

	DlUInt32	GetListID() const;
	//	build the list by reading it
	void			Read(StrInputStream& inp, DlInt32 count, const frame_data& data);
	void 			Write(StrOutputStream & out, const frame_data& data) const;

	PropertyImp*	ReadOne(StrInputStream& inp, const frame_data& data) const;
	void			WriteOne(StrOutputStream& inp, const frame_data& data, const PropertyImp* p) const;
	
	bool					HasPropertyForElement(const char* elementType) const;
	PropertyEnumerator		FindCompatibleForElement(const char* elementType) const;
	Property				FindPropertyForElement(const char* elementType) const;
	
							// return property matching prop or null
	PropertyImp*			FindMatchingProperty(const PropertyImp* prop) const;
//	void					add(Property *n);
//	void					remove(Property *n);
//
	static const PropertyList* GetList(const PropertyEnumerator& l) { return l.GetList(); }
	static PropertyList* GetList(PropertyEnumerator& l) { return l.GetList(); }
};

//---------------------------------- Inlines -----------------------------------

#if 0
inline void PropertyList::add(Property * n) 
{
	EnumeratorImp<PropertyImp>::add(static_cast<PropertyImp*>(n));
}

inline void PropertyList::remove(Property * n)
{
	EnumeratorImp<PropertyImp>::remove(static_cast<PropertyImp*>(n));
}
#endif

#endif

//	eof