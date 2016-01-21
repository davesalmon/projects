//345678901234567890123456789012345678901234567890123456789012345678901234567890
/*+
 *	File:		GPSRenum.h
 *
 *	Contains:	renumbering interface
 *
 *	Written by:	David C. Salmon
 *
 *	Copyright:	COPYRIGHT (C) 2001 By David C. Salmon.  *WORLDWIDE RIGHTS RESERVED*
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

#ifndef _H_GPSRenum
#define _H_GPSRenum

//---------------------------------- Includes ----------------------------------

#include "DlTypes.h"
#include <valarray>
#include <memory>

//#define _DEBUG

//---------------------------------- Class -------------------------------------

class GPSRenum
{
	typedef std::valarray<DlInt32> array_type;

public:
	GPSRenum(const array_type & connect, const array_type & diag, bool profile=true);

	bool isBetter() const { return itsBetter; }
	
	DlInt32 mapDof( DlInt32 i ) const { return itsNew[i]; }
	DlInt32 invMapDof( DlInt32 i ) const { return itsBetter ? itsReverse[i] : itsNew[i]; }

	DlInt32 size() const { return itsNeq; }

private:

	//	this is one based
	DlInt32 * getWork( DlInt32 offset ) { return itsWorkingSet.get() + offset - 1; }

	void buildConnect(const array_type & connect, const array_type & starts);

	void minimizeBandProfile();
	
	void findPseudoDiameter( DlInt32 avail, DlInt32 nLeft, 
			DlInt32 & stNode, DlInt32 & rvNode, DlInt32 * trees[], 
			DlInt32 & fWidth, DlInt32 & bWidth );
	
	void buildLevelTree( DlInt32 stNode, DlInt32 avail, DlInt32 nLeft,
			DlInt32 * list, DlInt32 & width );

	void buildNarrowerTree( DlInt32 stNode, DlInt32 avail,
			DlInt32 maxDepth, DlInt32 * list, DlInt32 & width,
			DlInt32 & maxWidth );

	void transitionToII(DlInt32 avail,   
			DlInt32 * levelList[], DlInt32 * levelPtr[], DlInt32 nextNum, 
			DlInt32 & width1, DlInt32 & width2, bool & oneIs1);

	void convertToLevelNums( const DlInt32 * levelList, 
			const DlInt32 * levelPtr,
			DlInt32 * levelNum, bool reverse );

	void combineRootedTrees( DlInt32 width1,
			DlInt32 width2, DlInt32 * tree1, DlInt32 * tree2, DlInt32 * tmp, DlInt32 worlLen, 
			DlInt32 * inc1, DlInt32 * inc2, DlInt32 * total, bool oneIs1, bool & reverse );

	void findConnectedComps( DlInt32 * status, DlInt32 nReduce, 
			DlInt32 * tmp, DlInt32 maxComp, DlInt32 * start, 
			DlInt32 * size, DlInt32 & compns );

	void transitionToIII( DlInt32 * lStruct, DlInt32 * levelList, 
			DlInt32 * levelPtr,
			DlInt32 * lTotal );

	void cutHillMckeeRenum( DlInt32 * invNum, DlInt32 sNode1, DlInt32 sNode2,
			bool reverse, DlInt32 * levelList, DlInt32 * levelPtr, 
			DlInt32 * levelNum );

	void kingRenum( DlInt32 workLen, DlInt32 & nextNum, DlInt32 * levelList, 
			DlInt32 * levelPtr, DlInt32 * levelNum );

	void compBandProfile( DlInt32 * newNum);
	void compBandProfileWithRev( DlInt32 * newNum );

	void sortIndexListDecreasing( DlInt32 n, DlInt32 * key, DlInt32 * data );

	void sortListDecreasing( DlInt32 n, DlInt32 * key );

	void sortDegreeDecreasing( DlInt32 n, DlInt32 * index );

	void sortDegreeIncreasing( DlInt32 n, DlInt32 * index );

	#ifdef _DEBUG
	void printTree( char * msg, DlInt32 avail, const DlInt32 * tree, DlInt32 depth, 
			DlInt32 active, DlInt32 width );
	void printVector( char * msg, DlInt32 n, const DlInt32 * v );
	#endif

	DlInt32	itsNeq;
	DlInt32	itsNonZero;

	DlInt32 itsActive;
	DlInt32 itsDepth;
	DlInt32 itsTree1;
	DlInt32 itsTree2;

	DlInt32		itsBand;
	DlInt32		itsProfile;
	DlInt32		itsWorkSize;

	bool		itsBetter;
	bool		itsOptPro;

	array_type	itsStarts;
	array_type	itsDegree;
	array_type	itsConnect;
	array_type	itsReverse;
	array_type	itsNew;
	std::auto_ptr<DlInt32>	itsWorkingSet;	
};

#endif

