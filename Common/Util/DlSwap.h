/*
 *  DlSwap.h
 *  Common_Util
 *
 *  Created by David Salmon on 12/29/07.
 *  Copyright 2007 David C. Salmon All rights reserved.
 *
 * Contains swap template methods for most data types.
 */
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

#ifndef _H_DlSwap
#define _H_DlSwap

//
//	Conventions are 
//		DlSwapLE -- swap a little endian value.
//		DlSwapBE -- swap a big endian value.
//	
//	It actually does not matter which way the swap goes.
//

#include <Carbon/Carbon.h>

// -----------------------------------------
// Little endian to host
// -----------------------------------------

inline 
void DlSwapLE(DlInt8& v)
{
	// nothing
}

inline 
void DlSwapLE(DlUInt8& v)
{
	// nothing
}

inline 
void DlSwapLE(DlInt16& v)
{
	v = CFSwapInt16LittleToHost(v);
}

inline 
void DlSwapLE(DlUInt16& v)
{
	v = CFSwapInt16LittleToHost(v);
}

inline 
void DlSwapLE(DlInt32& v)
{
	v = CFSwapInt32LittleToHost(v);
}

inline 
void DlSwapLE(DlUInt32& v)
{
	v = CFSwapInt32LittleToHost(v);
}

inline 
void DlSwapLE(DlInt64& v)
{
	v = CFSwapInt64LittleToHost(v);
}

inline 
void DlSwapLE(DlUInt64& v)
{
	v = CFSwapInt64LittleToHost(v);
}

inline
void DlSwapLE(DlFloat32& f)
{
	DlSwapLE((DlUInt32&)f);
}

inline
void DlSwapLE(DlFloat64& f)
{
	DlSwapLE((DlUInt64&)f);
}

// -----------------------------------------
// Big endian to host
// -----------------------------------------

inline 
void DlSwapBE(DlInt8& v)
{
	// nothing
}

inline 
void DlSwapBE(DlUInt8& v)
{
	// nothing
}

inline 
void DlSwapBE(DlInt16& v)
{
	v = CFSwapInt16BigToHost(v);
}

inline 
void DlSwapBE(DlUInt16& v)
{
	v = CFSwapInt16BigToHost(v);
}

inline 
void DlSwapBE(DlInt32& v)
{
	v = CFSwapInt32BigToHost(v);
}

inline 
void DlSwapBE(DlUInt32& v)
{
	v = CFSwapInt32BigToHost(v);
}

inline 
void DlSwapBE(DlInt64& v)
{
	v = CFSwapInt64BigToHost(v);
}

inline 
void DlSwapBE(DlUInt64& v)
{
	v = CFSwapInt64BigToHost(v);
}

inline
void DlSwapBE(DlFloat32& f)
{
	DlSwapBE((DlUInt32&)f);
}

inline
void DlSwapBE(DlFloat64& f)
{
	DlSwapBE((DlUInt64&)f);
}

#endif
