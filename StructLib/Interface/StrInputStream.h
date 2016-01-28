/*+
 *	File:		StrInputStream.h
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

#ifndef _H_StrInputStream
#define _H_StrInputStream

//---------------------------------- Includes ----------------------------------

#include "DlTypes.h"
#include "DlString.h"

//---------------------------------- Class -------------------------------------

class StrInputStream
{
public:
    virtual ~StrInputStream() {};
	virtual bool 		GetBool() 	= 0;
	virtual DlUInt8 	GetByte() 	= 0;
	virtual DlInt32 	GetInt() 	= 0;
	virtual DlFloat32 	GetFloat() 	= 0;
	virtual DlFloat64 	GetDouble() = 0;
	virtual DlString 	GetString() = 0;
};

#endif
