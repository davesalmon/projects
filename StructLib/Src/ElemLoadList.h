/*+
 *	File:		ElemLoadList.h
 *
 *	Contains:	Element Load List interface
 *
 *	Written by:	David C. Salmon
 *
 *	Copyright:	COPYRIGHT (C) 2000 By David C. Salmon. *WORLDWIDE RIGHTS RESERVED*
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

#ifndef _H_ElemLoadList
#define _H_ElemLoadList

//---------------------------------- Includes ----------------------------------

#include "EnumeratorImp.h"
#include "ElementLoadEnumerator.h"
#include "ElemLoadImp.h"

class	frame_data;
class	StrInputStream;
class	StrOutputStream;
class	ElementLoad;

//---------------------------------- Class -------------------------------------

class	ElemLoadList : public EnumeratorImp<ElemLoadImp, ElementLoad>
{
public:
	ElemLoadList();

	DlUInt32		GetListID() const;

	ElemLoadImp*	FindMatchingLoad(const ElemLoadImp* prop) const;

	//	build the list by reading it
//	void			Read(StrInputStream & inp, DlInt32 count, const frame_data & data);
//	void 			Write(StrOutputStream & out, const frame_data & data) const;
	static const ElemLoadList* GetList(const ElementLoadEnumerator& l) { return l.GetList(); }
	static ElemLoadList* GetList(ElementLoadEnumerator& l) { return l.GetList(); }
};

//---------------------------------- Inlines -----------------------------------


#endif
