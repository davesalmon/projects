/*+
 *	File:		graphics.cpp
 *
 *	Contains:	graphics object interface
 *
 *	Written by:	David C. Salmon
 *
 *	Copyright:	COPYRIGHT (C) 2000 By David C. Salmon.  *WORLDWIDE RIGHTS RESERVED*
 *
 *
 *	To Do:
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
-*/

#include "DlPlatform.h"
#include "graphics.h"
#include <limits.h>
#include "DlMacros.h"

//----------------------------------------------------------------------------------------
//  graphics::ReadScreenRect
//
//      read the screen rect from the input stream.
//
//  StrInputStream& s      -> stream
//
//  returns DlScreenRect2d <- point
//----------------------------------------------------------------------------------------
DlScreenRect2d 
graphics::ReadScreenRect(StrInputStream& s) 
{
	DlScreenRect2d r;
	r.l = s.GetInt();
	r.t = s.GetInt();
	r.r = s.GetInt();
	r.b = s.GetInt();
	return r;
}

//----------------------------------------------------------------------------------------
//  graphics::WriteScreenRect
//
//      write the screen rect to the ouput stream.
//
//  const DlScreenRect2d& r    -> rect to write
//  StrOutputStream& s         -> stream
//
//  returns nothing
//----------------------------------------------------------------------------------------
void 
graphics::WriteScreenRect(const DlScreenRect2d& r, StrOutputStream& s)
 {
	s.PutInt(r.l);
	s.PutInt(r.t);
	s.PutInt(r.r);
	s.PutInt(r.b);
}

//----------------------------------------------------------------------------------------
//  graphics::ReadMGPoint
//
//      read a point from the input stream.
//
//  StrInputStream& s  -> stream
//
//  returns MGPoint    <- point
//----------------------------------------------------------------------------------------
MGPoint
graphics::ReadMGPoint(StrInputStream& s)
{
	MGPoint p;
	p.x = s.GetFloat();
	p.y = s.GetFloat();
	return p;
}

//----------------------------------------------------------------------------------------
//  graphics::ReadMGSize
//
//      read a size from the input stream.
//
//  StrInputStream& s  -> stream
//
//  returns MGSize     <- size
//----------------------------------------------------------------------------------------
MGSize
graphics::ReadMGSize(StrInputStream& s)
{
	MGSize p;
	p.width = s.GetFloat();
	p.height = s.GetFloat();
	return p;
}

//----------------------------------------------------------------------------------------
//  graphics::ReadMGRect
//
//      read a rect from the input stream.
//
//  StrInputStream& s  -> stream
//
//  returns MGRect     <- rect
//----------------------------------------------------------------------------------------
MGRect
graphics::ReadMGRect(StrInputStream& s) 
{
	MGRect r;
	r.origin = ReadMGPoint(s);
	r.size = ReadMGSize(s);
	return r;
}

//----------------------------------------------------------------------------------------
//  graphics::WriteMGPoint
//
//      write a point to the output stream.
//
//  const MGPoint& p   -> point
//  StrOutputStream& s -> stream
//
//  returns nothing
//----------------------------------------------------------------------------------------
void
graphics::WriteMGPoint(const MGPoint& p, StrOutputStream& s) {
	s.PutFloat(p.x);
	s.PutFloat(p.y);
}

//----------------------------------------------------------------------------------------
//  graphics::WriteMGSize
//
//      write a size to the output stream.
//
//  const MGSize& sz   -> size
//  StrOutputStream& s -> stream
//
//  returns nothing
//----------------------------------------------------------------------------------------
void
graphics::WriteMGSize(const MGSize& sz, StrOutputStream& s) {
	s.PutFloat(sz.width);
	s.PutFloat(sz.height);
}

//----------------------------------------------------------------------------------------
//  graphics::WriteMGRect
//
//      write a rect to the output stream.
//
//  const MGRect& r    -> rect
//  StrOutputStream& s -> stream
//
//  returns nothing
//----------------------------------------------------------------------------------------
void
graphics::WriteMGRect(const MGRect& r, StrOutputStream& s) {
	WriteMGPoint(r.origin, s);
	WriteMGSize(r.size, s);
}

//----------------------------------------------------------------------------------------
//  graphics::createGraphics
//
//      create a new graphics object.
//
//  const DlScreenRect2d& sr   -> screen rectangle.
//  const WorldRect& wr        -> world rectangle.
//
//  returns graphics*          <- graphics object.
//----------------------------------------------------------------------------------------
graphics* 
graphics::createGraphics(const DlScreenRect2d& sr, const WorldRect& wr)
{
	return NEW graphics(sr, wr);
}

//----------------------------------------------------------------------------------------
//  graphics::createGraphics
//
//      create a new graphics object.
//
//  DlFloat64 scale        -> the scale.
//  const WorldRect& wr    -> the world.
//
//  returns graphics*      <- the graphics object.
//----------------------------------------------------------------------------------------
graphics* 
graphics::createGraphics(DlFloat64 scale, const WorldRect& wr)
{
	return NEW graphics(scale, wr);
}

//----------------------------------------------------------------------------------------
//  graphics::createGraphics
//
//      create a new graphics object.
//
//  StrInputStream& inp    -> the input stream.
//
//  returns graphics*      <- the new graphics object.
//----------------------------------------------------------------------------------------
graphics*
graphics::createGraphics(StrInputStream& inp) 
{
	return NEW graphics(inp);
}

//----------------------------------------------------------------------------------------
//  graphics::graphics                                                        constructor
//
//      construct a graphics object from the screen and world.
//
//  const DlScreenRect2d& sr   -> the screen rect.
//  const WorldRect& sr        -> the world rect.
//
//  returns nothing
//----------------------------------------------------------------------------------------
graphics::graphics(const DlScreenRect2d& sr_, const WorldRect& wr_)
	: sr(sr_)
	, wr(wr_)
	, xScale(1)
	, yScale(1)
	, currScale(72)
	, thickness(0)
	, fillColor(0xffffffff)
	, lineColor(0xff000000)
	, backColor(0xffffffff)
{
	initScale();
}

//----------------------------------------------------------------------------------------
//  graphics::graphics                                                        constructor
//
//      construct a graphics object from the scale and world.
//
//  DlFloat64 scale        -> the scale
//  const WorldRect& wr    -> the world rect.
//
//  returns nothing
//----------------------------------------------------------------------------------------
graphics::graphics(DlFloat64 scale, const WorldRect& wr_)
	: wr(wr_)
	, xScale(1)
	, yScale(1)
	, currScale(72)
	, thickness(0)
	, fillColor(0xffffffff)
	, lineColor(0xff000000)
	, backColor(0xffffffff)
{
	SetScale(scale);
}

//----------------------------------------------------------------------------------------
//  graphics::graphics                                                        constructor
//
//      construct a graphics object from the input stream.
//
//  StrInputStream& writer    -> the input stream
//
//  returns nothing
//----------------------------------------------------------------------------------------
graphics::graphics(StrInputStream& inp)
	: sr(ReadScreenRect(inp))
	, wr(inp)
	, xScale(1)
	, yScale(1)
	, currScale(72)
	, thickness(0)
	, fillColor(0xffffffff)
	, lineColor(0xff000000)
	, backColor(0xffffffff)
{
	initScale();
}

//----------------------------------------------------------------------------------------
//  graphics::Save
//
//      save the graphics object to the output stream.
//
//  StrOutputStream& writer    -> the output stream.
//
//  returns nothing
//----------------------------------------------------------------------------------------
void
graphics::Save(StrOutputStream& writer) {
	WriteScreenRect(sr, writer);
	wr.write(writer);
}

//----------------------------------------------------------------------------------------
//  graphics::updateScreenRect
//
//      update the screen rectangle from scale and world.
//
//  returns nothing
//----------------------------------------------------------------------------------------
void 
graphics::updateScreenRect()
{
	DlFloat64 scrWid = std::lround(std::abs((wr.right() - wr.left()) * xScale)) + sr.l;
#if SCREEN_FLIPPED
	DlFloat64 scrHgt = std::lround(std::abs((wr.top() - wr.bottom()) * yScale)) + sr.t;
#else
	DlFloat64 scrHgt = std::lround(std::abs((wr.top() - wr.bottom()) * yScale)) + sr.b;
#endif

	if (scrWid > (INT_MAX-1) || scrHgt > (INT_MAX-1))
    {
		DlFloat64 xFactor = scrWid / INT_MAX;
		DlFloat64 yFactor = scrHgt / INT_MAX;
        DlFloat64 factor = xFactor > yFactor ? xFactor : yFactor;
        scrWid /= factor;
        scrHgt /= factor;
	}

	sr.r = sr.l + scrWid;
#if SCREEN_FLIPPED
	sr.b = sr.t + scrHgt;
#else
	sr.t = sr.b + scrHgt;
#endif
	initScale();
}

//----------------------------------------------------------------------------------------
//  graphics::initScale
//
//      initialize the scale given the screen rect and the world rect.
//
//  returns nothing
//----------------------------------------------------------------------------------------
void 
graphics::initScale() 
{
	DlInt32	wid = sr.r - sr.l;
	DlInt32	hgt = sr.t - sr.b;
	DlFloat64	fwid = wr.right() - wr.left();
	DlFloat64	fhgt = wr.top() - wr.bottom();

	xScale = wid / fwid;
	yScale = hgt / fhgt;
}

//----------------------------------------------------------------------------------------
//  graphics::SetScale
//
//      set the scale.
//
//  DlFloat64 scale    -> the new scale
//
//  returns nothing
//----------------------------------------------------------------------------------------
void
graphics::SetScale(DlFloat64 scale)
{
	DlFloat64 scrWid = lround(std::abs((wr.right() - wr.left()) * scale));
	DlFloat64 scrHgt = lround(std::abs((wr.top() - wr.bottom()) * scale));

	if (scrWid > INT_MAX || scrHgt > INT_MAX)
		throw kScaleOutOfBounds;

	sr.r = sr.l + scrWid;
#if SCREEN_FLIPPED
	sr.b = sr.t + scrHgt;
#else
	sr.t = sr.b + scrHgt;
#endif
	initScale();
}

//----------------------------------------------------------------------------------------
//  graphics::getScaleLimits
//
//      return the maximum and minimum scale factors.
//
//  const DlScreenPoint2d& smallSize   -> the smallest screen size in points.
//  const WorldRect& wr                -> the current world rect.
//
//  returns WorldPoint                 <- the smallest and largest scale factors.
//----------------------------------------------------------------------------------------
WorldPoint
graphics::getScaleLimits(const DlScreenPoint2d& smallSize, const WorldRect& wr)
{
	WorldPoint limits;
    
    DlFloat64 smallScaleX = std::abs(smallSize.x / (wr.right() - wr.left()));
    DlFloat64 smallScaleY = std::abs(smallSize.y / (wr.top() - wr.bottom()));
    
    limits.x() = smallScaleX < smallScaleY ? smallScaleY : smallScaleX;
    
    DlFloat64 largeScaleX = std::abs((INT_MAX - 1) / (wr.right() - wr.left()));
    DlFloat64 largeScaleY = std::abs((INT_MAX - 1) / (wr.top() - wr.bottom()));

    limits.y() = largeScaleX < largeScaleY ? largeScaleX : largeScaleY;
    	
	return limits;    
}


