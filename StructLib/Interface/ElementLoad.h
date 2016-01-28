/*+
 *	File:		ElementLoad.h
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

#ifndef _H_ElementLoad
#define _H_ElementLoad

//---------------------------------- Includes ----------------------------------

class	ElemLoadImp;
class	DlString;

//---------------------------------- Class -------------------------------------

enum ElementLoadType {
	ElementLoadLateral,
	ElementLoadAxial,
	ElementLoadPreforce,
	ElementLoadPremoment,
	ElementLoadCount
};

const DlInt32	kElementLoadOK = 0;
const DlInt32	kElementLoadWrongUnits = -1;
const DlInt32	kElementLoadBadValue = -2;

class ElementLoad
{
	public:

	ElementLoad() : imp(0) {}
	ElementLoad(ElemLoadImp* elem) : imp(elem) {}
	
	operator ElemLoadImp* () const { return imp; }

	bool 		Empty() const { return imp == 0; }
	void		GetValues(DlFloat32 vals[ElementLoadCount]) const;

	//	set the value as a string.
	void		SetValue(ElementLoadType which, const char* val);
	void		SetValue(ElementLoadType which, DlFloat64 value);
	
	//	get the value as a string
	DlString	GetValue(ElementLoadType which, bool wantUnits) const;

	static DlFloat64 ConvertElemLoadValue(ElemLoadImp* l, DlInt32 whichDof, const char* value);

private:

	ElemLoadImp* imp;

};

#endif

//	eof