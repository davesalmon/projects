/*+
 *	File:		ElemLoadImp.h
 *
 *	Contains:	Frame Element Load internal interface
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

#ifndef _H_ElemLoadImp
#define _H_ElemLoadImp

//---------------------------------- Includes ----------------------------------

#include "ElementLoad.h"
#include "UnitTable.h"

class StrInputStream;
class StrOutputStream;
class frame_data;

const UnitType kElementLoadUnits[ElementLoadCount] = {
	UnitsDistribForce,
	UnitsDistribForce,
	UnitsForce,
	UnitsMoment
};	

//---------------------------------- Class -------------------------------------

class ElemLoadImp
{
public:

	ElemLoadImp();
	ElemLoadImp(StrInputStream& input, const frame_data& data);

	bool 	operator==(const ElemLoadImp& p) const;

	void	Write(StrOutputStream& output, const frame_data& data) const;

	DlFloat64 GetValue(ElementLoadType which) const { return value[which]; }
	void SetValue(ElementLoadType which, DlFloat64 val) { value[which] = val; }

	void Clear() { 	memset(value, 0, sizeof(value)); }
	void Add(const ElemLoadImp* imp, DlFloat32 factor);
	bool IsCombo() const { return isCombo; }
	
private:

	DlFloat32	value[ElementLoadCount];
	bool		isCombo;
};

#endif

//	eof