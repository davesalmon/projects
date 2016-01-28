/*+
 *	File:		WorldPoint.cpp
 *
 *	Contains:	floating point point interface
 *
 *	Written by:	David C. Salmon
 *
 *	Copyright:	COPYRIGHT (C) 1990-96 By David C. Salmon.  *WORLDWIDE RIGHTS RESERVED*
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

#include "DlPlatform.h"
#include "WorldPoint.h"
#include "DlException.h"
#include <float.h>

//----------------------------------------------------------------------------------------
//  WorldPoint::dist
//
//      compute distance between points.
//
//  const WorldPoint& pt   -> the point to  use
//
//  returns DlFloat64      <- distance from this to pt.
//----------------------------------------------------------------------------------------
DlFloat64
WorldPoint::dist(const WorldPoint& pt) const 
{
	return dist(*this, pt);
}

//----------------------------------------------------------------------------------------
//  WorldPoint::dist
//
//      compute distance between points.
//
//  const WorldPoint& a    -> first point
//  const WorldPoint& b    -> second point
//
//  returns DlFloat64      <- distance between points.
//----------------------------------------------------------------------------------------
DlFloat64
WorldPoint::dist(const WorldPoint& a, const WorldPoint& b)
{
	return (a - b).length();
}

//----------------------------------------------------------------------------------------
//  WorldPoint::distLine
//
//      compute distance between point and line.
//
//  const WorldPoint& pstart   -> start of line
//  const WorldPoint& pend     -> end of line
//
//  returns DlFloat64          <- distance.
//----------------------------------------------------------------------------------------
DlFloat64
WorldPoint::distLine(const WorldPoint& pstart, const WorldPoint& pend) const
{
	WorldPoint v2 = pend - pstart;
	DlFloat64 len = v2.length();
	if (len == 0)
		throw DlException("Zero length vector for WorldPoint::distLine.");
	return std::fabs(cross(*this - pstart, v2)/len);
}

//----------------------------------------------------------------------------------------
//  WorldPoint::transform
//
//      transform this point to the coordinate system define by pstart and pend. The x 
//      coordinate of the resulting point is the distance along the line from pstart, and 
//      the y coordinate is the perpendicular distance, positive values being in the 
//      right-hand coordinate direction.
//
//  const WorldPoint& pstart   -> start of line
//  const WorldPoint& pend     -> end of line
//
//  returns WorldPoint         <- point relative to new coords.
//----------------------------------------------------------------------------------------
WorldPoint
WorldPoint::transform(const WorldPoint& pstart, const WorldPoint& pend) const
{
	WorldPoint v2 = pend - pstart;
	DlFloat64 len = v2.length();
	if (len == 0)
		throw DlException("Zero length vector for WorldPoint::transform.");
	WorldPoint v = *this - pstart;
	return WorldPoint(dot(v2,v)/len, cross(v2,v)/len);
}

//----------------------------------------------------------------------------------------
//  WorldPoint::norm
//
//      compute a unit vector from p1 to p2
//
//  const WorldPoint& p1   -> first point
//  const WorldPoint& p2   -> second point
//
//  returns WorldPoint     <- unit vector p1 -> p2
//----------------------------------------------------------------------------------------
WorldPoint 
WorldPoint::norm(const WorldPoint& p1, const WorldPoint& p2)
{
	WorldPoint	p = p2 - p1;
	DlFloat64 len = p.length();
	if (len == 0)
		throw DlException("Zero length vector for WorldPoint::norm.");
	return p /= len;
}

// eof