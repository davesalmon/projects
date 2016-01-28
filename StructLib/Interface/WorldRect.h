/*+
 *	File:		WorldRect.h
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

#ifndef _H_WorldRect
#define _H_WorldRect

//---------------------------------- Includes ----------------------------------

#include "DlTypes.h"
#include "WorldPoint.h"
#include "DlAssert.h"

//---------------------------------- Class -------------------------------------

//
// WorldRect provides a floating point rectangle class to complement the 
//	native ScreenRect. Note that the coordinates of WorldRect run up and to 
//	the right, instead of down and to the right as in screen coordinate.
//
class	WorldRect 
{
public:

	WorldRect();
	
	WorldRect(StrInputStream& input);
	
	//	define the coords. Requires l < r and b < t
	WorldRect(DlFloat64 l, DlFloat64 b, DlFloat64 r, DlFloat64 t);
	
	//	construct from 2 points.
	WorldRect(const WorldPoint& p1, const WorldPoint& p2);
	
	// construct a rectangle centered at pt with a width and 2 * size
	WorldRect(const WorldPoint& pt, DlFloat64 size);

	void		write(StrOutputStream& s);
	//	reset. Requires l < r and b < t
	void		set(DlFloat64 l, DlFloat64 b, DlFloat64 r, DlFloat64 t);

	//	return _true if the rect contains this point
	bool		contains(const WorldPoint& pt) const;
	//	return _true if the rectangles intersect
	bool		intersects(const WorldRect& r) const;
	// return _true if the line segment defined by p1 and p2 intersects.
	bool		intersects(const WorldPoint& p1, const WorldPoint& p2) const;
	
	//	make rect large enough to include pt
	void		addBounds(const WorldPoint& pt);
	
	const WorldRect& operator+=(const WorldPoint& pt);
	WorldRect operator+(const WorldPoint& pt);

	const WorldRect& operator+=(const WorldRect& pt);
	WorldRect operator+(const WorldRect& pt);

	//	access
	DlFloat64 	top() const;
	DlFloat64	left() const;
	DlFloat64	right() const;
	DlFloat64	bottom() const;
	
	//	access
	DlFloat64&	top();
	DlFloat64&	left();
	DlFloat64&	right();
	DlFloat64&	bottom();
	
	//	return points
	const WorldPoint& topRight() const;
	const WorldPoint& bottomLeft() const;

	WorldPoint& topRight();
	WorldPoint& bottomLeft();

	WorldPoint center() const;
	
	//  return size
	DlFloat64 width() const;
	DlFloat64 height() const;
	
	//	test for equality
	bool operator==(const WorldRect& r) const;
	
private:
	WorldPoint	_bl;
	WorldPoint	_tr;
};

//---------------------------------- Inlines -----------------------------------

inline 
WorldRect::WorldRect() 
{
}

inline 
WorldRect::WorldRect(StrInputStream& inp)
	: _bl(inp), _tr(inp) 
{
}

inline 
WorldRect::WorldRect(DlFloat64 l, DlFloat64 b, DlFloat64 r, DlFloat64 t) 
	: _bl(l, b), _tr(r,t) 
{
	_DlAssert(_bl.x() <= _tr.x() && _bl.y() <= _tr.y());
}

inline 
WorldRect::WorldRect(const WorldPoint& p1, const WorldPoint& p2) 
	: _bl(p1), _tr(p2)
{
	if (left() > right()) {
		left() = p2.x(); right() = p1.x();
	}
	if (bottom() > top()) {
		bottom() = p2.y(); top() = p1.y();
	}
}

inline 
WorldRect::WorldRect(const WorldPoint& pt, DlFloat64 size) 
	: _bl(pt.x() - size, pt.y() - size), _tr(pt.x() + size, pt.y() + size) 
{
}

inline void
WorldRect::write(StrOutputStream& s) 
{
	_bl.write(s);
	_tr.write(s);	
}

inline DlFloat64
WorldRect::top() const 
{
	return _tr.y();
}

inline DlFloat64
WorldRect::left() const 
{
	return _bl.x();
}

inline DlFloat64
WorldRect::right() const 
{
	return _tr.x();
}

inline DlFloat64
WorldRect::bottom() const 
{
	return _bl.y();
}

inline DlFloat64& 
WorldRect::top() 
{
	return _tr.y();
}

inline DlFloat64& 
WorldRect::left() 
{
	return _bl.x();
}

inline DlFloat64& 
WorldRect::right() 
{
	return _tr.x();
}

inline DlFloat64& 
WorldRect::bottom()
{
	return _bl.y();
}

inline void 
WorldRect::set(DlFloat64 l, DlFloat64 b, DlFloat64 r, DlFloat64 t)
{
	left() = l; right() = r; top() = t; bottom() = b;
	_DlAssert(_bl.x() <= _tr.x() && _bl.y() <= _tr.y());
}

inline bool 
WorldRect::operator==(const WorldRect& r) const
{
	return _tr == r._tr && _bl == r._bl;
}

inline const WorldPoint& 
WorldRect::topRight() const
{
	return _tr;
}

inline const WorldPoint& 
WorldRect::bottomLeft() const
{
	return _bl;
}

inline WorldPoint&
WorldRect::topRight()
{
	return _tr;
}

inline WorldPoint&
WorldRect::bottomLeft()
{
	return _bl;
}

inline WorldPoint 
WorldRect::center() const 
{
	return (_tr + _bl) /= 2.0;
}

inline DlFloat64 
WorldRect::width() const 
{
	return _tr.x() - _bl.x();
}

inline DlFloat64
WorldRect::height() const 
{
	return _tr.y() - _bl.y();
}

inline const WorldRect& 
WorldRect::operator+=(const WorldPoint& pt)
{
	addBounds(pt);
	return *this;
}

inline WorldRect
WorldRect::operator+(const WorldPoint& pt)
{
	WorldRect r(*this);
	r.addBounds(pt);
	return r;
}

inline const WorldRect& 
WorldRect::operator+=(const WorldRect& rr)
{
	addBounds(rr.bottomLeft());
	addBounds(rr.topRight());
	return *this;
}

inline WorldRect 
WorldRect::operator+(const WorldRect& rr)
{
	WorldRect r(*this);
	return r += rr;
}

#endif

//	eof
