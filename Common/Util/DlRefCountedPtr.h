//+
//	DlRefCountedPtr.h
//
//	Copyright © 2001 David C. Salmon. All Rights Reserved
//
//	Contains clipboard class.
//-
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

#ifndef _H_DlRefCountedPtr
#define _H_DlRefCountedPtr

#include "DlAssert.h"

// ---------------------------- Declares --------------------------------------

template <class T> class DlRefCountedPtr {

public:

	DlRefCountedPtr();	
	explicit DlRefCountedPtr(T* p);

	DlRefCountedPtr(const DlRefCountedPtr& q);
	
	~DlRefCountedPtr();

	DlRefCountedPtr& operator=(const DlRefCountedPtr& u);
	
	template <class U> 
	DlRefCountedPtr(const DlRefCountedPtr<U>& q);

	template <class U> 
	DlRefCountedPtr& operator=(const DlRefCountedPtr<U> & u);

	template <class U> 
	DlRefCountedPtr<U> cast(const DlRefCountedPtr<U> &) const;

	bool operator==(const DlRefCountedPtr& u) const;
	operator bool () const;

	T* get() const;
	T* operator->() const;
	T& operator*() const;

    //	provided for cast. Not safe!
    //	These must be public since the types are U and T.
    DlInt32* getCount() const;
	DlRefCountedPtr(T* p, DlInt32* count);	
private:

	void	IncCount();
	bool	DecCount();
	
	T*			_p;
	DlInt32* 	_count;
};

// ----------------------------- Methods ---------------------------------------

//------------------------------------------------------------------------------
//	DlRefCountedPtr<T>::DlRefCountedPtr
//
//		default constructor.
//
//------------------------------------------------------------------------------
template <class T>
inline
DlRefCountedPtr<T>::DlRefCountedPtr() 
	: _p(0), _count(0) 
{
}

//------------------------------------------------------------------------------
//	DlRefCountedPtr<T>::DlRefCountedPtr
//
//		pseudo copy constructor for cast operator.
//
//	T* p			-> pointer to store
//	DlInt32* count	-> associated count
//------------------------------------------------------------------------------
template <class T>
inline
DlRefCountedPtr<T>::DlRefCountedPtr(T* p, DlInt32* count) 
	: _p(p), _count(count) 
{
	IncCount();
}

//------------------------------------------------------------------------------
//	DlRefCountedPtr<T>::DlRefCountedPtr
//
//		constructor for pointer.
//
//	T* p			-> pointer to store
//------------------------------------------------------------------------------
template <class T>
inline
DlRefCountedPtr<T>::DlRefCountedPtr(T* p) 
	: _p(p), _count(_p ? NEW DlInt32(1) : 0) 
{
}

//------------------------------------------------------------------------------
//	DlRefCountedPtr<T>::DlRefCountedPtr
//
//		copy constructor.
//
//	const DlRefCountedPtr& q	-> pointer to copy
//------------------------------------------------------------------------------
template <class T>
inline
DlRefCountedPtr<T>::DlRefCountedPtr(const DlRefCountedPtr& q) 
	: _p(q._p), _count(q._count) 
{
	IncCount();
}

//------------------------------------------------------------------------------
//	DlRefCountedPtr<T>::~DlRefCountedPtr
//
//		destructor.
//
//------------------------------------------------------------------------------
template <class T>
inline
DlRefCountedPtr<T>::~DlRefCountedPtr() 
{
	if (DecCount()) {
		delete _p;
		delete _count;
	}
}

//------------------------------------------------------------------------------
//	DlRefCountedPtr<T>::operator=
//
//		assignment operator.
//
//	const DlRefCountedPtr& u	-> pointer to assign to this
//	returns 					<- this
//------------------------------------------------------------------------------
template <class T>
inline
DlRefCountedPtr<T>& 
DlRefCountedPtr<T>::operator=(const DlRefCountedPtr& u) 
{
	if (&u != this) {
		if (DecCount())
		{
			delete _p;
			delete _count;
		}
		_p = u._p;
		_count = u._count;
		IncCount();
	}
	
	return *this;
}

//------------------------------------------------------------------------------
//	DlRefCountedPtr<T>::DlRefCountedPtr
//
//		conversion copy constructor. Copy to superclass (T) of U.
//		class T must be assignable from class U.
//
//	const DlRefCountedPtr<U>& u	-> object to copy.
//------------------------------------------------------------------------------
template <class T>
template <class U> 
inline
DlRefCountedPtr<T>::DlRefCountedPtr(const DlRefCountedPtr<U>& q) 
		: _p(q.get()), _count(q.getCount()) 
{
	IncCount();
}

//------------------------------------------------------------------------------
//	DlRefCountedPtr<T>::operator=
//
//		conversion assignment. Convert to a superclass (T) of U
//		class T must be assignable from class U.
//
//	const DlRefCountedPtr<U>& u	-> object to assign to this.
//	returns 					<- this
//------------------------------------------------------------------------------
template <class T>
template <class U> 
inline
DlRefCountedPtr<T>& 
DlRefCountedPtr<T>::operator=(const DlRefCountedPtr<U> & u) 
{
	if ((void*)this != (void*)(&u)) {
		if (DecCount())
		{
			delete _p;
			delete _count;
		}
		_p = u.get();
		_count = u.getCount();
		IncCount();
	}
	return *this;
}

//------------------------------------------------------------------------------
//	DlRefCountedPtr<T>::cast
//
//		construct a new object, dynamic casting this to U.
//
//	const DlRefCountedPtr<U>& 	-> dummy to allow compiler to determine U.
//	returns						<- dynamic casted DlRefCountedPtr
//------------------------------------------------------------------------------
template <class T>
template <class U> 
inline
DlRefCountedPtr<U> 
DlRefCountedPtr<T>::cast(const DlRefCountedPtr<U> &) const
{
	return DlRefCountedPtr<U>(dynamic_cast<U*>(_p), _count);
}

//------------------------------------------------------------------------------
//	DlRefCountedPtr<T>::operator==
//
//		compare pointers.
//
//	const DlRefCountedPtr<U>& u	-> pointer to compare
//	returns						<- true if pointers are the same
//------------------------------------------------------------------------------
template <class T>
inline
bool 
DlRefCountedPtr<T>::operator==(const DlRefCountedPtr& u) const 
{
	return _p == u.get();
}

//------------------------------------------------------------------------------
//	DlRefCountedPtr<T>::operator bool
//
//		cast to bool. Allows expressions like if (p) {}
//
//	returns						<- true if underlying pointer is not NULL
//------------------------------------------------------------------------------
template <class T>
inline
DlRefCountedPtr<T>::operator bool () const 
{
	return _p != 0;
}

//------------------------------------------------------------------------------
//	DlRefCountedPtr<T>::get
//
//		return the pointer
//
//	returns				<- the underlying pointer
//------------------------------------------------------------------------------
template <class T>
inline
T* 
DlRefCountedPtr<T>::get() const 
{
	return _p;
}

//------------------------------------------------------------------------------
//	DlRefCountedPtr<T>::operator->
//
//		overload the -> operator to allow expressions line p->...
//
//	returns				<- the underlying pointer
//------------------------------------------------------------------------------
template <class T>
inline
T* 
DlRefCountedPtr<T>::operator->() const 
{
	_DlAssert(_p != NULL);
	return _p;
}

//------------------------------------------------------------------------------
//	DlRefCountedPtr<T>::operator*
//
//		overload the * operator to allow expressions line (*p)....
//
//	returns				<- reference to the underlying pointer
//------------------------------------------------------------------------------
template <class T>
inline
T& 
DlRefCountedPtr<T>::operator*() const 
{
	_DlAssert(_p != NULL);
	return *_p;
}

//------------------------------------------------------------------------------
//	DlRefCountedPtr<T>::getCount
//
//		return a pointer to the reference count. Do not use this function.
//
//	returns				<- the object containing the reference count.
//------------------------------------------------------------------------------
template <class T>
inline
DlInt32* 
DlRefCountedPtr<T>::getCount() const 
{
	return _count;
}

//------------------------------------------------------------------------------
//	DlRefCountedPtr<T>::IncCount
//
//		increment the reference count.
//
//------------------------------------------------------------------------------
template <class T>
inline
void
DlRefCountedPtr<T>::IncCount() 
{
	if (_count) 
		(*_count)++;
}

//------------------------------------------------------------------------------
//	DlRefCountedPtr<T>::DecCount
//
//		decrement the reference count. return true if count goes to zero.
//
//	returns		<- true if count is now zero.
//------------------------------------------------------------------------------
template <class T>
inline
bool
DlRefCountedPtr<T>::DecCount() 
{
	return _count && --(*_count) == 0;
}


#endif
