/*++++++++++++++++++++++
**	random.c
**
**		Implement random number generators. Adapted from 
**		Numerical recipes in C.
 *
 *	Copyright:	COPYRIGHT (C) 1989-2016 By David C. Salmon.  *WORLDWIDE RIGHTS RESERVED*
 *
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

#include "DlPlatform.h"
#include "random.h"
#include <stdlib.h>

#define	 NUM_STORED	98
#define	 MAX_RAND	RAND_MAX	/* defined in C ref manual */
#define	 maxran		(MAX_RAND+1.0)

#define	 MAC_MAX_RAND	65535
#define	 SANE_MAX_RAND	(scalb881(1.0,31))

static	 DlFloat	vv[NUM_STORED];

/*-------------------------
**	ran0
**
**		Improved system random number generator. This random
**		number generator creates a table of random numbers. Each
**		time the random number generator is called, a prior random
**		number is used to index the table. The random number is 
**		drawn from the table and replaced with a new number.
*/

DlFloat	
ran0(long *idum)
{
	static DlFloat	y;
	DlFloat			dum;
	static int		iff = 0;
	int				j;
	
	/* •• Initialize the random number generator */

	if ((*idum < 0) || (iff == 0)) {
		iff = 1;
		srand(*idum);
		*idum = 1;
		for (j = 1; j < NUM_STORED; j++) dum = rand();
		for (j = 1; j < NUM_STORED; j++) vv[j] = rand();
		y = rand();
	}

	/*	Compute an index into the table from the previous random number.
		*/

	j = (int)(1 + (((NUM_STORED-1.0) * y) / (MAX_RAND+1.0)));
	_RecipesAssert(j < NUM_STORED && j >= 1);
	y = vv[j];
	vv[j] = rand();
	return y/(MAX_RAND+1.0);
}

/*--------------------------------
**	ran1
**
**		Improved random number generator with
**		more variation than ran0
*/

/*	Define the multipliers and constants for three random 
	number generators that max out at sizeof long */

#define	M1		259200
#define	IA1		7141
#define	IC1		54773
#define	RM1		(1.0/M1)

#define	M2		134456
#define	IA2		8121
#define	IC2		28411
#define	RM2		(1.0/M2)

#define	M3		243000
#define	IA3		4561
#define	IC3		51349

DlFloat
ran1(long *idum)
{
	static	unsigned long	ix1, ix2, ix3;
	static	DlFloat	temp;
	static	int		iff = 0;
	int		j;
	
	/*	Initialize the three sequences of random numbers
		and the table. 
		*/

	if (*idum < 0 || iff == 0) {
		long theSeed = *idum;
		if (theSeed > 0) theSeed = -theSeed;
		iff = 1;
		ix1 = (IC1 - theSeed) % M1;		/* • Use seed to init ix1 */
		ix1 = (IA1 * ix1 + IC1) % M1;		/* • First number */
		ix2 = ix1 % M2;						/* • Use ix1 to init ix2 */
		ix1 = (IA1 * ix1 + IC1) % M1;		/* • next ix1 number */
		ix3 = ix1 % M3;						/* • Use ix1 to init ix3 */
		for (j = 1; j < NUM_STORED; j++) {/* • Fill the table */
			ix1 = (IA1 * ix1 + IC1) % M1;	/* • Generate ix1 */
			ix2 = (IA2 * ix2 + IC2) % M2;	/* • Generate ix2 */
			vv[j] = (ix1 + ix2 * RM2) * RM1;	/* • Combine to fill table */
		}
		*idum = 1;
	}
	
	/* •• Here is where we usually start */

	ix1 = (IA1 * ix1 + IC1) % M1;		/* • Next ix1 */
	ix2 = (IA2 * ix2 + IC2) % M2;		/* • Next ix2 */
	ix3 = (IA3 * ix3 + IC3) % M3;		/* • Next ix3 */

	/* •• Use ix3 to look into table */

	j = (int)(1 + (((NUM_STORED - 1.0) * ix3) / M3));
	if (j > NUM_STORED - 1 || j < 1) 
		throw RecipesError("random number failed");

	/* •• Grab the number from the table */

	temp = vv[j];

	/* •• Refuel the table with a new number */
	
	vv[j] = (ix1 + ix2 * RM2) * RM1;
	
	return temp;
}

/*--------------------------
**	randx
**
**		System supplied random number
**		Generator.  Returns 0-1
*/

DlFloat	
randx(long *seed)
{
	if ((*seed) < 0) {
		srand(-(*seed));
		*seed = 1;
	}

	return rand()/(MAX_RAND+1.0);
}

/*------------------------
**	randy
**
**		MacOS random number generator
*/

DlFloat	
randy(long	*seed)
{
#if defined(MAC_OS)

	unsigned	short	random;
	if ((*seed) < 0) {
		qd.randSeed = -(*seed);
		*seed = 1;
	}
	random = (unsigned short)Random();
	return random / (MAC_MAX_RAND+1.0);

#else

	return randx(seed);

#endif	/* MAC_OS */
}


/*------------------------
**	randz
**
**		SANE random number generator
**
**		VERY SLOW!!!
*/

DlFloat	randz(long *seed)
{
#if defined(MAC_OS)

	static long double	myseed;
	if ((*seed) < 0) {
		myseed = -(*seed);
		*seed = 1;
	}
	
	return randomx881(&myseed) / (SANE_MAX_RAND-1.0);

#else

	return randx(seed);

#endif	/* MAC_OS */
}
