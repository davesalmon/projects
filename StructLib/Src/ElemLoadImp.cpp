/*+
 *	File:		ElemLoadImp.cpp
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

//---------------------------------- Includes ----------------------------------

#include "DlPlatform.h"
#include "ElemLoadImp.h"
#include "StrInputStream.h"
#include "StrOutputStream.h"

//---------------------------------- Includes ----------------------------------

ElemLoadImp::ElemLoadImp()
{
	memset(value, 0, sizeof(value));
}

ElemLoadImp::ElemLoadImp(StrInputStream & input, const frame_data & )
{
	for (DlInt32 i = 0; i < ElementLoadCount; i++)
		value[i] = input.GetDouble();
}

bool
ElemLoadImp::operator==(const ElemLoadImp& p) const
{
	for (auto i = 0; i < ElementLoadCount; i++)
	{
		if (value[i] != p.value[i])
			return false;
	}
	
	return true;
}

void	
ElemLoadImp::Write(StrOutputStream & output, const frame_data & ) const
{
	for (DlInt32 i = 0; i < ElementLoadCount; i++)
		output.PutDouble(value[i]);
}

void
ElemLoadImp::Add(const ElemLoadImp* imp, float factor)
{
	isCombo = true;
	for (DlInt32 i = 0; i < ElementLoadCount; i++) {
		value[i] += factor * imp->value[i];
	}
}


//	eof