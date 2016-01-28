/*+
 *	File:		EnumeratorImp.h
 *
 *	Contains:	Enumerator template class definition.
 *				This is a wrapper class for BaseEnumerator that adds type 
 *				checking and iteration service to the class
 *
 *	Written by:	David C. Salmon
 *
 *	Copyright:	COPYRIGHT (C) 2003 By David C. Salmon.  *WORLDWIDE RIGHTS RESERVED*
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

#ifndef _H_EnumeratorImp
#define _H_EnumeratorImp

//--------------------------------------- Includes ---------------------------------------

#include "BaseEnumerator.h"
#include "DlAssert.h"
#include "StrMessage.h"
#include <memory>

class frame_data;
class StrInputStream;
class StrOutputStream;

//--------------------------------------- Class ------------------------------------------

template <class T, class Pub>
class EnumeratorImp : public BaseEnumerator
{
public:

	explicit EnumeratorImp();
//	EnumeratorImp(const EnumeratorImp & e);

	//	return the next element in the list
	const T*	 	Next() const;
	T*		  		Next();

	//	add element(s)
	bool			Add(T* elem);
	bool			Add(const EnumeratorImp<T, Pub>* list);
	//	insert. dont use if using contains or locate!
//	void			Insert(BaseIter where, T* elem);
	//	remove element(s)
	void			Remove(const EnumeratorImp<T, Pub>* list);
	void			Remove(T* elem);
	//	erase the whole list
	void			Erase();

	bool			Contains(const T* elem) const;
	BaseConstIter	Locate(const T* elem) const;
	BaseIter		Locate(const T* elem);

	DlInt32			IndexOf(const T* elem) const;
	T*				ElementAt(DlInt32 offset) const;

	//	apply operator() of p to each element of the list
	template <class P> void Foreach(P& p);
	template <class P> void Foreach(P& p) const;

	void 			Read(StrInputStream& s, DlInt32 count, const frame_data& data);
	void 			Write(StrOutputStream& s, const frame_data& data) const;

	void			SendMessage(bool isAdd, T* elem);
	void			SendMessage(bool isAdd, const EnumeratorImp<T, Pub>* list);

protected:

};

//--------------------------------------- Inlines ----------------------------------------

//----------------------------------------------------------------------------------------
//	EnumeratorImp<T>::EnumeratorImp									constructor
//
//		const a new T enumerator
//
//	initElems			->	the initial number of elements in the list.
//----------------------------------------------------------------------------------------
template <class T, class Pub> inline
EnumeratorImp<T,Pub>::EnumeratorImp() 
{
}

//----------------------------------------------------------------------------------------
//	EnumeratorImp<T,Pub>::next
//
//		return the next element as const
//
//	returns				<-	pointer to the next element in the list.
//----------------------------------------------------------------------------------------
template <class T, class Pub> inline
const T*
EnumeratorImp<T,Pub>::Next() const
{
	_DlAssert(_iter != end());
	return reinterpret_cast<const T*>(*_iter++);
}

//----------------------------------------------------------------------------------------
//	EnumeratorImp<T,Pub>::next
//
//		return the next element
//
//	returns				<-	pointer to the next element in the list.
//----------------------------------------------------------------------------------------
template <class T, class Pub> inline 
T* 
EnumeratorImp<T,Pub>::Next() 
{
	_DlAssert(_iter != end());
	return reinterpret_cast<T*>(*_iter++);
}

//----------------------------------------------------------------------------------------
//	EnumeratorImp<T,Pub>::add
//
//		add the element to the list
//
//	elem				->	pointer to element to add.
//----------------------------------------------------------------------------------------
template <class T, class Pub> inline 
bool
EnumeratorImp<T,Pub>::Add(T* elem)
{
	bool added = BaseEnumerator::Add((void *)elem);
	if (added)
		SendMessage(true, elem);
	return added;
}

//----------------------------------------------------------------------------------------
//	EnumeratorImp<T,Pub>::add
//
//		add the element to the list
//
//	elem				->	pointer to element to add.
//----------------------------------------------------------------------------------------
template <class T, class Pub> inline 
bool 
EnumeratorImp<T,Pub>::Add(const EnumeratorImp<T,Pub>* list)
{
	bool added = BaseEnumerator::Add(list);
	if (added)
		SendMessage(true, list);
	return added;
}

//----------------------------------------------------------------------------------------
//	EnumeratorImp<T,Pub>::insert
//
//		insert the element into the list
//
//	where				->	insert location.
//	elem				->	pointer to element to insert.
//----------------------------------------------------------------------------------------
//template <class T, class Pub> inline 
//void 
//EnumeratorImp<T,Pub>::Insert(BaseIter where, T* elem)
//{
//	BaseEnumerator::Insert(where, (void *)elem);
//	SendMessage(true, elem);
//}

//----------------------------------------------------------------------------------------
//	EnumeratorImp<T,Pub>::remove
//
//		remove the list of elements from this list
//
//	list				->	list of elements to remove.
//----------------------------------------------------------------------------------------
template <class T, class Pub> inline 
void 
EnumeratorImp<T,Pub>::Remove(const EnumeratorImp<T,Pub>* list)
{
	BaseEnumerator::Remove(list);
	SendMessage(false, list);
}

//----------------------------------------------------------------------------------------
//	EnumeratorImp<T,Pub>::remove
//
//		remove a single elements from this list
//
//	list				->	list of elements to remove.
//----------------------------------------------------------------------------------------
template <class T, class Pub> inline
void 
EnumeratorImp<T,Pub>::Remove(T* elem)
{
	BaseEnumerator::Remove((void *)elem);
	SendMessage(false, elem);
}

//----------------------------------------------------------------------------------------
//	EnumeratorImp<T,Pub>::contains
//
//		return true if the specified element is in the list
//
//	elem				->	element to test for.
//	returns				<-	true if element is in list
//----------------------------------------------------------------------------------------
template <class T, class Pub> inline 
bool 
EnumeratorImp<T,Pub>::Contains(const T* elem) const
{
	return BaseEnumerator::Contains(static_cast<const void *>(elem));
}

//----------------------------------------------------------------------------------------
//	EnumeratorImp<T,Pub>::locate
//
//		return true if the specified element is in the list
//
//	elem				->	element to test for.
//	returns				<- 	iterator for elem or end()
//----------------------------------------------------------------------------------------
template <class T, class Pub> inline 
BaseConstIter
EnumeratorImp<T,Pub>::Locate(const T* elem) const
{
	return BaseEnumerator::Locate(static_cast<const void *>(elem));
}

//----------------------------------------------------------------------------------------
//	EnumeratorImp<T,Pub>::locate
//
//		return true if the specified element is in the list
//
//	elem				->	element to test for.
//	returns				<- 	iterator for elem or end()
//----------------------------------------------------------------------------------------
template <class T, class Pub> inline 
BaseIter
EnumeratorImp<T,Pub>::Locate(const T* elem)
{
	return BaseEnumerator::Locate(static_cast<const void *>(elem));
}

//----------------------------------------------------------------------------------------
//  IndexOf
//
//      return the index of the specified element.
//
//  const T* elem       -> the element
//
//  returns DlInt32     <- 
//----------------------------------------------------------------------------------------
template <class T, class Pub> inline 
DlInt32
EnumeratorImp<T,Pub>::IndexOf(const T* elem) const
{
	return BaseEnumerator::IndexOf(elem);
}

//----------------------------------------------------------------------------------------
//  EnumeratorImp<T,Pub>::ElementAt
//
//      return the element at the specified index.
//
//  DlInt32 offset      -> the offset
//
//  returns T*          <- the element at offset.
//----------------------------------------------------------------------------------------
template <class T, class Pub> inline 
T*
EnumeratorImp<T,Pub>::ElementAt(DlInt32 offset) const
{
	return (T*)BaseEnumerator::ElementAt(offset);
}

//----------------------------------------------------------------------------------------
//	EnumeratorImp<T,Pub>::erase
//
//		erase all the elements in the list, deleting them
//
//----------------------------------------------------------------------------------------
template <class T, class Pub> inline 
void 
EnumeratorImp<T,Pub>::Erase()
{
	Reset();
	while(HasMore()) {
		delete Next();
	}
	clear();
    _map.clear();
	Reset();
}

//----------------------------------------------------------------------------------------
//	EnumeratorImp<T,Pub>::foreach
//
//		apply p(const T*) to each element
//
//	p			->	operator object to apply to p
//----------------------------------------------------------------------------------------
template <class T, class Pub> template <class P> inline
void 
EnumeratorImp<T,Pub>::Foreach(P& p) const
{
	Reset();
	DlInt32 index = 0;
	while(HasMore())
		p(Next(), index++);
}

//----------------------------------------------------------------------------------------
//	EnumeratorImp<T,Pub>::foreach
//
//		apply p(T*) to each element
//
//	p			->	operator object to apply to p
//----------------------------------------------------------------------------------------
template <class T, class Pub> template <class P> inline
void 
EnumeratorImp<T,Pub>::Foreach(P& p)
{
	Reset();
	DlInt32 index = 0;
	while(HasMore())
		p(Next(), index++);
}

//----------------------------------------------------------------------------------------
//  EnumeratorImp<T,Pub>::Read
//
//      read the list, creating and reading elements.
//
//  StrInputStream& s        -> stream to read from.
//  DlInt32 count            -> number of elements to read.
//  const frame_data& data   -> context data.
//
//  returns nothing 
//----------------------------------------------------------------------------------------
template <class T, class Pub> inline void 
EnumeratorImp<T,Pub>::Read(StrInputStream& s, DlInt32 count, const frame_data& data)
{
	reserve(count);
	for (DlInt32 i = 0; i < count; i++) {
		std::auto_ptr<T> val(NEW T(s, data));
		Add(val.release());
	}
}

//----------------------------------------------------------------------------------------
// EnumeratorImp<T,Pub>::Write
//
//      write the list.
//
//  StrOutputStream& s         -> output stream
//  const frame_data& data     -> context data
//
//  returns nothing 
//----------------------------------------------------------------------------------------
template <class T, class Pub> inline void 
EnumeratorImp<T,Pub>::Write(StrOutputStream& s, const frame_data& data) const
{
	//	write count?
	Reset();
	while(HasMore()) {
		Next()->Write(s, data);
	}
}

template <class T, class Pub> inline void 
EnumeratorImp<T,Pub>::SendMessage(bool isAdd, T* elem)
{
	Pub n(elem);
	ListMessage m;
	m.id = GetListID();
	m.element = &n;
	BroadcastMessage(isAdd ? MessageListAddOne : MessageListRemoveOne, &m);
}

template <class T, class Pub> inline void 
EnumeratorImp<T,Pub>::SendMessage(bool isAdd, const EnumeratorImp<T, Pub>* list)
{
	ListMessage m;
	m.id = GetListID();
	m.element = (void*)list;
	BroadcastMessage(isAdd ? MessageListAddMultiple : MessageListRemoveMultiple, &m);
}


#endif

