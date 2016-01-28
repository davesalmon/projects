/*+
 *	File:		BaseEnumerator.h
 *
 *	Contains:	base object list class to hold data elements.
 *
 *	This class extends vector to store pointers to objects. Methods are provided
 *	for placing, removing and locating these pointers. The ownership of the objects
 *	stored is left to subclasses. These classes are always wrapped in Enumerator objects
 *	which reference count the pointer lists. So, each time a wrapper Enumerator is 
 *	constructed around a particular list, the reference count is increased and each
 *	time it is destructed the reference count is decreased.
 *
 *  When views of the master lists are created (through selection, say) copies of 
 *  these pointers are moved to the view list. If the elements of the view list are
 *	deleted, the objects are removed from the primary list, but kept (for undo) in
 *	the view list. Only when the remove action is finalized are the actual objects
 *	removed.
 *
 *	Written by:	David C. Salmon
 *
 *	Copyright:	COPYRIGHT (C) 2001 By David C. Salmon.  *WORLDWIDE RIGHTS RESERVED*
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

#ifndef _H_BaseEnumerator
#define _H_BaseEnumerator

//---------------------------------- Includes ----------------------------------

#include <vector>
//#include <iterator>
#include <map>
#include "DlTypes.h"
#include "DlBroadcaster.h"
#include "DlAssert.h"

typedef std::vector<void*>	BaseList;
typedef BaseList::iterator BaseIter;
typedef BaseList::const_iterator BaseConstIter;

class BaseEnumerator;

//---------------------------------- Class -------------------------------------

class	BaseEnumerator : public BaseList, public DlBroadcaster
{
typedef std::map<const void*, DlUInt32> PointerMap;
typedef PointerMap::iterator PMIter;
typedef PointerMap::const_iterator PMConstIter;

public:

	virtual DlUInt32 GetListID() const = 0;

//
//	reference counts
//
	//	return true if count is 1
	bool			IsOwner() const;
	//	increment count to denote aliasing
	void			IncReference() const;
	//	decrement the reference for p
	template <class Imp> static void DecReference(Imp*& p);
//
//	iteration
//
	//	return true if there are more elements
	bool			HasMore() const;
	//	return the number of elements
	DlInt32 		Length() const;
	//	reset the iterator to start
	void			Reset() const;
	
protected:
	// all operations through void* must be proxied.
	
	//	remove elements from enumerator and add to this
	void			MoveIn(BaseEnumerator& enumerator);
	//
	//	list modifiers
	//
	//	add an element (sorted by address) return true if added
	bool			Add(void* elem);
	//	add a list of elements using add
	bool			Add(const BaseEnumerator* list);
	//	remove matching elements
	void			Remove(const BaseEnumerator* list);
	//	remove a single element
	void			Remove(void* elem, bool remap = true);
//
//	lookup
//
	//	return true if the list contains elem
	bool			Contains(const void* elem) const;
	//	return an iterator locating elem
	BaseIter		Locate(const void* elem);
	//	return const iterator locating elem
	BaseConstIter	Locate(const void* elem) const;
	//	return the offset of elem in the list
	DlInt32			IndexOf(const void* elem) const;
	void*			ElementAt(DlInt32 index) const;

//	void			Sort();
//	void			Unsort() {_sorted = false;}

private:
	BaseEnumerator& operator=(const BaseEnumerator& e);

	void rebuildMap();
	void adjustMapIndices(DlUInt32 removed);

protected:
	BaseEnumerator(const BaseEnumerator& e);
	explicit BaseEnumerator();
	~BaseEnumerator();

	PointerMap				_map;
	mutable DlInt32			_count;
	mutable BaseConstIter 	_iter;
};

//---------------------------------- Inlines -----------------------------------

//------------------------------------------------------------------------------
//	BaseEnumerator::decReference
//
//		decrement the reference count belonging to p and delete p if necessary
//
//	p			->	reference to pointer to delete if necessary
//------------------------------------------------------------------------------
template <class Imp> inline 
void
BaseEnumerator::DecReference(Imp*& p)
{
	_DlAssert(p->_count != 0);
	if (--p->_count == 0)
	{
		delete p;
		p = 0;
	}
}

#endif


//eof

