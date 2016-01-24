/*+
 *	File:		DlSets.c
 *
 *	Contains:	DlSet implementation
 *
 *	Written by:	David C. Salmon
 *
 *	Copyright:	COPYRIGHT (C) 1990-94 by David C. Salmon.  *WORLDWIDE RIGHTS RESERVED*
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

#include 	"DlPlatform.h"
#include	"DlSets.h"
#if !TARG_OS_DARWIN
#include	<Resources.h>
#endif

static const char * const kFloatChars = "0123456789.eE+-";

const DlUInt32 kMasks[32] = {
	  0x01000000	// 0b0000 0001 0000 0000 0000 0000 0000 0000
	, 0x02000000	// 0b0000 0010 0000 0000 0000 0000 0000 0000,
	, 0x04000000	// 0b0000 0100 0000 0000 0000 0000 0000 0000,
	, 0x08000000	// 0b0000 1000 0000 0000 0000 0000 0000 0000,
	, 0x10000000	// 0b0001 0000 0000 0000 0000 0000 0000 0000,
	, 0x20000000	// 0b0010 0000 0000 0000 0000 0000 0000 0000,
	, 0x40000000	// 0b0100 0000 0000 0000 0000 0000 0000 0000,
	, 0x80000000	// 0b1000 0000 0000 0000 0000 0000 0000 0000,

	, 0x00010000	// 0b0000 0000 0000 0001 0000 0000 0000 0000,
	, 0x00020000	// 0b0000 0000 0000 0010 0000 0000 0000 0000,
	, 0x00040000	// 0b0000 0000 0000 0100 0000 0000 0000 0000,
	, 0x00080000	// 0b0000 0000 0000 1000 0000 0000 0000 0000,
	, 0x00100000	// 0b0000 0000 0001 0000 0000 0000 0000 0000,
	, 0x00200000	// 0b0000 0000 0010 0000 0000 0000 0000 0000,
	, 0x00400000	// 0b0000 0000 0100 0000 0000 0000 0000 0000,
	, 0x00800000	// 0b0000 0000 1000 0000 0000 0000 0000 0000,

	, 0x00000100	// 0b0000 0000 0000 0000 0000 0001 0000 0000,
	, 0x00000200	// 0b0000 0000 0000 0000 0000 0010 0000 0000,
	, 0x00000400	// 0b0000 0000 0000 0000 0000 0100 0000 0000,
	, 0x00000800	// 0b0000 0000 0000 0000 0000 1000 0000 0000,
	, 0x00001000	// 0b0000 0000 0000 0000 0001 0000 0000 0000,
	, 0x00002000	// 0b0000 0000 0000 0000 0010 0000 0000 0000,
	, 0x00004000	// 0b0000 0000 0000 0000 0100 0000 0000 0000,
	, 0x00008000	// 0b0000 0000 0000 0000 1000 0000 0000 0000,
	
	, 0x00000001	// 0b0000 0000 0000 0000 0000 0000 0000 0001,
	, 0x00000002	// 0b0000 0000 0000 0000 0000 0000 0000 0010,
	, 0x00000004	// 0b0000 0000 0000 0000 0000 0000 0000 0100,
	, 0x00000008	// 0b0000 0000 0000 0000 0000 0000 0000 1000,
	, 0x00000010	// 0b0000 0000 0000 0000 0000 0000 0001 0000,
	, 0x00000020	// 0b0000 0000 0000 0000 0000 0000 0010 0000,
	, 0x00000040	// 0b0000 0000 0000 0000 0000 0000 0100 0000,
	, 0x00000080	// 0b0000 0000 0000 0000 0000 0000 1000 0000
	};

/* ---------------------------- Defines ------------------------------------ */

inline DlUInt32& _GetValue(DlInt32 value, DlUInt32* s) {
	return s[(value & 0x7f) >> 5];
}

inline DlUInt32 _GetValue(DlInt32 value, const DlUInt32* s) {
	return s[(value & 0x7f) >> 5];
}

inline DlUInt32 _GetMask(DlInt32 value) {
	return kMasks[value & 0x1f];
}

inline void 
setIndex(DlInt32 value, DlInt32 & offset, DlUInt32 & mask) 
{
	offset = (value & 0x7f) >> 5;
	mask = kMasks[value & 0x1f];
//	offset = (value & 127) >> 3;
//	mask = 1 << (value & 7);
}

/* ---------------------------- Routines ----------------------------------- */

/* ----------------------------------------------------------------------------
 * makeFloatSet	-	returns the set of valid characters for a floating point #
 *
 * (-> input param, <- output param, <-> i/o param)
 * 
 * sPtr			->	pointer to set storage
 * return		<-	pointer to set or nil
 * ----------------------------------------------------------------------------
 */
DlSet
DlMakeFloatSet()
{
	return DlSetFromString(kFloatChars);
}


/* ----------------------------------------------------------------------------
 * DlSetFromString	-	turns a character string into a set
 *
 * (-> input param, <- output param, <-> i/o param)
 * 
 * s			->	the string to turn into a set
 * theSet			<->	storage for set
 * return			<-	pointer to result
 * ----------------------------------------------------------------------------
 */
DlSet
DlSetFromString(const char * s)
{
	DlSet theSet = DlSetEmpty();
	while(*s) {
		DlSetAdd((unsigned)(*s++), theSet);
	}
	return theSet;
}

/* ----------------------------------------------------------------------------
 * DlSetMember	-	Test for membership
 *
 * (-> input param, <- output param, <-> i/o param)
 * 
 * value		->	value to test
 * theSet		->	the set to test
 * return		<-	true if member
 * ----------------------------------------------------------------------------
 */
DlBoolean
DlSetMember(DlInt32 value, const DlSet& theSet)
{
	return (_GetValue(value, theSet.val) & _GetMask(value)) != 0;
}


/* ----------------------------------------------------------------------------
 * DlSetAdd		-	adds an element to a set
 *
 * (-> input param, <- output param, <-> i/o param)
 * 
 * value		->	the value to add
 * theSet		->	the set to add it to
 * ----------------------------------------------------------------------------
 */
void
DlSetAdd(DlInt32 value, DlSet& theSet)
{
	_GetValue(value, theSet.val) |= _GetMask(value);
//	DlInt32			offset;
//	DlUInt32	mask;
//	setIndex(value, offset, mask);
//	theSet[offset] |= mask;
}

/* ----------------------------------------------------------------------------
 * DlSetRemove	-	remove an element from a set
 *
 * (-> input param, <- output param, <-> i/o param)
 * 
 * value		->	the value to remove
 * theSet		->	the set to remove it from
 * ----------------------------------------------------------------------------
 */
void
DlSetRemove(DlInt32 value, DlSet& theSet)
{
	_GetValue(value, theSet.val) &= ~_GetMask(value);
}

/* ----------------------------------------------------------------------------
 * DlSetUnion	-	returns the union of two sets
 *
 * (-> input param, <- output param, <-> i/o param)
 * 
 * set1			->	first set
 * set2			->	second set
 * setUnion		<->	storage for union
 * return		<-	pointer to storage
 * ----------------------------------------------------------------------------
 */
DlSet
DlSetUnion(const DlSet& set1, const DlSet& set2)
{
	DlSet setUnion;
	setUnion.val[0] = set1.val[0] | set2.val[0];
	setUnion.val[1] = set1.val[1] | set2.val[1];
	setUnion.val[2] = set1.val[2] | set2.val[2];
	setUnion.val[3] = set1.val[3] | set2.val[3];
	return setUnion;
}


/* ----------------------------------------------------------------------------
 * DlSetSect	-	set intersection
 *
 * (-> input param, <- output param, <-> i/o param)
 * 
 * set1			->	first set
 * set2			->	second set
 * intersection	<->	storage for intersection
 * return		<-	pointer to intersection
 * ----------------------------------------------------------------------------
 */
DlSet
DlSetSect(const DlSet& set1, const DlSet& set2)
{
	DlSet intersection;
	intersection.val[0] = set1.val[0] & set2.val[0];
	intersection.val[1] = set1.val[1] & set2.val[1];
	intersection.val[2] = set1.val[2] & set2.val[2];
	intersection.val[3] = set1.val[3] & set2.val[3];
	return intersection;
}


/* ----------------------------------------------------------------------------
 * DlSetNotSect	-	complement of the intersection between sets
 *
 * (-> input param, <- output param, <-> i/o param)
 * 
 * set1			->	first set
 * set2			->	second set
 * result		<->	storage for result
 * return		<-	pointer to result
 * ----------------------------------------------------------------------------
 */
DlSet
DlSetNotSect(const DlSet& set1, const DlSet& set2)
{
	DlSet result;
	result.val[0] = ~(set1.val[0] & set2.val[0]);
	result.val[1] = ~(set1.val[1] & set2.val[1]);
	result.val[2] = ~(set1.val[2] & set2.val[2]);
	result.val[3] = ~(set1.val[3] & set2.val[3]);
	return result;
}


/* ----------------------------------------------------------------------------
 * DlSetDiff	-	returns set1 - set2
 *
 * Really difference between set1 and intersection of sets
 * 
 * (-> input param, <- output param, <-> i/o param)
 * 
 * set1			->	first operand
 * set2			->	second operand
 * result		<->	resulting set storage
 * return		<-	pointer to result
 * ----------------------------------------------------------------------------
 */
DlSet
DlSetDiff(const DlSet& set1, const DlSet& set2)
{
	DlSet result;
	result.val[0] = set1.val[0] - (set1.val[0] & set2.val[0]);
	result.val[1] = set1.val[1] - (set1.val[1] & set2.val[1]);
	result.val[2] = set1.val[2] - (set1.val[2] & set2.val[2]);
	result.val[3] = set1.val[3] - (set1.val[3] & set2.val[3]);
	return result;
}


/* ----------------------------------------------------------------------------
 * DlSetEmpty	-	Create an empty set
 *
 * (-> input param, <- output param, <-> i/o param)
 * 
 * empty		<->	set to empty
 * return		<-	pointer to set
 * ----------------------------------------------------------------------------
 */
DlSet
DlSetEmpty()
{
	DlSet empty = {{ 0,0,0,0 }};
	return  empty;
}


/* ----------------------------------------------------------------------------
 * DlSetIsEmpty	-	Test for empty set
 *
 * (-> input param, <- output param, <-> i/o param)
 * 
 * test			->	set to test
 * return		<-	true if empty
 * ----------------------------------------------------------------------------
 */
DlBoolean
DlSetIsEmpty(const DlSet& test)
{
	return !test.val[0] && !test.val[1] && !test.val[2] && !test.val[3];
}


/* ----------------------------------------------------------------------------
 * DlSetNot		-	complement of a set
 *
 * (-> input param, <- output param, <-> i/o param)
 * 
 * input		->	set to complement
 * result		->	complement of set
 * return		<-	pointer to set
 * ----------------------------------------------------------------------------
 */
DlSet
DlSetNot(const DlSet& input)
{
	DlSet result;
	
	result.val[0] = ~input.val[0];
	result.val[1] = ~input.val[1];
	result.val[2] = ~input.val[2];
	result.val[3] = ~input.val[3];
	return result;
}

//	eof
