/*+
 *	File:		NodeLoadList.h
 *
 *	Contains:	Node Load List interface
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

#ifndef _H_NodeLoadList
#define _H_NodeLoadList

//---------------------------------- Includes ----------------------------------

#include "EnumeratorImp.h"
#include "NodeLoadEnumerator.h"
#include "NodeLoadImp.h"

class	frame_data;
class	NodeLoad;
class	StrInputStream;
class	StrOutputStream;

//---------------------------------- Class -------------------------------------

class	NodeLoadList : public EnumeratorImp<NodeLoadImp, NodeLoad> 
{
public:
	NodeLoadList() {}

	DlUInt32		GetListID() const;
	
	NodeLoadImp*			FindMatchingLoad(const NodeLoadImp* prop) const;

	//	build the list by reading it
//	void	Read(StrInputStream& inp, DlInt32 count, const frame_data& data);
//	void 	Write(StrOutputStream& out, const frame_data& data) const;

	static const NodeLoadList* GetList(const NodeLoadEnumerator& l) { return l.GetList(); }
	static NodeLoadList* GetList(NodeLoadEnumerator& l) { return l.GetList(); }
};

//---------------------------------- Inlines -----------------------------------


#endif

//	eof
