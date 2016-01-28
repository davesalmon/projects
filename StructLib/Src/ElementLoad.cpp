/*+
 *	File:		ElementLoad.cpp
 *
 *	Contains:	Interface for element loads
 *	
 *	Property elements contain the defintions for element loads.
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

#include "DlPlatform.h"
#include "ElementLoad.h"
#include "ElemLoadImp.h"
//#include "DlStringUtil.h"
#include "UnitTable.h"
#include "DlAssert.h"
#include "DlString.h"

//#include <stdlib.h>

//---------------------------------- Class -------------------------------------

void
ElementLoad::GetValues(DlFloat32 vals[ElementLoadCount]) const 
{
	for  (DlInt32 i = 0; i < ElementLoadCount; ++i) {
		vals[i] = imp->GetValue(ElementLoadType(i));
	}
}

//	set the value as a string
void
ElementLoad::SetValue(ElementLoadType index, const char* val)
{
	_DlAssert(index >= 0 && index < ElementLoadCount);
	imp->SetValue(index, UnitTable::ParseValue(val, kElementLoadUnits[index]));
}

void
ElementLoad::SetValue(ElementLoadType index, DlFloat64 value)
{
	_DlAssert(index >= 0 && index < ElementLoadCount);
	imp->SetValue(index, value);
}

	//	get the value as a string
DlString
ElementLoad::GetValue(ElementLoadType index, bool wantUnits) const
{
	_DlAssert(index >= 0 && index < ElementLoadCount);
	return UnitTable::FormatValue(imp->GetValue(index), kElementLoadUnits[index],
					DlFloatFormat(4, 0, DlFloatFormatType::Fit), wantUnits);
}

DlFloat64 
ElementLoad::ConvertElemLoadValue(ElemLoadImp* l, DlInt32 whichDof, const char* value) 
{
	return UnitTable::ParseValue(value, kElementLoadUnits[whichDof]);
}


//	eof
