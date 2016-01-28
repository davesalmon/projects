/*+
 *	File:		PointEnumeratorImp.h
 *
 *	Contains:	Interface for point enumerator
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

#ifndef _H_PointEnumeratorImp
#define _H_PointEnumeratorImp

//---------------------------------- Includes ----------------------------------

#include "WorldPoint.h"
#include <vector>

//---------------------------------- Class -------------------------------------

class	PointEnumeratorImp : public std::vector<WorldPoint>
{
	typedef	std::vector<WorldPoint>					Base;
	typedef std::vector<WorldPoint>::iterator		Iter;
	typedef std::vector<WorldPoint>::const_iterator	ConstIter;

public:
	PointEnumeratorImp(DlInt32 size = 0);
	PointEnumeratorImp(const PointEnumeratorImp & e);
	
	void incReference() const;
	bool isOwner() const;
	
	void reset() const;
	const WorldPoint& next() const;
	bool hasMore() const;
	DlInt32 length() const;
	
	//	set is special in that it expands the array if necessary
	//	but only if index is one more than the size
	void set(DlUInt32 index, const WorldPoint& pt);
	const WorldPoint& get(DlUInt32 index) const;
	
	void add(const WorldPoint& wp);
	void remove(const WorldPoint& wp);
	
	static void decReference(PointEnumeratorImp * & p);

private:
	
	mutable ConstIter	itsIter;
	mutable DlInt32		itsRefCount;
};

//------------------------------------------------------------------------------
//	PointEnumeratorImp::incReference
//
//		increment the reference count
//
//	
//------------------------------------------------------------------------------
inline void
PointEnumeratorImp::incReference() const
{
	itsRefCount++;
}

//------------------------------------------------------------------------------
//	PointEnumeratorImp::isOwner
//
//		decrement the reference, delete if 0
//
//	returns					<-	true if there are no shared references to this
//------------------------------------------------------------------------------
inline bool 
PointEnumeratorImp::isOwner() const
{
	return itsRefCount == 1;
}

//------------------------------------------------------------------------------
//	PointEnumeratorImp::reset
//
//		reset element pointer to begin
//
//------------------------------------------------------------------------------
inline void
PointEnumeratorImp::reset() const
{
	itsIter = begin();
}

//------------------------------------------------------------------------------
//	PointEnumeratorImp::next
//
//		return the next element in the list
//
//	returns					<-	true if there are no shared references to this
//------------------------------------------------------------------------------
inline const WorldPoint& 
PointEnumeratorImp::next() const
{
	return *itsIter++;
}

//------------------------------------------------------------------------------
//	PointEnumeratorImp::hasMore
//
//		return true if we have not reached the end of the list
//
//	returns					<-	true if there are more elements
//------------------------------------------------------------------------------
inline bool
PointEnumeratorImp::hasMore() const
{
	return itsIter != end();
}

//------------------------------------------------------------------------------
//	PointEnumeratorImp::length
//
//		return the number of elements in the list
//
//	returns					<-	element count
//------------------------------------------------------------------------------
inline DlInt32 
PointEnumeratorImp::length() const
{
	return size();
}

//------------------------------------------------------------------------------
//	PointEnumeratorImp::add
//
//		add a point to the list
//
//	wp						->	point to add
//------------------------------------------------------------------------------
inline void
PointEnumeratorImp::add(const WorldPoint& wp)
{
	push_back(wp);
}

inline const WorldPoint& 
PointEnumeratorImp::get(DlUInt32 index) const {
	if (index < size()) 
		return (*this)[index];
	throw DlException("PointEnumerator: index out of bounds.");
}




#endif

//	eof
