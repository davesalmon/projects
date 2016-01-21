/*+
 *	File:		GPSRenum.cpp
 *
 *	Contains:	dof renumbering to minimize bandwidth.
 *
 *				Port of GPS algorithm by Gibbs, Poole, and Stockmeyer
 *				and Gibbs, King. from ACM Transactions on Mathematical
 *				Software, Vol. 8, No. 2, 1982.
 *
 *	Written by:	David C. Salmon
 *
 *	Copyright:	COPYRIGHT (C) 2001 By David C. Salmon.  *WORLDWIDE RIGHTS RESERVED*
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

#include "DlPlatform.h"
#include "recipes.h"
#include "GPSRenum.h"
//#include "DlAssert.h"

#ifdef DlDebugging
#define assertOrThrow(cond) if (!(cond)) { DlAssert( #cond, __FILE__, __LINE__ ); }
#else
#define assertOrThrow(cond) if (!(cond)) { throw -1; }
#endif


#ifdef _PRINT
#include <cstdio>
#endif

#include <cstdlib>

#include "DlArray.h"
using namespace DlArray;

//------------------------------------------------------------------------------
//	GPSRenum::GPSRenum												constructor
//
//		Construct the renumbering. The connectivity array has entries only for
//		diagonal elements and the non-diagonal elements in the upper right.
//		starts are the indices of the diagonal elements.
//
//	connect				->	one-based array of connectivity for upper triangle.
//	starts				->	list of indices of diagonal elements.
//------------------------------------------------------------------------------
GPSRenum::GPSRenum(const array_type & connect, const array_type & starts, bool profile)
	: itsNeq(static_cast<DlInt32>(starts.size()) - 1)
	, itsNonZero(static_cast<DlInt32>(connect.size()))
	, itsActive(0)
    , itsDepth(0)
    , itsTree1(0)
    , itsTree2(0)
    , itsBand(0)
    , itsProfile(0)
    , itsWorkSize(6 * itsNeq + 3)
    , itsBetter(false)
    , itsOptPro(profile)
	, itsWorkingSet((DlInt32*)operator new(sizeof(DlInt32)*(itsWorkSize + 1)))
{
	std::memset(itsWorkingSet.get(), 0, (itsWorkSize + 1) * sizeof(DlInt32));
	//	the number of equations is diag.size	
	itsNew.resize(itsNeq +1);

	buildConnect(connect, starts);
	
	minimizeBandProfile();
	
	if (itsBetter)
	{
		itsReverse.resize(itsNeq+1);
		for (DlInt32 i = 1; i <= itsNeq; i++)
			itsReverse[i] = itsWorkingSet.get()[i];
	}

	delete itsWorkingSet.release();
}

//------------------------------------------------------------------------------
//	GPSRenum::buildConnect
//
//		Build the full adjacency graph from the upper right.
//
//		Initilizes:
//			itsConnect		the full adjacency graph
//			itsDegree		the size of each row
//			itsStarts		the indices of the diagonal elements
//
//	connect				->	one-based array of connectivity for upper triangle.
//	starts				->	list of indices of diagonal elements.
//------------------------------------------------------------------------------
void
GPSRenum::buildConnect(const array_type & connect, const array_type & starts)
{
	DlInt32	i, j;
	DlInt32 * starts2 = getWork(1);
	itsConnect.resize(2 * itsNonZero - itsNeq + 1);
	itsDegree.resize(itsNeq + 1);
	itsStarts.resize(itsNeq + 2);

	DlOneBasedConstIter<DlInt32> onestarts(starts);
	DlOneBasedConstIter<DlInt32> oneconnect(connect);

	//	compute the initial degree
	for (i = 1; i <= itsNeq; i++)
		itsDegree[i] = onestarts[i+1] - onestarts[i];
	
	//	copy connect elements to itsConnect	
	itsConnect[std::slice(1, connect.size(), 1)] = connect;

	//	now compute the additional number of off diagonals needed for 
	//	the full matrix
	for (j = 1; j < itsNeq; j++) 
	{
		DlInt32 kStart = onestarts[j];
		DlInt32 kStop = onestarts[j+1] - 1;
		while (kStart <= kStop) 
		{
			DlInt32 i = oneconnect[kStart++];
			_RecipesAssert(i >= j && i <= itsNeq);
//			if (i < j || i > itsNeq)
//				throw -1;	//	FIXME
			if (i > j)
				itsDegree[i]++;
		}
	}

	//	compute the new starts vector by adding
	//	the new degrees to the original starts
	//	array.
	starts2[1] = 1;
	if (itsNeq <= 1)
		return;

	for (i = 2; i <= itsNeq; i++)
		starts2[i] = starts2[i-1] + itsDegree[i-1];

	for(j = itsNeq; j >= 1; j--) 
	{
		//	start at the end
		DlInt32 jStart = starts2[j];				//	new start
		DlInt32 kStart = onestarts[j];					//	original start
		DlInt32 nk = onestarts[j+1] - kStart;			//	original number of elements
		if (nk > 0) 
		{
			DlInt32 rk = nk;					//	offset
			while(--rk >= 0) 
			{
				DlInt32 k = kStart + rk;			//	original connect position
				DlInt32 jk = jStart + rk;			//	new connect position
				DlInt32 i = itsConnect[k];				//	original connect
				itsConnect[jk] = i;			//	set new connect
				if (i != j) 
				{				//	upper triangle
					DlInt32 iStart = starts2[i];	//	new connect
					itsConnect[iStart] = j;	//	add to connect
					starts2[i] = iStart + 1;//	add starts to sym element
				}
			}
			starts2[j] = jStart + nk;		//	total for j
		}
	}

	for (j = 1; j <= itsNeq; j++) 
	{
		itsStarts[j] = starts2[j] - itsDegree[j];
		itsNew[j] = j;
	}
	
	itsStarts[itsNeq+1] = itsStarts[itsNeq] + itsDegree[itsNeq];
}

//------------------------------------------------------------------------------
//	GPSRenum::minimizeBandProfile
//
//		minimize the bandwidth or profile for the equations given by 
//		inp. Place the permutation into out.
//
//------------------------------------------------------------------------------
void
GPSRenum::minimizeBandProfile()
{
    DlInt32	i;	
	bool	revrs1;
	bool	oneIs1;
	
	DlInt32 *	treeArray[4];
	DlInt32 *	levelArray[4];

	DlInt32 	nextNum = 1;

//	compute the maximum degree and check that all members of itsDegree have
//	at least one element
	DlInt32 maxdg = 0;
    for (i = 1; i <= itsNeq; i++) 
    {
    	assertOrThrow(itsDegree[i] > 0);
//    	if (itsDegree[i] <= 0) 
//    		throw -1;	//	fix me
    	if (itsDegree[i] > maxdg)
    		maxdg = itsDegree[i];
    }

	//	number all the nodes
	while (nextNum <= itsNeq) 
	{

		//	find an unnumbered node with the smallest number of connections
		DlInt32 lowdg = maxdg + 1;
		DlInt32 stNode = 0;
		
		for (i = 1; i <= itsNeq; i++) {
			if (itsDegree[i] > 0 && itsDegree[i] < lowdg) {			
				lowdg = itsDegree[i];
				stNode = i;
			}
		}

  		_RecipesAssert(stNode != 0);
//		if (stNode == 0)
//			throw -1;	// ### FIX ME
		
		DlInt32 avail = (itsWorkSize - nextNum + 1) / 3;
		DlInt32 nLeft = itsNeq - nextNum + 1;

		//	set up three arrays for tree. Then find trees with minimal width.

#ifdef _PRINT
		std::printf("nextNum = %d\n", nextNum);
#endif
		treeArray[1] = getWork(nextNum); // set(itsWorkingSet, nextNum);	//	one based
		treeArray[2] = treeArray[1] + avail;
		treeArray[3] = treeArray[2] + avail;
		
		DlInt32 rvNode;
		DlInt32 width1;
		DlInt32 width2;
		findPseudoDiameter(avail, nLeft, stNode, rvNode, treeArray, width1, width2);

		//	locate the ends of each level tree, and build three pointers to the 
		//	level indices. 
		DlInt32 lvlPtr = nextNum + avail - itsDepth;
#ifdef _PRINT
		std::printf("depth = %d, lvlPtr = %d\n", itsDepth, lvlPtr);
#endif
		levelArray[1] = getWork(lvlPtr);
		levelArray[2] = levelArray[1] + avail;
		levelArray[3] = levelArray[2] + avail;
		
		//	the output from findPseudoDiameter is a pair of trees in the form of lists
		//	of nodes by level. Convert this to two lists of level numbers by node. Also,
		//	pack the working storage to move one of the trees to the end of the working
		//	storage.
		transitionToII(avail, treeArray, levelArray, nextNum, width1, width2, oneIs1);

#ifdef _PRINT
		printVector("tree 1", itsNeq, getWork(itsTree1));
		printVector("tree 2", itsNeq, getWork(itsTree2));
#endif

		//	check the location of the trees. Tree1 is at the back of storage
		//	and Tree2 is at the front.
		assertOrThrow(itsTree1 == itsWorkSize - itsNeq + 1 && itsTree2 == nextNum);
//		if (itsTree1 != itsWorkSize - itsNeq + 1 || itsTree2 != nextNum)
//			throw -1;	//	FIX ME

		//	copmpute the available storage. We've used nextNum elements for
		//	numbering, 2 * itsNeq to store the level numbers, and we need
		//	3 arrays of itsDepth to store sTotal, inc1, and inc2. avail
		//	is the size available for empty.
		avail = itsWorkSize - nextNum + 1 - 2 * itsNeq - 3 * itsDepth;
		DlInt32 sTotal = itsNeq + nextNum;
		DlInt32 empty = sTotal + itsDepth;
		DlInt32 inc1 = itsTree1 - itsDepth;
		DlInt32 inc2 = inc1 - itsDepth;
		
/*
C         ... COMBINE THE TWO LEVEL TREES INTO A MORE GENERAL
C             LEVEL STRUCTURE.
*/
		combineRootedTrees(width1, width2, 
			getWork(itsTree1), 
			getWork(itsTree2),
			getWork(empty), 
			avail, 
			getWork(inc1), 
			getWork(inc2),
			getWork(sTotal), oneIs1, revrs1);
		
#ifdef _PRINT
		printVector("combined tree 1", itsNeq, getWork(itsTree1));
#endif
		
		lvlPtr = itsTree1 - (itsDepth + 1);
		DlInt32 lvlList = lvlPtr - itsActive;
		assertOrThrow(sTotal + itsDepth <= lvlPtr);
//		if (sTotal + itsDepth > lvlPtr)
//			throw -1; //	FIX ME
/*
C         ... COMBINED LEVEL STRUCTURE IS REPRESENTED BY combineRootedTrees AS
C             A VECTOR OF LEVEL NUMBERS.  FOR RENUMBERING PHASE,
C             CONVERT THIS ALSO TO THE INVERSE PERMUTATION.
*/
		transitionToIII(
			getWork(itsTree1), 
			getWork(lvlList),
			getWork(lvlPtr), 
			getWork(sTotal));

/*
C         ... NOW RENUMBER ALL MEMBERS OF THIS COMPONENT USING
C             EITHER A REVERSE CUTHILL-MCKEE OR A KING STRATEGY,
C             AS PROFILE OR BANDWIDTH REDUCTION IS MORE IMPORTANT.
*/
		if (!itsOptPro) 
		{
			cutHillMckeeRenum(
				getWork(nextNum), 
				stNode, rvNode, revrs1,  
				getWork(lvlList), 
				getWork(lvlPtr), 
				getWork(itsTree1));
			nextNum = nextNum + itsActive;
		} 
		else 
		{
			kingRenum(lvlList - 1, nextNum, 
				getWork(lvlList), 
				getWork(lvlPtr), 
				getWork(itsTree1));
		}

	}	//	end while

	
/*
C     ... CHECK WHETHER INITIAL NUMBERING OR FINAL NUMBERING
C         PROVIDES BETTER RESULTS
*/
	if (!itsOptPro) 
	{
		compBandProfile(getWork(itsNeq + 1));
	} 
	else 
	{
		compBandProfileWithRev(getWork(itsNeq+1));
	}
}

//------------------------------------------------------------------------------
//	GPSRenum::findPseudoDiameter
//
//		build two level trees, one from the top and the other from the bottom.
//
//		build level tree from stNode
//		repeat
//			build level tree from each node in the last level of tree
//			replace stNode with new node if a deeper-narrower tree found.
//		until there is no improvement.
//
//		the tree node are placed at the beginning of each tree array.
//		the indices of the start of each level are built starting at the
//		end of each tree array.
//
//		The tree:
//
//			1
//		2		5
//	  3  7    4   6
//
//	looks like
//
//	1 2 5 3 7 4 6 ... 8 4 2 1
//
//	avail				->	the space available for tree and level indices
//	nLeft				->	the number of un-numbered nodes.
//	stNode				<->	the top node for narrowest tree.
//	rvNode				<-	the bottom node for narrowest tree
//	trees				<-	array of trees
//	fWidth				<-	width of top-down tree
//	bWidth				<-	width of bottom-up tree
//------------------------------------------------------------------------------
void
GPSRenum::findPseudoDiameter(DlInt32 avail, DlInt32 nLeft, 
		DlInt32 & stNode, DlInt32 & rvNode, DlInt32 * trees[],  
		DlInt32 & fWidth, DlInt32 & bWidth)
{
	itsTree1 = 1;
	itsTree2 = 3;

	DlInt32 backward = 2;

//	build initial level tree from stNode.
//	Set itsActive with the number of nodes in the tree.

	DlInt32 width;
	buildLevelTree(stNode, avail, nLeft, trees[itsTree1], width);

	//	this is the maximum depth we can store.
	DlInt32 maxDepth = avail - itsActive - 1;
/*
C     ==========================================
C     REPEAT UNTIL NO DEEPER TREES ARE FOUND ...
C     ==========================================
*/
	bool improv = true;
	DlInt32 fDepth = 0;
	while(improv) 
	{
		fWidth = width;
		fDepth = itsDepth;
		DlInt32 lastLevel = avail - itsDepth + 1;
		DlInt32 nLast = trees[itsTree1][lastLevel - 1] - trees[itsTree1][lastLevel];
		lastLevel = trees[itsTree1][lastLevel];
		bWidth = itsNeq + 1;

/*
C         ... SORT THE DEEPEST LEVEL OF 'FORWD' TREE INTO INCREASING
C             ORDER OF NODE DEGREE.
*/
		sortDegreeIncreasing(nLast, trees[itsTree1] + lastLevel - 1);
	
#ifdef _PRINT
		printTree("after sort", avail, trees[itsTree1], itsDepth, itsActive, width);
		std::printf("last level = %ld\n", lastLevel);
#endif
		improv = false;
		
		//	for each node in the last level of the tree
		for (DlInt32 i = 1; i <= nLast; i++)
		{
			//	find last node in tree
			DlInt32 backNode = trees[itsTree1][lastLevel + i - 1];
			
/*
C         ... BUILD LEVEL TREE FROM NODES IN 'LSTLVL' UNTIL A DEEPER
C             AND NARROWER TREE IS FOUND OR THE LIST IS EXHAUSTED.
*/
			buildNarrowerTree(backNode, avail, maxDepth, trees[backward], width, bWidth);			
/*
C                 ... NEW DEEPER TREE ... MAKE IT NEW 'FORWD' TREE
C                     AND BREAK OUT OF 'DO' LOOP.
*/
			if (itsDepth > fDepth) 
			{
				improv = true;
				DlInt32 t = itsTree1;
				itsTree1 = backward;
				backward = t;
				stNode = backNode;
				break;
			}
/*	
C
C                 ... ELSE CHECK FOR NARROWER TREE.
C
*/
			else if (width < bWidth) 
			{
				DlInt32 t = itsTree2;
				itsTree2 = backward;
				backward = t;
				bWidth = width;
				rvNode = backNode;
			}
		}
/*
C         ... END OF REPEAT LOOP
C         ----------------------
*/
	}
	itsDepth = fDepth;
}

void 
GPSRenum::buildLevelTree(DlInt32 stNode, DlInt32 avail, DlInt32 nLeft,
	DlInt32 * list, DlInt32 & width)
{
/*
C     ==================================================================
C     BUILD THE LEVEL TREE ROOTED AT 'STNODE' IN THE SPACE PROVIDED IN
C     LIST.  CHECK FOR OVERRUN OF SPACE ALLOCATION.
C     ==================================================================
C
C     ... PARAMETERS:
C
C         INPUT ...
C
C             N, DEGREE, RSTART, CONNEC -- DESCRIBE THE MATRIX STRUCTURE
C
C             STNODE -- THE ROOT OF THE LEVEL TREE.
C
C             AVAIL  -- THE LENGTH OF THE WORKING SPACE AVAILABLE
C
C             NLEFT  -- THE NUMBER OF NODES YET TO BE NUMBERED
C
C             LIST   -- THE WORKING SPACE.
C
C         OUTPUT ...
C
C             ACTIVE -- THE NUMBER OF NODES IN THE COMPONENT
C
C             DEPTH  -- THE DEPTH OF THE LEVEL TREE ROOTED AT  STNODE.
C
C             WIDTH  -- THE WIDTH OF THE LEVEL TREE ROOTED AT  STNODE.
C
C             ERROR  -- ZERO UNLESS STORAGE WAS INSUFFICIENT.
C
C     ------------------------------------------------------------------
C
*/
/*
C     ... BUILD THE LEVEL TREE USING  LIST  AS A QUEUE AND LEAVING
C         THE NODES IN PLACE.  THIS GENERATES THE NODES ORDERED BY LEVEL
C         PUT POINTERS TO THE BEGINNING OF EACH LEVEL, BUILDING FROM
C         THE BACK OF THE WORK AREA.
*/
	DlInt32 lStart = 1;
	DlInt32 front = 1;
	DlInt32 nLevel = avail;

	itsActive = 1;
	itsDepth = 0;
	width = 0;
	list[itsActive] = stNode;
	itsDegree[stNode] = -itsDegree[stNode];
	list[avail] = 1;

	while(1) 
	{
/*
C     ... REPEAT UNTIL QUEUE BECOMES EMPTY OR WE RUN OUT OF SPACE.
C     ------------------------------------------------------------
*/
		if (front >= lStart) 
		{
			lStart = itsActive + 1;
			width = std::max(width, lStart - list[nLevel]);
			nLevel--;
			itsDepth++;
			assertOrThrow(nLevel > itsActive)
//			if (nLevel <= itsActive)
//				throw -1;	// FIXME 
			
			list[nLevel] = lStart;
		}

/*
C         ... FIND ALL NEIGHBORS OF CURRENT NODE, ADD THEM TO QUEUE.
*/
		DlInt32 lFront = list[front];
		DlInt32 ptr = itsStarts[lFront];
		DlInt32 cDegree = -itsDegree[lFront];
		
		assertOrThrow(cDegree > 0);
//		if (cDegree <= 0)
//			throw -1;	// FIXME 
	
		for (DlInt32 j = 1; j <= cDegree; j++) 
		{
			DlInt32 newNode = itsConnect[ptr];
			ptr++;

/*
C             ... ADD TO QUEUE ONLY NODES NOT ALREADY IN QUEUE
*/
			if (itsDegree[newNode] > 0) 
			{
				itsDegree[newNode] = -itsDegree[newNode];
				itsActive++;
				assertOrThrow(nLevel > itsActive && itsActive <= nLeft);
//				if (nLevel <= itsActive)
//					throw -1;
//				if (itsActive > nLeft)
//					throw -1;
				list[itsActive] = newNode;
			}
		}
		
		front++;

/*
C         ... IS QUEUE EMPTY?
C         -------------------
*/
		if (front > itsActive)
			break;
	}

/*
C     ... YES, THE TREE IS BUILT.  UNDO OUR MARKINGS.
*/
	for (DlInt32 j = 1; j <= itsActive; j++) 
	{
		itsDegree[list[j]] = -itsDegree[list[j]];
	}
#ifdef _PRINT
	printTree("from front", avail, list, itsDepth, itsActive, width);
#endif
}

void
GPSRenum::buildNarrowerTree(DlInt32 stNode, DlInt32 avail,
		DlInt32 maxDepth, DlInt32 * list, DlInt32 & width,
		DlInt32 & maxWidth)
{
/*
C
C     ==================================================================
C     BUILD THE LEVEL TREE ROOTED AT 'STNODE' IN THE SPACE PROVIDED IN
C     LIST.  OVERFLOW CHECK NEEDED ONLY ON DEPTH OF TREE.
C
C     BUILD THE LEVEL TREE TO COMPLETION ONLY IF THE WIDTH OF ALL
C     LEVELS IS SMALLER THAN 'MAXWID'.  IF A WIDER LEVEL IS FOUND
C     TERMINATE THE CONSTRUCTION.
C     ==================================================================
C
C     ... PARAMETERS:
C
C         INPUT ...
C
C             N, DEGREE, RSTART, CONNEC -- DESCRIBE THE MATRIX STRUCTURE
C
C             STNODE -- THE ROOT OF THE LEVEL TREE.
C
C             AVAIL  -- THE LENGTH OF THE WORKING SPACE AVAILABLE
C
C             NLEFT  -- THE NUMBER OF NODES YET TO BE NUMBERED
C
C             ACTIVE -- THE NUMBER OF NODES IN THE COMPONENT
C
C             MXDPTH -- MAXIMUM DEPTH OF LEVEL TREE POSSIBLE IN
C                       ALLOTTED WORKING SPACE
C
C             LIST   -- THE WORKING SPACE.
C
C         OUTPUT ...
C
C             DEPTH  -- THE DEPTH OF THE LEVEL TREE ROOTED AT  STNODE.
C
C             WIDTH  -- THE WIDTH OF THE LEVEL TREE ROOTED AT  STNODE.
C
C             MAXWID -- LIMIT ON WIDTH OF THE TREE.  TREE WILL NOT BE
C                       USED IF WIDTH OF ANY LEVEL IS AS GREAT AS
C                       MAXWID, SO CONSTRUCTION OF TREE NEED NOT
C                       CONTINUE IF ANY LEVEL THAT WIDE IS FOUND.
C             ERROR  -- ZERO UNLESS STORAGE WAS INSUFFICIENT.
C
C     ------------------------------------------------------------------
C
C     ... BUILD THE LEVEL TREE USING  LIST  AS A QUEUE AND LEAVING
C         THE NODES IN PLACE.  THIS GENERATES THE NODES ORDERED BY LEVEL
C         PUT POINTERS TO THE BEGINNING OF EACH LEVEL, BUILDING FROM
C         THE BACK OF THE WORK AREA.
*/
	DlInt32 lStart = 1;
	DlInt32 front = 1;
	DlInt32 nLevel = avail;
	DlInt32 back = 1;

	itsDepth = 0;
	width = 0;
	list[back] = stNode;
	itsDegree[stNode] = -itsDegree[stNode];
	list[avail] = 1;

	while(1) 
	{
/*
C     ... REPEAT UNTIL QUEUE BECOMES EMPTY OR WE RUN OUT OF SPACE.
C     ------------------------------------------------------------
*/
		if (front >= lStart) 
		{
			lStart = back + 1;
			width = std::max(lStart - list[nLevel], width);
			if (width >= maxWidth) {
				width = itsNeq + 1;
				itsDepth = 0;
				goto done;
			}
			nLevel--;
			itsDepth++;
			assertOrThrow(itsDepth <= maxDepth);
//			if (itsDepth > maxDepth)
//				throw -2; // fixme
			
			list[nLevel] = lStart;
		}

/*
C         ... FIND ALL NEIGHBORS OF CURRENT NODE, ADD THEM TO QUEUE.
*/
		DlInt32 lFront = list[front];
		DlInt32 sPtr = itsStarts[lFront];
		DlInt32 fPtr = sPtr - itsDegree[lFront] - 1;
		
		for (DlInt32 ptr = sPtr; ptr <= fPtr; ptr++) 
		{
			DlInt32 newNode = itsConnect[ptr];
/*
C             ... ADD TO QUEUE ONLY NODES NOT ALREADY IN QUEUE
*/
			if (itsDegree[newNode] > 0) 
			{
				itsDegree[newNode] = -itsDegree[newNode];
				back++;
				list[back] = newNode;
			}
		}
		
		front++;

/*
C         ... IS QUEUE EMPTY?
C         -------------------
*/
		if (front > back)
			break;
	}

	assertOrThrow(back == itsActive);
//	if (back != itsActive)
//		throw -1;	// fixme

done:
	for (DlInt32 j = 1; j <= back; j++) 
	{
		DlInt32 t = list[j];
		itsDegree[t] = -itsDegree[t];
	}

#ifdef _PRINT
	if (itsDepth != 0)
		printTree("narrower tree", avail, list, itsDepth, itsActive, width);
#endif
}

void
GPSRenum::transitionToII(DlInt32 avail, 
		DlInt32 * levelList[], DlInt32 * levelPtr[], DlInt32 nextNum, 
		DlInt32 & width1, DlInt32 & width2, bool & oneIs1)
{
/*
C     ==================================================================
C
C     TRANSITION BETWEEN ALGORITHM I AND ALGORITHM II OF
C     THE GIBBS-POOLE-STOCKMEYER PAPER.
C
C     IN THIS IMPLEMENTATION ALGORITHM I REPRESENTS LEVEL TREES AS
C     LISTS OF NODES ORDERED BY LEVEL.  ALGORITHM II APPEARS TO REQUIRE
C     LEVEL NUMBERS INDEXED BY NODE -- VECTORS FOR EFFICIENCY.
C     THIS SUBROUTINE CHANGES THE LEVEL TREE REPRESENTATION TO THAT
C     REQUIRED BY ALGORITHM II.  NOTE THAT THE FIRST ALGORITHM CAN BE
C     CARRIED OUT WITH THE LEVEL NUMBER VECTOR FORMAT, PROBABLY REQURING
C     MORE COMPUTATION TIME, BUT PERHAPS LESS STORAGE.
C
C     INPUT:  TWO LEVEL TREES, AS LEVEL LISTS AND LEVEL POINTERS,
C             FOUND IN TWO OF THE THREE COLUMNS OF THE ARRAYS 'LVLLST'
C             AND 'LVLPTR'
C
C     OUTPUT: TWO LEVEL TREES, AS VECTORS OF LEVEL NUMBERS,
C             ONE PACKED TO THE FRONT, ONE TO THE REAR OF THE WORKING
C             AREA 'WORK'.  NOTE THAT 'WORK', 'LVLLST' AND 'LVLPTR'
C             SHARE COMMON LOCATIONS.
C
C     ================================================================
C
C     ... STRUCTURE OF WORKSPACE
C
C         INPUT .. (OUTPUT FROM findPseudoDiameter)
C
C     --------------------------------------------------------------
C     : NUMBERED : TLIST1  PTR1  :  TLIST2  PTR2  :  TLIST3  PTR3  :
C     --------------------------------------------------------------
C
C         OUTPUT .. (GOES TO COMBIN)
C
C     --------------------------------------------------------------
C     : NUMBERED :  TREE2  :           ...               :  TREE1  :
C     --------------------------------------------------------------
C
C     ==================================================================
*/
/*
C     ... CHECK THAT WE HAVE ENOUGH ROOM TO DO THE NECESSARY UNPACKING
*/
	assertOrThrow(3 * avail <= itsWorkSize && avail >= itsNeq);
//	if (3 * avail > itsWorkSize || avail < itsNeq)
//		throw -2;	// fixme

/*
C     ... INPUT HAS THREE POSSIBLE CASES:
C             LVLLST(*,1) IS EMPTY
C             LVLLST(*,2) IS EMPTY
C             LVLLST(*,3) IS EMPTY
*/
	DlInt32 fTree = itsTree1;
	DlInt32 bTree = itsTree2;
	DlInt32 fWidth = width1;
	DlInt32 bWidth =  width2;
	
	itsTree1 = itsWorkSize - itsNeq + 1;
	itsTree2 = nextNum;

	if (fTree != 1 && bTree != 1) 
	{
/*
C         ... CASE 1:  1ST SLOT IS EMPTY.  UNPACK 3 INTO 1, 2 INTO 3
*/
		if (fTree == 2) 
		{
			oneIs1 = true;
			width1 = fWidth;
			width2 = bWidth;
		} 
		else 
		{
			oneIs1 = false;
			width1 = bWidth;
			width2 = fWidth;
		}
		
		convertToLevelNums(levelList[3], levelPtr[3], getWork(itsTree2), oneIs1);
		convertToLevelNums(levelList[2], levelPtr[2], getWork(itsTree1), !oneIs1);

	} 
/*
C         ... CASE 2:  2ND SLOT IS EMPTY.  TO ENABLE COMPLETE
C              REPACKING, MOVE 3 INTO 2, THEN FALL INTO NEXT CASE
*/
	else 
	{

		if (fTree != 2 && bTree != 2) 
		{
			DlInt32 i;
			for (i = 1; i <= itsActive; i++)
				levelList[2][i] = levelList[3][i]; 

			for (i = 1; i <= itsDepth; i++)
				levelPtr[2][i] = levelPtr[3][i]; 
		}

/*
C         ... CASE 3:  SLOT 3 IS EMPTY.  MOVE 1 INTO 3, THEN 2 INTO 1.
*/
		if (fTree != 1) 
		{
			oneIs1 = false;
			width1 = bWidth;
			width2 = fWidth;
		} 
		else
		{
			oneIs1 = true;
			width1 = fWidth;
			width2 = bWidth;
		}

		convertToLevelNums(levelList[1], levelPtr[1], getWork(itsTree1), !oneIs1);
		convertToLevelNums(levelList[2], levelPtr[2], getWork(itsTree2), oneIs1);
	}
}


void
GPSRenum::convertToLevelNums(const DlInt32 * levelList, const DlInt32 * levelPtr,
	DlInt32 * levelNum, bool reverse)
{
/*
C     ==================================================================
C
C     CONVERT LEVEL STRUCTURE REPRESENTATION FROM A LIST OF NODES
C     GROUPED BY LEVEL TO A VECTOR GIVING LEVEL NUMBER FOR EACH NODE.
C
C     LVLLST, LVLPTR -- LIST OF LISTS
C
C     LVLNUM -- OUTPUT VECTOR OF LEVEL NUMBERS
C
C     REVERS -- IF .TRUE., NUMBER LEVEL STRUCTURE FROM BACK END
C               INSTEAD OF FROM FRONT
C
C     ==================================================================
*/
/*
C         ... IF NOT ALL NODES OF GRAPH ARE ACTIVE, MASK OUT THE
C             NODES WHICH ARE NOT ACTIVE
*/
	if (itsActive != itsNeq) 
	{
		for (DlInt32 i = 1; i <= itsNeq; i++)
			levelNum[i] = 0;
	}
	
	for (DlInt32 level = 1; level <= itsDepth; level++) 
	{
		DlInt32 xLevel = level;
		DlInt32 plStart = itsDepth - level + 1;
		if (reverse)
			xLevel = plStart;
		DlInt32 lStart = levelPtr[plStart];
		DlInt32 lEnd = levelPtr[plStart - 1] - 1;
					

		for (DlInt32 i = lStart; i <= lEnd; i++) 
		{
			DlInt32 lvlLsti = levelList[i];
			levelNum[lvlLsti] = xLevel;
		}
	}
}

void
GPSRenum::combineRootedTrees(DlInt32 width1, DlInt32 width2, 
	DlInt32 * tree1, DlInt32 * tree2, DlInt32 * tmp, DlInt32 workLen,
	DlInt32 * inc1, DlInt32 * inc2, DlInt32 * total, bool oneIs1, bool & reverse)
{
/*
C     ==================================================================
C
C     COMBINE THE TWO ROOTED LEVEL TREES INTO A SINGLE LEVEL STRUCTURE
C     WHICH MAY HAVE SMALLER WIDTH THAN EITHER OF THE TREES.  THE NEW
C     STRUCTURE IS NOT NECESSARILY A ROOTED STRUCTURE.
C
C     PARAMETERS:
C
C         N, DEGREE, RSTART, CONNEC -- GIVE THE DIMENSION AND STRUCTURE
C                                      OF THE SPARSE SYMMETRIC MATRIX
C
C         ACTIVE -- THE NUMBER OF NODES IN THIS CONNECTED COMPONENT OF
C                   THE MATRIX GRAPH
C
C         TREE1  -- ON INPUT, ONE OF THE INPUT LEVEL TREES.  ON
C                   OUTPUT, THE COMBINED LEVEL STRUCTURE
C
C         TREE2  -- THE SECOND INPUT LEVEL TREE
C
C         WIDTH1 -- THE MAXIMUM WIDTH OF A LEVEL IN TREE1
C
C         WIDTH2 -- THE MAXIMUM WIDTH OF A LEVEL IN TREE2
C
C         WORK   -- A WORKING AREA OF LENGTH 'WRKLEN'
C
C         INC1,  -- VECTORS OF LENGTH 'DEPTH'
C         INC2,
C         TOTAL
C
C         ONEIS1 -- INDICATES WHETHER TREE1 OR TREE2 REPRESENTS THE
C                   FORWARD TREE OR THE BACKWARDS TREE OF PHASE 1.
C                   USED TO MIMIC ARBITRARY TIE-BREAKING PROCEDURE OF
C                   ORIGINAL GIBBS-POOLE-STOCKMEYER CODE.
C
C         REVRS1 -- OUTPUT PARAMETER INDICATING WHETHER A BACKWARDS
C                   ORDERING WAS USED FOR THE LARGEST COMPONENT OF
C                   THE REDUCED GRAPH
C
C         ERROR  -- NON-ZERO ONLY IF FAILURE OF SPACE ALLOCATION OR
C                   DATA STRUCTURE ERROR FOUND
C
C         SPACE -- MINIMUM SPACE REQUIRED TO RERUN OR COMPLETE PHASE.
C
C     ------------------------------------------------------------------
C     ==================================================================
C
C     << REMOVE ALL NODES OF PSEUDO-DIAMETERS >>
C     << FIND CONNECTED COMPONENTS OF REDUCED GRAPH >>
C     << COMBINE LEVEL TREES, COMPONENT BY COMPONENT >>
C
C     ==================================================================
C
C     STRUCTURE OF WORKSPACE ...
C
C     ------------------------------------------------------------------
C     : NUMBERED : TREE2 : TOTAL : NODES : START : SIZE : INC1 : INC2 :
C     ------------------------------------------------------------------
C
C     --------
C      TREE1 :
C     --------
C
C         NUMBERED  IS THE SET OF  NUMBERED NODES (PROBABLY EMPTY)
C
C         TREE1 AND TREE1 ARE LEVEL TREES (LENGTH N)
C         TOTAL, INC1 AND INC2  ARE VECTORS OF NODE COUNTS PER LEVEL
C             (LENGTH 'DEPTH')
C         NODES IS THE SET OF NODES IN THE REDUCED GRAPH (THE NODES
C             NOT ON ANY SHORTEST PATH FROM ONE END OF THE
C             PSEUDODIAMETER TO THE OTHER)
C         START, SIZE ARE POINTERS INTO 'NODES', ONE OF EACH FOR
C         EACH CONNECTED COMPONENT OF THE REDUCED GRAPH.
C         THE SIZES OF NODES, START AND SIZE ARE NOT KNOWN APRIORI.
C
C     ==================================================================
C
C     ... FIND ALL SHORTEST PATHS FROM START TO FINISH.  REMOVE NODES ON
C         THESE PATHS AND IN OTHER CONNECTED COMPONENTS OF FULL GRAPH
C         FROM FURTHER CONSIDERATION.  SIGN OF ENTRIES IN TREE1 IS USED
C         AS A MASK.
*/
	DlInt32 i;
	DlInt32 offDiag = itsActive;
	
	for (i = 1; i <= itsDepth; i++)
		total[i] = 0;

	for (i = 1; i <= itsNeq; i++) 
	{
		DlInt32 tree1i = tree1[i];
		if (tree1i == tree2[i] && tree1i != 0) 
		{
			total[tree1i]++;
			tree1[i] = -tree1[i];
			offDiag--;
		}
	}

#ifdef _PRINT
	std::printf("offDiag = %d\n", offDiag);
	printVector("tree1", itsNeq, tree1);
	printVector("tree2", itsNeq, tree2);
	printVector("total", itsDepth, total);
#endif

	if (offDiag != 0) 
	{
		assertOrThrow(offDiag >= 0);
//		if (offDiag < 0)
//			throw -1;

/*
C     ... FIND CONNECTED COMPONENTS OF GRAPH INDUCED BY THE NODES NOT
C         REMOVED.  'MXCOMP' IS THE LARGEST NUMBER OF COMPONENTS
C         REPRESENTABLE IN THE WORKING SPACE AVAILABLE.
*/
		DlInt32 avail = workLen - offDiag;
		DlInt32 maxComp = avail / 2;
		DlInt32 start = offDiag + 1;
		DlInt32 size = start + maxComp;
		DlInt32 compns;

		assertOrThrow(maxComp > 0);
//		if (maxComp <= 0)
//			throw -1;

		findConnectedComps(tree1, offDiag, tmp, maxComp, tmp + start - 1,
			tmp + size - 1, compns);

#ifdef _PRINT
		printVector("start", compns, tmp + start - 1);
		printVector("size", compns, tmp + size - 1);
		printVector("tree1", itsNeq, tree1);
		printVector("tmp", itsNeq, tmp);
		printVector("total", itsDepth, total);
#endif

/*
C     ... SORT THE COMPONENT START POINTERS INTO INCREASING ORDER
C         OF SIZE OF COMPONENT
*/
		if (compns > 1) 
		{
			sortIndexListDecreasing(compns, tmp + size - 1, tmp + start - 1);
		}
		
/*
C     ... FOR EACH COMPONENT IN TURN, CHOOSE TO USE THE ORDERING OF THE
C         'FORWARD' TREE1 OR OF THE 'BACKWARD' TREE2 TO NUMBER THE NODES
C         IN THIS COMPONENT.  THE NUMBERING IS CHOSEN TO MINIMIZE THE
C         MAXIMUM INCREMENT TO ANY LEVEL.
*/
		for (DlInt32 compon = 1; compon <= compns; compon++) 
		{
			DlInt32 i;
			DlInt32 pcStart = start + compon - 1;
			DlInt32 cStart = tmp[pcStart];
			DlInt32 pcSize = size + compon - 1;
			DlInt32 cSize = tmp[pcSize];
			DlInt32 cStop = cStart + cSize - 1;
			assertOrThrow(cSize >= 0 && cSize <= offDiag);
//			if (cSize < 0 || cSize > offDiag)
//				throw -1;

			for (i = 1; i <= itsDepth; i++) 
			{
				inc1[i] = 0;
				inc2[i] = 0;
			}

			DlInt32 maxInc1 = 0;
			DlInt32 maxInc2 = 0;

			for (i = cStart; i <= cStop; i++) 
			{
				DlInt32 worki = tmp[i];
				DlInt32 tWorki = -tree1[worki];
				inc1[tWorki]++;
				tWorki = tree2[worki];
				inc2[tWorki]++;
			}

#ifdef _PRINT
			printVector("inc1", itsDepth, inc1);
			printVector("inc2", itsDepth, inc2);
#endif

/*
C         ... BAROQUE TESTS BELOW DUPLICATE THE GIBBS-POOLE-STOCKMEYER-
C             CRANE PROGRAM, *** NOT *** THE PUBLISHED ALGORITHM.
*/
			for (i = 1; i <= itsDepth; i++) 
			{
				if (inc1[i] != 0 || inc2[i] != 0) 
				{
					if (maxInc1 < total[i] + inc1[i])
						maxInc1 = total[i] + inc1[i];
					if (maxInc2 < total[i] + inc2[i])
						maxInc2 = total[i] + inc2[i];
				}
			}
			
/*
C         ... USE ORDERING OF NARROWER TREE UNLESS IT INCREASES
C             WIDTH MORE THAN WIDER TREE.  IN CASE OF TIE, USE TREE 2!
*/
			if (!(
				maxInc1 > maxInc2 || (
				maxInc1 == maxInc2 && (
					width1 > width2 || (width1 == width2 && oneIs1))))) 
			{
				if (compon == 1)
					reverse = !oneIs1;

				for (i = 1; i <= itsDepth; i++) 
				{
					total[i] = total[i] + inc1[i];
				}

			} 
			else
			{
				if (compon == 1)
					reverse = oneIs1;
				
				for (i = cStart; i <= cStop; i++) 
				{
					DlInt32 worki = tmp[i];
					tree1[worki] = -tree2[worki];
				}
				
				for (i = 1; i <= itsDepth; i++) 
				{
					total[i] += inc2[i];
				}
			}
 		}
 		return;
	}

	reverse = true;
}

void
GPSRenum::findConnectedComps(DlInt32 * status, DlInt32 nReduce,
		DlInt32 * tmp, DlInt32 maxComp, DlInt32 * start, DlInt32 * size, DlInt32 & compns)
{
/*
C     ==================================================================
C
C     FIND THE CONNECTED COMPONENTS OF THE GRAPH INDUCED BY THE SET
C     OF NODES WITH POSITIVE 'STATUS'.  WE SHALL BUILD THE LIST OF
C     CONNECTED COMPONENTS IN 'WORK', WITH A LIST OF POINTERS
C     TO THE BEGINNING NODES OF COMPONENTS LOCATED IN 'START'
C
C     PARAMETERS ...
C
C         N      -- DIMENSION OF THE ORIGINAL MATRIX
C         DEGREE, RSTART, CONNEC -- THE STRUCTURE OF THE ORIGINAL MATRIX
C
C         STATUS -- DERIVED FROM A LEVEL TREE. POSITIVE ENTRIES INDICATE
C                   ACTIVE NODES.  NODES WITH STATUS <= 0 ARE IGNORED.
C
C         NREDUC -- THE NUMBER OF ACTIVE NODES
C
C         WORK   -- WORK SPACE, USED AS A QUEUE TO BUILD CONNECTED
C                   COMPONENTS IN PLACE.
C
C         MXCOMP -- MAXIMUM NUMBER OF COMPONENTS ALLOWED BY CURRENT
C                   SPACE ALLOCATION.  MUST NOT BE VIOLATED.
C
C         START  -- POINTER TO BEGINNING OF  I-TH  CONNECTED COMPONENT
C
C         SIZE   -- SIZE OF EACH COMPONENT
C
C         COMPNS -- NUMBER OF COMPONENTS ACTUALLY FOUND
C
C         ERROR  -- SHOULD BE ZERO ON RETURN UNLESS WE HAVE TOO LITTLE
C                   SPACE OR WE ENCOUNTER AN ERROR IN THE DATA STRUCTURE
C
C         SPACE  -- MAXIMUM AMOUNT OF WORKSPACE USED / NEEDED
C
C     ==================================================================
*/
/*
C     REPEAT
C         << FIND AN UNASSIGNED NODE AND START A NEW COMPONENT >>
C         REPEAT
C             << ADD ALL NEW NEIGHBORS OF FRONT NODE TO QUEUE, >>
C             << REMOVE FRONT NODE.                            >>
C         UNTIL <<QUEUE EMPTY>>
C     UNTIL << ALL NODES ASSIGNED >>
*/
	DlInt32 free = 1;
	DlInt32 root = 1;
	DlInt32 node = 0;
	compns = 0;

/*
C     ... START OF OUTER REPEAT LOOP
*/
	while(1) 
	{
/*
C         ... FIND AN UNASSIGNED NODE
*/
		for (DlInt32 i = root; i <= itsNeq; i++) 
		{
			if (status[i] >= 0) 
			{
				node = i;
				break;
			}
		}
		
		assertOrThrow(node != 0);
//		if (node == 0)
//			throw -1;

/*
C         ... START NEW COMPONENT
*/
		compns++;
		root = node + 1;
		assertOrThrow(compns <= maxComp);
//		if (compns > maxComp)
//			throw -2;
		
		start[compns] = free;
		tmp[free] = node;
		status[node] = -status[node];
		DlInt32 front = free++;

/*
C             ... INNER REPEAT UNTIL QUEUE BECOMES EMPTY
*/
		while (1) 
		{
			node = tmp[front++];

			DlInt32 jPtr = itsStarts[node];
			DlInt32 cDegree = itsDegree[node];
			for (DlInt32 j = 1; j <= cDegree; j++) 
			{
				DlInt32 jNode = itsConnect[jPtr++];
				if (status[jNode] >= 0) 
				{
					assertOrThrow(status[jNode] != 0);
//					if (status[jNode] == 0)
//						throw -1;
					
					status[jNode] = -status[jNode];
					tmp[free++] = jNode;
				}
			}

			if (front >= free)
				break;

		}
/*
C         ... END OF INNER REPEAT.  COMPUTE SIZE OF COMPONENT AND
C             SEE IF THERE ARE MORE NODES TO BE ASSIGNED
*/
		size[compns] = free - start[compns];
		if (free > nReduce)
			break;
	}

	assertOrThrow(free == nReduce + 1);
//	if (free != nReduce + 1)
//		throw -1;
}

void
GPSRenum::transitionToIII(DlInt32 * lStruct, DlInt32 * levelList, DlInt32 * levelPtr,
		DlInt32 * lTotal)
{
/*
C     ==================================================================
C
C     TRANSITIONAL SUBROUTINE, ALGORITHM II TO IIIA OR IIIB.
C
C     CONVERT LEVEL STRUCTURE GIVEN AS VECTOR OF LEVEL NUMBERS FOR NODES
C     TO STRUCTURE AS LIST OF NODES BY LEVEL
C
C     N, ACTIVE, DEPTH -- PROBLEM SIZES
C     LSTRUC -- INPUT LEVEL STRUCTURE
C     LVLLST, LVLPTR -- OUTPUT LEVEL STRUCTURE
C     LTOTAL -- NUMBER OF NODES AT EACH LEVEL (PRECOMPUTED)
C
C     ===============================================================
C
C     STRUCTURE OF WORKSPACE ..
C
C         INPUT (FROM COMBIN) ..
C
C     ------------------------------------------------------------------
C     :  NUMBERED  :  ..(N)..  :  TOTAL  :         ...        :  TREE  :
C     ------------------------------------------------------------------
C
C         OUTPUT (TO cutHillMckeeRenum OR kingRenum) ..
C
C     ------------------------------------------------------------------
C     :  NUMBERED  :       ...             :  TLIST  :  TPTR  :  TREE  :
C     ------------------------------------------------------------------
C
C     HERE, NUMBERED IS THE SET OF NODES IN NUMBERED COMPONENTS
C         TOTAL IS A VECTOR OF LENGTH 'DEPTH' GIVING THE NUMBER
C         OF NODES IN EACH LEVEL OF THE 'TREE'.
C         TLIST, TPTR ARE LISTS OF NODES OF THE TREE, ARRANGED
C         BY LEVEL.  TLIST IS OF LENGTH 'ACTIVE', TPTR 'DEPTH+1'.
C
C     =================================================================
*/
/*
C     ... ESTABLISH STARTING AND ENDING POINTERS FOR EACH LEVEL
*/
	DlInt32 i;
	DlInt32 start = 1;

	for (i = 1; i <= itsDepth; i++) 
	{
		levelPtr[i] = start;
		start += lTotal[i];
		lTotal[i] =  start;
	}
	levelPtr[itsDepth + 1] = start;

	DlInt32 account = 0;
	for (i = 1; i <= itsNeq; i++) 
	{
		if (lStruct[i] < 0) 
		{
			DlInt32 level = -lStruct[i];
			lStruct[i] = level;
			DlInt32 pLevel = levelPtr[level];
			levelList[pLevel] = i;
			levelPtr[level]++;
			account++;
			
			assertOrThrow(levelPtr[level] <= lTotal[level]);

//			if (levelPtr[level] > lTotal[level])
//				throw -1;
		} 
		else 
		{
			assertOrThrow(lStruct[i] <= 0);

//			if (lStruct[i] > 0) 
//				throw -1;
		}
	}

/*
C     ... RESET STARTING POINTERS
*/
	levelPtr[1] = 1;
	for (i = 1; i <= itsDepth; i++) {
		levelPtr[i+1] = lTotal[i];
	}
}

void
GPSRenum::cutHillMckeeRenum(DlInt32 * invNum, DlInt32 sNode1, DlInt32 sNode2,
		bool reverse, DlInt32 * levelList, DlInt32 * levelPtr, DlInt32 * levelNum)
{
/*
C     ==================================================================
C
C     NUMBER THE NODES IN A GENERALIZED LEVEL STRUCTURE ACCORDING
C     TO A GENERALIZATION OF THE CUTHILL MCKEE STRATEGY.
C
C     N      -- DIMENSION OF ORIGINAL PROBLEM
C     DEGREE, RSTART, CONNEC -- GIVE STRUCTURE OF SPARSE AND
C                               SYMMETRIC MATRIX
C
C     NCOMPN -- NUMBER OF NODES IN THIS COMPONENT OF MATRIX GRAPH
C
C     INVNUM -- WILL BECOME A LIST OF THE ORIGINAL NODES IN THE ORDER
C               WHICH REDUCES THE BANDWIDTH OF THE MATRIX.
C
C     NXTNUM -- THE NEXT INDEX TO BE ASSIGNED (1 FOR FIRST COMPONENT)
C
C     REVRS1 -- IF .TRUE., FIRST COMPONENT OF REDUCED GRAPH WAS NUMBERED
C               BACKWARDS.
C
C     LVLLST -- LIST OF NODES IN LEVEL TREE ORDERED BY LEVEL.
C
C     LVLPTR -- POSITION OF INITIAL NODE IN EACH LEVEL OF LVLLST.
C
C     LVLNUM -- LEVEL NUMBER OF EACH NODE IN COMPONENT
C
C     ==================================================================
C
C     NUMBERING REQUIRES TWO QUEUES, WHICH CAN BE BUILD IN PLACE
C     IN INVNUM.
C
C     ==================================================================
C     A L G O R I T H M    S T R U C T U R E
C     ==================================================================
C
C     << SET QUEUE1 TO BE THE SET CONTAINING ONLY THE START NODE. >>
C
C     FOR LEVEL = 1 TO DEPTH DO
C
C         BEGIN
C         LOOP
C
C             REPEAT
C                 BEGIN
C                 << CNODE <- FRONT OF QUEUE1                        >>
C                 << ADD UNNUMBERED NEIGHBORS OF CNODE TO THE BACK   >>
C                 << OF QUEUE1 OR QUEUE2 (USE QUEUE1 IF NEIGHBOR     >>
C                 << AT SAME LEVEL, QUEUE2 IF AT NEXT LEVEL).  SORT  >>
C                 << THE NEWLY QUEUED NODES INTO INCREASING ORDER OF >>
C                 << DEGREE.  NUMBER CNODE, DELETE IT FROM QUEUE1.   >>
C                 END
C             UNTIL
C                 << QUEUE1 IS EMPTY >>
C
C         EXIT IF << ALL NODES AT THIS LEVEL NUMBERED >>
C
C             BEGIN
C             << FIND THE UNNUMBERED NODE OF MINIMAL DEGREE AT THIS >>
C             << LEVEL, RESTART QUEUE1 WITH THIS NODE.              >>
C             END
C
C         END << LOOP LOOP >>
C
C         << PROMOTE QUEUE2 TO BE INITIAL QUEUE1 FOR NEXT ITERATION >>
C         << OF  FOR  LOOP.                                         >>
C
C         END <<FOR LOOP>>
C
C     ==================================================================
C
C     STRUCTURE OF WORKSPACE ..
C
C     --------------------------------------------------------------
C     : NUMBERED :  QUEUE1  :  QUEUE2  : ... : TLIST : TPTR : TREE :
C     --------------------------------------------------------------
C
C     ON COMPLETION, WE HAVE ONLY A NEW, LONGER NUMBERED SET.
C
C     ==================================================================
*/
/*
C     ------------------------------------------------------------------
C     ... GIBBS-POOLE-STOCKMEYER HEURISTIC CHOICE OF ORDER
C     ... SET UP INITIAL QUEUES AT FRONT OF 'INVNUM' FOR FORWRD ORDER,
C         AT BACK FOR REVERSED ORDER.
*/
	bool	forward;
	DlInt32 stNode;
	
	if (itsDegree[sNode1] <= itsDegree[sNode2]) 
	{
		forward = reverse;
		stNode = sNode1;
	}
	else 
	{
		forward = !reverse;
		stNode = sNode2;
	}

	DlInt32 queue1;
	DlInt32 inc;
	if (!forward) 
	{
		inc = -1;
		queue1 = itsActive;
	} 
	else 
	{
		inc = 1;
		queue1 = 1;
	}

/*
C     -------------------------------
C     NUMBER NODES LEVEL BY LEVEL ...
C     -------------------------------
*/
	invNum[queue1] = stNode;
	levelNum[stNode] = 0;

	bool rLevel = (levelNum[stNode] == itsDepth);
	DlInt32 fq1 = queue1;
	DlInt32 bq1 = queue1 + inc;

/*
C         ==============================================================
C         ... 'LOOP' CONSTRUCT BEGINS AT STATEMENT 1000
C                 THE INNER 'REPEAT' WILL BE DONE AS MANY TIMES AS
C                 IS NECESSARY TO NUMBER ALL THE NODES AT THIS LEVEL.
C         ==============================================================
*/
	for (DlInt32 xLevel = 1; xLevel <= itsDepth; xLevel++) 
	{
		DlInt32 level = xLevel;
		if (rLevel)
			level = itsDepth - xLevel + 1;
		DlInt32 lStart = levelPtr[level];
		DlInt32 lWidth = levelPtr[level + 1] - lStart;
		DlInt32 nLeft = lWidth;
		DlInt32 queue2 = queue1 + inc * lWidth;
		DlInt32 bq2 = queue2;

		while(true) 
		{
/*
C             ==========================================================
C             ... REPEAT ... UNTIL QUEUE1 BECOMES EMPTY
C                 TAKE NODE FROM FRONT OF QUEUE1, FIND EACH OF ITS
C                 NEIGHBORS WHICH HAVE NOT YET BEEN NUMBERED, AND
C                 ADD THE NEIGHBORS TO QUEUE1 OR QUEUE2 ACCORDING TO
C                 THEIR LEVELS.
C             ==========================================================
*/
			while(true) 
			{
				DlInt32 cNode = invNum[fq1];
				fq1 += inc;
				DlInt32 sq1 = bq1;
				DlInt32 sq2 = bq2;
				nLeft--;

				DlInt32 cPtr = itsStarts[cNode];
				DlInt32 cDegree = itsDegree[cNode];
				for (DlInt32 i = 1; i <= cDegree; i++) 
				{
					DlInt32 iNode = itsConnect[cPtr];
					cPtr++;
					DlInt32 iLevel = levelNum[iNode];
					if (iLevel != 0) 
					{
						levelNum[iNode] = 0;
						if (iLevel != level) 
						{
							assertOrThrow(std::abs(level - iLevel) == 1);

//							if (std::abs(level - iLevel) != 1)
//								throw -1;
							invNum[bq2] = iNode;
							bq2 += inc;
						}
						else
						{
							invNum[bq1] = iNode;
							bq1 += inc;
						}
					}
				}
/*
C                 ==================================================
C                 ... SORT THE NODES JUST ADDED TO QUEUE1 AND QUEUE2
C                     SEPARATELY INTO INCREASING ORDER OF DEGREE.
C                 ==================================================
*/
				DlInt32 nSort = std::abs(bq1 - sq1);
				if (nSort > 1) 
				{
					if (!forward) 
					{
						sortDegreeDecreasing(nSort, invNum + bq1 + 1);
					}
					else 
					{
						sortDegreeIncreasing(nSort, invNum + sq1);
					}
				}
				
				nSort = std::abs(bq2 - sq2);
				if (nSort > 1) 
				{
					if (!forward) 
					{
						sortDegreeDecreasing(nSort, invNum + (bq2 + 1));
					}
					else
					{
						sortDegreeIncreasing(nSort, invNum + (sq2));
					}
				}
/*
C                     ... END OF REPEAT LOOP
*/
				if (fq1 == bq1)
					break;
			}
/*
C         ==============================================================
C         ... QUEUE1 IS NOW EMPTY ...
C             IF THERE ARE ANY UNNUMBERED NODES LEFT AT THIS LEVEL,
C             FIND THE ONE OF MINIMAL DEGREE AND RETURN TO THE
C             REPEAT LOOP ABOVE.
C         ==============================================================
*/
			if (bq1 == queue2 && nLeft == 0)
				break;

			assertOrThrow(nLeft > 0 && nLeft == inc * (queue2 - bq1));
//			if (nLeft <= 0 || nLeft != inc * (queue2 - bq1))
//				throw -1;

			DlInt32 lowDegree = itsNeq + 1;
			DlInt32 bPtr = itsNeq + 1;
			DlInt32 cPtr = lStart - 1;

			for (DlInt32 i = 1; i <= nLeft; i++) 
			{
				DlInt32 lvllsc;
				while(1) 
				{
					cPtr++;
					lvllsc = levelList[cPtr];
					if (levelNum[lvllsc] == level)
						break;
					assertOrThrow(levelNum[lvllsc] == 0);
//					if (levelNum[lvllsc] != 0)
//						throw -1;
				}

				if (itsDegree[lvllsc] < lowDegree) 
				{
					lowDegree = itsDegree[lvllsc];
					bPtr = cPtr;
				}
			}
/*
C             ... MINIMAL DEGREE UNNUMBERED NODE FOUND ...
*/
			assertOrThrow(bPtr <= itsNeq);
//			if (bPtr > itsNeq)
//				throw -1;
			
			DlInt32 lvllsb = levelList[bPtr];
			invNum[bq1] = lvllsb;
			levelNum[lvllsb] = 0;
			bq1 += inc;
		}
/*
C             =============================================
C             ... ADVANCE QUEUE POINTERS TO MAKE QUEUE2 THE
C                 NEW QUEUE1 FOR THE NEXT ITERATION.
C             =============================================
*/
		queue1 = queue2;
		fq1 = queue1;
		bq1 = bq2;

		assertOrThrow(bq1 != fq1 || xLevel >= itsDepth);
//		if (bq1 == fq1 && xLevel < itsDepth)
//			throw -1;
	}
/*
C     ... CHANGE SIGN OF DEGREE TO MARK THESE NODES AS 'NUMBERED'
*/
	for (DlInt32 i = 1; i <= itsActive; i++) 
	{
		itsDegree[invNum[i]] = -itsDegree[invNum[i]];
	}
}

void
GPSRenum::kingRenum(DlInt32 workLen, DlInt32 & nextNum, DlInt32 * levelList, 
			DlInt32 * levelPtr, DlInt32 * levelNum)
{
/*
C     ==================================================================
C
C     NUMBER NODES IN A GENERALIZED LEVEL STRUCTURE ACCORDING TO
C     A GENERALIZATION OF THE KING ALGORITHM, WHICH REDUCES
C     THE PROFILE OF THE SPARSE SYMMETRIC MATRIX.
C
C     ---------------------
C
C     CODE USES A PRIORITY QUEUE TO CHOOSE THE NEXT NODE TO BE NUMBERED
C     THE PRIORITY QUEUE IS REPRESENTED BY A SIMPLE LINEAR-LINKED LIST
C     TO SAVE SPACE.  THIS WILL REQUIRE MORE SEARCHING THAN A FULLY
C     LINKED REPRESENTATION, BUT THE DATA MANIPULATION IS SIMPLER.
C
C     -------------------
C
C     << ESTABLISH PRIORITY QUEUE 'ACTIVE' FOR LEVEL 1 NODES >>
C
C     FOR I = 1 TO DEPTH DO
C         << SET QUEUE 'QUEUED' TO BE EMPTY, LIST 'NEXT' TO BE >>
C         << SET OF NODES AT NEXT LEVEL.                       >>
C
C         FOR J = 1 TO 'NODES AT THIS LEVEL' DO
C             << FIND FIRST NODE IN ACTIVE WITH MINIMAL CONNECTIONS >>
C             << TO 'NEXT'.  NUMBER THIS NODE AND REMOVE HIM FROM   >>
C             << 'ACTIVE'.  FOR EACH NODE IN 'NEXT' WHICH CONNECTED >>
C             << TO THIS NODE, MOVE IT TO 'QUEUED' AND REMOVE IT    >>
C             << FROM 'NEXT'.                                       >>
C
C         << SET NEW QUEUE 'ACTIVE' TO BE 'QUEUED' FOLLOWED BY ANY >>
C         << NODES STILL IN 'NEXT'.                                >>
C
C     ==================================================================
C
C     DATA STRUCTURE ASSUMPTIONS:
C     THE FIRST 'NXTNUM-1' ELEMENTS OF  WORK  ARE ALREADY IN USE.
C     THE LEVEL STRUCTURE 'LVLLST' IS CONTIGUOUS WITH  WORK, THAT IS,
C     IT RESIDES IN ELEMENTS  WRKLEN+1, ...  OF  WORK.  'LVLPTR' AND
C     'LVLNUM' ARE ALSO EMBEDDED IN WORK, BEHIND 'LVLLST'.  THE
C     THREE VECTORS ARE PASSED SEPARATELY TO CLARIFY THE INDEXING,
C     BUT THE QUEUES DEVELOPED WILL BE ALLOWED TO OVERRUN 'LVLLST'
C     AS NEEDED.
C
C     ... BUILD THE FIRST 'ACTIVE' QUEUE STARTING W1 LOCATIONS FROM
C         THE FRONT OF THE CURRENT WORKING AREA  (W1 IS THE WIDTH OF THE
C         FIRST LEVEL).  BUILD THE FIRST 'QUEUED' QUEUE STARTING FROM
C         THE BACK OF WORK SPACE.  THE LIST 'NEXT' WILL BE REALIZED
C         IMPLICITLY IN 'LVLNUM' AS:
C                  LVLNUM(I) > 0   <== LEVEL NUMBER OF NODE.  'NEXT' IS
C                                      SET WITH LVLNUM(I) = LEVEL+1
C                  LVLNUM(I) = 0   <== I-TH NODE IS IN 'QUEUED' OR IS
C                                      NOT IN THIS COMPONENT OF GRAPH,
C                                      OR HAS JUST BEEN NUMBERED.
C                  LVLNUM(I) < 0   <== I-TH NODE IS IN 'ACTIVE' AND IS
C                                      CONNECTED TO -LVLNUM(I) NODES IN
C                                      'NEXT'.
C
C     ==================================================================
C
C     STRUCTURE OF WORKSPACE ..
C
C     --------------------------------------------------------------
C     : NUMBERED : DONE : ACTIVE : ALEVEL : ... : QUEUED : LVLLST :
C     --------------------------------------------------------------
C
C     -------------------
C       LVLPTR : LVLNUM :
C     -------------------
C
C     IN THE ABOVE,
C         NUMBERED IS THE SET OF NODES ALREADY NUMBERED FROM
C         PREVIOUS COMPONENTS AND EARLIER LEVELS OF THIS COMPONENT.
C         DONE, ACTIVE, ALEVEL  ARE VECTORS OF LENGTH THE WIDTH OF
C         THE CURRENT LEVEL.  ACTIVE IS A SET OF INDICES INTO
C         ALEVEL.  AS THE NODES IN ALEVEL ARE NUMBERED, THEY
C         ARE PLACED INTO 'DONE'.
C         QUEUED IS A QUEUE OF NODES IN THE 'NEXT' LEVEL, WHICH
C         GROWS FROM THE START OF THE 'NEXT' LEVEL IN LVLLST
C         FORWARDS TOWARD 'ALEVEL'.  QUEUED IS OF LENGTH NO MORE
C         THAN THE WIDTH OF THE NEXT LEVEL.
C         LVLLST IS THE LIST OF UNNUMBERED NODES IN THE TREE,
C         ARRANGED BY LEVEL.
C
C     ==================================================================
*/	
	DlInt32 tWorkLen = workLen + itsActive + itsNeq + itsDepth + 1;
	DlInt32 unused = tWorkLen;

	DlInt32 aStart = levelPtr[1];
	DlInt32 lWidth = levelPtr[2] - aStart;
	aStart = workLen + 1;
	DlInt32 active = nextNum + lWidth + 1;
	DlInt32 nActive = lWidth;
	DlInt32 nFinal = nextNum + itsActive;

	DlInt32 nNext = levelPtr[3] - levelPtr[2];
	DlInt32 queueD = workLen;
	DlInt32 queueB = queueD;
	DlInt32 maxQueue = active + lWidth;
/*
C     ... BUILD FIRST PRIORITY QUEUE 'ACTIVE'
*/
	DlInt32 lowDegree = -(itsNeq + 1);
	DlInt32 lPtr = levelPtr[1];
	DlInt32 * work = getWork(1);

	for (DlInt32 i = 1; i <= lWidth; i++) 
	{
		DlInt32 nConnect = 0;
		DlInt32 lvllsl = levelList[lPtr];
		DlInt32 jPtr = itsStarts[lvllsl];
		DlInt32 lDegree = itsDegree[lvllsl];

		for (DlInt32 j = 1; j <= lDegree; j++) 
		{
			if (levelNum[itsConnect[jPtr]] == 2)
				nConnect--;
			jPtr++;
		}

		work[active + i - 1] = i;
		levelNum[lvllsl] = nConnect;
		lowDegree = std::max(lowDegree, nConnect);
		lPtr++;
	}
	work[active-1] = 0;
/*
C     -----------------------------------
C     NOW NUMBER NODES LEVEL BY LEVEL ...
C     -----------------------------------
*/
	for (DlInt32 level = 1; level <= itsDepth; level++) 
	{
		DlInt32 cNode;
/*
C         ... NUMBER ALL NODES IN THIS LEVEL
*/
		for (DlInt32 i = 1; i <= lWidth; i++)
		{
			DlInt32 pPtr = -1;
			DlInt32 ptr = work[active-1];
			if (nNext != 0)
			{
/*
C                 ... IF NODES REMAIN IN NEXT, FIND THE EARLIEST NODE
C                     IN ACTIVE OF MINIMAL DEGREE.
*/
				DlInt32 mpPtr;
				DlInt32 mPtr;
				DlInt32 minDegree = -(itsNeq + 1);
				DlInt32 j;
				for (j = 1; j <= nActive; j++)
				{
					cNode = work[aStart + ptr];
					if (levelNum[cNode] == lowDegree)
						break;
					if (levelNum[cNode] > minDegree) 
					{
						mpPtr = pPtr;
						mPtr = ptr;
						minDegree = levelNum[cNode];
					}
					
					pPtr = ptr;
					ptr = work[active + ptr];
				}
/*
C                     ... ESTABLISH  PTR  AS FIRST MIN DEGREE NODE
C                         PPTR AS PREDECESSOR IN LIST.
*/
				if (j > nActive) 
				{
					ptr = mPtr;
					pPtr = mpPtr;
				}

				cNode = work[aStart + ptr];
				lowDegree = levelNum[cNode];
				levelNum[cNode] = 0;
				DlInt32 jPtr = itsStarts[cNode];
/*
C                 ... UPDATE CONNECTION COUNTS FOR ALL NODES WHICH
C                     CONNECT TO  CNODE'S  NEIGHBORS IN  NEXT.
*/
				DlInt32 cDegree = itsDegree[cNode];
				DlInt32 startIc = queueB;

				for (j = 1; j <= cDegree; j++)
				{
					DlInt32 jNode = itsConnect[jPtr];
					jPtr++;

					if (levelNum[jNode] == level + 1)
					{
						assertOrThrow(queueB >= maxQueue);
//						if (queueB < maxQueue)
//							throw -2;
						work[queueB] = jNode;
						queueB--;
						nNext--;
						levelNum[jNode] = 0;

						if (nActive != 1) 
						{
							DlInt32 kPtr = itsStarts[jNode];
							DlInt32 jDegree = itsDegree[jNode];

							for (DlInt32 k = 1; k <= jDegree; k++)
							{
								DlInt32 kNode = itsConnect[kPtr];
								kPtr++;

								if (levelNum[kNode] < 0) 
								{
									levelNum[kNode]++;
									if (lowDegree < levelNum[kNode])
										lowDegree = levelNum[kNode];
								}
							}
						}
					}
				}
/*
C                 ... TO MIMIC THE ALGORITHM AS IMPLEMENTED BY GIBBS,
C                     SORT THE NODES JUST ADDED TO THE QUEUE INTO
C                     INCREASING ORDER OF ORIGINAL INDEX. (BUT, BECAUSE
C                     THE QUEUE IS STORED BACKWARDS IN MEMORY, THE SORT
C                     ROUTINE IS CALLED FOR DECREASING INDEX.)
C
C                     TREAT  0, 1 OR 2  NODES ADDED AS SPECIAL CASES
*/
				DlInt32 added = startIc - queueB;
				if (added == 2) 
				{
					if (work[startIc-1] < work[startIc])
					{
						DlInt32 jNode = work[startIc];
						work[startIc] = work[startIc-1];
						work[startIc-1] = jNode;
					}
				} 
				else if (added > 2)
				{
					sortListDecreasing(added, work + (queueB + 1));
				}
			}
/*
C                 ... NUMBER THIS NODE AND DELETE IT FROM 'ACTIVE'.
C                     MARK IT UNAVAILABLE BY CHANGING SIGN OF DEGREE
*/
			nActive--;
			cNode = work[aStart + ptr];
			work[nextNum] = cNode;
			itsDegree[cNode] = -itsDegree[cNode];
			nextNum++;
/*
C             ... DELETE LINK TO THIS NODE FROM LIST
C
*/
			work[active + pPtr] = work[active + ptr];
		}
/*
C         ... NOW MOVE THE QUEUE 'QUEUED' FORWARD, AT THE SAME
C             TIME COMPUTING CONNECTION COUNTS FOR ITS ELEMENTS.
C             THEN DO THE SAME FOR THE REMAINING NODES IN 'NEXT'.
*/
		if (unused > queueB - maxQueue)
			unused = queueB - maxQueue;
		
		assertOrThrow(nextNum == active - 1);
//		if (nextNum != active - 1)
//			throw -2;

		if (level != itsDepth)
		{
			DlInt32 lStart = levelPtr[level+1];
			lWidth = levelPtr[level+2] - lStart;
			active = nextNum + lWidth + 1;
			aStart = active + lWidth;
			nActive = lWidth;
			maxQueue = aStart + lWidth;
			assertOrThrow(maxQueue <= queueB + 1);
//			if (maxQueue > queueB + 1)
//				throw -2;
			if (unused > queueB - maxQueue + 1)
				unused = queueB - maxQueue + 1;

			DlInt32 qCount = queueD - queueB;
			lowDegree = -(itsNeq + 1);
			work[active-1] = 0;

			DlInt32 ptr = lStart;
			for(DlInt32 i = 1; i <= lWidth; i++)
			{
/*
C                 ... CHOOSE NEXT NODE FROM EITHER 'QUEUED' OR 'NEXT'
*/
				if (i <= qCount)
				{
					cNode = work[queueD + 1 - i];
				} 
				else 
				{
					while(1) 
					{
						cNode = levelList[ptr];
						ptr++;
						assertOrThrow(ptr <= levelPtr[level+2]);
//						if (ptr > levelPtr[level+2])
//							throw -2;
						if (levelNum[cNode] > 0)
							break;
					}
				}
				
				if (level+1 != itsDepth)
				{
					DlInt32 rPtr = itsStarts[cNode];
					DlInt32 nConnect = 0;
					DlInt32 jDegree = itsDegree[cNode];

					for (DlInt32 j = 1; j <= jDegree; j++)
					{
						if (levelNum[itsConnect[rPtr]] == level + 2)
							nConnect--;
						rPtr++;
					}
					levelNum[cNode] = nConnect;
					lowDegree = std::max(lowDegree, nConnect);
/*
C             ... ADD CNODE TO NEW 'ACTIVE' QUEUE
*/
				}
				work[active + i - 1] = i;
				work[aStart + i - 1] = cNode;
			}

			if (itsDepth != level + 1)
			{
				nNext = levelPtr[level+3] - levelPtr[level+2];
				queueD = lStart - 1 + lWidth + workLen;
				queueB = queueD;
			} 
			else 
			{
				nNext = 0;
			}
		}
	}

	assertOrThrow(nextNum == nFinal);
//	if (nextNum != nFinal)
//		throw -1;
}

void
GPSRenum::compBandProfile(DlInt32 * newNum)
{
/*
C     ==================================================================
C
C
C     COMPUTE THE BANDWIDTH AND PROFILE FOR THE RENUMBERING GIVEN
C     BY 'INVNUM' AND ALSO FOR THE RENUMBERING GIVEN BY 'OLDNUM'.
C     'NEWNUM' WILL BE A PERMUTATION VECTOR COPY OF THE NODE
C     LIST 'INVNUM'.
C
C     ==================================================================
C
*/
/*
C     ------------------------------------------------------------------
C     ... CREATE NEWNUM AS A PERMUTATION VECTOR
*/
	DlInt32 * invNum = getWork(1);
	DlInt32 i;
	for (i = 1; i <= itsNeq; i++) 
	{
		newNum[invNum[i]] = i;
	}
/*
C     ... COMPUTE PROFILE AND BANDWIDTH FOR BOTH THE OLD AND THE NEW
C         ORDERINGS.
*/
	DlInt32 oldBand = 0;
	DlInt32 oldProfile = 0;
	DlInt32 newBand = 0;
	DlInt32 newProfile = 0;

	for (i = 1; i <= itsNeq; i++) 
	{
		if (itsDegree[i] != 0) 
		{
			assertOrThrow(itsDegree[i] <= 0);
//			if (itsDegree[i] > 0)
//				throw -1;
			DlInt32 iDegree = -itsDegree[i];
			itsDegree[i] = iDegree;
			DlInt32 newOrigin = newNum[i];
			DlInt32 oldOrigin = itsNew[i];
			DlInt32 newRwd = 0;
			DlInt32 oldRwd = 0;
			DlInt32 jPtr = itsStarts[i];
			
/*
C             ... FIND NEIGHBOR WHICH IS NUMBERED FARTHEST AHEAD OF THE
C                 CURRENT NODE.
*/
			for (DlInt32 j = 1; j <= iDegree; j++) 
			{
				DlInt32 jNode = itsConnect[jPtr];
				jPtr++;
				newRwd = std::max(newRwd, newOrigin - newNum[jNode]);
				oldRwd = std::max(oldRwd, oldOrigin - itsNew[jNode]);
			}

			newProfile += newRwd;
			newBand = std::max(newRwd, newBand);
			oldProfile += oldRwd;
			oldBand = std::max(oldRwd, oldBand);
		}
	}
/*
C     ... IF NEW ORDERING HAS BETTER BANDWIDTH THAN OLD ORDERING,
C         REPLACE OLD ORDERING BY NEW ORDERING
*/
	if (newBand <= oldBand)
	{
		itsBetter = true;
		itsBand = newBand;
		itsProfile = newProfile;
		for (DlInt32 i = 1; i <= itsNeq; i++)
			itsNew[i] = newNum[i];
/*
C     ... RETAIN OLD ORDERING
*/
	}
	else 
	{
		itsBetter = false;
		itsBand = oldBand;
		itsProfile = oldProfile;
	}
}

void
GPSRenum::compBandProfileWithRev(DlInt32 * newNum)
{
/*
C     ==================================================================
C
C
C     COMPUTE THE BANDWIDTH AND PROFILE FOR THE RENUMBERING GIVEN
C     BY 'INVNUM', BY THE REVERSE OF NUMBERING 'INVNUM', AND ALSO
C     BY THE RENUMBERING GIVEN IN 'OLDNUM'.
C     'NEWNUM' WILL BE A PERMUTATION VECTOR COPY OF THE NODE
C     LIST 'INVNUM'.
C
C     ==================================================================
C
*/
/*
C     ------------------------------------------------------------------
C     ... CREATE NEWNUM AS A PERMUTATION VECTOR
*/
	DlInt32 * invNum = getWork(1);
	DlInt32 i;
	
	for (i = 1; i <= itsNeq; i++)
		newNum[invNum[i]] = i;
/*
C     ... COMPUTE PROFILE AND BANDWIDTH FOR BOTH THE OLD AND THE NEW
C         ORDERINGS.
*/
	DlInt32 oldBand = 0;
	DlInt32 newBand = 0;
	DlInt32 nRvBand = 0;
	DlInt32 oldProfile = 0;
	DlInt32 newProfile = 0;
	DlInt32 nRvProfile = 0;

	for (i = 1; i <= itsNeq; i++)
	{
		assertOrThrow(itsDegree[i] <= 0);
//		if (itsDegree[i] > 0)
//			throw -1;
		if (itsDegree[i] != 0) 
		{
			DlInt32 iDegree = -itsDegree[i];
			itsDegree[i] = iDegree;
			DlInt32 newRwd = 0;
			DlInt32 oldRwd = 0;
			DlInt32 nRvRwd = 0;
			DlInt32 newOrigin = newNum[i];
			DlInt32 oldOrigin = itsNew[i];
			DlInt32 nRvOrigin = itsNeq - newNum[i] + 1;
			DlInt32 jPtr = itsStarts[i];
/*
C             ... FIND NEIGHBOR WHICH IS NUMBERED FARTHEST AHEAD OF THE
C                 CURRENT NODE.
*/
			for (DlInt32 j = 1; j <= iDegree; j++)
			{
				DlInt32 jNode = itsConnect[jPtr];
				jPtr++;
				newRwd = std::max(newRwd, newOrigin - newNum[jNode]);
				oldRwd = std::max(oldRwd, oldOrigin - itsNew[jNode]);
				nRvRwd = std::max(nRvRwd, nRvOrigin - itsNeq + newNum[jNode] - 1);
			}

			newProfile += newRwd;
			newBand = std::max(newBand, newRwd);
			nRvProfile += nRvRwd;
			nRvBand = std::max(nRvBand, nRvRwd);
			oldProfile += oldRwd;
			oldBand = std::max(oldRwd, oldBand);
		}
	}
/*
C     ... IF NEW ORDERING HAS BETTER BANDWIDTH THAN OLD ORDERING,
C         REPLACE OLD ORDERING BY NEW ORDERING
*/
	if (newProfile < oldProfile && newProfile < nRvProfile) 
	{
		itsBetter = true;
		itsBand = newBand;
		itsProfile = newProfile;
		for (DlInt32 i = 1; i <= itsNeq; i++) 
		{
			itsNew[i] = newNum[i];
		}
/*
C     ... CHECK NEW REVERSED ORDERING FOR BEST PROFILE
*/
	} 
	else if (nRvProfile < oldProfile) 
	{
		itsBetter = true;
		itsBand = nRvBand;
		itsProfile = nRvProfile;
		for (i = 1; i <= itsNeq; i++)
		{
			itsNew[i] = itsNeq - newNum[i] + 1;
			if (i <= itsNeq/2) 
			{
				DlInt32 j = invNum[i];
				DlInt32 nmip1 = (itsNeq + 1) - i;
				invNum[i] = invNum[nmip1];
				invNum[nmip1] = j;
			}
		}
/*
C     ... RETAIN OLD ORDERING
*/
	}
	else 
	{
		itsBetter = false;
		itsBand = oldBand;
		itsProfile = oldProfile;
	}
}

void
GPSRenum::sortIndexListDecreasing(DlInt32 n, DlInt32 * key, DlInt32 * data)
{
/*
C     ==================================================================
C
C     I N S E R T I O N    S O R T
C
C     INPUT:
C         N    -- NUMBER OF ELEMENTS TO BE SORTED
C         KEY  -- AN ARRAY OF LENGTH  N  CONTAINING THE VALUES
C                 WHICH ARE TO BE SORTED
C         DATA -- A SECOND ARRAY OF LENGTH  N  CONTAINING DATA
C                 ASSOCIATED WITH THE INDIVIDUAL KEYS.
C
C     OUTPUT:
C         KEY  -- WILL BE ARRANGED SO THAT VALUES ARE IN DECREASING
C                 ORDER
C         DATA -- REARRANGED TO CORRESPOND TO REARRANGED KEYS
C         ERROR -- WILL BE ZERO UNLESS THE PROGRAM IS MALFUNCTIONING,
C                  IN WHICH CASE IT WILL BE EQUAL TO 1.
C
C
C     ==================================================================
*/
	if (n == 1)
		return;
	assertOrThrow(n > 0);
//	if (n <= 0) 
//		throw -1;
/*
C     ... INSERTION SORT ... FOR I := N-1 STEP -1 TO 1 DO ...
*/
	DlInt32 i = n - 1;
	DlInt32 ip1 = n;

	while (1) 
	{
		if (key[i] < key[ip1])
		{
/*
C             ... OUT OF ORDER ... MOVE UP TO CORRECT PLACE
*/
			DlInt32 k = key[i];
			DlInt32 d = data[i];
			DlInt32 j = ip1;
			DlInt32 jm1 = i;
/*
C             ... REPEAT ... UNTIL 'CORRECT PLACE FOR K FOUND'
*/
			do 
			{
				key[jm1] = key[j];
				data[jm1] = data[j];
				jm1 = j;
				j++;
				if (j > n)
					break;
			} 
			while(key[j] > k);

			key[jm1] = k;
			data[jm1] = d;
		}
		
		ip1 = i;
		i--;
		if (i <= 0)
			break;
	}
}

void
GPSRenum::sortListDecreasing(DlInt32 n, DlInt32 * key)
{
/*
C     ==================================================================
C
C     I N S E R T I O N    S O R T
C
C     INPUT:
C         N    -- NUMBER OF ELEMENTS TO BE SORTED
C         KEY  -- AN ARRAY OF LENGTH  N  CONTAINING THE VALUES
C                 WHICH ARE TO BE SORTED
C
C     OUTPUT:
C         KEY  -- WILL BE ARRANGED SO THAT VALUES ARE IN DECREASING
C                 ORDER
C
C     ==================================================================
*/
	if (n == 1)
		return;
	assertOrThrow(n > 0);
//	if (n <= 0) 
//		throw -1;
/*
C     ... INSERTION SORT ... FOR I := N-1 STEP -1 TO 1 DO ...
*/
	DlInt32 i = n - 1;
	DlInt32 ip1 = n;

	do 
	{
		if (key[i] < key[ip1]) 
		{
/*
C             ... OUT OF ORDER ... MOVE UP TO CORRECT PLACE
*/
			DlInt32 k = key[i];
			DlInt32 j = ip1;
			DlInt32 jm1 = i;
/*
C             ... REPEAT ... UNTIL 'CORRECT PLACE FOR K FOUND'
*/
			do 
			{
				key[jm1] = key[j];
				jm1 = j++;
				if (j > n)
					break;
			} 
			while(key[j] > k);

			key[jm1] = k;
		}
		ip1 = i--;
	} 
	while(i > 0);
}

void
GPSRenum::sortDegreeDecreasing(DlInt32 n, DlInt32 * index)
{
/*
C     ==================================================================
C
C     I N S E R T I O N      S O R T
C
C     INPUT:
C         N    -- NUMBER OF ELEMENTS TO BE SORTED
C         INDEX  -- AN ARRAY OF LENGTH  N  CONTAINING THE INDICES
C                 WHOSE DEGREES ARE TO BE SORTED
C         DEGREE -- AN  NVEC  VECTOR, GIVING THE DEGREES OF NODES
C                   WHICH ARE TO BE SORTED.
C
C     OUTPUT:
C         INDEX  -- WILL BE ARRANGED SO THAT VALUES ARE IN DECREASING
C                 ORDER
C         ERROR -- WILL BE ZERO UNLESS THE PROGRAM IS MALFUNCTIONING,
C                  IN WHICH CASE IT WILL BE EQUAL TO 1.
C
C     ==================================================================
*/
	if (n == 1)
		return;
	assertOrThrow(n > 0);
//	if (n <= 0)
//		throw -1;
/*
C     ------------------------------------------------------------------
C     INSERTION SORT THE ENTIRE FILE
C     ------------------------------------------------------------------
C     ... INSERTION SORT ... FOR I := N-1 STEP -1 TO 1 DO ...
*/
	DlInt32 i = n - 1;
	DlInt32 ip1 = n;

	do
	{
		DlInt32 indexI = index[i];
		DlInt32 indexI1 = index[ip1];
		if (itsDegree[indexI] < itsDegree[indexI1])
		{
/*
C             ... OUT OF ORDER ... MOVE UP TO CORRECT PLACE
*/
			DlInt32 v = itsDegree[indexI];
			DlInt32 j = ip1;
			DlInt32 jm1 = i;
			DlInt32 indexJ = index[j];
/*
C             ... REPEAT ... UNTIL 'CORRECT PLACE FOR V FOUND'
*/
			do 
			{
				index[jm1] = indexJ;
				jm1 = j++;
				if (j > n)
					break;
				indexJ = index[j];
			} 
			while (itsDegree[indexJ] > v);

			index[jm1] = indexI;
		}
		
		ip1 = i--;
	} 
	while(i > 0);
}

void
GPSRenum::sortDegreeIncreasing(DlInt32 n, DlInt32 * index)
{
/*
C     ==================================================================
C
C     I N S E R T I O N      S O R T
C
C     INPUT:
C         N    -- NUMBER OF ELEMENTS TO BE SORTED
C         INDEX  -- AN ARRAY OF LENGTH  N  CONTAINING THE INDICES
C                 WHOSE DEGREES ARE TO BE SORTED
C         DEGREE -- AN  NVEC  VECTOR, GIVING THE DEGREES OF NODES
C                   WHICH ARE TO BE SORTED.
C
C     OUTPUT:
C         INDEX  -- WILL BE ARRANGED SO THAT VALUES ARE IN INCREASING
C                   ORDER
C         ERROR -- WILL BE ZERO UNLESS THE PROGRAM IS MALFUNCTIONING,
C                  IN WHICH CASE IT WILL BE EQUAL TO 1.
C
C     ==================================================================
*/
	if (n == 1)
		return;
	assertOrThrow(n > 0);
//	if (n <= 0)
//		throw -1;
/*
C     ------------------------------------------------------------------
C     INSERTION SORT THE ENTIRE FILE
C     ------------------------------------------------------------------
C     ... INSERTION SORT ... FOR I := N-1 STEP -1 TO 1 DO ...
*/
	DlInt32 i = n - 1;
	DlInt32 ip1 = n;

	do
	{
		DlInt32 indexI = index[i];
		DlInt32 indexI1 = index[ip1];
		if (itsDegree[indexI] > itsDegree[indexI1])
		{
/*
C             ... OUT OF ORDER ... MOVE UP TO CORRECT PLACE
*/
			DlInt32 v = itsDegree[indexI];
			DlInt32 j = ip1;
			DlInt32 jm1 = i;
			DlInt32 indexJ = index[j];
/*
C             ... REPEAT ... UNTIL 'CORRECT PLACE FOR V FOUND'
*/
			do 
			{
				index[jm1] = indexJ;
				jm1 = j++;
				if (j > n)
					break;
				indexJ = index[j];
			} 
			while(itsDegree[indexJ] < v);

			index[jm1] = indexI;
		}
		ip1 = i--;
	} 
	while (i > 0);
}

#ifdef _PRINT
//------------------------------------------------------------------------------
//	::printTree
//
//		print the tree. For debugging
//
//	throws			:	PP_PowerPlant::LException
//
//	which			->	defines the file to be copied
//------------------------------------------------------------------------------
void
GPSRenum::printTree(char * msg, DlInt32 avail, const DlInt32 * tree, DlInt32 depth, DlInt32 active, DlInt32 width)
{
	DlInt32 i;
	DlInt32 len = tree[avail - depth] - 1;
	std::printf("%s\n", msg);
	std::printf("depth = %ld, width = %ld, active = %ld\n", depth, width, active);
	std::printf("tree:");
	for (i = 1; i <= len; i++) {
		std::printf("%d ", tree[i]);
	}
	
	std::printf("\nqueue\n");
	for (i = 1; i <= depth + 1; i++) {
		std::printf("%d ", tree[avail - i + 1]);
	}

	std::printf("\n\n");
}

//------------------------------------------------------------------------------
//	::printVector
//
//		print the tree. For debugging
//
//	throws			:	PP_PowerPlant::LException
//
//	which			->	defines the file to be copied
//------------------------------------------------------------------------------
void
GPSRenum::printVector(char * msg, DlInt32 n, const DlInt32 * v)
{
	DlInt32 i;
	std::printf("%s: ", msg);
	for (i = 1; i <= n; i++) {
		std::printf("%d ", v[i]);
	}
	std::printf("\n\n");
}

#endif
