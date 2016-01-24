//+
//	DlExceptionDefs.h
//
//	Copyright © 1999 David C. Salmon. All Rights Reserved
//
//	Define basic exception macros and types.
//-
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

#ifndef _H_DlExceptionDefs
#define	_H_DlExceptionDefs

#include "DlTypes.h"

//---------------------------------- Defines -----------------------------------

/// The resource type.
const DlUInt32	kErrorResType = 'ErrS';

/// Macro to get the exception name.
#define	DlExceptionName( c )	(kString##c)
/// Macro for declaring the exception name.
#define	DlExceptionDeclare( c ) const char* kString##c = #c

/// Macro to define the start of an exception block.
#define	DlExceptionBegin( c )	public: enum {
/// Macro defining the end of an exception block.
#define	DlExceptionEnd()		}; private:

#endif

//	eof
