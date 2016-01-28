/*+
 *	File:		PointEnumerator.cpp
 *
 *	Contains:	Interface for WorldPoint enumerator
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
#include "PointEnumerator.h"
#include "PointEnumeratorImp.h"

//---------------------------------- Class -------------------------------------

PointEnumerator::PointEnumerator() 
	: itsList(NEW PointEnumeratorImp()) 
{
	itsList->incReference();
}

PointEnumerator::PointEnumerator(const PointEnumeratorImp* e)
	: itsList(const_cast<PointEnumeratorImp*>(e))
{
	itsList->incReference();
}

PointEnumerator::PointEnumerator(const PointEnumerator& e)
	: itsList(e.itsList)
{
	itsList->incReference();
}

PointEnumerator& 
PointEnumerator::operator=(const PointEnumerator& e)
{
	if (this != &e)
	{
		itsList->decReference(itsList);
		itsList = e.itsList;
		itsList->incReference();
	}
	return *this;
}

PointEnumerator::~PointEnumerator()
{
	itsList->decReference(itsList);
}

bool
PointEnumerator::HasMore() const
{
	return itsList->hasMore();
}

const WorldPoint&
PointEnumerator::Next() const
{
	return itsList->next();
}

void
PointEnumerator::Reset() const
{
	itsList->reset();
}

DlInt32
PointEnumerator::Length() const
{
	return itsList->length();
}

void
PointEnumerator::Append(const WorldPoint & n)
{
	itsList->add(n);
}

const WorldPoint& 
PointEnumerator::ElementAt(DlUInt32 index) const 
{
	return itsList->get(index);
}

void
PointEnumerator::Set(DlUInt32 index, const WorldPoint& pt) 
{
	itsList->set(index, pt);
}

// eof