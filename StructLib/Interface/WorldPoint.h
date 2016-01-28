/*+
 *	File:		WorldPoint.h
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

#ifndef _H_WorldPoint
#define _H_WorldPoint
#include "StrInputStream.h"
#include "StrOutputStream.h"
#include <cmath>
#include "DlTypes.h"

class	WorldPoint {

public:

	WorldPoint();
	WorldPoint(StrInputStream& inp);
	WorldPoint(DlFloat64 xc, DlFloat64 yc);

	//	set
	const WorldPoint& 	set(DlFloat64 x, DlFloat64 y);

	void			write(StrOutputStream& s);
	//	test for equality
	bool 			operator== (const WorldPoint& pt) const;
	bool			operator< (const WorldPoint& p) const;

	//	add points
	const WorldPoint& 	operator+= (const WorldPoint& pt);
	WorldPoint 			operator+ (const WorldPoint& pt) const;

	//	subtract points
	const WorldPoint& 	operator-= (const WorldPoint& pt);
	WorldPoint 			operator- (const WorldPoint& pt) const;
	
	// unary negate
	WorldPoint			operator- () const;

	//	divide
	const WorldPoint& 	operator/= (DlFloat64 s);

	//	compute a unit vector from p1 to p2
	static WorldPoint 	norm(const WorldPoint& p1, const WorldPoint& p2);

	//	vector dot product
	DlFloat64			dot(const WorldPoint& pt) const;
	static DlFloat64	dot(const WorldPoint& a, const WorldPoint& b);

	//	vector cross product
	DlFloat64			cross(const WorldPoint& pt) const;
	static DlFloat64	cross(const WorldPoint& a, const WorldPoint& b);

	//	return the distance between this point and another
	DlFloat64			dist(const WorldPoint& pt) const;
	static DlFloat64	dist(const WorldPoint& a, const WorldPoint& b);
	DlFloat64			manhattan(const WorldPoint& pt) const;
	static DlFloat64	manhattan(const WorldPoint& a, const WorldPoint& b);
	
	//	return distance between point and line defined by two point
	DlFloat64			distLine(const WorldPoint& pstart, const WorldPoint& pend) const;
	
	//	transform this points coordinates to the system defined by pstart and pend.
	//	xc=dist fro pstart along pstart--pend, yc = perpendicular dist
	WorldPoint			transform(const WorldPoint& pstart, const WorldPoint& pend) const;

	WorldPoint			abs() const;

	//	return dist from origin
	DlFloat64 			length() const;

	DlFloat64 			x() const;
	DlFloat64 			y() const;

	DlFloat64 & 		x();
	DlFloat64 & 		y();

private:
	DlFloat64	xc;
	DlFloat64	yc;
};

struct	WorldTransform {
    WorldTransform(const WorldPoint& n1, const WorldPoint& n2);
    
    WorldPoint 	cosines;
    DlFloat64	len;
};

//----------------------------------------------------------------------------------------
//  WorldPoint::WorldPoint                                             constructor inline
//
//      constructor.
//
//  returns nothing
//----------------------------------------------------------------------------------------
inline 
WorldPoint::WorldPoint() 
	: xc(0), yc(0) 
{
}

//----------------------------------------------------------------------------------------
//  WorldPoint::WorldPoint                                             constructor inline
//
//      constructor.
//
//  DlFloat64 xc   -> x coordinate
//
//  returns nothing
//----------------------------------------------------------------------------------------
inline 
WorldPoint::WorldPoint(StrInputStream& s) 
	: xc(s.GetDouble()), yc(s.GetDouble()) 
{
}

//----------------------------------------------------------------------------------------
//  WorldPoint::WorldPoint                                             constructor inline
//
//      constructor.
//
//  DlFloat64 xc   -> x coordinate
//  DlFloat64 yc   -> y coordinate
//
//  returns nothing
//----------------------------------------------------------------------------------------
inline 
WorldPoint::WorldPoint(DlFloat64 xc_, DlFloat64 yc_)
	: xc(xc_), yc(yc_)
{
}

//----------------------------------------------------------------------------------------
//  WorldPoint::write                                                             inline
//
//      set new point coordinates.
//
//  StrOutputStream s          -> stream
//
//  returns nothing.
//----------------------------------------------------------------------------------------
inline void 
WorldPoint::write(StrOutputStream& s) 
{
	s.PutDouble(xc);
	s.PutDouble(yc);
}


//----------------------------------------------------------------------------------------
//  WorldPoint::set                                                                inline
//
//      set new point coordinates.
//
//  DlFloat64 x                -> x coordinate
//  DlFloat64 y                -> y coordinate
//
//  returns const WorldPoint&  <- reference to point.
//----------------------------------------------------------------------------------------
inline const WorldPoint& 
WorldPoint::set(DlFloat64 x, DlFloat64 y) 
{
	xc = x; 
	yc = y;
	return *this;
}

//----------------------------------------------------------------------------------------
//  WorldPoint::operator==                                                         inline
//
//      test for equality. Note: this tests for an EXACT match. Really for debugging.
//
//  const WorldPoint& pt   -> point to check.
//
//  returns bool           <- true if points are identical.
//----------------------------------------------------------------------------------------
inline bool 
WorldPoint::operator==(const WorldPoint& pt) const
{
	return pt.xc == xc && pt.yc == yc;
}

//----------------------------------------------------------------------------------------
//  WorldPoint::operator<                                                          inline
//
//      test for less.
//
//  const WorldPoint& p    -> point to test
//
//  returns bool           <- true if this point has x < px and y < py
//----------------------------------------------------------------------------------------
inline bool
WorldPoint::operator<(const WorldPoint& p) const
{
	return xc < p.xc && yc < p.yc;
}

//----------------------------------------------------------------------------------------
//  WorldPoint::operator+=                                                         inline
//
//      add points.
//
//  const WorldPoint& pt       -> point to add to this.
//
//  returns const WorldPoint&  <- this
//----------------------------------------------------------------------------------------
inline const WorldPoint& 
WorldPoint::operator+= (const WorldPoint& pt) 
{
	xc += pt.xc;
	yc += pt.yc;
	return *this;
}

//----------------------------------------------------------------------------------------
//  WorldPoint::operator+                                                          inline
//
//      add points
//
//  const WorldPoint& pt   -> point to add to this
//
//  returns WorldPoint     <- new point.
//----------------------------------------------------------------------------------------
inline WorldPoint 
WorldPoint::operator+ (const WorldPoint& pt) const 
{
	return WorldPoint(xc+pt.xc, yc+pt.yc);
}

//----------------------------------------------------------------------------------------
//  WorldPoint::operator-=                                                         inline
//
//      subtract points
//
//  const WorldPoint & pt      -> point to subtract from this.
//
//  returns const WorldPoint&  <- this.
//----------------------------------------------------------------------------------------
inline const WorldPoint& 
WorldPoint::operator-= (const WorldPoint & pt)
{
	xc -= pt.xc;
	yc -= pt.yc;
	return *this;
}

//----------------------------------------------------------------------------------------
//  WorldPoint::operator-                                                          inline
//
//      subtract points.
//
//  const WorldPoint& pt   -> point to subtract from this.
//
//  returns WorldPoint     <- new point.
//----------------------------------------------------------------------------------------
inline WorldPoint 
WorldPoint::operator- (const WorldPoint& pt) const 
{
	return WorldPoint(xc - pt.xc, yc - pt.yc);
}

//----------------------------------------------------------------------------------------
//  WorldPoint::operator-                                                          inline
//
//      negate point.
//
//  returns WorldPoint     <- negated point.
//----------------------------------------------------------------------------------------
inline WorldPoint 
WorldPoint::operator- () const 
{
	return WorldPoint(-xc, -yc);
}

//----------------------------------------------------------------------------------------
//  WorldPoint::operator/=                                                         inline
//
//      divide by constant.
//
//  DlFloat64 s                -> constant to divide this by.
//
//  returns const WorldPoint&  <- this
//----------------------------------------------------------------------------------------
inline const WorldPoint& 
WorldPoint::operator/= (DlFloat64 s) 
{
	xc /= s; 
	yc /= s;
	return *this;
}

//----------------------------------------------------------------------------------------
//  WorldPoint::dot                                                                inline
//
//      dot product: x * px + y * py
//
//  const WorldPoint& pt   -> point to dot this against
//
//  returns DlFloat64      <- 
//----------------------------------------------------------------------------------------
inline DlFloat64 
WorldPoint::dot (const WorldPoint& pt) const 
{
	return xc * pt.xc + yc * pt.yc;
}

//----------------------------------------------------------------------------------------
//  WorldPoint::dot                                                                inline
//
//      compute dot product. ax * bx + ay * by.
//
//  const WorldPoint& a    -> first point
//  const WorldPoint& b    -> second point.
//
//  returns DlFloat64      <- dot product.
//----------------------------------------------------------------------------------------
inline DlFloat64 
WorldPoint::dot(const WorldPoint& a, const WorldPoint& b) 
{
	return a.dot(b);
}

//----------------------------------------------------------------------------------------
//  WorldPoint::cross                                                              inline
//
//      compute cross product. x * py - y * px
//
//  const WorldPoint& pt   -> point to cross.
//
//  returns DlFloat64      <- cross product.
//----------------------------------------------------------------------------------------
inline DlFloat64
WorldPoint::cross(const WorldPoint& pt) const 
{
	return xc * pt.yc - yc * pt.xc;
}

//----------------------------------------------------------------------------------------
//  WorldPoint::cross                                                              inline
//
//      compute cross product.
//
//  const WorldPoint& a    -> 
//  const WorldPoint& b    -> 
//
//  returns DlFloat64      <- 
//----------------------------------------------------------------------------------------
inline DlFloat64 
WorldPoint::cross(const WorldPoint& a, const WorldPoint& b) 
{
	return a.cross(b);
}

inline DlFloat64
WorldPoint::manhattan(const WorldPoint& pt) const {
	return manhattan(*this, pt);
}

inline DlFloat64
WorldPoint::manhattan(const WorldPoint& a, const WorldPoint& b)
{
	WorldPoint d((a-b).abs());
	return d.xc + d.yc;
}

//----------------------------------------------------------------------------------------
//  WorldPoint::length                                                             inline
//
//      compute distance from origin.
//
//  returns DlFloat64  <- distance.
//----------------------------------------------------------------------------------------
inline DlFloat64 
WorldPoint::length() const 
{
	return std::sqrt(dot(*this));
}

//----------------------------------------------------------------------------------------
//  WorldPoint::x                                                                  inline
//
//      return x coord.
//
//  returns DlFloat64  <- x
//----------------------------------------------------------------------------------------
inline DlFloat64
WorldPoint::x() const 
{
	return xc;
}

//----------------------------------------------------------------------------------------
//  WorldPoint::y                                                                  inline
//
//      return y coord.
//
//  returns DlFloat64  <- y
//----------------------------------------------------------------------------------------
inline DlFloat64 
WorldPoint::y() const 
{ 
	return yc; 
}

//----------------------------------------------------------------------------------------
//  WorldPoint::x                                                                  inline
//
//      return x coord.
//
//  returns DlFloat64& <- x
//----------------------------------------------------------------------------------------
inline DlFloat64& 
WorldPoint::x() 
{
	return xc;
}

//----------------------------------------------------------------------------------------
//  WorldPoint::y                                                                  inline
//
//      return y coord.
//
//  returns DlFloat64& <- y
//----------------------------------------------------------------------------------------
inline DlFloat64& 
WorldPoint::y() 
{ 
	return yc; 
}

//----------------------------------------------------------------------------------------
//  WorldPoint::abs                                                                inline
//
//      return absolution value of point.
//
//  returns WorldPoint <- new point with (|x|, |y|)
//----------------------------------------------------------------------------------------
inline WorldPoint
WorldPoint::abs() const 
{
	return WorldPoint(std::fabs(xc), std::fabs(yc));
}

inline
WorldTransform::WorldTransform(const WorldPoint& n1, const WorldPoint& n2)
: cosines(n2 - n1)
{
    len = cosines.length();
    cosines /= len;
}

#endif

//	eof
