/*+
 *	File:		DlSets.h
 *
 *	Contains:	Include file for set functions.
 *
 *	The set format is byte aligned with the bits defined in order from the lowest
 *	to the highest byte. Thus, element 0 has a mask of 1 and an index of 0.
 *	The offset is given by value >> 3, and the mask by value & 7.
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


#ifndef	_H_DlSets
#define	_H_DlSets

#include "DlTypes.h"

/* ---------------------------- Declares ----------------------------------- */

typedef		struct DlSet { DlUInt32 val[4]; } DlSet;

#ifdef __cplusplus
extern "C" {
#endif

DlSet		DlMakeFloatSet();

DlBoolean	DlSetMember(DlInt32 value, const DlSet& theSet);
void		DlSetAdd(DlInt32 value, DlSet& theSet);
void		DlSetRemove(DlInt32 value, DlSet& theSet);

DlSet		DlSetFromString(const char* string);

DlSet		DlSetUnion(const DlSet& set1, const DlSet& set2);
DlSet		DlSetSect(const DlSet& set1, const DlSet& set2);
DlSet		DlSetNotSect(const DlSet& set1, const DlSet& set2);
DlSet		DlSetDiff(const DlSet& set1, const DlSet& set2);

DlSet		DlSetEmpty();
DlBoolean	DlSetIsEmpty(const DlSet& test);

DlSet		DlSetNot(const DlSet& input);

#ifdef __cplusplus
}
#endif

#endif
