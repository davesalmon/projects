/*+
 *	File:		DlMacros.h
 *
 *	Contains:	macros for all sorts of stuff
 *
 *	Written by:	David C. Salmon
 *
 *	Copyright:	COPYRIGHT © 1990-95 by David C. Salmon.  *WORLDWIDE RIGHTS RESERVED*
 *
 *	Change History:
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

/* ---------------------------- Includes ----------------------------------- */

#ifndef _H_DlMacros
#define	_H_DlMacros

#include "DlTypes.h"

/* ---------------------------- Defines ------------------------------------ */

#ifdef	__cplusplus

//#if TARG_OS_MAC && !TARG_OS_DARWIN
//#if TARG_OS_MAC
#include <CoreServices/CoreServices.h>
//#endif

//--------------------------------- constants ----------------------------------
//	For C++

//--------------------------------- inlines ------------------------------------

//	byte access

#if TARG_BIGENDIAN
inline DlUInt16	& DlWord0(const DlUInt32 & l) { return *((DlUInt16 *)&l + 1); }
inline DlUInt16	& DlWord1(const DlUInt32 & l) { return *((DlUInt16 *)&l); }
inline DlUInt8	& DlByte0(const DlUInt32 & l) { return *((DlUInt8 *)&l + 3); }
inline DlUInt8	& DlByte1(const DlUInt32 & l) { return *((DlUInt8 *)&l + 2); }
inline DlUInt8	& DlByte2(const DlUInt32 & l) { return *((DlUInt8 *)&l + 1); }
inline DlUInt8	& DlByte3(const DlUInt32 & l) { return *((DlUInt8 *)&l); }
#else
#ifndef TARG_LITTLENDIAN
#warning TARG_BIGENDIAN or TARG_LITTLENDIAN should be defined
#endif
inline DlUInt16	& DlWord0(const DlUInt32 & l) { return *((DlUInt16 *)&l); }
inline DlUInt16	& DlWord1(const DlUInt32 & l) { return *((DlUInt16 *)&l + 1); }
inline DlUInt8	& DlByte0(const DlUInt32 & l) { return *((DlUInt8 *)&l); }
inline DlUInt8	& DlByte1(const DlUInt32 & l) { return *((DlUInt8 *)&l + 1); }
inline DlUInt8	& DlByte2(const DlUInt32 & l) { return *((DlUInt8 *)&l + 2); }
inline DlUInt8	& DlByte3(const DlUInt32 & l) { return *((DlUInt8 *)&l + 3); }
#endif

#if TARG_OS_MAC && ! __LP64__
inline void DlMemCopy(const void * from, void * to, DlInt32 len) { ::BlockMoveData(from, to, len); }
#else
inline void DlMemCopy(const void * from, void * to, DlInt32 len) { memmove(to, from, len); }
#endif

//	Simple math

template <class T> inline T DlMin(const T& a, const T& b) { return a < b ? a : b; }
template <class T> inline T DlMax(const T& a, const T& b) { return a > b ? a : b; }
template <class T> inline T DlAbs(const T& a) { return a < 0 ? -a : a; }
template <class T> inline T DlSign(const T& a, const T& b) { return a < 0 ? -b : b; }

template <class T> inline bool DlIsZero(const T& val, DlFloat tol) {return val < tol && -tol < val;}
inline DlFloat DlChop(DlFloat val, DlFloat tol) {return DlIsZero(val, tol) ? 0 : val;}

#define			DlFieldAddr(v,f)		((Ptr)&((v).f))
#define			DlFieldOffset(type,f)	DlFieldAddr((type *)(0L))

#else	// not c++

//--------------------------------- defines ------------------------------------
//	For C

//	Conversion Macros

#	if TARG_BIGENDIAN
#		define	DlWord0(l)			(*((short *)&(l) + 1))
#		define	DlWord1(l)			(*((short *)&(l) + 0))
#		define	DlByte0(l)			(*((Byte *)&(l) + 3))
#		define	DlByte1(l)			(*((Byte *)&(l) + 2))
#		define	DlByte2(l)			(*((Byte *)&(l) + 1))
#		define	DlByte3(l)			(*((Byte *)&(l) + 0))
#	else
#		ifndef TARG_LITTLENDIAN
#			warning TARG_BIGENDIAN or TARG_LITTLENDIAN should be defined
#		endif
#		define	DlWord0(l)			(*((short *)&(l) + 0))
#		define	DlWord1(l)			(*((short *)&(l) + 1))
#		define	DlByte0(l)			(*((Byte *)&(l) + 0))
#		define	DlByte1(l)			(*((Byte *)&(l) + 1))
#		define	DlByte2(l)			(*((Byte *)&(l) + 2))
#		define	DlByte3(l)			(*((Byte *)&(l) + 3))

#	endif

#	if TARG_OS_MAC
#		define DlMemCopy(from, to, len) ::BlockMoveData(from, to, len)
#	else
#		define DlMemCopy(from, to, len) memmove(to, from, len)
#	endif

//	Utility Macros

#	define	DlMin(a,b)			((a) < (b) ? (a) : (b))
#	define	DlMax(a,b)			((a) > (b) ? (a) : (b))
#	define	DlAbs(a)			((a) < 0 ? -(a) : (a))
#	define	DlSign(a,b)			((a) < 0 ? -(b) : (b))

#	define	DlFieldAddr(v,f)		((Ptr)&((v).f))
#	define	DlFieldOffset(type,f)	DlFieldAddr((type *)(0L))

#	endif

#endif

// eof
