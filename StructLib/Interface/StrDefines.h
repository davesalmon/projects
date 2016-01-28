/*+
 *	File:		StrDefines.h
 *
 *	Contains:	Defines for structures
 *
 *	Written by:	David C. Salmon
 *
 *	Copyright:	COPYRIGHT (C) 1990-96 By David C. Salmon.  *WORLDWIDE RIGHTS RESERVED*
 *
 *
 *	To Do:
 *
 *	04/24/2104 dcs increment version number for property names.
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

#ifndef _H_StrDefines
#define _H_StrDefines

//---------------------------------- Includes ----------------------------------

#include "DlTypes.h"

//---------------------------------- Declares ----------------------------------

typedef DlUInt32 LoadCase;

enum {
	  XCoord = 0
	, YCoord
	, TCoord
	, DOF_PER_NODE
};

const int kCurrentMajorVersion = 1;
const int kCurrentMinorVersion = 4;

const DlUInt32 kMaxLoadCases = 5;
const DlUInt32 kInvalidLoadCase = 0xffffffff;
const DlUInt32 kAnyLoadCase = 0xffffffff;

const DlFloat64 kFloatTolerance = 1.0e-9;

inline constexpr DlInt64 MakeVersion(DlInt32 major, DlInt32 minor)
{
	return ((DlInt64)major << 32) + minor;
}

// the version that first contains load case combos
constexpr DlInt64 kFrameVersionCombos = MakeVersion(1, 3);

// define the first version titles for properties.
constexpr DlInt64 kFrameVersionPropertyTitles = MakeVersion(1, 2);

constexpr DlInt64 kFrameVersionDynamicLoadCounts = MakeVersion(1, 4);

#endif

//	eof