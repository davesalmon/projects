//+
//	DlStorage.h
//
//	©1998 David C. Salmon. All Rights Reserved
//
//	Define a template class for managing memory. It manages both 
//	stack and heap memory, depending on constructor.
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

#ifndef _H_DlStorage
#define _H_DlStorage

/* ---------------------------- Includes ----------------------------------- */

#include	"DlAssert.h"

#include <cstring>

#if defined(DlDebugging)
#include <list>
#include <algorithm>
#endif

using namespace std;

/* ---------------------------- Declares ----------------------------------- */

const int	DlStoreInvRef = -10000L;

//------------------------------------------------------------------------------
//	class	DlStorage
//
//	base storage class. This class can allocate storage or manage storage from
//	other sources. If the storage is from other sources, it cannot be resized.
//
//------------------------------------------------------------------------------
template <typename T> class DlStorage {

	class	DlBlock 
	{
	public:
					//	empty block
				DlBlock(); 
					//	new block
				explicit
				DlBlock(size_t size);
					//	static storage wrapper (if ptr not NULL)
				DlBlock(void * ptr, size_t sze);
					//	true copy
				DlBlock(const DlBlock & block);

					//	destruct the block
				~DlBlock();
					//	pointer to memory
		void *	getPtr() const;
					//	return the size of the block
		size_t	getSize() const;
					//	increment the block
		void	incCount();
					//	decrement this block. Disappears when ref count == 0;
		void	decCount();
					//	ref count 0 indicates owned
		bool	owned() const; 
					//	return true if this is not a clone
		bool	unique() const; 

#if defined(DlDebugging)
	typedef	list<const DlBlock*>	_BlockList;
	static _BlockList	sBlockList;
#endif

	private:
		const DlBlock &	operator=(const DlBlock & block);

		void *	itsMem;
		size_t	itsSize;
		int		itsRefCount;
		bool	itsOwned;
	};

public:
				//	construct empty storage
			DlStorage();
				//	construct static storage 
			DlStorage(T* buf, size_t len);
				//	destruct
			virtual ~DlStorage();
				//	copy
			DlStorage(const DlStorage<T>& store);
				//	copy
	DlStorage<T>& 	
			operator = (const DlStorage<T>& store);
				//	make a true copy
				//	requires specialization for T
	DlStorage<T>
			copy(size_t theSize = 0) const;
				//	change the size
				//	requires specialization for T
	void	allocate(size_t reqSize);
				//	set the data
	void	set(const T* p, size_t len, size_t ofs);
				//	set a value
	void	set(const T& val, size_t ofs);
				//	move stuff around
	void	move(size_t bytesToMove, size_t from, size_t to);
				//	get an element
	const T&
			get(size_t ofs) const;
				//	get a block
	void	get(size_t ofs, size_t len, T* buf) const;
				//	pointer to mem
	const T*
			ptr() const;
				//	pointer to mem
	T*		ptr();
				//	number of storage elements
	size_t	numelems() const;
				//	true if owned
	bool	owned() const;

#if defined(DlDebugging)
	static void	dumpblocks();
#endif

protected:
	void	branch();

	//	these function do not check size or branch. Only call after
	//	allocate.
	void	pmove(size_t bytesToMove, size_t from, size_t to);
	void	pset(const T* p, size_t len, size_t ofs);
	T*		pptr();

			DlStorage(size_t len);

	DlBlock* itsBlock;
};

/* ---------------------------- Inlines  ----------------------------------- */

template <typename T> inline 
DlStorage<T>::DlBlock::DlBlock() 
		: itsMem(0)
		, itsSize(0)
		, itsRefCount(1)
		, itsOwned(true) 
{
#if defined(DlDebugging)
	sBlockList.push_back(this);
#endif
}

template <typename T> inline 
DlStorage<T>::DlBlock::DlBlock(size_t size) 
		: itsMem(size ? OPERATORNEW(size) : 0)
		, itsSize(size)
		, itsRefCount(1)
		, itsOwned(true) {
#if defined(DlDebugging)
	sBlockList.push_back(this);
#endif
}

//	static storage wrapper (if ptr not NULL)
template <typename T> inline 
DlStorage<T>::DlBlock::DlBlock(void * ptr, size_t sze) 
		: itsMem(ptr)
		, itsSize(ptr != 0 ? sze : 0)
		, itsRefCount(1)
		, itsOwned(ptr == 0) {
#if defined(DlDebugging)
	sBlockList.push_back(this);
#endif
}

//	true copy
template <typename T> inline 
DlStorage<T>::DlBlock::DlBlock(const DlBlock & block)
		: itsMem(block.itsSize ? OPERATORNEW(block.itsSize) : 0)
		, itsSize(block.itsSize)
		, itsRefCount(1) 
{
#if defined(DlDebugging)
	sBlockList.push_back(this);
#endif
	if (itsSize)
		memcpy(itsMem, block.itsMem, itsSize);
}

//	destruct the block
template <typename T> inline 
DlStorage<T>::DlBlock::~DlBlock() 
{
	_DlAssert(itsRefCount == 0);
	if (itsOwned && itsMem) {
		operator delete(itsMem);
	}
#if defined(DlDebugging)
	typename _BlockList::iterator i = find(sBlockList.begin(), sBlockList.end(), this);
	_DlAssert(i != sBlockList.end());
	sBlockList.erase(i);
#endif
}

//	pointer to memory
template <typename T> inline 
void *
DlStorage<T>::DlBlock::getPtr() const 
{
	return itsMem;
}

//	return the size of the block
template <typename T> inline 
size_t		
DlStorage<T>::DlBlock::getSize() const 
{
	return itsSize;
}

//	increment the block
template <typename T> inline 
void	
DlStorage<T>::DlBlock::incCount() 
{
	itsRefCount++;
}

//	decrement this block. Disappears when
//	ref count == 0;
template <typename T> inline 
void		
DlStorage<T>::DlBlock::decCount() 
{
	if (--itsRefCount == 0)
		delete this;
}

//	ref count 0 indicates owned
template <typename T> inline 
bool		
DlStorage<T>::DlBlock::owned() const 
{
	return itsOwned;
}

//	return true if this is not a clone
template <typename T> inline 
bool		
DlStorage<T>::DlBlock::unique() const 
{
	return !owned() || itsRefCount == 1;
}

/* ---------------------------- Inlines  ----------------------------------- */

//	construct empty storage
template <typename T> inline 
DlStorage<T>::DlStorage() 
	: itsBlock(NEW DlBlock()) 
{
	_DlAssert(itsBlock->getPtr()==0);
}

//	construct static storage 
template <typename T> inline 
DlStorage<T>::DlStorage(T* buf, size_t len) 
	: itsBlock(NEW DlBlock(buf, len)) 
{
}

//	destruct
template <typename T> inline 
DlStorage<T>::~DlStorage() 
{
	itsBlock->decCount();
}

//	copy
template <typename T> inline 
DlStorage<T>::DlStorage(const DlStorage<T> & store) 
	: itsBlock(store.itsBlock) 
{
	itsBlock->incCount();
}

//	copy
template <typename T> inline 
DlStorage<T>& 
DlStorage<T>::operator = (const DlStorage<T>& store) 
{
	if (&store != this) {
		itsBlock->decCount();
		itsBlock = store.itsBlock;
		itsBlock->incCount();
	}
	return *this;
}

//	set the data
template <typename T> inline 
void
DlStorage<T>::set(const T* p, size_t len, size_t ofs) 
{
	allocate(ofs + len);
	pset(p, len, ofs);
}

//	set a value
template <typename T> inline 
void
DlStorage<T>::set(const T& val, size_t ofs) 
{
	allocate(ofs);
	pptr()[ofs] = val;
}

//	move stuff around
template <typename T> inline 
void
DlStorage<T>::move(size_t bytesToMove, size_t from, size_t to) 
{
	allocate(bytesToMove + from > to ? from : to);
	pmove(bytesToMove, from, to);
}

//	get an element
template <typename T> inline 
const T	&
DlStorage<T>::get(size_t ofs) const 
{
	_DlAssert(ofs >= 0 && ofs < numelems());
	return ptr()[ofs];
}

//	pointer to mem
template <typename T> inline 
const T*
DlStorage<T>::ptr() const 
{
	return (const T*)itsBlock->getPtr();
}

//	pointer to mem
template <typename T> inline 
T*
DlStorage<T>::ptr() 
{
	branch();
	return (T*)itsBlock->getPtr();
}

//	number of storage elements
template <typename T> inline 
size_t
DlStorage<T>::numelems() const
{
	return itsBlock->getSize();
}

//	true if owned
template <typename T> inline 
bool
DlStorage<T>::owned() const 
{
	return itsBlock->owned();
}

#if defined(DlDebugging)
template <typename T> inline 
void	
DlStorage<T>::dumpblocks() 
{
	_DlAssert(DlBlock::sBlockList.empty());
	if (!DlBlock::sBlockList.empty()) {
		typename DlBlock::_BlockList::iterator iter = DlBlock::sBlockList.begin();
		while(iter != DlBlock::sBlockList.end()) {
			delete *iter;
			iter++;
		}
	}
}
#endif

template <typename T> inline 
T*
DlStorage<T>::pptr() 
{
	return (T*)itsBlock->getPtr();
}

template <typename T> inline 
DlStorage<T>::DlStorage(size_t len) 
	: itsBlock(NEW DlBlock(len)) 
{
}

typedef	DlStorage<char>	DlBuffer;

inline
DlBuffer
MakeBuffer(void * buf, size_t len) 
{
	return DlBuffer((char *)buf, len);
}

#endif

//	eof
