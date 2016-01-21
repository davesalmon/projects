/*+
 *	File:		DlVector.h
 *
 *	Contains:	define vector functions and macros
 *
 *		The following are implemented as macros if LDlInt32 is not defined
 *
 *		double	VectorLength(Point3d *);
 *		double	VectorNorm(Point3d *, double *);
 *		void	AddPoint3d(Point3d *, Point3d *, Point3d *);
 *		void	SubPoint3d(Point3d *, Point3d *, Point3d *);
 *		void	VectorCross(Point3d *, Point3d *, Point3d *);
 *		double	VectorDot(Point3d *, Point3d *);
 *		Boolean	VectorEqual(Point3d *, Point3d *);
 *
 *	The following are always functions
 *
 *		Boolean	SectSegSeg(a1, b1, a2, b2, zi);
 *		Boolean	SectSegLine(c1, c2, p, v, zi);
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

#ifndef _H_DlVector
#define	_H_DlVector

//#include	<Types.h>
//#include	<fp.h>

#ifndef _H_DlTypes
#include "DlTypes.h"
#endif
#include	<float.h>

typedef struct	Point3d {
	DlFloat	x;
	DlFloat	y;
	DlFloat	z;
}	Point3d;

typedef	DlFloat	*DlPointPtr;

#ifdef DlUseSingleFloat
	#define	VECT_TOL	FLT_EPSILON
#elif defined(DlUseANSIFloat)
	#define	VECT_TOL	DLB_EPSILON
#else
	#define	VECT_TOL	LDBL_EPSILON
#endif

#ifdef __cplusplus
extern "C" {
#endif

DlBoolean	SectSegLine(const Point3d *, const Point3d *, const Point3d *, 
				const Point3d *, DlFloat *);
DlBoolean	SectSegSeg(const Point3d *, const Point3d *, const Point3d *, 
				const Point3d *, DlFloat *);
DlBoolean	SectLineLine(const Point3d *, const Point3d *, const Point3d *, 
				const Point3d *, Point3d *);
DlFloat		DistLineLine(const Point3d *, const Point3d *, const Point3d *, 
				const Point3d *, Point3d *, Point3d *);
DlFloat		VectorNorm(Point3d *v);

#ifdef __cplusplus
}
#endif

#ifdef __cplusplus
inline DlFloat	
VectorDot(const Point3d *v1, const Point3d *v2)
{
	return v1->x * v2->x + v1->y * v2->y + v1->z * v2->z;
}

inline DlFloat
VectorLength(const Point3d *v)
{
	return sqrt(VectorDot(v, v));
}

inline void
VectorScale(Point3d *v, DlFloat s)
{
	v->x *= s;
	v->y *= s;
	v->z *= s;
}
inline void
SubPoint3d(const Point3d *p1, const Point3d *p2, Point3d *d)
{
	d->x = p2->x - p1->x;
	d->y = p2->y - p1->y;
	d->z = p2->z - p1->z;
}
inline void
AddPoint3d(const Point3d *p1, const Point3d *p2, Point3d *d)
{
	d->x = p2->x + p1->x;
	d->y = p2->y + p1->y;
	d->z = p2->z + p1->z;
}
inline void
VectorCross(const Point3d *v1, const Point3d *v2, Point3d *d)
{
	d->x = v1->y * v2->z - v1->z * v2->y;
	d->y = v1->z * v2->x - v1->x * v2->z;
	d->z = v1->x * v2->y - v1->y * v2->x;
}
inline DlBoolean
VectorEqual(const Point3d *v1, const Point3d *v2, DlFloat t)
{
	return fabs(v1->x - v2->x) < t && fabs(v1->y - v2->y) < t
				&& fabs(v1->z - v2->z) < t;
}

#else

#define	VectorDot(v1,v2)		(	(v1)->x * (v2)->x	\
								+	(v1)->y * (v2)->y	\
								+	(v1)->z * (v2)->z	\
								)

#define	VectorLength(v)		sqrt(VectorDot(v,v))

#define	VectorScale(v,s)		{							\
								(v)->x *= (s);				\
								(v)->y *= (s);				\
								(v)->z *= (s);				\
								}

#define	SubPoint3d(p1,p2,d)	(	(d)->x = (p2)->x - (p1)->x	\
								,	(d)->y = (p2)->y - (p1)->y	\
								,	(d)->z = (p2)->z - (p1)->z	\
								)
#define	AddPoint3d(p1,p2,d)	(	(d)->x = (p2)->x + (p1)->x	\
								,	(d)->y = (p2)->y + (p1)->y	\
								,	(d)->z = (p2)->z + (p1)->z	\
								)
#define	VectorCross(v1,v2,d)	(	(d)->x = (v1)->y * (v2)->z - (v1)->z * (v2)->y	\
								,	(d)->y = (v1)->z * (v2)->x - (v1)->x * (v2)->z	\
								,	(d)->z = (v1)->x * (v2)->y - (v1)->y * (v2)->x	\
								)
#define	VectorEqual(v1,v2,t)	(	(fabs((v1)->x - (v2)->x) < t)	\
								&&	(fabs((v1)->y - (v2)->y) < t)	\
								&&	(fabs((v1)->z - (v2)->z) < t)	\
								)
#endif	// cplusplus

#endif
