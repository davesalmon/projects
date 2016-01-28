/*+
 *	File:		ElemLoadList.cpp
 *
 *	Contains:	Frame Element Load List interface
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

//---------------------------------- Includes ----------------------------------

#include "DlPlatform.h"
#include "ElemLoadList.h"
#include "ElemLoadImp.h"
#include "StrMessage.h"

//---------------------------------- Methods -----------------------------------

ElemLoadList::ElemLoadList() 
{
}

DlUInt32
ElemLoadList::GetListID() const
{
	return ElementLoadListID;
}

ElemLoadImp*
ElemLoadList::FindMatchingLoad(const ElemLoadImp* prop) const
{
	Reset();
	while(HasMore()) {
		const ElemLoadImp* p = Next();
		if (p == prop || *p == *prop)
			return const_cast<ElemLoadImp*>(p);
	}
	
	return nullptr;
}

#if 0
void
ElemLoadList::Read(StrInputStream & inp, DlInt32 count, const frame_data & data)
{
	reserve(count);
	for (DlInt32 i = 0; i < count; i++) {
		std::auto_ptr<ElemLoadImp> val(NEW ElemLoadImp(inp, data));
//		val->read(inp, data);
		Add(val.release());
	}
}

void 
ElemLoadList::Write(StrOutputStream & out, const frame_data & data) const
{
	//	write count?
	while(HasMore()) {
		Next()->Write(out, data);
	}
}

#endif

// eof