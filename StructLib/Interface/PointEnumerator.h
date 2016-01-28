/*+
 *	File:		PointEnumerator.h
 *
 *	Contains:	Interface for element enumerator
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

#ifndef _H_PointEnumerator
#define _H_PointEnumerator

//---------------------------------- Includes ----------------------------------

//#include "Enumerator.h"


//---------------------------------- Class -------------------------------------

class	WorldPoint;
class	PointEnumeratorImp;

//---------------------------------- Class -------------------------------------

class	PointEnumerator 
{
public:
	explicit PointEnumerator();

	explicit PointEnumerator(const PointEnumeratorImp* e);
	PointEnumerator(const PointEnumerator& e);

	PointEnumerator& operator=(const PointEnumerator& e);
	~PointEnumerator();

	operator PointEnumeratorImp* () const {
		 return itsList;
	}

	bool				HasMore() const;
	const WorldPoint&	Next() const;
	void				Reset() const;
	DlInt32 			Length() const;

	void				Append(const WorldPoint& n);
	const WorldPoint&	ElementAt(DlUInt32 offset) const;

	void				Set(DlUInt32 index, const WorldPoint& pt);

//	void				Remove(const WorldPoint& n);

private:

	void			Clone();

	PointEnumeratorImp* itsList;

};

#endif

//eof
