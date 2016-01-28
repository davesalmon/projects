/*+
 *	File:		PointEnumeratorImp.cpp
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

//---------------------------------- Includes ----------------------------------

#include "DlPlatform.h"
#include "PointEnumeratorImp.h"

//---------------------------------- Class -------------------------------------

//------------------------------------------------------------------------------
//	PointEnumeratorImp::PointEnumeratorImp							constructor
//
//		construct point enumerator
//
//	size					->	the number of initial elements
//------------------------------------------------------------------------------
PointEnumeratorImp::PointEnumeratorImp(DlInt32 size) 
	: Base(size), itsRefCount(0)
{
	reset();
}

//------------------------------------------------------------------------------
//	PointEnumeratorImp::PointEnumeratorImp							constructor
//
//		copy point enumerator
//
//	size					->	the number of initial elements
//------------------------------------------------------------------------------
PointEnumeratorImp::PointEnumeratorImp(const PointEnumeratorImp & e) 
	: Base(e), itsRefCount(0)
{
	reset();
}

//------------------------------------------------------------------------------
//	PointEnumeratorImp::decReference
//
//		decrement the reference, delete if 0
//
//	p						->	the list
//------------------------------------------------------------------------------
void PointEnumeratorImp::decReference(PointEnumeratorImp * & p)
{
	_DlAssert(p->itsRefCount != 0);
	if (--p->itsRefCount == 0)
	{
		delete p;
		p = 0;
	}
}

void
PointEnumeratorImp::set(DlUInt32 index, const WorldPoint& pt) 
{
	if (index > size())
		throw DlException("PointEnumerator: Array index out of bounds.");
	
	if (index == size())
		add(pt);
	else
		(*this)[index] = pt;
}

//------------------------------------------------------------------------------
//	PointEnumeratorImp::remove
//
//		remove a point matching the specified point from the list
//
//	wp						->	point to match
//------------------------------------------------------------------------------
void PointEnumeratorImp::remove(const WorldPoint& wp)
{
	Iter	iter = begin();
	while(iter != end())
	{
		if (wp == *iter)
		{
			erase(iter);
			break;
		}
	}
}

//	eof
