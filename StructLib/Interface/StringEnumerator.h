/*+
 *	File:		StringEnumerator.h
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

#ifndef _H_StringEnumerator
#define _H_StringEnumerator

//---------------------------------- Includes ----------------------------------

//#include "Enumerator.h"

//---------------------------------- Class -------------------------------------

class StringList;
class DlString;

class	StringEnumerator
{
	friend class StringList;
public:
	StringEnumerator();
	StringEnumerator(const StringList * imp);
	StringEnumerator(const StringEnumerator & e);
	StringEnumerator& operator=(const StringEnumerator&e);
	~StringEnumerator();

	bool				HasMore() const;
	const DlString&		Next() const;
	void				Reset() const;
	DlInt32 			Length() const;
	
//	void				Insert(const DlString & s);

	void				Add(const DlString & n);
	void				Remove(const DlString & n);

    const StringList*	GetList() const { return itsList; }
    StringList* 		GetList() { return itsList; }
    
private:
	void				Clone();
	
	StringList	* itsList;
};

#endif

