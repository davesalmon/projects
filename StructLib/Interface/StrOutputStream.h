/*+
 *	File:		StrOutputStream.h
 *
 *	Contains:	Interface for input stream
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

#ifndef _H_StrOutputStream
#define _H_StrOutputStream

//---------------------------------- Includes ----------------------------------

#include "DlTypes.h"
class DlString;

//---------------------------------- Class -------------------------------------

class StrOutputStream
{
public:
    virtual ~StrOutputStream() {};
	virtual void PutBool(bool val) = 0;
	virtual void PutByte(DlUInt8 val) = 0;
	virtual void PutInt(DlInt32 val) = 0;
	virtual void PutFloat(DlFloat32 val) = 0;
	virtual void PutDouble(DlFloat64 val) = 0;
	virtual void PutString(const char* str) = 0;
};

#endif
