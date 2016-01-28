/*+
 *	File:		WorldRect.cpp
 *
 *	Contains:	rectangle interface
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
#include "WorldRect.h"

	//	return the distance between this point and another
bool
WorldRect::contains(const WorldPoint& pt) const
{
	return pt.x() >= _bl.x() && pt.x() <= _tr.x() && pt.y() >= _bl.y() && pt.y() <= _tr.y();
}

bool
WorldRect::intersects(const WorldRect& r) const
{
	return !(
		r.right() < left()   || r.left()   > right() ||
		r.top()   < bottom() || r.bottom() > top()
	); 
}

// return _true if the line segment defined by p1 and p2 intersects.
bool
WorldRect::intersects(const WorldPoint& p1, const WorldPoint& p2) const
{
	DlFloat64 xmin;
	DlFloat64 xmax;
	if (p1.x() < p2.x()) {
		xmin = p1.x(); xmax = p2.x();
	} else {
		xmax = p1.x(); xmin = p2.x();
	}

	DlFloat64 ymin;
	DlFloat64 ymax;
	if (p1.y() < p2.y()) {
		ymin = p1.y(); ymax = p2.y();
	} else {
		ymax = p1.y(); ymin = p2.y();
	}
	
	if (!(xmax < left()   || xmin > right() ||
		  ymax < bottom() || ymin > top()))
	{
		//	now do the real work...
		DlFloat64 dx = p2.x() - p1.x();
		if (dx == 0) {
			return (ymax >= top() && ymin <= top()) || (ymax >= bottom() && ymin <= bottom());
		}
		
		DlFloat64 dy = p2.y() - p1.y();
		if (dy == 0) {
			return (xmax >= right() && xmin <= right()) || (xmax >= left() && xmin <= left());
		}

		// if it completely contains the line return true
		if (contains(p1) && contains(p2))
			return true;

		//	general case:
		DlFloat64 fact = dy / dx;
		// check left
		DlFloat c = p1.y() + fact * (left() - p1.x());
		if (c <= top() && c >= bottom())
			return true;
		// check right
		c = p1.y() + fact * (right() - p1.x());
		if (c <= top() && c >= bottom())
			return true;
		
		// check bottom
		fact = dx / dy;
		c = p1.x() + fact * (top() - p1.y());
		if (c <= right() && c >= left())
			return true;
		// check right
		c = p1.x() + fact * (bottom() - p1.y());
		if (c <= right() && c >= left())
			return true;
	}
	return false;
}


void 
WorldRect::addBounds(const WorldPoint& pt)
{
	if (pt.y() > top())
		top() = pt.y();
	if (pt.y() < bottom())
		bottom() = pt.y();

	if (pt.x() > right())
		right() = pt.x();
	if (pt.x() < left())
		left() = pt.x();
}

//	eof
