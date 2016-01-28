/*+
 *	File:		BaseEnumerator.cpp
 *
 *	Contains:	base object list class to hold data elements.
 *
 *	Written by:	David C. Salmon
 *
 *	Copyright:	COPYRIGHT (C) 1990-96 By David C. Salmon.  *WORLDWIDE RIGHTS RESERVED*
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

//--------------------------------------- Includes ---------------------------------------

#include "DlPlatform.h"
#include "BaseEnumerator.h"
//#include "StrMessage.h"

//--------------------------------------- Inlines ----------------------------------------

//--------------------------------------- Methods ----------------------------------------

//----------------------------------------------------------------------------------------
//	BaseEnumerator::moveIn
//
//		splice in the list. Removes the elements from e
//
//	e					->	elements to move in.
//----------------------------------------------------------------------------------------
void
BaseEnumerator::MoveIn(BaseEnumerator& e)
{
	BaseIter pos = e.end();
	while(pos != e.begin())
	{
		pos--;
		Add(*pos);
		
//		SendMessage(MessageListAdd, *pos);
		e.BaseList::erase(pos);
	}
	
	e.rebuildMap();
	Reset();
}

//----------------------------------------------------------------------------------------
//	BaseEnumerator::add
//
//		add an element to the list.
//
//	elem				->	element to add.
//----------------------------------------------------------------------------------------
bool 
BaseEnumerator::Add(void* elem)
{
	PMIter loc = _map.find(elem);
	if (loc == _map.end()) {
		push_back(elem);
		_map[elem] = size() - 1;
		Reset();
		return true;
	}
	return true;
}

//----------------------------------------------------------------------------------------
//	BaseEnumerator::add
//
//		add an element to the list.
//
//	elem				->	element to add.
//----------------------------------------------------------------------------------------
bool 
BaseEnumerator::Add(const BaseEnumerator* list)
{
	bool added = false;
	BaseConstIter i = list->begin();
	while(i != list->end())
	{
		added |= Add(*i);
		i++;
//		SendMessage(MessageListAdd, *i);
	}
	Reset();
	return added;
}

//----------------------------------------------------------------------------------------
//	BaseEnumerator::remove
//
//		remove a single element.
//
//	e					->	element to remove.
//----------------------------------------------------------------------------------------
void 
BaseEnumerator::Remove(void* e, bool remap)
{
	PMIter loc = _map.find(e);
	if (loc != _map.end()) {
		erase(begin() + loc->second);
		if (remap)
			adjustMapIndices(loc->second);
		_map.erase(loc);
//		rebuildMap();
	}
//	removeElem(*this, _sorted, e);
//	SendMessage(MessageListRemove, e);
	Reset();
}

//----------------------------------------------------------------------------------------
//	BaseEnumerator::remove
//
//		remove a list of elements.
//
//	e					->	elements to remove.
//----------------------------------------------------------------------------------------
void 
BaseEnumerator::Remove(const BaseEnumerator* e)
{
	BaseIter	pos = begin();
	while(pos != end())
	{
		if (e->Contains(*pos)) {
			pos = erase(pos);
		} else {
			pos++;
		}
//		Remove(*pos, false);
//		SendMessage(MessageListRemove, *pos);
//		pos++;
	}
	
	rebuildMap();
	Reset();
}

	
//----------------------------------------------------------------------------------------
//	BaseEnumerator::isOwner
//
//		return true if this is the only copy.
//
//	returns				<-	true if this is the only reference to the list.
//----------------------------------------------------------------------------------------
bool 
BaseEnumerator::IsOwner() const
{
	return _count == 1;
}

//----------------------------------------------------------------------------------------
//	BaseEnumerator::hasMore
//
//		return true if the enumerator has not reached the end of the list
//
//	returns				<-	true if there are more elements
//----------------------------------------------------------------------------------------
bool 
BaseEnumerator::HasMore() const
{
	return _iter != end();
}

//----------------------------------------------------------------------------------------
//	BaseEnumerator::length
//
//		return the length of the list
//
//	returns				<-	total length of the list
//----------------------------------------------------------------------------------------
DlInt32 
BaseEnumerator::Length() const 
{
	return size();
}

//----------------------------------------------------------------------------------------
//	BaseEnumerator::reset
//
//		reset the iterator
//
//----------------------------------------------------------------------------------------
void 
BaseEnumerator::Reset() const
{
	_iter = begin();
}

//----------------------------------------------------------------------------------------
//	BaseEnumerator::incReference
//
//		const a new void* enumerator
//
//	initElems			->	the initial number of elements in the list.
//----------------------------------------------------------------------------------------
void 
BaseEnumerator::IncReference() const
{
	++_count;
}

//----------------------------------------------------------------------------------------
//	BaseEnumerator::contains
//
//		return true if the list contains the specified element.
//
//	e					->	elements to find.
//	returns				<-	true if the element is in the list
//----------------------------------------------------------------------------------------
bool 
BaseEnumerator::Contains(const void* e) const
{
	return _map.find(e) != _map.end();
}

//----------------------------------------------------------------------------------------
//	BaseEnumerator::locate
//
//		return the location of the element in the list.
//
//	elem				->	elements to locate.
//	returns				<-	iterator to position
//----------------------------------------------------------------------------------------
BaseIter 
BaseEnumerator::Locate(const void* elem) 
{
	PMConstIter i = _map.find(elem);
	if (i != _map.end())
		return begin() + i->second;
	return end();
}

//----------------------------------------------------------------------------------------
//	BaseEnumerator::locate
//
//		return the location of the element in the list.
//
//	elem				->	elements to locate.
//	returns				<-	iterator to position
//----------------------------------------------------------------------------------------
BaseConstIter 
BaseEnumerator::Locate(const void* elem) const
{
	return const_cast<BaseEnumerator*>(this)->Locate(elem);
}

//----------------------------------------------------------------------------------------
//	BaseEnumerator::indexOf
//
//		return the location of the element in the list as an offset from the 
//		first.
//
//	Note: this restricts the base list to vector or deque.
//
//		This is used for matching lists, where the identifier of an element is
//		maintained in one list and the value in a separate list. For properties
//		there is only one name list per element type, but each property object
//		has its own list of values. These values are looked up by name using
//		the name list to find the offset. The value can then be returned.
//
//	elem				->	elements to locate.
//	returns				<-	offset to position
//----------------------------------------------------------------------------------------
DlInt32	
BaseEnumerator::IndexOf(const void* elem) const
{
	PMConstIter i = _map.find(elem);
	if (i != _map.end())
		return i->second;
	return size();
}

//----------------------------------------------------------------------------------------
//  BaseEnumerator::ElementAt
//
//      Return the element at this index.
//
//  DlInt32 index  -> the element index
//
//  returns void*  <- the value
//----------------------------------------------------------------------------------------
void*
BaseEnumerator::ElementAt(DlInt32 index) const
{
	return (*this)[index];
}

//----------------------------------------------------------------------------------------
//	BaseEnumerator::BaseEnumerator									          constructor
//
//		copy the given list
//
//	e				->	the list to copy.
//----------------------------------------------------------------------------------------
BaseEnumerator::BaseEnumerator(const BaseEnumerator& e) 
	: BaseList(e)
	, _count(0)
{
	rebuildMap();
	Reset();
}

//----------------------------------------------------------------------------------------
//	BaseEnumerator::BaseEnumerator									          constructor
//
//		const a new void* enumerator
//
//	initElems			->	the initial number of elements in the list.
//----------------------------------------------------------------------------------------
BaseEnumerator::BaseEnumerator() 
	: _count(0)
	//, _sorted(false)
{
	Reset();
}

//----------------------------------------------------------------------------------------
//	BaseEnumerator::~BaseEnumerator									          destructor
//
//		destroy the list, without erasing the element. To completely destroy
//		call erase
//
//	e				->	the list to copy.
//----------------------------------------------------------------------------------------
BaseEnumerator::~BaseEnumerator()
{
}

//----------------------------------------------------------------------------------------
//  BaseEnumerator::rebuildMap
//
//      rebuild the map between object pointers and index into the vector.
//
//  returns nothing
//----------------------------------------------------------------------------------------
void
BaseEnumerator::rebuildMap() 
{
	_map.clear();
	for (DlUInt32 i = 0; i < size(); ++i)
		_map[operator[](i)] = i;
}

//----------------------------------------------------------------------------------------
//  BaseEnumerator::adjustMapIndices
//
//      adjust the map between pointers and index into the vector to account for remove.
//
//  DlUInt32 removed   -> the index of the removed element.
//
//  returns nothing
//----------------------------------------------------------------------------------------
void 
BaseEnumerator::adjustMapIndices(DlUInt32 removed) 
{
	PMIter i = _map.begin();
	for (; i != _map.end(); i++)
		if (i->second >= removed)
			--i->second;
}

//	eof
