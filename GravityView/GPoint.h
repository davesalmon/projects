/*
 *  GPoint.h
 *  GravityComp
 *
 *  Copyright 2008 David Salmon. All rights reserved.
 *
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

#pragma once

#include "DlPlatform.h"
#include "DlRefCountedPtr.h"
#include <vector>

class GPoint
{
public:
	GPoint();

	GPoint(double x, double y);

    GPoint operator+(const GPoint& p) const;    
    const GPoint& operator+=(const GPoint& p);
	
    GPoint operator-(const GPoint& p) const ;    
    const GPoint& operator-=(const GPoint& p);
	
	GPoint operator * (double val) const ;
	const GPoint& operator*=(double val) ;
	
	// the error is p - *this
	void error(const GPoint& p);
	
	double getX() const;
    double getY() const;
    
	void setX(double x);
	void setY(double y);
	
    double hypotSq() const;

private:
	double x;
    double y;
};

typedef std::vector<GPoint>	GPointList;
typedef DlRefCountedPtr<GPointList> GPointListPtr;


//	inline implementation

//----------------------------------------------------------------------------------------
//  GPoint::GPoint                                                     constructor inline
//
//      default constructor
//
//  returns nothing
//----------------------------------------------------------------------------------------
inline
GPoint::GPoint() 
	: x(0), y(0) 
{
}

//----------------------------------------------------------------------------------------
//  GPoint::GPoint                                                     constructor inline
//
//      construct with values.
//
//  double x   -> 
//  double y   -> 
//
//  returns nothing
//----------------------------------------------------------------------------------------
inline
GPoint::GPoint(double x, double y) 
	: x(x), y(y) 
{
}
    
//----------------------------------------------------------------------------------------
//  GPoint::operator+                                                              inline
//
//      add this and GPoint.
//
//  const GPoint& p -> the point
//
//  returns p + this
//----------------------------------------------------------------------------------------
inline
GPoint 
GPoint::operator+(const GPoint& p) const 
{
	return GPoint(x + p.x, y + p.y);
}

//----------------------------------------------------------------------------------------
//  GPoint::operator+=                                                             inline
//
//      add GPoint to this
//
//  const GPoint& p -> the point
//
//  returns this
//----------------------------------------------------------------------------------------
inline
const GPoint& 
GPoint::operator+=(const GPoint& p)
{
	x += p.x;
	y += p.y;
	
	return *this;
}

//----------------------------------------------------------------------------------------
//  GPoint::operator-                                                              inline
//
//      subtract this and GPoint.
//
//  const GPoint& p -> the point
//
//  returns p + this
//----------------------------------------------------------------------------------------
inline
GPoint 
GPoint::operator-(const GPoint& p) const 
{
	return GPoint(x - p.x, y - p.y);
}

//----------------------------------------------------------------------------------------
//  GPoint::operator-=                                                             inline
//
//      subtract GPoint from this
//
//  const GPoint& p -> the point
//
//  returns this
//----------------------------------------------------------------------------------------
inline
const GPoint& 
GPoint::operator-=(const GPoint& p)
{
	x -= p.x;
	y -= p.y;
	
	return *this;
}

//----------------------------------------------------------------------------------------
//  GPoint::operator*                                                              inline
//
//      multiply this and GPoint.
//
//  const GPoint& p -> the point
//
//  returns p + this
//----------------------------------------------------------------------------------------
inline
GPoint 
GPoint::operator * (double val) const 
{
	return GPoint(x * val, y * val);
}

//----------------------------------------------------------------------------------------
//  GPoint::operator*=                                                             inline
//
//      multiply this by GPoint
//
//  const GPoint& p -> the point
//
//  returns this
//----------------------------------------------------------------------------------------
inline
const GPoint& 
GPoint::operator*=(double val) 
{
	x *= val;
	y *= val;
	return *this;
}

//----------------------------------------------------------------------------------------
//  GPoint::error                                                                  inline
//
//      return the error.
//
//  const GPoint& p    -> 
//
//  returns nothing
//----------------------------------------------------------------------------------------
inline
void 
GPoint::error(const GPoint& p)
{
	x = p.x - x;
	y = p.y - y;        
}

//----------------------------------------------------------------------------------------
//  GPoint::getX                                                                   inline
//
//      return x
//
//  returns double <- 
//----------------------------------------------------------------------------------------
inline
double 
GPoint::getX() const 
{
	return x;
}

//----------------------------------------------------------------------------------------
//  GPoint::getY                                                                   inline
//
//      return y
//
//  returns double <- 
//----------------------------------------------------------------------------------------
inline
double 
GPoint::getY() const 
{
	return y;
}

//----------------------------------------------------------------------------------------
//  GPoint::setX                                                                   inline
//
//      set x
//
//  double x   -> 
//
//  returns nothing
//----------------------------------------------------------------------------------------
inline
void 
GPoint::setX(double x) 
{ 
	this->x = x; 
}

//----------------------------------------------------------------------------------------
//  GPoint::setY                                                                   inline
//
//      set y
//
//  double y   -> 
//
//  returns nothing
//----------------------------------------------------------------------------------------
inline
void 
GPoint::setY(double y) 
{ 
	this->y = y; 
}

//----------------------------------------------------------------------------------------
//  GPoint::hypotSq                                                                inline
//
//      return the sum of squares.
//
//  returns double <- 
//----------------------------------------------------------------------------------------
inline
double 
GPoint::hypotSq() const 
{ 
	return x * x + y * y; 
}


