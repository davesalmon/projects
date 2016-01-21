/*+
 *	File:		DlVector_f.c
 *
 *	Contains:	Implementation of Vector Utilities for DlFloat
 *
 *		The following are implemented as macros if LDlInt32 is not defined
 *
 *		double	VectorLength( Point3d * );
 *		double	VectorNorm( Point3d *, double * );
 *		void	AddPoint3d( Point3d *, Point3d *, Point3d * );
 *		void	SubPoint3d( Point3d *, Point3d *, Point3d * );
 *		void	VectorCross( Point3d *, Point3d *, Point3d * );
 *		double	VectorDot( Point3d *, Point3d * );
 *		Boolean	VectorEqual( Point3d *, Point3d * );
 *
 *	The following are always functions
 *
 *		Boolean	SectSegSeg( a1, b1, a2, b2, zi );
 *		Boolean	SectSegLine( c1, c2, p, v, zi );
 *
 *	Written by:	David C. Salmon
 *
 *	Copyright:	COPYRIGHT (C) 1990-96 by David C. Salmon.  *WORLDWIDE RIGHTS RESERVED*
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

#ifdef DlFloatOption
#undef DlFloatOption
#endif
#define	DlFloatOption 2
#include	"DlVector.c"

//	eof
