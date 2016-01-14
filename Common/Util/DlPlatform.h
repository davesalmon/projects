/*!
 *  \file DlPlatform.h
 *  
 *	\author	David C. Salmon.
 *	\date 5/11/2002
 *  
 *  Copyright (c) 2002 David Salmon. All rights reserved.
 *
 *	This file contain pre-processer logic for determining the operating system
 *	and compiler type for the code. For each type, the following macros are defined:
 *	- #TARG_OS_MAC. The target is the macintosh operating system. Either OS9 or OSX
 *	- #TARG_OS_UNIX. The target is unix.
 *	- #TARG_OS_DARWIN. The target is the darwin variant of unix.
 *	- #TARG_OS_WIN32. The target is win32.
 *
 *	Additionally, the specific API is also specified.
 *	- #TARG_API_OSX. The OSX apis can be used.
 *	- #TARG_API_COCOA. The compiler supports Cocoa. Objective C is supported.
 *	- #TARG_API_POWERPLANT. No cocoa support. Interface is PowerPlant
 *
 *	Also, the "endian-ness" of the target system can be determined as:
 *	- #TARG_BIGENDIAN. Set true if the target is BigEndian (multi-byte memory goes from
 *	lsb to msb.
 *	- #TARG_LITTLENDIAN. Set true if the machine stores the most significant byte first in 
 *	multi-byte constructs.
 *
 * Compiler types are also defined:
 *	- #TARG_COMPILER_MWERKS. Set true if the compiler is Metrowerks.
 *	- #TARG_COMPILER_GCC. Set true if the compiler is GCC. 
 *  - #TARG_COMPILER_MS. Set true if microsoft c++
 *
 * Chip-set architecture is also defined:
 *	- #TARG_ARCH_PPC. Set true if the architecture is PowerPC.
 *	- #TARG_ARCH_68K. Set true if the architecture is M68000 series. 
 *  - #TARG_ARCH_PENTIUM. Set true if architecture is Pentium.
 *
 * Finally, #TARG_CPP is set if the compiler is c-plus-plus
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

/// \def TARG_OS_UNIX
///	set to 1 if the target is a unix variant.

///	\def TARG_OS_DARWIN
///	set to 1 if the target is darwin.

/// \def TARG_OS_MAC
///	set to 1 if the target is a macintosh.

/// \def TARG_OS_WIN32
/// set to 1 if the target is windows.

///	\def TARG_API_OSX
///	set to 1 if OSX API's are supported.

///	\def TARG_API_COCOA
///	set to 1 if targetting Cocoa

///	\def TARG_API_POWERPLANT
///	set to 1 if targetting power plant.

///	\def TARG_BIGENDIAN
///	set to 1 if the target uses lsb->msb byte order in memory.

///	\def TARG_LITTLENDIAN
///	set to 1 if target uses msb->lsb byte order in memory.

///	\def TARG_COMPILER_MWERKS
///	set to 1 if compiler is metrowerks.

///	\def TARG_COMPILER_GCC
///	set to 1 if compiler is gcc.

///	\def TARG_COMPILER_MS
///	set to 1 if compiler is microsoft.

///	\def TARG_ARCH_PPC
///	set to 1 if chip architecture is power pc.

///	\def TARG_ARCH_PENTIUM
///	set to 1 if chip architecture is pentium.

///	\def TARG_ARCH_68K
///	set to 1 if chip architecture is M68000 series.

///	\def TARG_CPP
///	set to 1 if compiler is c++ compiler.

#ifndef _H_DlPlatform

#ifdef __cplusplus
#	define TARG_CPP 1
#else
#	define TARG_CPP 0
#endif

#if defined(__MWERKS__)
#	define TARG_COMPILER_MWERKS 1
#	define TARG_COMPILER_GCC 0
#	define TARG_COMPILER_MS 0
#	if defined(__POWERPC__)
#		define TARG_ARCH_PPC 1
#		define TARG_ARCH_PENTIUM 0
#		define TARG_ARCH_68K 0
#		if defined(__MACH__)
#			ifndef __CARBON__
#				include <Carbon/Carbon.h>
#			endif
#			if _MSL_USING_MW_C_HEADERS
#				define _LIMITS_H___
#				define _SYS_SIGNAL_H_
#			endif
#			define TARG_OS_UNIX	1
#			define TARG_API_OSX	1
#			if __option (objective_c)
#				define TARG_API_COCOA		1
#				define TARG_API_POWERPLANT 	0
#			else
#				define TARG_API_COCOA		0
#				define TARG_API_POWERPLANT 	1
#			endif
#		else
#			if USE_PRECOMPILED_MAC_HEADERS
#				include <MacHeadersCarbon++>
#			else
#				include <MacHeaders.c>
#			endif
#			define TARG_OS_UNIX	0
#			define TARG_API_OSX	0
#			define TARG_API_COCOA		0
#			define TARG_API_POWERPLANT 	1
#		endif
#		define TARG_OS_DARWIN	TARG_OS_UNIX
#		define TARG_OS_MAC		1
#		define TARG_OS_WIN32	0
#		define TARG_BIGENDIAN	1
#		define TARG_LITTLENDIAN	0
#	else
#		define TARG_ARCH_PPC 0
#		define TARG_ARCH_PENTIUM 0
#		define TARG_ARCH_68K 1
#	endif
#elif defined(__GNUC__)
#	define TARG_COMPILER_MWERKS 0
#	define TARG_COMPILER_GCC 1
#	define TARG_COMPILER_MS 0
#	define TARG_ARCH_PPC __POWERPC__
#	define TARG_ARCH_PENTIUM __I386__
#	define TARG_ARCH_68K 0
#	include <Carbon/Carbon.h>
#	define TARG_OS_UNIX		1
#	define TARG_OS_DARWIN	1
#	define TARG_OS_MAC		1
#	define TARG_OS_WIN32	0
#	define TARG_BIGENDIAN	__BIG_ENDIAN__
#	define TARG_LITTLENDIAN	__LITTLE_ENDIAN__
#	define TARG_API_OSX	1
#	define TARG_API_COCOA		1
#	define TARG_API_POWERPLANT 	0
#else
#error Target compiler undefined
#endif

#include "DlTypes.h"

#if EXECUTE_DOXYGEN_ONLY
#	define TARG_OS_UNIX		1
#	define TARG_OS_DARWIN	1
#	define TARG_OS_MAC		1
#	define TARG_OS_WIN32	1
#	define TARG_BIGENDIAN	1
#	define TARG_LITTLENDIAN	1
#	define TARG_API_OSX	1
#	define TARG_API_COCOA		1
#	define TARG_API_POWERPLANT 	1
#	define TARG_COMPILER_MWERKS 1
#	define TARG_COMPILER_GCC 1
#	define TARG_COMPILER_MS 1
#	define TARG_ARCH_PPC 1
#	define TARG_ARCH_PENTIUM 1
#	define TARG_ARCH_68K 1
#endif

#endif // _H_DlPlatform

