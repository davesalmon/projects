/*+
 *	File:		Enumerator.h
 *
 *	Contains:	Enumerator interface template. This defines non-type specific operations
 *				on lists that can be enumerated. Oject is the data object and List is 
 *				a list that can generate or return items of type Object.
 *
 *				List is basically and instance of EnumeratorImp for typename Object.
 *
 *				Each type in instantiated from Enumerator.tem.
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

#ifndef _H_Enumerator
#define _H_Enumerator

#include "DlTypes.h"
#include <initializer_list>

//---------------------------------- Class -------------------------------------

template <typename List, typename Object> class Enumerator 
{
public:
	Enumerator();
	Enumerator(std::initializer_list<Object> init);
	Enumerator(const List* e);
	Enumerator(const Enumerator& e);
	Enumerator& operator=(const Enumerator& e);
	~Enumerator();

	bool			HasMore() const;
	Object 			Next() const;
	void			Reset() const;
	DlInt32 		Length() const;
	Object			At(DlInt32 index) const;

	bool			Add(const Object& n);
	void			Remove(const Object& n);

	bool			Contains(const Object& n);

    List*			GetList() { return itsList; }
    const List*		GetList() const { return itsList; }
    
private:

	void			Clone();
	List* 			itsList;

};

#endif

//eof
