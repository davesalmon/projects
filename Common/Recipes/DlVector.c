/*+
 *	File:		DlVector.c
 *
 *	Contains:	define vector utilities
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

#include "DlPlatform.h"
#include "DlVector.h"

#define		Sub		SubPoint3d
#define		Dot		VectorDot
#define		Cross	VectorCross

/* ----------------------------------------------------------------------------
 * VectorNorm	-	normalize the given vector and return the length
 * 
 * (-> input param, <- output param, <-> i/o param)
 * 
 * v			<->	vector
 * return		<-	original length of vector
 * ----------------------------------------------------------------------------
 */
DlFloat
VectorNorm( Point3d *v )
{
	DlFloat	len = VectorLength( v );
	if ( len > VECT_TOL ) {
		DlFloat	fact = 1.0/len;
		VectorScale( v, fact );
		return len;
	}
	return 0;
}

/* ----------------------------------------------------------------------------
 * SectSegLine	-	find the intersection of a segment with a line
 *
 *		Find the intersection of a segment delimited by c1 and c2 with 
 *		the line defined by p and v the intersection parameter is 
 *		returned as zi.  Method assumes that the lines share a common plane.
 *		If they dont share a common plane, zi is the length of the projection
 *		of (c2-c1) onto 
 *
 *		zi * (c2-c1) ^ v = (p-c1) ^ v			- normal vectors have same dir.
 *
 *		Dot both sides with (c2-c1) ^ v
 *
 *		zi * len = (p-c1) ^ v . (c2-c1) ^ v		- give's zi
 * 
 * (-> input param, <- output param, <-> i/o param)
 * 
 * c1			->	first segment coordinate
 * c2			->	second segment coordinate
 * p			->	point on line
 * v			->	line direction
 * zi			<-	fraction of distance between c1 and c2 for intersection
 * return		<-	true if intersection is within segment
 * ----------------------------------------------------------------------------
 */
DlBoolean
SectSegLine( const Point3d *c1, const Point3d *c2, const Point3d *p, 
	const Point3d *v, DlFloat *zi )
{
	DlFloat		len;
	Point3d		dSeg;
	Point3d		n1;
	
	Sub( c1, c2, &dSeg );			/* segment vector */

	Cross( &dSeg, v, &n1 );				/* normal to plane */
	len = Dot( &n1, &n1 );				/* area / zi */

	if ( len >= VECT_TOL ) {		//	not parallel
		Point3d		n2;
		Point3d		diff;

		Sub( c1, p, &diff );
		Cross( &diff, v, &n2 );				/* area vector */
	
		*zi = Dot( &n1, &n2 ) / len;
	
		return *zi >= 0.0 && *zi <= 1.0;
	}
	return DlFalse;
}

/* ----------------------------------------------------------------------------
 * SectSegSeg	-	Intersect two line segments
 *
 * (-> input param, <- output param, <-> i/o param)
 * 
 * a1			->	first segment start
 * b1			->	first segment end
 * a2			->	second segment start
 * b2			->	second segment end
 * zi			<-	parametric coordinate for first segment
 * return		<-	true if intersetion is within both segments
 * ----------------------------------------------------------------------------
 */
DlBoolean
SectSegSeg( const Point3d *a1, const Point3d *b1, const Point3d *a2, 
	const Point3d *b2, DlFloat *zi )
{
	DlBoolean	result = DlFalse;
	DlFloat		len;
	Point3d		dSeg1;
	Point3d		dSeg2;
	Point3d		n1;
	
	Sub( b1, a1, &dSeg1 );			/* segment vector */
	Sub( b2, a2, &dSeg2 );			/* segment vector */
	
	Cross( &dSeg1, &dSeg2, &n1 );	/* normal to plane */
	len = Dot( &n1, &n1 );			/* area / zi */
	if ( len >= VECT_TOL ) {		//	not parallel 
		Point3d		diff;
		Point3d		n2;

		Sub( a1, a2, &diff );				/* distance between starts */
		Cross( &diff, &dSeg2, &n2 );		/* area vector */
	
		*zi = Dot( &n1, &n2 ) / len;
	
		if ( *zi >= 0 && *zi <= 1.0 )  {	//	in bounds
			zi++;
			Cross( &diff, &dSeg1, &n2 );
			*zi = Dot( &n1, &n2 ) / len;
	
			result = *zi >= 0.0 && *zi <= 1.0;
		}
	}
	return result;
}

/* ----------------------------------------------------------------------------
 * SectLineLine	-	find the intersection of two lines
 *
 *		Intersect two lines without worrying about bounds.  
 *		pt is intersection point.
 *
 *		Note:	if lines are oblique the point returned is the 
 *				closest point on line1 to line 2.
 *
 *		line1 = c1 + zi1 * v1;
 *		line2 = c2 + zi2 * v2;
 * 
 * (-> input param, <- output param, <-> i/o param)
 * 
 * c1			->	coordinate for line 1
 * v1			->	direction of line 1
 * c2			->	coordinate for line 2
 * v2			->	direction of line 2
 * pt			<-	intersection or nearest point
 * return		<-	true if they intersect
 * ----------------------------------------------------------------------------
 */
DlBoolean
SectLineLine( const Point3d *c1, const Point3d *v1, const Point3d *c2, 
		const Point3d *v2, Point3d *pt )
{
	Point3d	diff;
	Point3d	n1, n2;
	DlFloat	zi, len;
	
	Cross( v1, v2, &n1 );			/* Normal to plane of lines */
	len = Dot( &n1, &n1 );			/* area of parallelogram */

	if ( len >= VECT_TOL ) {
	
		Sub( c1, c2, &diff );		/* distance between line starts */
		Cross( &diff, v2, &n2 );		/* normal to plane of diff, v2 */
		
		zi = Dot( &n1, &n2 ) / len;		/* area of par2 / area of par1 */
		
		pt->x = c1->x + zi * v1->x;
		pt->y = c1->y + zi * v1->y;
		pt->z = c1->z + zi * v1->z;

		return DlTrue;
	}
	return DlFalse;
}
	
/* ----------------------------------------------------------------------------
 * DistLineLine	-
 *
 * 	Find the closest point between two lines and return the distance separating
 *	them.
 *
 *		line1 = c1 + zi1 * v1;
 *		line2 = c2 + zi2 * v2;
 * 
 * (-> input param, <- output param, <-> i/o param)
 * 
 * c1			->	coordinate for line 1
 * v1			->	direction of line 1
 * c2			->	coordinate for line 2
 * v2			->	direction of line 2
 * pt1			<-	point on line 1
 * pt2			<-	point on line 2
 * return		<-	distance
 * ----------------------------------------------------------------------------
 */
DlFloat
DistLineLine( const Point3d *c1, const Point3d *v1, const Point3d *c2, 
		const Point3d *v2, Point3d *pt1, Point3d *pt2 )
{
	DlFloat		d1, d2;
	DlFloat		zi, det;
	DlFloat		d12, r2, r1;
	Point3d		diff;
	DlBoolean	parallel;
	
	d1 = Dot( v1, v1 );
	d2 = Dot( v2, v2 );
	d12 = Dot( v1, v2 );
	det = d1 * d2 - d12 * d12;

	Sub( c1, c2, &diff );

	parallel = fabs(det) < VECT_TOL;
	if ( parallel ) 
		zi = 0.0;
	else {
		r1 = Dot( &diff, v1 );
		r2 = Dot( &diff, v2 );
		
		zi = (d2 * r1 - d12 * r2) / det;
	}
	
	pt1->x = c1->x + zi * v1->x;
	pt1->y = c1->y + zi * v1->y;
	pt1->z = c1->z + zi * v1->z;
	
	if ( !parallel )
		zi = (d12 * r1 - d1 * r2) / det;
	
	pt2->x = c2->x + zi * v2->x;
	pt2->y = c2->y + zi * v2->y;
	pt2->z = c2->z + zi * v2->z;
	
	Sub( pt1, pt2, &diff );
	return VectorLength( &diff );
}

#if 0
/*-----------------
 *	basic utilities
*/
		
DlFloat	VectorLength( Point3d *v )
{
	return __VectorLength(v);
}

void	AddPoint3d( Point3d *a, Point3d *b, Point3d *c )
{
	__AddPoint3d( a, b, c );
}

void	SubPoint3d( Point3d *a, Point3d *b, Point3d *c )
{
	__SubPoint3d( a, b, c );
}
void	VectorCross( Point3d *a, Point3d *b, Point3d *c )
{
	__VectorCross( a, b, c );
}

DlFloat	VectorDot( Point3d *a, Point3d *b )
{
	return __VectorDot( a, b );
}

DlBoolean	VectorEqual( Point3d *a, Point3d *b, DlFloat tol )
{
	return __VectorEqual( a, b, tol );
}

#endif

//	EOF
