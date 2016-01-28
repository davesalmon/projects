/*+
 *	File:		PropertyTypeEnumerator.h
 *
 *	Contains:	Interface for PropertyType lists
 *	
 *	List of Property types.
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

#ifndef _H_PropertyTypeEnumerator
#define _H_PropertyTypeEnumerator

//---------------------------------- Includes ----------------------------------

//	for Property type declatation

//---------------------------------- Class -------------------------------------

// PropertyType is defined in Property.h
class PropertyTypeList;
typedef struct PropertyType	PropertyType;
//class DlString;

class	PropertyTypeEnumerator
{
public:
	PropertyTypeEnumerator();
	PropertyTypeEnumerator(const PropertyTypeList * imp);
	PropertyTypeEnumerator(const PropertyTypeEnumerator & e);
	PropertyTypeEnumerator& operator=(const PropertyTypeEnumerator&e);
	~PropertyTypeEnumerator();

	bool				HasMore() const;
	const PropertyType*	Next() const;
	void				Reset() const;
	DlInt32 			Length() const;
	const PropertyType* At(DlInt32 index) const;
	
	DlInt32				IndexOf(const PropertyType* prop) const;
	
//	void				Insert(const DlString & s);

//	void				Add(const DlString & n);
//	void				Remove(const DlString & n);

	PropertyTypeList*			GetList() { return itsList; }
    const PropertyTypeList*		GetList() const { return itsList; }

private:
	void				Clone();
	
	PropertyTypeList*	itsList;
};

#endif

