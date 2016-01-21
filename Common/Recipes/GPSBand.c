//+
//	GPSBand.c
//
//	Copyright © 2000, David C. Salmon. All Rights Reserved
//
//	Contains C implementation of the Gibb-Poole-Stockmeyer and 
//	Gibbs-Pool-King algorithm for minizing bandwidth/profile..
//
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

//---------------------------------- Includes ----------------------------------

//#include "recipes.h"

#include <stdlib.h>
#include "GPSBand.h"

#ifdef _DEBUG
#include <stdio.h>
#endif

//---------------------------------- Declares ----------------------------------

//#define INDEX( w, n, i, j ) (w + ((i)-1) + n * ((j)-1))

//int	gpska( GPSInput * inp, GPSOutput * out );

static int	findPseudoDiameter( GPSInput * inp, long avail, long nLeft, 
				long & stNode, long & rvNode, oneBased trees[], long & forward, 
				long & bestBack, long & nNodes, long & depth, long & fWidth, 
				long & bWidth, long & space );

static int buildLevelTree( GPSInput * inp, long stNode, long avail, long nLeft,
				oneBased & list, long & active, long & depth, long & width );

static int	buildNarrowerTree( GPSInput * inp, long stNode, long avail, 
				long active, long maxDepth, oneBased list, long & depth, 
				long & width, long & maxWidth );

static int	transitionToII( long n, long avail, long active, long depth, 
				long workLen, oneBased levelList[], oneBased levelPtr[], 
				oneBased work, long nextNum, long & tree1, long & tree2, 
				long & width1, long & width2, int & oneIs1 );

static void convertToLevelNums( long n, long active, long depth,
			const oneBased & levelList, const oneBased & levelPtr,
			oneBased levelNum, int reverse );

//static void convertToLevelNums( long n, long active, long depth, 
//				oneBased levelList, oneBased levelPtr, oneBased levelNum, 
//				int reverse );

static int	combineRootedTrees( GPSInput * inp, long active, long width1,
				long width2, oneBased tree1, oneBased tree2, oneBased work, 
				long workLen, long depth, oneBased inc1, oneBased inc2, 
				oneBased total, int oneIs1, int & reverse, long & space );

static int	findConnectedComps( GPSInput * inp, oneBased status, long nReduce, 
				oneBased work, long maxComp, oneBased start, oneBased size, 
				long & compns, long & space );

static int	transitionToIII( long n, long depth, oneBased lStruct, 
				oneBased levelList, oneBased levelPtr, oneBased lTotal );

static int	cutHillMckeeRenum( GPSInput * inp, long nCompn, oneBased invNum, 
				long sNode1, long sNode2, int reverse, long depth, 
				oneBased levelList, oneBased levelPtr, oneBased levelNum,
				long & space );

static int	kingRenum( GPSInput * inp, long workLen, long & nextNum, oneBased work,
				long nCompn, long depth, oneBased levelList, oneBased levelPtr, 
				oneBased levelNum, long & space );

static int	compBandProfile( GPSInput * inp, oneBased invNum, oneBased newNum, 
				oneBased oldNum, long & bandwd, long & profile, long & space );

static int	compBandProfileWithRev( GPSInput * inp, oneBased invNum, 
				oneBased newNum, oneBased oldNum, long & bandWidth, 
				long & profile, long & space );

static int	sortIndexListDecreasing( long n, oneBased key, oneBased data );

static int	sortListDecreasing( long n, oneBased key );

static int	sortDegreeDecreasing( long n, oneBased index, oneBased degree );

static int	sortDegreeIncreasing( long n, oneBased index, oneBased degree );

static long	max0( long a, long b );

#ifdef _DEBUG
static void printTree( char * msg, long avail, const oneBased & tree, 
	long depth, long active, long width );

static void printVector( char * msg, long n, const oneBased & v );
#endif

//---------------------------------- Methods  ----------------------------------

//------------------------------------------------------------------------------
//	gpska
//
//		minimize the bandwidth or profile for the equations given by 
//		inp. Place the permutation into out.
//
//	inp			->	define the connectivity of the equations
//	out			<->	permutation, bandwidth, profile for result.
//	return		<-	0 for success
//------------------------------------------------------------------------------
int
minimizeBandProfile( GPSInput * inp, GPSOutput * out )
{
    long	i, inc1, inc2, avail, nextNum, lowdg, stNode, nLeft, 
    		tree1, tree2, depth, empty, sTotal, reqd, cSpace, 
    		lvlList, lvlPtr, active, rvNode, width1, width2, maxdg;
	
	int		revrs1, oneIs1, err;

	long	space;
	
	oneBased	treeArray[4];
	oneBased	levelArray[4];

/*
C
      DlInt32EGER     I, INC1, INC2, AVAIL, NXTNUM, LOWDG, STNODE, NLEFT,
     1            TREE1, TREE2, DEPTH, EMPTY, STOTAL, REQD, CSPACE,
     2            LVLLST, LVLPTR, ACTIVE, RVNODE, WIDTH1, WIDTH2, MXDG
C
      LOGICAL     REVRS1, ONEIS1
*/


	if ( out->work.empty() ) {
		out->work.set( new long [ 4 * inp->n ] );
		if ( out->work.empty() )
			return NO_SPACE;
		out->wrkLen = 4 * inp->n;
	}

	nextNum = 1;
	err = 0;
	space = 2 * inp->n;

/*
C     ... INITIALIZE COUNTERS, THEN NUMBER ANY NODES OF DEGREE  0.
C         THE LIST OF NODES, BY NEW NUMBER, WILL BE BUILT IN PLACE AT
C         THE FRONT OF THE WORK AREA.
C
      NXTNUM = 1
      ERROR = 0
      SPACE = 2*N
*/
	maxdg = 0;
    for ( i = 1; i <= inp->n; i++ ) {
    	if ( inp->degree[i] < 0 ) {
    		return BAD_ELEMENT;
    	} else if ( inp->degree[i] == 0 ) {
    		out->work[nextNum++] = i;
    	} else {
    		if ( inp->degree[i] > maxdg )
    			maxdg = inp->degree[i];
    	}
    }

/*
C
      MXDG = 0
      DO 300 I = 1, N
          IF  (DEGREE(I))  6000, 100, 200
  100         WORK(NXTNUM) = I
              NXTNUM = NXTNUM + 1
              GO TO 300
  200         IF  (DEGREE(I) .GT. MXDG)  MXDG = DEGREE(I)
  300 CONTINUE
C
*/
	while( nextNum <= inp->n ) {
/*
C     ==============================
C     ... WHILE  NXTNUM <= N  DO ...
C     ==============================
C
 1000 IF  ( NXTNUM .GT. N )  GO TO 2000
C         ... FIND AN UNNUMBERED NODE OF MINIMAL DEGREE
C
*/
		lowdg = maxdg + 1;
		stNode = 0;
		
		for ( i = 1; i <= inp->n; i++ ) {
			if ( inp->degree[i] > 0 && inp->degree[i] < lowdg ) {			
				lowdg = inp->degree[i];
				stNode = i;
			}
		}
/*
          LOWDG = MXDG + 1
          STNODE = 0
          DO 400 I = 1, N
              IF ( (DEGREE(I) .LE. 0) .OR. (DEGREE(I) .GE. LOWDG) )
     1           GO TO 400
                  LOWDG = DEGREE(I)
                  STNODE = I
  400     CONTINUE
*/
		if ( stNode == 0 )
			return BAD_ELEMENT;	// ### FIX ME
/*
C
          IF ( STNODE .EQ. 0 )  GO TO 6100
*/
		avail = (out->wrkLen - nextNum + 1) / 3;
		nLeft = inp->n - nextNum + 1;
		
		space = max0( space, nextNum + 3 * inp->n - 1 );
		if ( avail < inp->n )
			return BAD_ELEMENT;
/*
C
C         ... SET UP PODlInt32ERS FOR THREE LISTS IN WORK AREA, THEN LOOK
C             FOR PSEUDO-DIAMETER, BEGINNING WITH STNODE.
C
          AVAIL = (WRKLEN - NXTNUM + 1) / 3
          NLEFT = N - NXTNUM + 1
          SPACE = MAX0 (SPACE, NXTNUM + 3*N - 1)
          IF ( AVAIL .LT. N )  GO TO 5200
*/
#ifdef _DEBUG
		printf( "nextNum = %d\n", nextNum );
#endif
		treeArray[1].set( out->work, nextNum );	//	one based
		treeArray[2].set( treeArray[1], avail );
		treeArray[3].set( treeArray[2], avail );
		err = findPseudoDiameter( inp, 
				avail, nLeft, 
				stNode, rvNode, 
				treeArray,
//				out->work+nextNum, //	this is actually three arrays length avail
				tree1, tree2,
				active, depth, 
				width1, width2, space );
		space = max0( space, nextNum + 3 * ( active + depth + 1 ) - 1 );
/*
C
          CALL findPseudoDiameter (N, DEGREE, RSTART, CONNEC, AVAIL, NLEFT,
     1                 STNODE, RVNODE, WORK(NXTNUM), TREE1, TREE2,
     2                 ACTIVE, DEPTH, WIDTH1, WIDTH2,
     3                 ERROR, SPACE)
          IF ( ERROR .NE. 0 )  GO TO 5000
          SPACE = MAX0 (SPACE, NXTNUM + 3*(ACTIVE+DEPTH+1) - 1)
*/
		reqd = max0( nextNum + 2 * inp->n + 3 * depth - 1, 3 * inp->n + 2 * depth + 1 );
		space = max0( space, reqd );
		
		if ( out->wrkLen < reqd )
			return NO_SPACE;
/*
C
C         ... DYNAMIC SPACE CHECK FOR MOST OF REMAINDER OF ALGORITHM
C
          REQD = MAX0 (NXTNUM + 2*N + 3*DEPTH - 1, 3*N + 2*DEPTH + 1)
          SPACE = MAX0 (SPACE, REQD)
          IF  ( WRKLEN .LT. REQD )  GO TO 5300
*/
		lvlPtr = nextNum + avail - depth;
		
#ifdef _DEBUG
		printf( "depth = %d, lvlPtr = %d\n", depth, lvlPtr );
#endif
		levelArray[1].set( out->work, lvlPtr );
		levelArray[2].set( levelArray[1], avail );
		levelArray[3].set( levelArray[2], avail );
		
		err = transitionToII( inp->n, avail, active, depth, out->wrkLen, treeArray,
			levelArray, out->work, nextNum, tree1, tree2, width1, width2, oneIs1 );
		
		if ( err != 0 )
			return err;

#ifdef _DEBUG
		printVector( "tree 1", inp->n, out->work.offset(tree1) );
		printVector( "tree 2", inp->n, out->work.offset(tree2) );
#endif

		if ( tree1 != out->wrkLen - inp->n + 1 || tree2 != nextNum )
			return BAD_ELEMENT;	//	FIX ME
/*
C
C
C         ... OUTPUT FROM findPseudoDiameter IS A PAIR OF LEVEL TREES, IN THE FORM
C             OF LISTS OF NODES BY LEVEL.  CONVERT THIS TO TWO LISTS OF
C             OF LEVEL NUMBER BY NODE.  AT THE SAME TIME PACK
C             STORAGE SO THAT ONE OF THE LEVEL TREE VECTORS IS AT THE
C             BACK END OF THE WORK AREA.
C
          LVLPTR = NXTNUM + AVAIL - DEPTH
          CALL transitionToII (N, AVAIL, ACTIVE, DEPTH, WRKLEN, WORK(NXTNUM),
     1                 WORK(LVLPTR), WORK(1), NXTNUM, TREE1,
     2                 TREE2, WIDTH1, WIDTH2, ONEIS1, ERROR, SPACE)
          IF ( ERROR .NE. 0 ) GO TO 5000
          IF (( TREE1 .NE. WRKLEN - N + 1 ) .OR. (TREE2 .NE. NXTNUM))
     1      GO TO 6200
*/
		avail = out->wrkLen - nextNum + 1 - 2 * inp->n - 3 * depth;
		sTotal = inp->n + nextNum;
		empty = sTotal + depth;
		inc1 = tree1 - depth;
		inc2 = inc1 - depth;
		
		err = combineRootedTrees( inp, active, width1, width2, out->work.offset( tree1 ), out->work.offset( tree2 ),
			out->work.offset( empty ), avail, depth, out->work.offset( inc1 ), out->work.offset( inc2 ),
			out->work.offset( sTotal ), oneIs1, revrs1, cSpace );
		
#ifdef _DEBUG
		printVector( "combined tree 1", inp->n, out->work.offset(tree1) );
#endif
		if ( err != 0 )
			return err;
		
		space = max0( space, nextNum + cSpace - 1 );
/*
C
C         ... COMBINE THE TWO LEVEL TREES DlInt32O A MORE GENERAL
C             LEVEL STRUCTURE.
C
          AVAIL = WRKLEN - NXTNUM + 1 - 2*N - 3*DEPTH
          STOTAL = N + NXTNUM
          EMPTY = STOTAL + DEPTH
          INC1 = TREE1 - DEPTH
          INC2 = INC1 - DEPTH
C
          CALL combineRootedTrees (N, DEGREE, RSTART, CONNEC, ACTIVE, WIDTH1,
     1                 WIDTH2, WORK(TREE1), WORK(TREE2), WORK(EMPTY),
     2                 AVAIL, DEPTH, WORK(INC1), WORK(INC2),
     3                 WORK(STOTAL), ONEIS1, REVRS1, ERROR, CSPACE)
C
          IF ( ERROR .NE. 0 )  GO TO 5000
          SPACE = MAX0 (SPACE, NXTNUM + CSPACE - 1)
*/
		lvlPtr = tree1 - (depth + 1);
		lvlList = lvlPtr - active;
		if ( sTotal + depth > lvlPtr )
			return BAD_ELEMENT; //	FIX ME

		err = transitionToIII( inp->n, depth, out->work.offset( tree1 ), out->work.offset( lvlList ),
			out->work.offset( lvlPtr ), out->work.offset( sTotal ) );

		if ( err != 0 )
			return err;
/*
C
C         ... COMBINED LEVEL STRUCTURE IS REPRESENTED BY combineRootedTrees AS
C             A VECTOR OF LEVEL NUMBERS.  FOR RENUMBERING PHASE,
C             CONVERT THIS ALSO TO THE INVERSE PERMUTATION.
C
          LVLPTR = TREE1 - (DEPTH + 1)
          LVLLST = LVLPTR - ACTIVE
          IF ( STOTAL + DEPTH .GT. LVLPTR )  GO TO 6300
C
          CALL transitionToIII (N, ACTIVE, DEPTH, WORK(TREE1), WORK(LVLLST),
     1                 WORK(LVLPTR), WORK(STOTAL), ERROR, SPACE)
          IF  (ERROR .NE. 0)  GO TO 5000
*/
		if ( !inp->optPro ) {
			err = cutHillMckeeRenum( inp, active, out->work.offset( nextNum ), stNode, rvNode, 
				revrs1, depth, out->work.offset( lvlList ), out->work.offset( lvlPtr ), 
				out->work.offset( tree1 ), space );
			nextNum = nextNum + active;
		} else {
			err = kingRenum( inp, lvlList - 1, nextNum, out->work, active, depth,
				out->work.offset( lvlList ), out->work.offset( lvlPtr ), out->work.offset( tree1 ), 
				space );
		}

		if ( err != 0 )
			return err;
/*
C
C         ... NOW RENUMBER ALL MEMBERS OF THIS COMPONENT USING
C             EITHER A REVERSE CUTHILL-MCKEE OR A KING STRATEGY,
C             AS PROFILE OR BANDWIDTH REDUCTION IS MORE IMPORTANT.
C
          IF ( OPTPRO )  GO TO 500
              CALL cutHillMckeeRenum (N, DEGREE, RSTART, CONNEC, ACTIVE,
     1                     WORK(NXTNUM), STNODE, RVNODE, REVRS1, DEPTH,
     2                     WORK(LVLLST), WORK(LVLPTR), WORK(TREE1),
     3                     ERROR, SPACE)
              IF ( ERROR .NE. 0 )  GO TO 5000
              NXTNUM = NXTNUM + ACTIVE
              GO TO 600
C
  500         CALL kingRenum (N, DEGREE, RSTART, CONNEC, LVLLST-1, NXTNUM,
     1                     WORK, ACTIVE, DEPTH, WORK(LVLLST),
     2                     WORK(LVLPTR), WORK(TREE1), ERROR, SPACE)
              IF ( ERROR .NE. 0 )  GO TO 5000
*/
	}	//	end while
/*
C
C         =========================================================
C         ... END OF WHILE LOOP ... REPEAT IF GRAPH IS DISCONNECTED
C         =========================================================
C
  600     GO TO 1000
*/
	if ( out->wrkLen < 2 * inp->n ) 
		return NO_SPACE;
	
	if ( !inp->optPro ) {
		err = compBandProfile( inp, out->work, out->work.offset( inp->n + 1 ), out->permut, 
				out->band, out->profile, space );
	} else {
		err = compBandProfileWithRev( inp, out->work, out->work.offset( inp->n + 1 ), out->permut, 
			out->band, out->profile, space );
	}

	return 0;
/*
C
C     ... CHECK WHETHER INITIAL NUMBERING OR FINAL NUMBERING
C         PROVIDES BETTER RESULTS
C
 2000 IF  (WRKLEN .LT. 2*N)  GO TO 5400
C
      IF  (OPTPRO)  GO TO 2100
          CALL compBandProfile (N, DEGREE, RSTART, CONNEC, WORK(1), WORK(N+1),
     1                 PERMUT, BANDWD, PROFIL, ERROR, SPACE)
          GO TO 2200
C
 2100     CALL compBandProfileWithRev (N, DEGREE, RSTART, CONNEC, WORK(1), WORK(N+1),
     1                 PERMUT, BANDWD, PROFIL, ERROR, SPACE)
C
 2200 RETURN
*/
}

static int
findPseudoDiameter( GPSInput * inp, long avail, long nLeft, 
		long & stNode, long & rvNode, oneBased work[], long & forward, long & bestBack, 
		long & nNodes, long & depth, long & fWidth, long & bWidth, long & space )
{
	
/*
      SUBROUTINE  findPseudoDiameter  (N, DEGREE, RSTART, CONNEC, AVAIL, NLEFT,     GPSKC446
     1                     STNODE, RVNODE, WORK, FORWD, BESTBK, NNODES,
     2                     DEPTH, FWIDTH, BWIDTH, ERROR, SPACE)
C
C     ==================================================================
C
C     FIND A PSEUDO-DIAMETER OF THE MATRIX GRAPH ...
C
C         << BUILD A LEVEL TREE FROM STNODE >>
C         REPEAT
C             << BUILD A LEVEL TREE FROM EACH NODE 'BKNODE' IN THE
C                DEEPEST LEVEL OF  STNODE'S TREE >>
C             << REPLACE 'STNODE' WITH 'BKNODE' IF A DEEPER AND
C                NARROWER TREE WAS FOUND. >>
C         UNTIL
C             << NO FURTHER IMPROVEMENT MADE >>
C
C     ... HEURISTIC ABOVE DIFFERS FROM THE ALGORITHM PUBLISHED IN
C         SIAM J. NUMERICAL ANALYSIS, BUT MATCHES THE CODE
C         DISTRIBUTED BY TOMS.
C
C
C     PARAMETERS :
C
C         N, DEGREE, RSTART & CONNEC  DESCRIBE THE MATRIX STRUCTURE
C
C         WORK   -- WORKING SPACE, OF LENGTH  3*AVAIL, USED TO STORE
C         THREE LEVEL TREES.
C
C         STNODE IS INITIALLY THE NUMBER OF A NODE TO BE USED TO
C             START THE PROCESS, TO BE THE ROOT OF THE FIRST TREE.
C             ON OUTPUT, STNODE IS THE END OF THE PSEUDO-DIAMETER WHOSE
C             LEVEL TREE IS NARROWEST.
C
C         RVNODE WILL BE THE OTHER END OF THE PSEUDO-DIAMETER.
C
C         NNODES WILL BE THE NUMBER OF NODES IN THIS CONNECTED
C             COMPONNENT OF THE MATRIX GRAPH, I.E., THE LENGTH OF
C             THE LEVEL TREES.
C
C         DEPTH  -- THE DEPTH OF THE LEVEL TREES BEING RETURNED,
C                   I.E., THE LENGTH OF THE PSEUDO-DIAMETER.
C
C     ==================================================================
C
C     STRUCTURE OF WORKSPACE ...
C
C     ---------------------------------------------------------------
C     : NUMBERED :  TLIST1  PTR1  :  TLIST2  PTR2  :  TLIST3  PTR3  :
C     ---------------------------------------------------------------
C
C     TLISTI IS A LIST OF NODES OF LENGTH  'ACTIVE'
C     PTRI   IS A LIST OF PODlInt32ERS DlInt32O TLISTI, OF LENGTH  'DEPTH+1'
C
C     ==================================================================
C
      DlInt32EGER     N, RSTART(N), AVAIL, NLEFT,
     1            STNODE, RVNODE, FORWD, BESTBK, NNODES, DEPTH, FWIDTH,
     4            BWIDTH, ERROR, SPACE
C
CIBM  DlInt32EGER *2  DEGREE(N), CONNEC(1), WORK(AVAIL,3)
      DlInt32EGER     DEGREE(N), CONNEC(1), WORK(AVAIL,3)
C
C     ----------------
C
*/
	long	backward, maxDepth, width, fDepth, lastLevel, nLast;
	long	t, i, backNode, lstLevelI;
	int		improv = 1;
	int		err;

/*
      DlInt32EGER     BACKWD, MXDPTH, WIDTH, FDEPTH, LSTLVL,
     1            NLAST, T, I, BKNODE, LSTLVI
C
      LOGICAL     IMPROV

*/
	forward = 1;
	backward = 2;
	bestBack = 3;
	
	err = buildLevelTree( inp, stNode, avail, nLeft, work[forward],
		nNodes, depth, width );
	
	if ( err != 0 )
		return err;
	
	maxDepth = avail - nNodes - 1;
/*
C
C
C     ... BUILD INITIAL LEVEL TREE FROM 'STNODE'.  FIND OUT HOW MANY
C         NODES LIE IN THE CURRENT CONNECTED COMPONENT.
C
      FORWD = 1
      BACKWD = 2
      BESTBK = 3
C
      CALL buildLevelTree (N, DEGREE, RSTART, CONNEC, STNODE, AVAIL, NLEFT,
     1              WORK(1,FORWD), NNODES, DEPTH, WIDTH, ERROR,
     2              SPACE)
      IF ( ERROR .NE. 0 )  GO TO 5000
C
      MXDPTH = AVAIL - NNODES - 1
*/

	while( improv ) {

/*
C
C     ==========================================
C     REPEAT UNTIL NO DEEPER TREES ARE FOUND ...
C     ==========================================
C
*/
		fWidth = width;
		fDepth = depth;
		lastLevel = avail - depth + 1;
		nLast = work[forward][lastLevel - 1] - work[forward][lastLevel];
		lastLevel = work[forward][lastLevel];
		bWidth = inp->n + 1;
/*
 1000     FWIDTH = WIDTH
          FDEPTH = DEPTH
          LSTLVL = AVAIL - DEPTH + 1
          NLAST = WORK (LSTLVL-1, FORWD) - WORK (LSTLVL, FORWD)
          LSTLVL = WORK (LSTLVL, FORWD)
          BWIDTH = N+1
*/
		err = sortDegreeIncreasing( nLast, work[forward].offset( lastLevel ), inp->degree );
		if ( err != 0 )
			return err;
	
#ifdef _DEBUG
		printTree( "after sort", avail, work[forward], depth, nNodes, width );
		printf( "last level = %ld\n", lastLevel );
#endif
/*
C
C         ... SORT THE DEEPEST LEVEL OF 'FORWD' TREE DlInt32O INCREASING
C             ORDER OF NODE DEGREE.
C
          CALL sortDegreeIncreasing (NLAST, WORK(LSTLVL,FORWD), N, DEGREE, ERROR)
          IF  (ERROR .NE. 0)  GO TO 6000
*/
		improv = 0;
		for ( i = 1; i <= nLast; i++ ) {
			lstLevelI = lastLevel + i - 1;
			backNode = work[forward][lstLevelI];
			
			err = buildNarrowerTree( inp, backNode, avail, nNodes, maxDepth, 
				work[backward], depth, width, bWidth );
			
			if ( err != 0 )
				return err;
			
/*
C
C         ... BUILD LEVEL TREE FROM NODES IN 'LSTLVL' UNTIL A DEEPER
C             AND NARROWER TREE IS FOUND OR THE LIST IS EXHAUSTED.
C
          IMPROV = .FALSE.
          DO 1200 I = 1, NLAST
              LSTLVI = LSTLVL + I - 1
              BKNODE = WORK (LSTLVI, FORWD)
              CALL buildNarrowerTree (N, DEGREE, RSTART, CONNEC, BKNODE, AVAIL,
     1                     NNODES, MXDPTH, WORK(1,BACKWD), DEPTH, WIDTH,
     2                     BWIDTH, ERROR, SPACE)
              IF ( ERROR .NE. 0 )  GO TO 5000
*/
			if ( depth > fDepth ) {
				improv = 1;
				t = forward;
				forward = backward;
				backward = t;
				stNode = backNode;
			}
/*
C
              IF ( DEPTH .LE. FDEPTH )  GO TO 1100
C
C                 ... NEW DEEPER TREE ... MAKE IT NEW 'FORWD' TREE
C                     AND BREAK OUT OF 'DO' LOOP.
C
                  IMPROV = .TRUE.
                  T = FORWD
                  FORWD = BACKWD
                  BACKWD = T
                  STNODE = BKNODE
                  GO TO 1300
*/
			else if ( width < bWidth ) {
				t = bestBack;
				bestBack = backward;
				backward = t;
				bWidth = width;
				rvNode = backNode;
			}
		}
/*	
C
C                 ... ELSE CHECK FOR NARROWER TREE.
C
 1100             IF ( WIDTH .GE. BWIDTH )  GO TO 1200
                      T = BESTBK
                      BESTBK = BACKWD
                      BACKWD = T
                      BWIDTH = WIDTH
                      RVNODE = BKNODE
 1200     CONTINUE
*/
	}
/*
C
C         ... END OF REPEAT LOOP
C         ----------------------
C
 1300     IF ( IMPROV )  GO TO 1000
C
*/
	depth = fDepth;
	
	return 0;
/*
      DEPTH = FDEPTH
      RETURN
C
C     ... IN CASE OF ERROR, SIMPLY RETURN ERROR FLAG TO USER.
C
 5000 RETURN
C
 6000 ERROR = 11
      SPACE = -1
      RETURN
C
      END
*/
}


/*
*/
static int 
buildLevelTree( GPSInput * inp, long stNode, long avail, long nLeft,
	oneBased & list, long & active, long & depth, long & width )
{
/*
      SUBROUTINE   buildLevelTree   (N, DEGREE, RSTART, CONNEC, STNODE, AVAIL,  GPSKC599
     1                       NLEFT, LIST, ACTIVE, DEPTH, WIDTH, ERROR,
     2                       SPACE)
C
C     ==================================================================
C     BUILD THE LEVEL TREE ROOTED AT 'STNODE' IN THE SPACE PROVIDED IN
C     LIST.  CHECK FOR OVERRUN OF SPACE ALLOCATION.
C     ==================================================================
C
      DlInt32EGER     N, RSTART(N), STNODE, AVAIL, NLEFT,
     1            ACTIVE, DEPTH, WIDTH, ERROR, SPACE
C
CIBM  DlInt32EGER *2  DEGREE(N), CONNEC(1), LIST(AVAIL)
      DlInt32EGER     DEGREE(N), CONNEC(1), LIST(AVAIL)
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
	long	lStart, nLevel, front, j, newNode, ptr, cDegree, lFront;

/*
      DlInt32EGER         LSTART, NLEVEL, FRONT, J, NEWNOD, PTR, CDGREE,
     1                LFRONT, LISTJ
C
C     ... BUILD THE LEVEL TREE USING  LIST  AS A QUEUE AND LEAVING
C         THE NODES IN PLACE.  THIS GENERATES THE NODES ORDERED BY LEVEL
C         PUT PODlInt32ERS TO THE BEGINNING OF EACH LEVEL, BUILDING FROM
C         THE BACK OF THE WORK AREA.
C
*/
	active = 1;
	depth = 0;
	width = 0;
	lStart = 1;
	front = 1;
	list[active] = stNode;
	inp->degree[stNode] = -inp->degree[stNode];
	list[avail] = 1;
	nLevel = avail;

/*
      ACTIVE = 1
      DEPTH = 0
      WIDTH = 0
      ERROR = 0
      LSTART = 1
      FRONT = 1
      LIST (ACTIVE) = STNODE
      DEGREE (STNODE) = -DEGREE (STNODE)
      LIST (AVAIL)  = 1
      NLEVEL = AVAIL
*/

	while( 1 ) {
		if ( front >= lStart ) {
/*
C
C     ... REPEAT UNTIL QUEUE BECOMES EMPTY OR WE RUN OUT OF SPACE.
C     ------------------------------------------------------------
C
 1000     IF ( FRONT .LT. LSTART ) GO TO 1100
C
C         ... FIRST NODE OF LEVEL.  UPDATE PODlInt32ERS.
C

*/
			lStart = active + 1;
			width = max0( width, lStart - list[nLevel] );
			nLevel--;
			depth++;
			if ( nLevel <= active )
				return BAD_ELEMENT;
			
			list[nLevel] = lStart;
/*
              LSTART = ACTIVE + 1
              WIDTH = MAX0 (WIDTH, LSTART - LIST(NLEVEL))
              NLEVEL = NLEVEL - 1
              DEPTH = DEPTH + 1
              IF ( NLEVEL .LE. ACTIVE )  GO TO 5000
                  LIST (NLEVEL) = LSTART
*/
		}

		lFront = list[front];
		ptr = inp->rstart[lFront];
		cDegree = -inp->degree[lFront];
		
		if ( cDegree <= 0 )
			return BAD_ELEMENT;
	
/*
C
C         ... FIND ALL NEIGHBORS OF CURRENT NODE, ADD THEM TO QUEUE.
C
 1100     LFRONT = LIST (FRONT)
          PTR = RSTART (LFRONT)
          CDGREE = -DEGREE (LFRONT)
          IF (CDGREE .LE. 0)  GO TO 6000
*/

		for ( j = 1; j <= cDegree; j++ ) {
			newNode = inp->connect[ptr];
			ptr++;

/*
          DO 1200 J = 1, CDGREE
              NEWNOD = CONNEC (PTR)
              PTR = PTR + 1
*/

			if ( inp->degree[newNode] > 0 ) {
				inp->degree[newNode] = -inp->degree[newNode];
				active++;
				if ( nLevel <= active )
					return NO_SPACE;
				if ( active > nLeft )
					return BAD_ELEMENT;
				list[active] = newNode;
			}
		}
		
		front++;

/*
C
C             ... ADD TO QUEUE ONLY NODES NOT ALREADY IN QUEUE
C
              IF ( DEGREE(NEWNOD) .LE. 0 )  GO TO 1200
                  DEGREE (NEWNOD) = -DEGREE (NEWNOD)
                  ACTIVE = ACTIVE + 1
                  IF ( NLEVEL .LE. ACTIVE )  GO TO 5000
                  IF ( ACTIVE .GT. NLEFT  )  GO TO 6000
                      LIST (ACTIVE) = NEWNOD
 1200     CONTINUE
          FRONT = FRONT + 1
*/

		if ( front > active )
			break;
	}
			
/*
C
C         ... IS QUEUE EMPTY?
C         -------------------
C
          IF ( FRONT .LE. ACTIVE )  GO TO 1000
*/

	for ( j = 1; j <= active; j++ ) {
		inp->degree[list[j]] = -inp->degree[list[j]];
	}
#ifdef _DEBUG
	printTree( "from front", avail, list, depth, active, width );
#endif
	return 0;
/*
C
C     ... YES, THE TREE IS BUILT.  UNDO OUR MARKINGS.
C
      DO 1300 J = 1, ACTIVE
          LISTJ = LIST(J)
          DEGREE (LISTJ) = -DEGREE (LISTJ)
 1300 CONTINUE
C
      RETURN
*/

}

static int
buildNarrowerTree( GPSInput * inp, long stNode, long avail, long active,
		long maxDepth, oneBased list, long & depth, long & width,
		long & maxWidth )
{

/*

      SUBROUTINE   buildNarrowerTree   (N, DEGREE, RSTART, CONNEC, STNODE, AVAIL,  GPSKC719
     1                       ACTIVE, MXDPTH, LIST, DEPTH, WIDTH, MAXWID,
     2                       ERROR, SPACE)
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
      DlInt32EGER     N, RSTART(N), STNODE, AVAIL, ACTIVE, MXDPTH,
     1            DEPTH, WIDTH, MAXWID, ERROR, SPACE
C
CIBM  DlInt32EGER *2  DEGREE(N), CONNEC(1), LIST(AVAIL)
      DlInt32EGER     DEGREE(N), CONNEC(1), LIST(AVAIL)
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
*/
	long lStart, nLevel, front, j, newNode, ptr, back, sPtr,
			fPtr, lFront;

/*
      DlInt32EGER     LSTART, NLEVEL, FRONT, J, NEWNOD, PTR, BACK,
     1            SPTR, FPTR, LFRONT, LISTJ
C
C     ... BUILD THE LEVEL TREE USING  LIST  AS A QUEUE AND LEAVING
C         THE NODES IN PLACE.  THIS GENERATES THE NODES ORDERED BY LEVEL
C         PUT PODlInt32ERS TO THE BEGINNING OF EACH LEVEL, BUILDING FROM
C         THE BACK OF THE WORK AREA.
C

*/
	back = 1;
	depth = 0;
	width = 0;
	lStart = 1;
	front = 1;
	list[back] = stNode;
	inp->degree[stNode] = -inp->degree[stNode];
	list[avail] = 1;
	nLevel = avail;

/*
      BACK = 1
      DEPTH = 0
      WIDTH = 0
      ERROR = 0
      LSTART = 1
      FRONT = 1
      LIST (BACK) = STNODE
      DEGREE (STNODE) = -DEGREE (STNODE)
      LIST (AVAIL)  = 1
      NLEVEL = AVAIL
*/

	while( 1 ) {
		if ( front >= lStart ) {
/*
C
C     ... REPEAT UNTIL QUEUE BECOMES EMPTY OR WE RUN OUT OF SPACE.
C     ------------------------------------------------------------
C
 1000     IF ( FRONT .LT. LSTART ) GO TO 1100
C
C         ... FIRST NODE OF LEVEL.  UPDATE PODlInt32ERS.
C
*/
			lStart = back + 1;
			width = max0( lStart - list[nLevel], width );
			if ( width >= maxWidth ) {
				width = inp->n + 1;
				depth = 0;
				goto done;
			}
			nLevel--;
			depth++;
			if ( depth > maxDepth )
				return NO_SPACE;
			
			list[nLevel] = lStart;
		}
/*
              LSTART = BACK + 1
              WIDTH = MAX0 (WIDTH, LSTART - LIST(NLEVEL))
              IF  ( WIDTH .GE. MAXWID )  GO TO 2000
              NLEVEL = NLEVEL - 1
              DEPTH = DEPTH + 1
              IF ( DEPTH .GT. MXDPTH )  GO TO 5000
                  LIST (NLEVEL) = LSTART
*/

		lFront = list[front];
		sPtr = inp->rstart[lFront];
		fPtr = sPtr - inp->degree[lFront] - 1;
		
		for ( ptr = sPtr; ptr <= fPtr; ptr++ ) {
			newNode = inp->connect[ptr];
/*
C
C         ... FIND ALL NEIGHBORS OF CURRENT NODE, ADD THEM TO QUEUE.
C
 1100     LFRONT = LIST (FRONT)
          SPTR = RSTART (LFRONT)
          FPTR = SPTR - DEGREE (LFRONT) - 1
          DO 1200 PTR = SPTR, FPTR
              NEWNOD = CONNEC (PTR)
*/
			if ( inp->degree[newNode] > 0 ) {
				inp->degree[newNode] = -inp->degree[newNode];
				back++;
				list[back] = newNode;
			}
		}
		
		front++;
/*
C
C             ... ADD TO QUEUE ONLY NODES NOT ALREADY IN QUEUE
C
              IF ( DEGREE(NEWNOD) .LE. 0 )  GO TO 1200
                  DEGREE (NEWNOD) = -DEGREE (NEWNOD)
                  BACK = BACK + 1
                  LIST (BACK) = NEWNOD
 1200     CONTINUE
          FRONT = FRONT + 1
*/

		if ( front > back )
			break;
	}
/*
C
C         ... IS QUEUE EMPTY?
C         -------------------
C
          IF ( FRONT .LE. BACK )  GO TO 1000
C
C     ... YES, THE TREE IS BUILT.  UNDO OUR MARKINGS.
C
*/

	if ( back != active )
		return BAD_ELEMENT;
/*
      IF (BACK .NE. ACTIVE)  GO TO 6000
C
*/
done:
	for ( j = 1; j <= back; j++ ) {
		long	t = list[j];
		inp->degree[t] = -inp->degree[t];
	}

#ifdef _DEBUG
	if ( depth != 0 )
		printTree( "narrower tree", avail, list, depth, active, width );
#endif

	return 0;
/*
 1300 DO 1400 J = 1, BACK
          LISTJ = LIST(J)
          DEGREE (LISTJ) = -DEGREE (LISTJ)
 1400 CONTINUE
C
      RETURN
C
C     ... ABORT GENERATION OF TREE BECAUSE IT IS ALREADY TOO WIDE
C
 2000 WIDTH = N + 1
      DEPTH = 0
      GO TO 1300
C
C     ... INSUFFICIENT STORAGE ...
C
 5000 SPACE = 3 * ( (ACTIVE+1-BACK)*DEPTH / ACTIVE + (ACTIVE+1-BACK) )
      ERROR = 111
      RETURN
C
 6000 ERROR = 13
      SPACE = -1
      RETURN
C
      END
*/

}

static int
transitionToII( long n, long avail, long active, long depth, long workLen, 
		oneBased levelList[], oneBased levelPtr[], oneBased work, long nextNum, 
		long & tree1, long & tree2, long & width1, long & width2, int & oneIs1 )
{

/*
      SUBROUTINE   transitionToII   (N, AVAIL, ACTIVE, DEPTH, WRKLEN,           GPSKC855
     1                       LVLLST, LVLPTR, WORK, NXTNUM, TREE1, TREE2,
     2                       WIDTH1, WIDTH2, ONEIS1, ERROR, SPACE)
C
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
C     INPUT:  TWO LEVEL TREES, AS LEVEL LISTS AND LEVEL PODlInt32ERS,
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
C
      DlInt32EGER     N, AVAIL, ACTIVE, DEPTH, WRKLEN, NXTNUM,
     1            WIDTH1, WIDTH2, TREE1, TREE2, ERROR, SPACE
C
CIBM  DlInt32EGER *2  LVLLST(AVAIL,3), LVLPTR(AVAIL,3), WORK(WRKLEN)
      DlInt32EGER     LVLLST(AVAIL,3), LVLPTR(AVAIL,3), WORK(WRKLEN)
C
      LOGICAL     ONEIS1
C
C     ------------------------------------------------------------------
C
*/
	long	i, bTree, fTree, fWidth, bWidth;
/*
      DlInt32EGER     I, BTREE, FTREE, FWIDTH, BWIDTH
C
*/
	if ( 3 * avail > workLen )
		return BAD_ELEMENT;
	if ( avail < n )
		return NO_SPACE;
/*
C
C     ... CHECK THAT WE HAVE ENOUGH ROOM TO DO THE NECESSARY UNPACKING
C
      IF (3*AVAIL .GT. WRKLEN)  GO TO 6000
      IF (AVAIL .LT. N)  GO TO 5100
*/
	fTree = tree1;
	bTree = tree2;
	fWidth = width1;
	bWidth =  width2;
	
	tree1 = workLen - n + 1;
	tree2 = nextNum;
/*
C
C     ... INPUT HAS THREE POSSIBLE CASES:
C             LVLLST(*,1) IS EMPTY
C             LVLLST(*,2) IS EMPTY
C             LVLLST(*,3) IS EMPTY
C
      FTREE = TREE1
      BTREE = TREE2
      FWIDTH = WIDTH1
      BWIDTH = WIDTH2
C
      TREE1 = WRKLEN - N + 1
      TREE2 = NXTNUM
*/
	if ( fTree != 1 && bTree != 1 ) {
/*
C
      IF ( (FTREE .EQ. 1) .OR. (BTREE .EQ. 1) )  GO TO 300
C
C         ... CASE 1:  1ST SLOT IS EMPTY.  UNPACK 3 DlInt32O 1, 2 DlInt32O 3
C
*/
		if ( fTree == 2 ) {
			oneIs1 = 1;
			width1 = fWidth;
			width2 = bWidth;
		} else {
			oneIs1 = 0;
			width1 = bWidth;
			width2 = fWidth;
		}
		
		convertToLevelNums( n, active, depth, levelList[3], levelPtr[3], work.offset( tree2 ), oneIs1 );
		convertToLevelNums( n, active, depth, levelList[2], levelPtr[2], work.offset( tree1 ), !oneIs1 );
/*
          IF (FTREE .NE. 2)  GO TO 100
              ONEIS1 = .TRUE.
              WIDTH2 = BWIDTH
              WIDTH1 = FWIDTH
              GO TO 200
C
  100         ONEIS1 = .FALSE.
              WIDTH1 = BWIDTH
              WIDTH2 = FWIDTH
C
  200     CALL convertToLevelNums (N, ACTIVE, DEPTH, LVLLST(1,3), LVLPTR(1,3),
     1                    WORK(TREE2), ONEIS1)
C
          CALL convertToLevelNums (N, ACTIVE, DEPTH, LVLLST(1,2), LVLPTR(1,2),
     1                    WORK(TREE1), .NOT. ONEIS1)
C
          GO TO 1000
C
C
*/
	} else {

		if ( fTree != 2 && bTree != 2 ) {
/*
  300 IF ( (FTREE .EQ. 2) .OR. (BTREE .EQ. 2) )  GO TO 600
C
C         ... CASE 2:  2ND SLOT IS EMPTY.  TO ENABLE COMPLETE
C              REPACKING, MOVE 3 DlInt32O 2, THEN FALL DlInt32O NEXT CASE
C
*/
			for ( i = 1; i <= active; i++ )
				levelList[2][i] = levelList[3][i]; 
/*
          DO 400 I = 1, ACTIVE
              LVLLST(I,2) = LVLLST(I,3)
  400     CONTINUE
*/
			for ( i = 1; i <= depth; i++ )
				levelPtr[2][i] = levelPtr[3][i]; 
/*
C
          DO 500 I = 1, DEPTH
              LVLPTR(I,2) = LVLPTR(I,3)
  500     CONTINUE
*/
		}

		if ( fTree != 1 ) {
			oneIs1 = 0;
			width1 = bWidth;
			width2 = fWidth;
/*
C
C         ... CASE 3:  SLOT 3 IS EMPTY.  MOVE 1 DlInt32O 3, THEN 2 DlInt32O 1.
C
600     IF (FTREE .EQ. 1) GO TO 700
          ONEIS1 = .FALSE.
          WIDTH1 = BWIDTH
          WIDTH2 = FWIDTH
          GO TO 800
*/
		} else {
			oneIs1 = 1;
			width1 = fWidth;
			width2 = bWidth;
/*
C
700         ONEIS1 = .TRUE.
          WIDTH1 = FWIDTH
          WIDTH2 = BWIDTH
*/
		}

		convertToLevelNums( n, active, depth, levelList[1], levelPtr[1],
			work.offset( tree1 ), !oneIs1 );
		convertToLevelNums( n, active, depth, levelList[2], levelPtr[2],
			work.offset( tree2 ), oneIs1 );
	}
/*
C
  800     CALL convertToLevelNums (N, ACTIVE, DEPTH, LVLLST(1,1), LVLPTR(1,1),
     1                    WORK(TREE1), .NOT. ONEIS1)
C
          CALL convertToLevelNums (N, ACTIVE, DEPTH, LVLLST(1,2), LVLPTR(1,2),
     1                    WORK(TREE2), ONEIS1)
*/
	return 0;
/*
 1000 RETURN
C
C     ------------------------------------------------------------------
C
 5100 SPACE = 3 * (N - AVAIL)
      ERROR = 120
      RETURN
C
 6000 ERROR = 20
      SPACE = -1
      RETURN
C
      END
*/
}


static void
convertToLevelNums( long n, long active, long depth, const oneBased & levelList, const oneBased & levelPtr,
	oneBased levelNum, int reverse )
{
/*
      SUBROUTINE  convertToLevelNums  (N, ACTIVE, DEPTH, LVLLST, LVLPTR, LVLNUM,    GPSKC996
     1                     REVERS)
C
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
C
      DlInt32EGER     N, ACTIVE, DEPTH
C
CIBM  DlInt32EGER *2  LVLLST(ACTIVE), LVLPTR(DEPTH), LVLNUM(N)
      DlInt32EGER     LVLLST(ACTIVE), LVLPTR(DEPTH), LVLNUM(N)
      LOGICAL     REVERS
C
C     ------------------------------------------------------------------
C
*/
	long i, level, lStart, lEnd, xLevel, plStart, lvlLsti;
/*
      DlInt32EGER     I, LEVEL, LSTART, LEND, XLEVEL, PLSTRT, LVLLSI
C
*/
	if ( active != n ) {
		for ( i = 1; i <= n; i++ )
			levelNum[i] = 0;
/*
      IF  (ACTIVE .EQ. N)  GO TO 200
C
C         ... IF NOT ALL NODES OF GRAPH ARE ACTIVE, MASK OUT THE
C             NODES WHICH ARE NOT ACTIVE
C
          DO 100 I = 1, N
              LVLNUM(I) = 0
  100     CONTINUE
C
*/
	}
	
	for ( level = 1; level <= depth; level++ ) {
		xLevel = level;
		plStart = depth - level + 1;
		if ( reverse )
			xLevel = plStart;
		lStart = levelPtr[plStart];
		lEnd = levelPtr[plStart - 1] - 1;
					
/*
  200 DO 400 LEVEL = 1, DEPTH
          XLEVEL = LEVEL
          PLSTRT = DEPTH - LEVEL + 1
          IF (REVERS) XLEVEL = PLSTRT
          LSTART = LVLPTR (PLSTRT)
          LEND = LVLPTR (PLSTRT - 1) - 1
C
*/
		for ( i = lStart; i <= lEnd; i++ ) {
			lvlLsti = levelList[i];
			levelNum[lvlLsti] = xLevel;
		}
/*
          DO 300 I = LSTART, LEND
              LVLLSI = LVLLST(I)
              LVLNUM (LVLLSI) = XLEVEL
  300     CONTINUE
*/
	}
/*
  400 CONTINUE
C
      RETURN
      END
*/
}

static int
combineRootedTrees( GPSInput * inp, long active, long width1,
	long width2, oneBased tree1, oneBased tree2, oneBased work, long workLen, long depth,
	oneBased inc1, oneBased inc2, oneBased total, int oneIs1, int & reverse, long & space )
{

/*
      SUBROUTINE   combineRootedTrees   (N, DEGREE, RSTART, CONNEC, ACTIVE, WIDTH1, GPSK1047
     1                       WIDTH2, TREE1, TREE2, WORK, WRKLEN, DEPTH,
     2                       INC1, INC2, TOTAL, ONEIS1, REVRS1, ERROR,
     3                       SPACE)
C
C     ==================================================================
C
C     COMBINE THE TWO ROOTED LEVEL TREES DlInt32O A SINGLE LEVEL STRUCTURE
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
C
      DlInt32EGER     N, RSTART(N), ACTIVE, WIDTH1, WIDTH2, WRKLEN, DEPTH,
     2            ERROR, SPACE
C
CIBM  DlInt32EGER *2  DEGREE(N), CONNEC(1), TREE1(N), TREE2(N),
      DlInt32EGER     DEGREE(N), CONNEC(1), TREE1(N), TREE2(N),
     1            WORK(WRKLEN), INC1(DEPTH), INC2(DEPTH), TOTAL(DEPTH)
C
      LOGICAL     ONEIS1, REVRS1
C
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
C         START, SIZE ARE PODlInt32ERS DlInt32O 'NODES', ONE OF EACH FOR
C         EACH CONNECTED COMPONENT OF THE REDUCED GRAPH.
C         THE SIZES OF NODES, START AND SIZE ARE NOT KNOWN APRIORI.
C
C     ==================================================================
*/
	long	i, size, avail, cStop, start, compon, tree1i, pcStart,
			cStart, maxInc1, maxInc2, compns, maxComp, offDiag,
			cSize, pcSize, worki, tWorki;
	int		err;
/*
      DlInt32EGER     I, SIZE, AVAIL, CSTOP, START, COMPON, TREE1I, PCSTRT,
     1            CSTART, MXINC1, MXINC2, COMPNS, MXCOMP, OFFDIA,
     2            CSIZE, PCSIZE, WORKI, TWORKI
C
C     ------------------------------------------------------------------
C
*/
	offDiag = active;
/*
C     ... FIND ALL SHORTEST PATHS FROM START TO FINISH.  REMOVE NODES ON
C         THESE PATHS AND IN OTHER CONNECTED COMPONENTS OF FULL GRAPH
C         FROM FURTHER CONSIDERATION.  SIGN OF ENTRIES IN TREE1 IS USED
C         AS A MASK.
C
      OFFDIA = ACTIVE
*/
	for ( i = 1; i <= depth; i++ )
		total[i] = 0;
/*
C
      DO 100 I = 1, DEPTH
          TOTAL(I) = 0
  100 CONTINUE
C
*/
	for ( i = 1; i <= inp->n; i++ ) {
		tree1i = tree1[i];
		if ( tree1i == tree2[i] && tree1i != 0 ) {
			total[tree1i]++;
			tree1[i] = -tree1[i];
			offDiag--;
		}
	}
#ifdef _DEBUG
	printf( "offDiag = %d\n", offDiag);
	printVector("tree1", inp->n, tree1);
	printVector("tree2", inp->n, tree2);
	printVector("total", depth, total);
#endif
/*
      DO 200 I = 1, N
          TREE1I = TREE1 (I)
          IF ((TREE1(I) .NE. TREE2(I)) .OR. (TREE1(I) .EQ. 0)) GO TO 200
              TOTAL (TREE1I) = TOTAL (TREE1I) + 1
              TREE1(I) = - TREE1(I)
              OFFDIA = OFFDIA - 1
  200 CONTINUE
*/
	if ( offDiag != 0 ) {
		if ( offDiag < 0 )
			return BAD_ELEMENT;
/*
C
      IF ( OFFDIA .EQ. 0 )  GO TO 1100
      IF ( OFFDIA .LT. 0 )  GO TO 6000
*/
		avail = workLen - offDiag;
		maxComp = avail / 2;
		start = offDiag + 1;
		size = start + maxComp;
/*
C
C     ... FIND CONNECTED COMPONENTS OF GRAPH INDUCED BY THE NODES NOT
C         REMOVED.  'MXCOMP' IS THE LARGEST NUMBER OF COMPONENTS
C         REPRESENTABLE IN THE WORKING SPACE AVAILABLE.
C
      AVAIL = WRKLEN - OFFDIA
      MXCOMP = AVAIL/2
      START = OFFDIA + 1
      SIZE = START + MXCOMP
C
*/
		if ( maxComp <= 0 )
			return BAD_ELEMENT;
/*
      IF  (MXCOMP .LE. 0)  GO TO 5100
C
*/
		err = findConnectedComps( inp, tree1, offDiag, work, maxComp, work.offset( start ),
			work.offset( size ), compns, space );
		if ( err != 0 )
			return err;
#ifdef _DEBUG
		printVector("start", compns, work.offset(start));
		printVector("size", compns, work.offset(size));
		printVector("tree1", inp->n, tree1);
		printVector("work", inp->n, work);
		printVector("total", depth, total);
#endif
/*
      CALL findConnectedComps (N, DEGREE, RSTART, CONNEC, TREE1, OFFDIA, WORK,
     1             MXCOMP, WORK(START), WORK(SIZE), COMPNS, ERROR,
     2             SPACE)
      IF ( ERROR .NE. 0 )  GO TO 5000
*/
		space = 2 * inp->n + 3 * depth + 2 * compns + offDiag;
/*
C
C     ... RECORD SPACE ACTUALLY USED  (NOT INCLUDING  NUMBERED )
C
      SPACE = 2*N + 3*(DEPTH) + 2*COMPNS + OFFDIA
*/
		if ( compns > 1 ) {
			err = sortIndexListDecreasing( compns, work.offset( size ), work.offset( start ) );
			if ( err != 0 )
				return err;
		}
/*
C
C     ... SORT THE COMPONENT START PODlInt32ERS DlInt32O INCREASING ORDER
C         OF SIZE OF COMPONENT
C
      IF (COMPNS .GT. 1)
     1    CALL sortIndexListDecreasing (COMPNS, WORK(SIZE), WORK(START), ERROR)
          IF  (ERROR .NE. 0)  GO TO 6200
*/
		for ( compon = 1; compon <= compns; compon++ ) {
			pcStart = start + compon - 1;
			cStart = work[pcStart];
			pcSize = size + compon - 1;
			cSize = work[pcSize];
			cStop = cStart + cSize - 1;
			if ( cSize < 0 || cSize > offDiag )
				return BAD_ELEMENT;
/*
C
C     ... FOR EACH COMPONENT IN TURN, CHOOSE TO USE THE ORDERING OF THE
C         'FORWARD' TREE1 OR OF THE 'BACKWARD' TREE2 TO NUMBER THE NODES
C         IN THIS COMPONENT.  THE NUMBERING IS CHOSEN TO MINIMIZE THE
C         MAXIMUM INCREMENT TO ANY LEVEL.
C
      DO 1000 COMPON = 1, COMPNS
          PCSTRT = START + COMPON - 1
          CSTART = WORK (PCSTRT)
          PCSIZE = SIZE + COMPON - 1
          CSIZE = WORK (PCSIZE)
          CSTOP  = CSTART + CSIZE - 1
          IF ( ( CSIZE .LT. 0 ) .OR. ( CSIZE .GT. OFFDIA ) )  GO TO 6100
C
*/
			for ( i = 1; i <= depth; i++ ) {
				inc1[i] = 0;
				inc2[i] = 0;
			}
/*
          DO 300 I = 1, DEPTH
              INC1(I) = 0
              INC2(I) = 0
  300     CONTINUE
*/
			maxInc1 = 0;
			maxInc2 = 0;
/*
C
          MXINC1 = 0
          MXINC2 = 0
*/
			for ( i = cStart; i <= cStop; i++ ) {
				worki = work[i];
				tWorki = -tree1[worki];
				inc1[tWorki]++;
				tWorki = tree2[worki];
				inc2[tWorki]++;
			}

#ifdef _DEBUG
			printVector("inc1", depth, inc1);
			printVector("inc2", depth, inc2);
#endif
/*
C
          DO 400 I = CSTART, CSTOP
              WORKI = WORK(I)
              TWORKI = -TREE1 (WORKI)
              INC1 (TWORKI) = INC1 (TWORKI) + 1
              TWORKI =  TREE2 (WORKI)
              INC2 (TWORKI) = INC2 (TWORKI) + 1
  400     CONTINUE
*/
			for ( i = 1; i <= depth; i++ ) {
				if ( inc1[i] != 0 || inc2[i] != 0 ) {
					if ( maxInc1 < total[i] + inc1[i] )
						maxInc1 = total[i] + inc1[i];
					if ( maxInc2 < total[i] + inc2[i] )
						maxInc2 = total[i] + inc2[i];
				}
			}
/*
C
C         ... BAROQUE TESTS BELOW DUPLICATE THE GIBBS-POOLE-STOCKMEYER-
C             CRANE PROGRAM, *** NOT *** THE PUBLISHED ALGORITHM.
C
          DO 500 I = 1, DEPTH
              IF ((INC1(I) .EQ. 0) .AND. (INC2(I) .EQ. 0))  GO TO 500
                  IF  (MXINC1  .LT.  TOTAL(I) + INC1(I))
     1                 MXINC1 = TOTAL(I) + INC1(I)
                  IF  (MXINC2  .LT.  TOTAL(I) + INC2(I))
     1                 MXINC2 = TOTAL(I) + INC2(I)
  500     CONTINUE
*/
			if ( !(
				maxInc1 > maxInc2 || (
				maxInc1 == maxInc2 && ( 
					width1 > width2 || (width1 == width2 && oneIs1 != 0) ) ) ) ) {
/*
C
C         ... USE ORDERING OF NARROWER TREE UNLESS IT INCREASES
C             WIDTH MORE THAN WIDER TREE.  IN CASE OF TIE, USE TREE 2!
C
          IF ( (MXINC1 .GT. MXINC2)  .OR.
     1         ( (MXINC1 .EQ. MXINC2) .AND. ( (WIDTH1 .GT. WIDTH2) .OR.
     2                                        ( (WIDTH1 .EQ. WIDTH2)
     3                                         .AND. ONEIS1) ) ) )
     4      GO TO 700
*/
				if ( compon == 1 )
					reverse = !oneIs1;
/*
C
              IF ( COMPON .EQ. 1 )  REVRS1 = .NOT. ONEIS1
*/			
				for ( i = 1; i <= depth; i++ ) {
					total[i] = total[i] + inc1[i];
				}
/*
C
              DO 600 I = 1, DEPTH
                  TOTAL(I) = TOTAL(I) + INC1(I)
  600         CONTINUE
              GO TO 1000
*/
			} else {
				if ( compon == 1 )
					reverse = oneIs1;
				
				for ( i = cStart; i <= cStop; i++ ) {
					worki = work[i];
					tree1[worki] = -tree2[worki];
				}
				
/*
C
  700         IF ( COMPON .EQ. 1 )  REVRS1 = ONEIS1
              DO 800 I = CSTART, CSTOP
                  WORKI = WORK(I)
                  TREE1 (WORKI) = - TREE2 (WORKI)
  800         CONTINUE
*/
				for ( i = 1; i <= depth; i++ ) {
					total[i] += inc2[i];
				}
/*
C
              DO 900 I = 1, DEPTH
                  TOTAL(I) = TOTAL(I) + INC2(I)
  900         CONTINUE
*/
			}
/*
C
 1000 CONTINUE
*/

 		}
 		return 0;
/*
      GO TO 2000
C
C     ... DEFAULT WHEN THE REDUCED GRAPH IS EMPTY
C
*/
	}

	reverse = 1;
	space = 2 * inp->n;
	return 0;
/*
 1100 REVRS1 = .TRUE.
      SPACE = 2*N
C
 2000 RETURN
C
C     ------------------------------------------------------------------
C
C     ERROR FOUND ...
C
 5000 SPACE = -1
      GO TO 2000
C
 5100 SPACE = 2 - AVAIL
      ERROR = 131
      GO TO 2000
C
 6000 ERROR = 30
      GO TO 5000
C
 6100 ERROR = 31
      GO TO 5000
C
 6200 ERROR = 32
      GO TO 5000
C
      END

*/
}

static int
findConnectedComps( GPSInput * inp, oneBased status, long nReduce, oneBased work,
		long maxComp, oneBased start, oneBased size, long & compns, long & space )
{
/*
      SUBROUTINE   findConnectedComps   (N, DEGREE, RSTART, CONNEC, STATUS, NREDUC, GPSK1289
     1                       WORK, MXCOMP, START, SIZE, COMPNS, ERROR,
     2                       SPACE)
C
C     ==================================================================
C
C     FIND THE CONNECTED COMPONENTS OF THE GRAPH INDUCED BY THE SET
C     OF NODES WITH POSITIVE 'STATUS'.  WE SHALL BUILD THE LIST OF
C     CONNECTED COMPONENTS IN 'WORK', WITH A LIST OF PODlInt32ERS
C     TO THE BEGINNING NODES OF COMPONENTS LOCATED IN 'START'
C
C
      DlInt32EGER     N, RSTART(N), NREDUC, MXCOMP, COMPNS, ERROR, SPACE
C
CIBM  DlInt32EGER *2  DEGREE(N), CONNEC(1), STATUS(N), WORK(NREDUC),
      DlInt32EGER     DEGREE(N), CONNEC(1), STATUS(N), WORK(NREDUC),
     1            START(MXCOMP), SIZE(MXCOMP)
C
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
C         START  -- PODlInt32ER TO BEGINNING OF  I-TH  CONNECTED COMPONENT
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
C
*/
	long i, j, free, jPtr, node, jNode, front, cDegree, root;
/*
      DlInt32EGER     I, J, FREE, JPTR, NODE, JNODE, FRONT, CDGREE, ROOT
C
C     ------------------------------------------------------------------
C
C
C     REPEAT
C         << FIND AN UNASSIGNED NODE AND START A NEW COMPONENT >>
C         REPEAT
C             << ADD ALL NEW NEIGHBORS OF FRONT NODE TO QUEUE, >>
C             << REMOVE FRONT NODE.                            >>
C         UNTIL <<QUEUE EMPTY>>
C     UNTIL << ALL NODES ASSIGNED >>
*/
	free = 1;
	compns = 0;
	root = 1;
	node = 0;
/*
C
      FREE   = 1
      COMPNS = 0
      ROOT   = 1
*/
	while( 1 ) {
/*
C
C     ... START OF OUTER REPEAT LOOP
*/
		for ( i = root; i <= inp->n; i++ ) {
			if ( status[i] >= 0 ) {
				node = i;
				break;
			}
		}
		
		if ( node == 0 )
			return BAD_ELEMENT;
/*
C
C         ... FIND AN UNASSIGNED NODE
C
  100     DO 200 I = ROOT, N
              IF (STATUS(I) .LE. 0) GO TO 200
                  NODE = I
                  GO TO 300
  200     CONTINUE
          GO TO 6100
*/
		compns++;
		root = node + 1;
		if ( compns > maxComp ) {
			space = nReduce - free;
			return NO_SPACE;
		}
		
		start[compns] = free;
		work[free] = node;
		status[node] = -status[node];
		front = free++;
/*
C
C         ... START NEW COMPONENT
C
  300     COMPNS = COMPNS + 1
          ROOT   = NODE + 1
          IF (COMPNS .GT. MXCOMP)  GO TO 5000
          START (COMPNS) = FREE
          WORK (FREE) = NODE
          STATUS (NODE) = -STATUS (NODE)
          FRONT = FREE
          FREE = FREE + 1
*/
		while ( 1 ) {
/*
C
C             ... INNER REPEAT UNTIL QUEUE BECOMES EMPTY
*/
			node = work[front++];
/*
C
  400         NODE = WORK (FRONT)
              FRONT = FRONT + 1
*/
			jPtr = inp->rstart[node];
			cDegree = inp->degree[node];
			for ( j = 1; j <= cDegree; j++ ) {
				jNode = inp->connect[jPtr++];
				if ( status[jNode] >= 0 ) {
					if ( status[jNode] == 0 )
						return BAD_ELEMENT;
					
					status[jNode] = -status[jNode];
					work[free++] = jNode;
				}
			}
/*
C
              JPTR = RSTART (NODE)
              CDGREE = DEGREE (NODE)
              DO 500 J = 1, CDGREE
                  JNODE = CONNEC (JPTR)
                  JPTR = JPTR + 1
                  IF (STATUS(JNODE) .LT. 0) GO TO 500
                  IF (STATUS(JNODE) .EQ. 0) GO TO 6000
                      STATUS (JNODE) = -STATUS (JNODE)
                      WORK (FREE) = JNODE
                      FREE = FREE + 1
  500         CONTINUE
*/
			if ( front >= free )
				break;
/*
C
              IF (FRONT .LT. FREE) GO TO 400
*/
		}
/*
C
C         ... END OF INNER REPEAT.  COMPUTE SIZE OF COMPONENT AND
C             SEE IF THERE ARE MORE NODES TO BE ASSIGNED
*/
		size[compns] = free - start[compns];
		if ( free > nReduce )
			break;
/*
C
          SIZE (COMPNS) = FREE - START (COMPNS)
          IF (FREE .LE. NREDUC)  GO TO 100
*/
	}
	if ( free != nReduce + 1 )
		return BAD_ELEMENT;
/*
C
      IF (FREE .NE. NREDUC+1)  GO TO 6200
      RETURN
C
C     ------------------------------------------------------------------
C
 5000 SPACE = NREDUC - FREE + 1
      ERROR = 130
      RETURN
C
*/
	return 0;

/*
 6000 ERROR = 33
      SPACE = -1
      RETURN
C
 6100 ERROR = 34
      SPACE = -1
      RETURN
C
 6200 ERROR = 35
      SPACE = -1
      RETURN
      END
*/
}

static int
transitionToIII( long n, long depth, oneBased lStruct, oneBased levelList, oneBased levelPtr,
		oneBased lTotal )
{
/*
      SUBROUTINE   transitionToIII   (N, ACTIVE, DEPTH, LSTRUC, LVLLST, LVLPTR,  GPSK1422
     1                       LTOTAL, ERROR, SPACE)
C
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
      DlInt32EGER     N, ACTIVE, DEPTH, ERROR, SPACE
C
CIBM  DlInt32EGER *2  LSTRUC(N), LVLLST(ACTIVE), LVLPTR(1), LTOTAL(DEPTH)
      DlInt32EGER     LSTRUC(N), LVLLST(ACTIVE), LVLPTR(1), LTOTAL(DEPTH)
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
	long i, account, start, level, pLevel;
/*
C
      DlInt32EGER     I, ACOUNT, START, LEVEL, PLEVEL
*/
	start = 1;
	for ( i = 1; i <= depth; i++ ) {
		levelPtr[i] = start;
		start += lTotal[i];
		lTotal[i] =  start;
	}
	levelPtr[depth + 1] = start;
/*
C
C     ... ESTABLISH STARTING AND ENDING PODlInt32ERS FOR EACH LEVEL
C
      START = 1
      DO 100 I = 1, DEPTH
          LVLPTR(I) = START
          START = START + LTOTAL(I)
          LTOTAL(I) = START
  100 CONTINUE
      LVLPTR(DEPTH+1) = START
*/
	account = 0;
	for ( i = 1; i <= n; i++ ) {
		if ( lStruct[i] < 0 ) {
			level = -lStruct[i];
			lStruct[i] = level;
			pLevel = levelPtr[level];
			levelList[pLevel] = i;
			levelPtr[level]++;
			account++;
			if ( levelPtr[level] > lTotal[level] )
				return BAD_ELEMENT;
		} else if ( lStruct[i] > 0 ) {
			return BAD_ELEMENT;
		}
	}
/*
C
      ACOUNT = 0
      DO 300 I = 1, N
          IF (LSTRUC(I)) 200, 300, 6000
  200         LEVEL = -LSTRUC(I)
              LSTRUC(I) = LEVEL
              PLEVEL = LVLPTR (LEVEL)
              LVLLST (PLEVEL) = I
              LVLPTR (LEVEL) = LVLPTR (LEVEL) + 1
              ACOUNT = ACOUNT + 1
              IF (LVLPTR (LEVEL) .GT. LTOTAL (LEVEL))  GO TO 6100
  300 CONTINUE
*/
	levelPtr[1] = 1;
	for ( i = 1; i <= depth; i++ ) {
		levelPtr[i+1] = lTotal[i];
	}
/*
C
C     ... RESET STARTING PODlInt32ERS
C
      LVLPTR(1) = 1
      DO 400 I = 1, DEPTH
          LVLPTR(I+1) = LTOTAL(I)
  400 CONTINUE
*/
	return 0;
/*
C
      RETURN
C
C     ------------------------------------------------------------------
C
 6000 ERROR = 40
      GO TO 6200
C
 6100 ERROR = 41
C
 6200 SPACE = -1
      RETURN
C
      END
*/
}

static int
cutHillMckeeRenum( GPSInput * inp, long nCompn, oneBased invNum, long sNode1, long sNode2,
		int reverse, long depth, oneBased levelList, oneBased levelPtr, oneBased levelNum,
		long & space )
{
/*
      SUBROUTINE   cutHillMckeeRenum   (N, DEGREE, RSTART, CONNEC,                 GPSK1510
     1                       NCOMPN, INVNUM, SNODE1, SNODE2, REVRS1,
     2                       DEPTH, LVLLST, LVLPTR, LVLNUM, ERROR,
     3                       SPACE)
C
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
C
      DlInt32EGER     N, RSTART(N), NCOMPN, SNODE1, SNODE2, DEPTH,
     1            ERROR, SPACE
C
CIBM  DlInt32EGER *2  DEGREE(N), CONNEC(1), INVNUM(NCOMPN),
      DlInt32EGER     DEGREE(N), CONNEC(1), INVNUM(NCOMPN),
     1            LVLLST(NCOMPN), LVLPTR(DEPTH), LVLNUM(N)
C
      LOGICAL     REVRS1
C
C
C     ==================================================================
C
C     NUMBERING REQUIRES TWO QUEUES, WHICH CAN BE BUILD IN PLACE
C     IN INVNUM.
C
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
C                 << THE NEWLY QUEUED NODES DlInt32O INCREASING ORDER OF >>
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
	long 	i, bq1, bq2, fq1, inc, cPtr, cNode, 
			iNode, level, nLeft, lStart, lWidth, queue1,
			queue2, cDegree, xLevel, stNode, iLevel, sq1, sq2,
			nSort, lowDegree, bPtr, lvllsc, lvllsb;
	int		forward, rLevel;
	
	int		err;
/*
      DlInt32EGER     I, BQ1, BQ2, FQ1, INC, CPTR, CNODE,
     1            INODE, LEVEL, NLEFT, LSTART, LWIDTH, QUEUE1,
     2            QUEUE2, CDGREE, XLEVEL, STNODE, ILEVEL, SQ1, SQ2,
     3            NSORT, LOWDG, BPTR, LVLLSC, LVLLSB, INVNMI
C
      LOGICAL     FORWRD, RLEVEL
C
C     ------------------------------------------------------------------
C
C     ... GIBBS-POOLE-STOCKMEYER HEURISTIC CHOICE OF ORDER
*/
	if ( inp->degree[sNode1] <= inp->degree[sNode2] ) {
		forward = reverse;
		stNode = sNode1;
	} else {
		forward = !reverse;
		stNode = sNode2;
	}
/*
C
      IF  (DEGREE(SNODE1) .GT. DEGREE(SNODE2))  GO TO 10
          FORWRD = REVRS1
          STNODE = SNODE1
          GO TO 20
C
   10     FORWRD = .NOT. REVRS1
          STNODE = SNODE2
C
C     ... SET UP INITIAL QUEUES AT FRONT OF 'INVNUM' FOR FORWRD ORDER,
C         AT BACK FOR REVERSED ORDER.
*/
	if ( !forward ) {
		inc = -1;
		queue1 = nCompn;
	} else {
		inc = 1;
		queue1 = 1;
	}
/*
C
   20 IF (FORWRD) GO TO 100
          INC = -1
          QUEUE1 = NCOMPN
          GO TO 200
C
  100     INC = +1
          QUEUE1 = 1
C
*/
	invNum[queue1] = stNode;
	rLevel = (levelNum[stNode] == depth);
	levelNum[stNode] = 0;
	fq1 = queue1;
	bq1 = queue1 + inc;
/*
  200 INVNUM (QUEUE1) = STNODE
      RLEVEL = (LVLNUM(STNODE) .EQ. DEPTH)
      LVLNUM (STNODE) = 0
      FQ1 = QUEUE1
      BQ1 = QUEUE1 + INC
C
C     -------------------------------
C     NUMBER NODES LEVEL BY LEVEL ...
C     -------------------------------
*/
	for ( xLevel = 1; xLevel <= depth; xLevel++ ) {
		level = xLevel;
		if ( rLevel )
			level = depth - xLevel + 1;
		lStart = levelPtr[level];
		lWidth = levelPtr[level + 1] - lStart;
		nLeft = lWidth;
		queue2 = queue1 + inc * lWidth;
		bq2 = queue2;
/*
C
      DO 3000 XLEVEL = 1, DEPTH
          LEVEL = XLEVEL
          IF  (RLEVEL)  LEVEL = DEPTH - XLEVEL + 1
C
          LSTART = LVLPTR (LEVEL)
          LWIDTH = LVLPTR (LEVEL+1) - LSTART
          NLEFT = LWIDTH
          QUEUE2 = QUEUE1 + INC*LWIDTH
          BQ2 = QUEUE2
C
C         ==============================================================
C         ... 'LOOP' CONSTRUCT BEGINS AT STATEMENT 1000
C                 THE INNER 'REPEAT' WILL BE DONE AS MANY TIMES AS
C                 IS NECESSARY TO NUMBER ALL THE NODES AT THIS LEVEL.
C         ==============================================================
*/
		while( 1 ) {
/*
C
 1000     CONTINUE
C
C             ==========================================================
C             ... REPEAT ... UNTIL QUEUE1 BECOMES EMPTY
C                 TAKE NODE FROM FRONT OF QUEUE1, FIND EACH OF ITS
C                 NEIGHBORS WHICH HAVE NOT YET BEEN NUMBERED, AND
C                 ADD THE NEIGHBORS TO QUEUE1 OR QUEUE2 ACCORDING TO
C                 THEIR LEVELS.
C             ==========================================================
*/
			while( 1 ) {
				cNode = invNum[fq1];
				fq1 += inc;
				sq1 = bq1;
				sq2 = bq2;
				nLeft--;
/*
C
 1100             CNODE = INVNUM (FQ1)
                  FQ1 = FQ1 + INC
                  SQ1 = BQ1
                  SQ2 = BQ2
                  NLEFT = NLEFT - 1
*/
				cPtr = inp->rstart[cNode];
				cDegree = inp->degree[cNode];
				for ( i = 1; i <= cDegree; i++ ) {
					iNode = inp->connect[cPtr];
					cPtr++;
					iLevel = levelNum[iNode];
					if ( iLevel != 0 ) {
						levelNum[iNode] = 0;
						if ( iLevel != level ) {
							if ( labs( level - iLevel ) != 1 )
								return BAD_ELEMENT;
							invNum[bq2] = iNode;
							bq2 += inc;
						} else {
							invNum[bq1] = iNode;
							bq1 += inc;
						}
					}
				}
/*
C
                  CPTR = RSTART (CNODE)
                  CDGREE = DEGREE (CNODE)
                  DO 1300 I = 1, CDGREE
                      INODE = CONNEC (CPTR)
                      CPTR = CPTR + 1
                      ILEVEL = LVLNUM (INODE)
                      IF (ILEVEL .EQ. 0)  GO TO 1300
                          LVLNUM (INODE) = 0
                          IF ( ILEVEL .EQ. LEVEL ) GO TO 1200
C
                              IF  (IABS(LEVEL-ILEVEL) .NE. 1) GO TO 6400
                                  INVNUM (BQ2) = INODE
                                  BQ2 = BQ2 + INC
                                  GO TO 1300
C
 1200                             INVNUM (BQ1) = INODE
                                  BQ1 = BQ1 + INC
 1300             CONTINUE
C
C                 ==================================================
C                 ... SORT THE NODES JUST ADDED TO QUEUE1 AND QUEUE2
C                     SEPARATELY DlInt32O INCREASING ORDER OF DEGREE.
C                 ==================================================
*/
				if ( labs( bq1 - sq1 ) > 1 ) {
					nSort = labs( bq1 - sq1 );
					if ( !forward ) {
						err = sortDegreeDecreasing( nSort, invNum.offset( bq1 + 1 ), inp->degree );
					} else {
						err = sortDegreeIncreasing( nSort, invNum.offset( sq1 ), inp->degree );
					}
					
					if ( err )
						return err;
/*
C
                  IF  (IABS (BQ1 - SQ1) .LE. 1)  GO TO 1500
                      NSORT = IABS (BQ1 - SQ1)
                      IF  (FORWRD)  GO TO 1400
                          CALL sortDegreeDecreasing (NSORT, INVNUM(BQ1+1), N, DEGREE,
     1                                 ERROR)
                          IF  (ERROR .NE. 0)  GO TO 6600
                          GO TO 1500
C
 1400                     CALL sortDegreeIncreasing (NSORT, INVNUM(SQ1), N, DEGREE,
     1                                 ERROR)
                          IF  (ERROR .NE. 0)  GO TO 6600
*/
				}
				
				if ( labs( bq2 - sq2 ) > 1 ) {
					nSort = labs( bq2 - sq2 );
					if ( !forward ) {
						err = sortDegreeDecreasing( nSort, invNum.offset( bq2 + 1 ), inp->degree );
					} else {
						err = sortDegreeIncreasing( nSort, invNum.offset( sq2 ), inp->degree );
					}
					
					if ( err )
						return err;
				}
/*
C
 1500             IF  (IABS (BQ2 - SQ2) .LE. 1)  GO TO 1700
                      NSORT = IABS (BQ2 - SQ2)
                      IF  (FORWRD)  GO TO 1600
                          CALL sortDegreeDecreasing (NSORT, INVNUM(BQ2+1), N, DEGREE,
     1                                 ERROR)
                          IF  (ERROR .NE. 0)  GO TO 6600
                          GO TO 1700
C
 1600                     CALL sortDegreeIncreasing (NSORT, INVNUM(SQ2), N, DEGREE,
     1                                 ERROR)
                          IF  (ERROR .NE. 0)  GO TO 6600
C
C                     ... END OF REPEAT LOOP
*/
				if ( fq1 == bq1 )
					break;
/*
C
 1700             IF  (FQ1 .NE. BQ1)  GO TO 1100
C
*/
			}
/*
C         ==============================================================
C         ... QUEUE1 IS NOW EMPTY ...
C             IF THERE ARE ANY UNNUMBERED NODES LEFT AT THIS LEVEL,
C             FIND THE ONE OF MINIMAL DEGREE AND RETURN TO THE
C             REPEAT LOOP ABOVE.
C         ==============================================================
*/
			if ( bq1 == queue2 && nLeft == 0 )
				break;
/*
C
 2000     IF  ((BQ1 .EQ. QUEUE2) .AND. (NLEFT .EQ. 0))  GO TO 2900
*/
			if ( nLeft <= 0 || nLeft != inc * (queue2 - bq1) )
				return BAD_ELEMENT;
/*
C
              IF ((NLEFT .LE. 0) .OR. (NLEFT .NE. INC * (QUEUE2 - BQ1)))
     1             GO TO 6200
*/
			lowDegree = inp->n + 1;
			bPtr = inp->n + 1;
			cPtr = lStart - 1;
/*
C
              LOWDG = N + 1
              BPTR  = N + 1
              CPTR  = LSTART - 1
*/
			for ( i = 1; i <= nLeft; i++ ) {
				while( 1 ) {
					cPtr++;
					lvllsc = levelList[cPtr];
					if ( levelNum[lvllsc] == level )
						break;
					if ( levelNum[lvllsc] != 0 )
						return BAD_ELEMENT;
				}
/* 
              DO 2800 I = 1, NLEFT
 2600             CPTR   = CPTR + 1
                  LVLLSC = LVLLST (CPTR)
                  IF (LVLNUM (LVLLSC) .EQ. LEVEL)  GO TO 2700
                      IF (LVLNUM (LVLLSC) .NE. 0)  GO TO 6300
                      GO TO 2600
*/
				if ( inp->degree[lvllsc] < lowDegree ) {
					lowDegree = inp->degree[lvllsc];
					bPtr = cPtr;
				}
/*
C
 2700             IF  (DEGREE(LVLLSC) .GE. LOWDG)  GO TO 2800
                      LOWDG = DEGREE (LVLLSC)
                      BPTR  = CPTR
*/
			}
/*
C
 2800         CONTINUE
C
C             ... MINIMAL DEGREE UNNUMBERED NODE FOUND ...
*/
			if ( bPtr > inp->n )
				return BAD_ELEMENT;
			
			lvllsb = levelList[bPtr];
			invNum[bq1] = lvllsb;
			levelNum[lvllsb] = 0;
			bq1 += inc;
/*
C
              IF  (BPTR .GT. N)  GO TO 6500
              LVLLSB = LVLLST (BPTR)
              INVNUM (BQ1) = LVLLSB
              LVLNUM (LVLLSB) = 0
              BQ1 = BQ1 + INC
              GO TO 1000
*/
		}
/*
C
C             =============================================
C             ... ADVANCE QUEUE PODlInt32ERS TO MAKE QUEUE2 THE
C                 NEW QUEUE1 FOR THE NEXT ITERATION.
C             =============================================
*/
		queue1 = queue2;
		fq1 = queue1;
		bq1 = bq2;
		if ( bq1 == fq1 && xLevel < depth )
			return BAD_ELEMENT;
/*
C
 2900     QUEUE1 = QUEUE2
          FQ1 = QUEUE1
          BQ1 = BQ2
          IF  ((BQ1 .EQ. FQ1) .AND. (XLEVEL .LT. DEPTH))  GO TO 6100
C
 3000 CONTINUE
*/
	}
/*
C
C     ... CHANGE SIGN OF DEGREE TO MARK THESE NODES AS 'NUMBERED'
*/
	for ( i = 1; i <= nCompn; i++ ) {
		inp->degree[invNum[i]] = -inp->degree[invNum[i]];
	}
/*
C
      DO 3100 I = 1, NCOMPN
          INVNMI = INVNUM(I)
          DEGREE (INVNMI) = -DEGREE (INVNMI)
 3100 CONTINUE
*/
	return 0;
/*
C
      RETURN
C
C     ------------------------------------------------------------------
C
 6000 SPACE = -1
      RETURN
C
 6100 ERROR = 51
      GO TO 6000
C
 6200 ERROR = 52
      GO TO 6000
C
 6300 ERROR = 53
      GO TO 6000
C
 6400 ERROR = 54
      GO TO 6000
C
 6500 ERROR = 55
      GO TO 6000
C
 6600 ERROR = 56
      GO TO 6000
C
      END
*/
}

static int
kingRenum( GPSInput * inp, long workLen, long & nextNum, oneBased work,
		long nCompn, long depth, oneBased levelList, oneBased levelPtr, oneBased levelNum,
		long & space )
{
/*
      SUBROUTINE  kingRenum  (N, DEGREE, RSTART, CONNEC, WRKLEN, NXTNUM,   GPSK1811
     1                     WORK, NCOMPN, DEPTH, LVLLST, LVLPTR, LVLNUM,
     2                     ERROR, SPACE)
C
      DlInt32EGER     N, RSTART(N), WRKLEN, NXTNUM, NCOMPN, DEPTH, ERROR,
     1            SPACE
C
CIBM  DlInt32EGER *2  DEGREE(N), CONNEC(1), WORK(WRKLEN), LVLLST(N),
      DlInt32EGER     DEGREE(N), CONNEC(1), WORK(WRKLEN), LVLLST(N),
     1            LVLPTR(DEPTH), LVLNUM(N)
C
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
C         THE CURRENT LEVEL.  ACTIVE IS A SET OF INDICES DlInt32O
C         ALEVEL.  AS THE NODES IN ALEVEL ARE NUMBERED, THEY
C         ARE PLACED DlInt32O 'DONE'.
C         QUEUED IS A QUEUE OF NODES IN THE 'NEXT' LEVEL, WHICH
C         GROWS FROM THE START OF THE 'NEXT' LEVEL IN LVLLST
C         FORWARDS TOWARD 'ALEVEL'.  QUEUED IS OF LENGTH NO MORE
C         THAN THE WIDTH OF THE NEXT LEVEL.
C         LVLLST IS THE LIST OF UNNUMBERED NODES IN THE TREE,
C         ARRANGED BY LEVEL.
C
C     ==================================================================
*/
	long	i, j, k, ptr, jPtr, kPtr, lPtr, mPtr, pPtr, rPtr;
	long	mpPtr, jNode, kNode, cNode, level, lowDegree, unused;
	long	maxQueue, nNext, aStart, minDegree, lStart, lWidth, active;
	long	queueB, queueD, qCount, nConnect, nActive, cDegree; 
	long	lDegree, nFinal, jDegree, startIc, added, tWorkLen;
	long	lvllsl;
//	long	connectJ;
//	long	connectR;
//	long	astPtr, actPtr;
//	long	activeI;
//	long	aStartI;
//	long	queueI;
//	long	acpPtr;
	
	int	err;
/*
      DlInt32EGER     I, J, K, PTR, JPTR, KPTR, LPTR, MPTR, PPTR, RPTR,
     1            MPPTR, JNODE, KNODE, CNODE, LEVEL, LOWDG, UNUSED,
     2            MXQUE, NNEXT, ASTART, MINDG, LSTART, LWIDTH, ACTIVE,
     2            QUEUEB, QUEUED, QCOUNT, NCONNC, NACTIV, CDGREE,
     3            LDGREE, NFINAL, JDGREE, STRTIC, ADDED, TWRKLN,
     4            LVLLSL, CONNEJ, CONNER, ASTPTR, ACTPTR, ACTIVI,
     5            ASTRTI, QUEUEI, ACPPTR
C
C     ------------------------------------------------------------------
*/
	tWorkLen = workLen + nCompn + inp->n + depth + 1;
	unused = tWorkLen;
/*
C
      TWRKLN = WRKLEN + NCOMPN + N + DEPTH + 1
      UNUSED = TWRKLN
*/
	aStart = levelPtr[1];
	lWidth = levelPtr[2] - aStart;
	aStart = workLen + 1;
	active = nextNum + lWidth + 1;
	nActive = lWidth;
	nFinal = nextNum + nCompn;
/*
C
      ASTART = LVLPTR(1)
      LWIDTH = LVLPTR(2) - ASTART
      ASTART = WRKLEN  + 1
      ACTIVE = NXTNUM + LWIDTH + 1
      NACTIV = LWIDTH
      NFINAL = NXTNUM + NCOMPN
*/
	nNext = levelPtr[3] - levelPtr[2];
	queueD = workLen;
	queueB = queueD;
	maxQueue = active + lWidth;
/*
C
      NNEXT = LVLPTR(3) - LVLPTR(2)
      QUEUED = WRKLEN
      QUEUEB = QUEUED
      MXQUE = ACTIVE + LWIDTH
C
C     ... BUILD FIRST PRIORITY QUEUE 'ACTIVE'
*/
	lowDegree = -(inp->n + 1);
	lPtr = levelPtr[1];
	for ( i = 1; i <= lWidth; i++ ) {
		nConnect = 0;
		lvllsl = levelList[lPtr];
		jPtr = inp->rstart[lvllsl];
		lDegree = inp->degree[lvllsl];
/*
C
      LOWDG = - (N + 1)
      LPTR = LVLPTR(1)
      DO 200 I = 1, LWIDTH
          NCONNC = 0
          LVLLSL= LVLLST (LPTR)
          JPTR = RSTART (LVLLSL)
          LDGREE = DEGREE(LVLLSL)
*/
		for ( j = 1; j <= lDegree; j++ ) {
//			connectJ = inp->connect[jPtr];
			if ( levelNum[inp->connect[jPtr]] == 2 )
				nConnect--;
			jPtr++;
/*
          DO 100 J = 1, LDGREE
              CONNEJ = CONNEC (JPTR)
              IF ( LVLNUM (CONNEJ) .EQ. 2 )  NCONNC = NCONNC - 1
              JPTR = JPTR + 1
*/
		}
//		activeI = active + i - 1;
		work[active + i - 1] = i;
		levelNum[lvllsl] = nConnect;
		lowDegree = max0( lowDegree, nConnect );
		lPtr++;
/*
  100     CONTINUE
C
          ACTIVI = ACTIVE + I - 1
          WORK (ACTIVI) = I
          LVLNUM (LVLLSL) = NCONNC
          LOWDG = MAX0 (LOWDG, NCONNC)
          LPTR = LPTR + 1
*/
	}
	work[active-1] = 0;
/*
  200 CONTINUE
      WORK (ACTIVE-1) = 0
C
C     -----------------------------------
C     NOW NUMBER NODES LEVEL BY LEVEL ...
C     -----------------------------------
*/
	for ( level = 1; level <= depth; level++ ) {
/*
C
      DO 2000 LEVEL = 1, DEPTH
C
C         ... NUMBER ALL NODES IN THIS LEVEL
*/
		for ( i = 1; i <= lWidth; i++ ) {
			pPtr = -1;
			ptr = work[active-1];
			if ( nNext != 0 ) {
/*
C
          DO 1100 I = 1, LWIDTH
              PPTR = -1
              PTR = WORK (ACTIVE-1)
              IF (NNEXT .EQ. 0)  GO TO 1000
C
C                 ... IF NODES REMAIN IN NEXT, FIND THE EARLIEST NODE
C                     IN ACTIVE OF MINIMAL DEGREE.
*/
				minDegree = -(inp->n + 1);
				for ( j = 1; j <= nActive; j++ ) {
//					astPtr = aStart + ptr;
					cNode = work[aStart + ptr];
					if ( levelNum[cNode] == lowDegree )
						break;
					if ( levelNum[cNode] > minDegree ) {
						mpPtr = pPtr;
						mPtr = ptr;
						minDegree = levelNum[cNode];
					}
					
					pPtr = ptr;
//					actPtr = active + ptr;
					ptr = work[active + ptr];
/*
C
                  MINDG = -(N+1)
                  DO 400 J = 1, NACTIV
                      ASTPTR = ASTART + PTR
                      CNODE = WORK (ASTPTR)
                      IF ( LVLNUM (CNODE) .EQ. LOWDG )  GO TO 500
                      IF ( LVLNUM (CNODE) .LE. MINDG )  GO TO 300
                          MPPTR = PPTR
                          MPTR = PTR
                          MINDG = LVLNUM (CNODE)
  300                 PPTR = PTR
                      ACTPTR = ACTIVE + PTR
                      PTR = WORK (ACTPTR)
*/
				}
/*
  400             CONTINUE
C
C                     ... ESTABLISH  PTR  AS FIRST MIN DEGREE NODE
C                         PPTR AS PREDECESSOR IN LIST.
*/
				if ( j > nActive ) {	//	no break
					ptr = mPtr;
					pPtr = mpPtr;
				}
/*
C
                  PTR = MPTR
                  PPTR = MPPTR
*/
//				astPtr = aStart + ptr;
				cNode = work[aStart + ptr];
				lowDegree = levelNum[cNode];
				levelNum[cNode] = 0;
				jPtr = inp->rstart[cNode];
/*
C
  500             ASTPTR = ASTART + PTR
                  CNODE = WORK (ASTPTR)
                  LOWDG = LVLNUM (CNODE)
                  LVLNUM (CNODE) = 0
                  JPTR = RSTART (CNODE)
C
C                 ... UPDATE CONNECTION COUNTS FOR ALL NODES WHICH
C                     CONNECT TO  CNODE'S  NEIGHBORS IN  NEXT.
*/
				cDegree = inp->degree[cNode];
				startIc = queueB;
/*
C
                  CDGREE = DEGREE(CNODE)
                  STRTIC = QUEUEB
*/
				for ( j = 1; j <= cDegree; j++ ) {
					jNode = inp->connect[jPtr];
					jPtr++;
/*
C
                  DO 700 J = 1, CDGREE
                      JNODE = CONNEC (JPTR)
                      JPTR = JPTR + 1
*/
					if ( levelNum[jNode] == level + 1 ) {
						if ( queueB < maxQueue )
							return NO_SPACE;
						work[queueB] = jNode;
						queueB--;
						nNext--;
						levelNum[jNode] = 0;
/*
                      IF (LVLNUM (JNODE) .NE. LEVEL+1 )  GO TO 700
                          IF (QUEUEB .LT. MXQUE)  GO TO 5000
                          WORK (QUEUEB) = JNODE
                          QUEUEB = QUEUEB - 1
                          NNEXT = NNEXT - 1
                          LVLNUM (JNODE) = 0
*/
						if ( nActive != 1 ) {
							kPtr = inp->rstart[jNode];
							jDegree = inp->degree[jNode];
/*
                          IF  (NACTIV .EQ. 1)  GO TO 700
                            KPTR = RSTART (JNODE)
                            JDGREE = DEGREE (JNODE)
*/
							for ( k = 1; k <= jDegree; k++ ) {
								kNode = inp->connect[kPtr];
								kPtr++;
/*
                            DO 600 K = 1, JDGREE
                                KNODE = CONNEC (KPTR)
                                KPTR = KPTR + 1
*/
								if ( levelNum[kNode] < 0 ) {
									levelNum[kNode]++;
									if ( lowDegree < levelNum[kNode] )
										lowDegree = levelNum[kNode];
/*
                                IF (LVLNUM (KNODE) .GE. 0)  GO TO 600
                                    LVLNUM (KNODE) = LVLNUM (KNODE) + 1
                                    IF  (LOWDG .LT. LVLNUM(KNODE))
     1                                   LOWDG = LVLNUM(KNODE)
*/
								}
							}
/*
  600                       CONTINUE
*/
						}
					}
				}
/*
  700             CONTINUE
C
C                 ... TO MIMIC THE ALGORITHM AS IMPLEMENTED BY GIBBS,
C                     SORT THE NODES JUST ADDED TO THE QUEUE DlInt32O
C                     INCREASING ORDER OF ORIGINAL INDEX. (BUT, BECAUSE
C                     THE QUEUE IS STORED BACKWARDS IN MEMORY, THE SORT
C                     ROUTINE IS CALLED FOR DECREASING INDEX.)
C
C                     TREAT  0, 1 OR 2  NODES ADDED AS SPECIAL CASES
*/
				added = startIc - queueB;
				if ( added == 2 ) {
/*
C
                  ADDED = STRTIC - QUEUEB
                  IF  (ADDED - 2)  1000, 800, 900
*/
					if ( work[startIc-1] < work[startIc] ) {
						jNode = work[startIc];
						work[startIc] = work[startIc-1];
						work[startIc-1] = jNode;
/*
C
  800                 IF (WORK(STRTIC-1) .GT. WORK(STRTIC))  GO TO 1000
                          JNODE = WORK(STRTIC)
                          WORK(STRTIC) = WORK(STRTIC-1)
                          WORK(STRTIC-1) = JNODE
                          GO TO 1000
*/
					}
				} else if ( added > 2 ) {
					err = sortListDecreasing( added, work.offset( queueB + 1 ) );
					if ( err )
						return err;
/*
C
  900                 CALL sortListDecreasing (ADDED, WORK(QUEUEB+1), ERROR)
                      IF  (ERROR .NE. 0)  GO TO 5500
C
*/
				}
			}
/*
C
C                 ... NUMBER THIS NODE AND DELETE IT FROM 'ACTIVE'.
C                     MARK IT UNAVAILABLE BY CHANGING SIGN OF DEGREE
C
*/
			nActive--;
//			astPtr = aStart + ptr;
			cNode = work[aStart + ptr];
			work[nextNum] = cNode;
			inp->degree[cNode] = -inp->degree[cNode];
			nextNum++;
/*
 1000         NACTIV = NACTIV - 1
              ASTPTR = ASTART + PTR
              CNODE = WORK (ASTPTR)
              WORK (NXTNUM) = CNODE
              DEGREE (CNODE) = -DEGREE (CNODE)
              NXTNUM = NXTNUM + 1
C
C             ... DELETE LINK TO THIS NODE FROM LIST
C
*/
//			acpPtr = active + pPtr;
//			actPtr = active + ptr;
			work[active + pPtr] = work[active + ptr];
/*
              ACPPTR = ACTIVE + PPTR
              ACTPTR = ACTIVE + PTR
              WORK (ACPPTR) = WORK (ACTPTR)
*/
		}
/*
 1100     CONTINUE
C
C         ... NOW MOVE THE QUEUE 'QUEUED' FORWARD, AT THE SAME
C             TIME COMPUTING CONNECTION COUNTS FOR ITS ELEMENTS.
C             THEN DO THE SAME FOR THE REMAINING NODES IN 'NEXT'.
C
*/
		if ( unused > queueB - maxQueue )
			unused = queueB - maxQueue;
		
		if ( nextNum != active - 1 )
			return NO_SPACE;
/*
          UNUSED = MIN0 (UNUSED, QUEUEB - MXQUE)
          IF ( NXTNUM .NE. ACTIVE-1 )  GO TO 5100
*/
		if ( level != depth ) {
			lStart = levelPtr[level+1];
			lWidth = levelPtr[level+2] - lStart;
			active = nextNum + lWidth + 1;
			aStart = active + lWidth;
			nActive = lWidth;
			maxQueue = aStart + lWidth;
			if ( maxQueue > queueB + 1 )
				return NO_SPACE;
			if ( unused > queueB - maxQueue + 1 )
				unused = queueB - maxQueue + 1;
/*
          IF ( LEVEL .EQ. DEPTH ) GO TO 2000
              LSTART = LVLPTR (LEVEL+1)
              LWIDTH = LVLPTR (LEVEL+2) - LSTART
              ACTIVE = NXTNUM + LWIDTH + 1
              ASTART = ACTIVE + LWIDTH
              NACTIV = LWIDTH
              MXQUE = ASTART + LWIDTH
              IF ( MXQUE .GT. QUEUEB + 1 )  GO TO 5000
              UNUSED = MIN0 (UNUSED, QUEUEB - MXQUE + 1)
C
*/
			qCount = queueD - queueB;
			lowDegree = -(inp->n + 1);
			work[active-1] = 0;
/*
              QCOUNT = QUEUED - QUEUEB
              LOWDG = -N-1
              WORK (ACTIVE-1) = 0
C
*/
			ptr = lStart;
			for( i = 1; i <= lWidth; i++ ) {
/*
              PTR = LSTART
              DO 1600 I = 1, LWIDTH
C
C                 ... CHOOSE NEXT NODE FROM EITHER 'QUEUED' OR 'NEXT'
C
*/
				if ( i <= qCount ) {
//					queueI = queueD + 1 - i;
					cNode = work[queueD + 1 - i];
/*
                  IF (I .GT. QCOUNT )  GO TO 1200
                      QUEUEI = QUEUED + 1 - I
                      CNODE = WORK (QUEUEI)
                      GO TO 1300
C
*/
				} else {
					while( 1 ) {
						cNode = levelList[ptr];
						ptr++;
						if ( ptr > levelPtr[level+2] )
							return NO_SPACE;
						if ( levelNum[cNode] > 0 )
							break;
/*
 1200                 CNODE = LVLLST (PTR)
                      PTR = PTR + 1
                      IF ( PTR .GT. LVLPTR(LEVEL+2) )  GO TO 5200
                          IF (LVLNUM (CNODE) .GT. 0)  GO TO 1300
                              GO TO 1200
C
*/
					}
				}
				
				if ( level+1 != depth ) {
/*
 1300             IF ( LEVEL+1 .EQ. DEPTH ) GO TO 1500
*/
					rPtr = inp->rstart[cNode];
					nConnect = 0;
					jDegree = inp->degree[cNode];
/*
C
                      RPTR = RSTART (CNODE)
                      NCONNC = 0
                      JDGREE = DEGREE (CNODE)
*/
					for ( j = 1; j <= jDegree; j++ ) {
//						connectR = inp->connect[rPtr];
						if ( levelNum[inp->connect[rPtr]] == level + 2 )
							nConnect--;
						rPtr++;
/*
                      DO 1400 J = 1, JDGREE
                          CONNER = CONNEC (RPTR)
                          IF ( LVLNUM (CONNER) .EQ. LEVEL+2 )
     1                        NCONNC = NCONNC - 1
                          RPTR = RPTR + 1
*/
					}
					levelNum[cNode] = nConnect;
					lowDegree = max0( lowDegree, nConnect );
/*
 1400                 CONTINUE
                      LVLNUM (CNODE) = NCONNC
                      LOWDG = MAX0 (LOWDG, NCONNC)
C
C             ... ADD CNODE TO NEW 'ACTIVE' QUEUE
C
*/
				}
//				activeI = active + i - 1;
//				aStartI = aStart + i - 1;
				work[active + i - 1] = i;
				work[aStart + i - 1] = cNode;
/*
 1500             ACTIVI = ACTIVE + (I - 1)
                  ASTRTI = ASTART + (I - 1)
                  WORK (ACTIVI) = I
                  WORK (ASTRTI) = CNODE
*/
			}
/*
 1600         CONTINUE
C
*/
			if ( depth != level + 1 ) {
				nNext = levelPtr[level+3] - levelPtr[level+2];
				queueD = lStart - 1 + lWidth + workLen;
				queueB = queueD;
/*
              IF (DEPTH .EQ. LEVEL+1 ) GO TO 1700
                  NNEXT = LVLPTR (LEVEL+3) - LVLPTR (LEVEL+2)
                  QUEUED = LSTART - 1 + LWIDTH + WRKLEN
                  QUEUEB = QUEUED
                  GO TO 2000
C
*/
			} else {
				nNext = 0;
/*
 1700             NNEXT = 0
C
*/
			}
		}
	}
/*
 2000 CONTINUE
C
*/
	if ( nextNum != nFinal )
		return BAD_ELEMENT;
	space = max0( space, tWorkLen - unused );
	return 0;
/*
      IF  (NXTNUM .NE. NFINAL)  GO TO 5300
      SPACE = MAX0 (SPACE, TWRKLN - UNUSED)
      RETURN
C
C
C     ------------------------------------------------------------------
C
 5000 SPACE = NACTIV + NNEXT
      ERROR = 160
      RETURN
C
 5100 ERROR = 61
      GO TO 5400
C
 5200 ERROR = 62
      GO TO 5400
C
 5300 ERROR = 63
C
 5400 RETURN
C
 5500 ERROR = 64
      GO TO 5400
C
      END
*/
}

static int
compBandProfile( GPSInput * inp, oneBased invNum, oneBased newNum, oneBased oldNum,
		long & bandWidth, long & profile, long & space )
{
/*
      SUBROUTINE   compBandProfile   (N, DEGREE, RSTART, CONNEC, INVNUM, NEWNUM, GPSK2154
     1                       OLDNUM, BANDWD, PROFIL, ERROR, SPACE)
C
C
      DlInt32EGER     N, RSTART(N), BANDWD, PROFIL, ERROR, SPACE
C
CIBM  DlInt32EGER *2  DEGREE(N), CONNEC(1), INVNUM(N), NEWNUM(N), OLDNUM(N)
      DlInt32EGER     DEGREE(N), CONNEC(1), INVNUM(N), NEWNUM(N), OLDNUM(N)
C
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
	long i, j, jPtr, iDegree, oldBand, oldProfile, newBand, newProfile,
			oldRwd, newRwd, oldOrigin, newOrigin, jNode;
/*
      DlInt32EGER     I, J, JPTR, IDGREE, OLDBND, OLDPRO, NEWBND, NEWPRO,
     1            OLDRWD, NEWRWD, OLDORG, NEWORG, JNODE, INVNMI
C
C     ------------------------------------------------------------------
C
C     ... CREATE NEWNUM AS A PERMUTATION VECTOR
C
*/
	for ( i = 1; i <= inp->n; i++ ) {
		newNum[invNum[i]] = i;
	}
/*
      DO 100 I = 1, N
          INVNMI = INVNUM (I)
          NEWNUM (INVNMI) = I
  100 CONTINUE
C
C     ... COMPUTE PROFILE AND BANDWIDTH FOR BOTH THE OLD AND THE NEW
C         ORDERINGS.
C
*/
	oldBand = 0;
	oldProfile = 0;
	newBand = 0;
	newProfile = 0;
/*
      OLDBND = 0
      OLDPRO = 0
      NEWBND = 0
      NEWPRO = 0
C
*/
	for ( i = 1; i <= inp->n; i++ ) {
		if ( inp->degree[i] != 0 ) {
			if ( inp->degree[i] > 0 )
				return BAD_ELEMENT;
			iDegree = -inp->degree[i];
			inp->degree[i] = iDegree;
			newOrigin = newNum[i];
			oldOrigin = oldNum[i];
			newRwd = 0;
			oldRwd = 0;
			jPtr = inp->rstart[i];
			
/*
      DO 300 I = 1, N
          IF (DEGREE(I) .EQ. 0)  GO TO 300
          IF (DEGREE(I) .GT. 0)  GO TO 6000
              IDGREE = -DEGREE(I)
              DEGREE(I) = IDGREE
              NEWORG = NEWNUM(I)
              OLDORG = OLDNUM(I)
              NEWRWD = 0
              OLDRWD = 0
              JPTR = RSTART (I)
C
C             ... FIND NEIGHBOR WHICH IS NUMBERED FARTHEST AHEAD OF THE
C                 CURRENT NODE.
C
*/
			for ( j = 1; j <= iDegree; j++ ) {
				jNode = inp->connect[jPtr];
				jPtr++;
				newRwd = max0( newRwd, newOrigin - newNum[jNode] );
				oldRwd = max0( oldRwd, oldOrigin - oldNum[jNode] );
			}
/*
              DO 200 J = 1, IDGREE
                  JNODE = CONNEC(JPTR)
                  JPTR = JPTR + 1
                  NEWRWD = MAX0 (NEWRWD, NEWORG - NEWNUM(JNODE))
                  OLDRWD = MAX0 (OLDRWD, OLDORG - OLDNUM(JNODE))
  200         CONTINUE
C
*/
			newProfile += newRwd;
			newBand = max0( newRwd, newBand );
			oldProfile += oldRwd;
			oldBand = max0( oldRwd, oldBand );
/*
              NEWPRO = NEWPRO + NEWRWD
              NEWBND = MAX0 (NEWBND, NEWRWD)
              OLDPRO = OLDPRO + OLDRWD
              OLDBND = MAX0 (OLDBND, OLDRWD)
*/
		}
	}
/*
  300 CONTINUE
C
C     ... IF NEW ORDERING HAS BETTER BANDWIDTH THAN OLD ORDERING,
C         REPLACE OLD ORDERING BY NEW ORDERING
C
*/
	if ( newBand <= oldBand ) {
		bandWidth = newBand;
		profile = newProfile;
		for ( i = 1; i <= inp->n; i++ )
			oldNum[i] = newNum[i];
/*
      IF  (NEWBND .GT. OLDBND)  GO TO 500
          BANDWD = NEWBND
          PROFIL = NEWPRO
          DO 400 I = 1, N
              OLDNUM(I) = NEWNUM(I)
  400     CONTINUE
          GO TO 600
C
C     ... RETAIN OLD ORDERING
C
*/
	} else {
		bandWidth = oldBand;
		profile = oldProfile;
	}
/*
  500     BANDWD = OLDBND
          PROFIL = OLDPRO
C
*/
	return 0;
/*
  600 RETURN
C
C     ------------------------------------------------------------------
C
 6000 SPACE = -1
      ERROR = 70
      RETURN
C
      END
*/
}

static int
compBandProfileWithRev( GPSInput * inp, oneBased invNum, oneBased newNum, oneBased oldNum,
		long & bandWidth, long & profile, long & space )
{
/*
      SUBROUTINE   compBandProfileWithRev   (N, DEGREE, RSTART, CONNEC, INVNUM, NEWNUM, GPSK2245
     1                       OLDNUM, BANDWD, PROFIL, ERROR, SPACE)
C
C
      DlInt32EGER     N, RSTART(N), BANDWD, PROFIL, ERROR, SPACE
C
CIBM  DlInt32EGER *2  DEGREE(N), CONNEC(N), INVNUM(N), NEWNUM(N), OLDNUM(N)
      DlInt32EGER     DEGREE(N), CONNEC(N), INVNUM(N), NEWNUM(N), OLDNUM(N)
C
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
	long i, j, jPtr, iDegree, oldBand, oldProfile, newBand, newProfile,
		oldRwd, newRwd, oldOrigin, newOrigin, jNode, nRvBand, nRvProfile,
		nRvOrigin, nRvRwd, nmip1;
/*
      DlInt32EGER     I, J, JPTR, IDGREE, OLDBND, OLDPRO, NEWBND, NEWPRO,
     1            OLDRWD, NEWRWD, OLDORG, NEWORG, JNODE, NRVBND, NRVPRO,
     2            NRVORG, NRVRWD, INVNMI, NMIP1
C
C     ------------------------------------------------------------------
C
C     ... CREATE NEWNUM AS A PERMUTATION VECTOR
*/
	for ( i = 1; i <= inp->n; i++ )
		newNum[invNum[i]] = i;
/*
C
      DO 100 I = 1, N
          INVNMI = INVNUM (I)
          NEWNUM (INVNMI) = I
  100 CONTINUE
C
C     ... COMPUTE PROFILE AND BANDWIDTH FOR BOTH THE OLD AND THE NEW
C         ORDERINGS.
*/
	oldBand = newBand = nRvBand = 0;
	oldProfile = newProfile = nRvProfile = 0;
/*
C
      OLDBND = 0
      OLDPRO = 0
      NEWBND = 0
      NEWPRO = 0
      NRVBND = 0
      NRVPRO = 0
*/
	for ( i = 1; i <= inp->n; i++ ) {
		if ( inp->degree[i] > 0 )
			return BAD_ELEMENT;
		if ( inp->degree[i] != 0 ) {
			iDegree = -inp->degree[i];
			inp->degree[i] = iDegree;
			newRwd = oldRwd = nRvRwd = 0;
			newOrigin = newNum[i];
			oldOrigin = oldNum[i];
			nRvOrigin = inp->n - newNum[i] + 1;
			jPtr = inp->rstart[i];
/*
C
      DO 300 I = 1, N
          IF (DEGREE(I) .EQ. 0)  GO TO 300
          IF (DEGREE(I) .GT. 0)  GO TO 6000
              IDGREE = -DEGREE(I)
              DEGREE(I) = IDGREE
              NEWRWD = 0
              OLDRWD = 0
              NRVRWD = 0
              NEWORG = NEWNUM(I)
              OLDORG = OLDNUM(I)
              NRVORG = N - NEWNUM(I) + 1
              JPTR = RSTART (I)
C
C             ... FIND NEIGHBOR WHICH IS NUMBERED FARTHEST AHEAD OF THE
C                 CURRENT NODE.
*/
			for ( j = 1; j <= iDegree; j++ ) {
				jNode = inp->connect[jPtr];
				jPtr++;
				newRwd = max0( newRwd, newOrigin - newNum[jNode] );
				oldRwd = max0( oldRwd, oldOrigin - oldNum[jNode] );
				nRvRwd = max0( nRvRwd, nRvOrigin - inp->n + newNum[jNode] - 1 );
			}
/*
C
              DO 200 J = 1, IDGREE
                  JNODE = CONNEC(JPTR)
                  JPTR = JPTR + 1
                  NEWRWD = MAX0 (NEWRWD, NEWORG - NEWNUM(JNODE))
                  OLDRWD = MAX0 (OLDRWD, OLDORG - OLDNUM(JNODE))
                  NRVRWD = MAX0 (NRVRWD, NRVORG - N + NEWNUM(JNODE) - 1)
  200         CONTINUE
C
*/
			newProfile += newRwd;
			newBand = max0( newBand, newRwd );
			nRvProfile += nRvRwd;
			nRvBand = max0( nRvBand, nRvRwd );
			oldProfile += oldRwd;
			oldBand = max0( oldRwd, oldBand );
		}
	}
/*
              NEWPRO = NEWPRO + NEWRWD
              NEWBND = MAX0 (NEWBND, NEWRWD)
              NRVPRO = NRVPRO + NRVRWD
              NRVBND = MAX0 (NRVBND, NRVRWD)
              OLDPRO = OLDPRO + OLDRWD
              OLDBND = MAX0 (OLDBND, OLDRWD)
  300 CONTINUE
C
C     ... IF NEW ORDERING HAS BETTER BANDWIDTH THAN OLD ORDERING,
C         REPLACE OLD ORDERING BY NEW ORDERING
C
*/
	if ( newProfile < oldProfile && newProfile < nRvProfile ) {
		bandWidth = newBand;
		profile = newProfile;
		for ( i = 1; i <= inp->n; i++ ) {
			oldNum[i] = newNum[i];
		}
/*
      IF  ((NEWPRO .GT. OLDPRO)  .OR. (NEWPRO .GT. NRVPRO)) GO TO 500
          BANDWD = NEWBND
          PROFIL = NEWPRO
          DO 400 I = 1, N
              OLDNUM(I) = NEWNUM(I)
  400     CONTINUE
          GO TO 800
C
C     ... CHECK NEW REVERSED ORDERING FOR BEST PROFILE
C
*/
	} else if ( nRvProfile < oldProfile ) {
		bandWidth = nRvBand;
		profile = nRvProfile;
		for ( i = 1; i <= inp->n; i++ ) {
			oldNum[i] = inp->n - newNum[i] + 1;
			if ( i <= inp->n/2 ) {
				j = invNum[i];
				nmip1 = (inp->n + 1) - i;
				invNum[i] = invNum[nmip1];
				invNum[nmip1] = j;
			}
		}
/*
  500 IF  (NRVPRO .GT. OLDPRO)  GO TO 700
          BANDWD = NRVBND
          PROFIL = NRVPRO
          DO 600 I = 1, N
              OLDNUM(I) = N - NEWNUM(I) + 1
              IF  (I .GT. N/2)  GO TO 600
                  J = INVNUM(I)
                  NMIP1 = (N + 1) - I
                  INVNUM(I) = INVNUM (NMIP1)
                  INVNUM (NMIP1) = J
  600     CONTINUE
          GO TO 800
C
C
C     ... RETAIN OLD ORDERING
C
*/
	} else {
		bandWidth = oldBand;
		profile = oldProfile;
/*
  700     BANDWD = OLDBND
          PROFIL = OLDPRO
C
*/
	}
	return 0;
/*
  800 RETURN
C
C     ------------------------------------------------------------------
C
 6000 ERROR = 71
      SPACE = -1
      RETURN
C
      END
*/
}

static int
sortIndexListDecreasing( long n, oneBased key, oneBased data )
{
/*
      SUBROUTINE   sortIndexListDecreasing   (N, KEY, DATA, ERROR)                       GPSK2361
C
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
C
      DlInt32EGER     N, ERROR
C
CIBM  DlInt32EGER *2  KEY(N), DATA(N)
      DlInt32EGER     KEY(N), DATA(N)
C
C     ------------------------------------------------------------------
C
*/
	long i, j, d, k, ip1, jm1;
/*
      DlInt32EGER     I, J, D, K, IP1, JM1
C
C     ------------------------------------------------------------------
C
*/
	if ( n == 1 )
		return 0;
	if ( n <= 0 ) 
		return BAD_ELEMENT;
/*
      IF (N .EQ. 1)  RETURN
      IF  (N .LE. 0)  GO TO 6000
C
      ERROR = 0
C
C     ... INSERTION SORT ... FOR I := N-1 STEP -1 TO 1 DO ...
C
*/
	i = n - 1;
	ip1 = n;
/*
 2400 I = N - 1
      IP1 = N
C
*/
	while ( 1 ) {
		if ( key[i] < key[ip1] ) {
/*
 2500     IF ( KEY (I) .GE. KEY (IP1) )  GO TO 2800
C
C             ... OUT OF ORDER ... MOVE UP TO CORRECT PLACE
C
*/
			k = key[i];
			d = data[i];
			j = ip1;
			jm1 = i;
/*
              K = KEY (I)
              D = DATA (I)
              J = IP1
              JM1 = I
C
C             ... REPEAT ... UNTIL 'CORRECT PLACE FOR K FOUND'
C
*/
			do {
				key[jm1] = key[j];
				data[jm1] = data[j];
				jm1 = j;
				j++;
				if ( j > n )
					break;
			} while( key[j] > k );
/*
 2600             KEY (JM1) = KEY (J)
                  DATA (JM1) = DATA (J)
                  JM1 = J
                  J = J + 1
                  IF  (J .GT. N)  GO TO 2700
                  IF (KEY (J) .GT. K)  GO TO 2600
C
*/
			key[jm1] = k;
			data[jm1] = d;
/*
 2700         KEY (JM1) = K
              DATA (JM1) = D
C
*/
		}
		
		ip1 = i;
		i--;
/*
 2800     IP1 = I
          I = I - 1
*/
		if ( i <= 0 )
			break;
	}
/*
          IF ( I .GT. 0 )  GO TO 2500
C
*/
	return 0;
/*
 3000 RETURN
C
 6000 ERROR = 1
      GO TO 3000
C
      END
*/
}

static int
sortListDecreasing( long n, oneBased key )
{
/*
      SUBROUTINE   sortListDecreasing   (N, KEY, ERROR)                             GPSK2436
C
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
C
      DlInt32EGER     N, ERROR
C
CIBM  DlInt32EGER *2  KEY(N)
      DlInt32EGER     KEY(N)
C
C     ------------------------------------------------------------------
C
*/
	long i, j, k, ip1, jm1;
/*
      DlInt32EGER     I, J, K, IP1, JM1
C
C     ------------------------------------------------------------------
C
*/
	if ( n == 1 )
		return 0;
	if ( n <= 0 ) 
		return BAD_ELEMENT;
/*
      IF (N .EQ. 1)  RETURN
      IF  (N .LE. 0)  GO TO 6000
C
      ERROR = 0
C
C     ... INSERTION SORT ... FOR I := N-1 STEP -1 TO 1 DO ...
C
*/
	i = n - 1;
	ip1 = n;
/*
 2400 I = N - 1
      IP1 = N
C
*/
	do {
		if ( key[i] < key[ip1] ) {
/*
 2500     IF ( KEY (I) .GE. KEY (IP1) )  GO TO 2800
C
C             ... OUT OF ORDER ... MOVE UP TO CORRECT PLACE
C
*/
			k = key[i];
			j = ip1;
			jm1 = i;
/*
              K = KEY (I)
              J = IP1
              JM1 = I
C
C             ... REPEAT ... UNTIL 'CORRECT PLACE FOR K FOUND'
C
*/
			do {
				key[jm1] = key[j];
				jm1 = j++;
				if ( j > n )
					break;
			} while( key[j] > k );
/*
 2600             KEY (JM1) = KEY (J)
                  JM1 = J
                  J = J + 1
                  IF  (J .GT. N)  GO TO 2700
                  IF (KEY (J) .GT. K)  GO TO 2600
C
*/
			key[jm1] = k;
/*
 2700         KEY (JM1) = K
C
*/
		}
		ip1 = i--;
	} while( i > 0 );
	
	return 0;
/*
 2800     IP1 = I
          I = I - 1
          IF ( I .GT. 0 )  GO TO 2500
C
 3000 RETURN
C
 6000 ERROR = 1
      GO TO 3000
C
      END
*/
}

static int
sortDegreeDecreasing( long n, oneBased index, oneBased degree )
{
/*
      SUBROUTINE  sortDegreeDecreasing  (N, INDEX, NVEC, DEGREE, ERROR)               GPSK2502
C
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
C
      DlInt32EGER     N, NVEC, ERROR
C
CIBM  DlInt32EGER *2  INDEX(N), DEGREE(NVEC)
      DlInt32EGER     INDEX(N), DEGREE(NVEC)
C
C     ------------------------------------------------------------------
C
*/
	long i, j, v, ip1, jm1, indexI, indexI1, indexJ;
/*
      DlInt32EGER     I, J, V, IP1, JM1, INDEXI, INDXI1, INDEXJ
C
C     ------------------------------------------------------------------
C
*/
	if ( n == 1 )
		return 0;
	if ( n <= 0 )
		return BAD_ELEMENT;
/*
      IF (N .EQ. 1)  RETURN
      IF  (N .LE. 0)  GO TO 6000
C
      ERROR = 0
C
C     ------------------------------------------------------------------
C     INSERTION SORT THE ENTIRE FILE
C     ------------------------------------------------------------------
C
C
C     ... INSERTION SORT ... FOR I := N-1 STEP -1 TO 1 DO ...
C
*/
	i = n - 1;
	ip1 = n;
/*
 2400 I = N - 1
      IP1 = N
C
*/
	do {
		indexI = index[i];
		indexI1 = index[ip1];
		if ( degree[indexI] < degree[indexI1] ) {
/*
 2500     INDEXI = INDEX (I)
          INDXI1 = INDEX (IP1)
          IF ( DEGREE(INDEXI) .GE. DEGREE(INDXI1) )  GO TO 2800
C
C             ... OUT OF ORDER ... MOVE UP TO CORRECT PLACE
C
*/
			v = degree[indexI];
			j = ip1;
			jm1 = i;
			indexJ = index[j];
/*
              V = DEGREE (INDEXI)
              J = IP1
              JM1 = I
              INDEXJ = INDEX (J)
C
C             ... REPEAT ... UNTIL 'CORRECT PLACE FOR V FOUND'
C
*/
			do {
				index[jm1] = indexJ;
				jm1 = j++;
				if ( j > n )
					break;
				indexJ = index[j];
			} while ( degree[indexJ] > v );
/*
 2600             INDEX (JM1) = INDEXJ
                  JM1 = J
                  J = J + 1
                  IF (J .GT. N)  GO TO 2700
                  INDEXJ = INDEX (J)
                  IF (DEGREE(INDEXJ) .GT. V)  GO TO 2600
C
*/
			index[jm1] = indexI;
/*
 2700         INDEX (JM1) = INDEXI
C
*/
		}
		
		ip1 = i--;
/*
 2800     IP1 = I
          I = I - 1
*/
	} while( i > 0 );
/*
          IF ( I .GT. 0 )  GO TO 2500
C
*/
	return 0;
/*
 3000 RETURN
C
 6000 ERROR = 1
      GO TO 3000
C
      END
*/
}

static int
sortDegreeIncreasing( long n, oneBased index, oneBased degree )
{
/*
      SUBROUTINE  GPSKCQ (N, INDEX, NVEC, DEGREE, ERROR)                GPSK2581
C
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
C
      DlInt32EGER     N, NVEC, ERROR
C
CIBM  DlInt32EGER *2  INDEX(N), DEGREE(NVEC)
      DlInt32EGER     INDEX(N), DEGREE(NVEC)
C
C     ------------------------------------------------------------------
C
*/
	long i, j, v, indexI, indexI1, indexJ, ip1, jm1;
/*
      DlInt32EGER     I, J, V, INDEXI, INDXI1, INDEXJ, IP1, JM1
C
C     ------------------------------------------------------------------
C
*/
	if ( n == 1 )
		return 0;
	if ( n <= 0 )
		return BAD_ELEMENT;
/*
      IF (N .EQ. 1)  RETURN
      IF  (N .LE. 0)  GO TO 6000
C
      ERROR = 0
C
C     ------------------------------------------------------------------
C     INSERTION SORT THE ENTIRE FILE
C     ------------------------------------------------------------------
C
C
C     ... INSERTION SORT ... FOR I := N-1 STEP -1 TO 1 DO ...
C
*/
	i = n - 1;
	ip1 = n;
/*
 2400 I = N - 1
      IP1 = N
C
*/
	do {
		indexI = index[i];
		indexI1 = index[ip1];
		if ( degree[indexI] > degree[indexI1] ) {
/*
 2500     INDEXI = INDEX (I)
          INDXI1 = INDEX (IP1)
          IF ( DEGREE(INDEXI) .LE. DEGREE(INDXI1) )  GO TO 2800
C
C             ... OUT OF ORDER ... MOVE UP TO CORRECT PLACE
C
*/
			v = degree[indexI];
			j = ip1;
			jm1 = i;
			indexJ = index[j];
/*
              V = DEGREE (INDEXI)
              J = IP1
              JM1 = I
              INDEXJ = INDEX (J)
C
C             ... REPEAT ... UNTIL 'CORRECT PLACE FOR V FOUND'
C
*/
			do {
				index[jm1] = indexJ;
				jm1 = j++;
				if ( j > n )
					break;
				indexJ = index[j];
			} while( degree[indexJ] < v );
/*
 2600             INDEX (JM1) = INDEXJ
                  JM1 = J
                  J = J + 1
                  IF (J .GT. N)  GO TO 2700
                  INDEXJ = INDEX (J)
                  IF (DEGREE(INDEXJ) .LT. V)  GO TO 2600
C
*/
			index[jm1] = indexI;
/*
 2700         INDEX (JM1) = INDEXI
C
*/
		}
		ip1 = i--;
/*
 2800     IP1 = I
          I = I - 1
*/
	} while ( i > 0 );
/*
          IF ( I .GT. 0 )  GO TO 2500
C
*/
	return 0;
/*
 3000 RETURN
C
 6000 ERROR = 1
      GO TO 3000
C
      END
*/
}

static long	max0( long a, long b )
{
	return a < b ? b : a;
}

#ifdef _DEBUG
//------------------------------------------------------------------------------
//	::printTree
//
//		print the tree. For debugging
//
//	throws			:	PP_PowerPlant::LException
//
//	which			->	defines the file to be copied
//------------------------------------------------------------------------------
static void printTree( char * msg, long avail, const oneBased & tree, long depth, long active, long width )
{
	long	i;
	long	len = tree[avail - depth] - 1;
	printf( "%s\n", msg );
	printf( "depth = %ld, width = %ld, active = %ld\n", depth, width, active );
	printf( "tree:" );
	for ( i = 1; i <= len; i++ ) {
		printf( "%d ", tree[i] );
	}
	
	printf( "\nqueue\n" );
	for ( i = 1; i <= depth + 1; i++ ) {
		printf( "%d ", tree[avail - i + 1] );
	}

	printf( "\n" );
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
static void printVector( char * msg, long n, const oneBased & v )
{
	long	i;
	printf( "%s: ", msg );
	for ( i = 1; i <= n; i++ ) {
		printf( "%d ", v[i] );
	}
	printf( "\n" );
}

#endif

//	eof
