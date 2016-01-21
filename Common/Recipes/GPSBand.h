/*+
 *	File:		GPSBand.h
 *
 *	Contains:	Bandwidth minimizer using GPS Algorithm
 *
 *	Written by:	David C. Salmon
 *
 *	Copyright:	COPYRIGHT (C) 1990-96 By David C. Salmon.  *WORLDWIDE RIGHTS RESERVED*
 *
 *	Based on ALGORITHM 582, COLLECTED ALGORITHMS FROM ACM.
 *     ALGORITHM APPEARED IN ACM-TRANS. MATH. SOFTWARE, VOL.8, NO. 2,
 *     JUN., 1982, P. 190.
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

#ifndef	_H_GPSBand
#define	_H_GPSBand

//#include "DlArray.h"
//#define _DEBUG

/* ---------------------------- Declares ----------------------------------- */

// ----------------------------------------------------------------------------
//	class oneBased
//
//		class to convert 0 based arrays to one based. This is not very
//		efficient.
//
// ----------------------------------------------------------------------------
class oneBased {

public:

	oneBased() : itsPtr(0) {}

	oneBased( long * ptr ) : itsPtr( ptr - 1 ) {}

	oneBased offset( long ofs ) {
		return oneBased( itsPtr + ofs );
	}
	
	void set( const oneBased & p, long ofs ) {
		itsPtr = p.itsPtr + ofs;
	}
	
	void set( long * ptr, long ofs = 0 ) 
	{
		itsPtr = ptr + ofs - 1;
	}
	
	bool empty() const {
		return itsPtr == 0;
	}
	
	long operator[] ( long i ) const { return itsPtr[i]; }
	
	long & operator[] ( long i ) { return itsPtr[i]; }
	
//	long * get() const {
//		return itsPtr;
//	}

private:

	long * itsPtr;
};

// ----------------------------------------------------------------------------
//	class oneBasedDouble
//
//		class to convert 0 based arrays to one based. This is not very
//		efficient.
//
// ----------------------------------------------------------------------------
class oneBasedDouble {

public:

	oneBasedDouble() : itsPtr(0) {}

	oneBasedDouble( double * ptr ) : itsPtr( ptr - 1 ) {}

	oneBasedDouble offset( long ofs ) {
		return oneBasedDouble( itsPtr + ofs + 1 );
	}
	
	void set( const oneBasedDouble & p, long ofs ) {
		itsPtr = p.itsPtr + ofs;
	}
	
	void set( double * ptr, long ofs = 0 ) 
	{
		itsPtr = ptr + ofs - 1;
	}
	
	bool empty() const {
		return itsPtr == 0;
	}
	
	double operator[] ( long i ) const { return itsPtr[i]; }
	
	double & operator[] ( long i ) { return itsPtr[i]; }

private:

	double * itsPtr;

};

typedef	struct	GPSInput {
	long		n;
	oneBased	degree;		//	number of non-zero terms in column i (rstart[i+1] - rstart[i])
	oneBased	rstart;		//	offset to column i from start
	oneBased	connect;	//	nonzero equation numbers.
	int			optPro;		//	true for bandwidth, false for profile
}				GPSInput;

typedef struct	GPSOutput {
	long		band;		//	bandwidth
	long		profile;	//	profile
	
	long		wrkLen;		//	size of work
	oneBased	work;		//	working array
	oneBased	permut;		//	permutation
}				GPSOutput;


#define	BAD_ELEMENT	2000
#define	NO_SPACE	2001

int	minimizeBandProfile( GPSInput * inp, GPSOutput * out );

#endif

//	eof
