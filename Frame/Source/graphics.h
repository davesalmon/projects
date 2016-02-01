/*+
 *	File:		graphics.h
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

#ifndef _H_graphics
#define _H_graphics

#include "WorldPoint.h"
#include "WorldRect.h"
#include "DlGraphTypes.h"
#include "PointEnumerator.h"

const DlUInt32 kScaleOutOfBounds = 'SooB';

#if TARG_OS_MAC
#	if TARG_OS_DARWIN
#		define SCREEN_FLIPPED 0
#		ifdef TARG_API_COCOA
#			include <Foundation/NSGeometry.h>
			typedef NSPoint	MGPoint;
			typedef NSRect	MGRect;
			typedef NSSize	MGSize;
#		else
#			include <CoreGraphics/CGGeometry.h>
			typedef CGPoint	MGPoint;
			typedef CGRect	MGRect;
			typedef CGSize	MGSize;
#		endif
#	endif
#endif

enum {
	kColorBlack = 0xff000000,
	kColorWhite = 0xffffffff
};

//
//	graphics provide a simple cross-platform floating point drawing environment.
//
//	The world rectangle defines the boundaries of the floating point space.
//	The x and y scale define the mapping to and from the screen space.
//	screen.x = sr.left + xScale * (pt.x - wr.left)
//	screen.y = sr.bottom + yScale * (pt.y - wr.bottom);

class graphics {

public:

	static DlScreenRect2d 	ReadScreenRect(StrInputStream& s);
	static void 			WriteScreenRect(const DlScreenRect2d& r, StrOutputStream& s);

	static MGRect 	ReadMGRect(StrInputStream& s);
	static void 	WriteMGRect(const MGRect& r, StrOutputStream& s);
	static MGPoint 	ReadMGPoint(StrInputStream& s);
	static void 	WriteMGPoint(const MGPoint& pt, StrOutputStream& s);
	static MGSize 	ReadMGSize(StrInputStream& s);
	static void 	WriteMGSize(const MGSize& sz, StrOutputStream& s);

	static graphics* createGraphics(const DlScreenRect2d& sr, const WorldRect& wr);
	static graphics* createGraphics(DlFloat64 scale, const WorldRect& wr);
	static graphics* createGraphics(StrInputStream& inp);

	void Save(StrOutputStream& writer);

	//	select palette color for drawing
	void SetFillColor(DlUInt32 clr) { fillColor = clr; }
	void SetBackColor(DlUInt32 clr) { backColor = clr; }
	void SetLineColor(DlUInt32 clr) { lineColor = clr; }
	void SetThickness(DlFloat32 t) { thickness = t; }
	void SetScreenRect(const DlScreenRect2d& r) { sr = r; initScale(); }
	void SetWorldRect(const WorldRect& r) {wr = r; updateScreenRect(); }
	void SetScale(DlFloat64 scale);

	MGRect DrawCurve(DlInt32 numPoint, PointEnumerator pts, DlUInt32 color=kColorBlack, 
			DlFloat32 thick=1.0f, bool fill=false, DlUInt32 fillColor=kColorWhite) const;

	MGRect DrawLine(const WorldPoint& p1, const WorldPoint& p2) const;

	void DrawArrow(const WorldPoint& p1, const WorldPoint& p2, DlInt32 len, DlInt32 wid) const;

	MGRect DrawCircle(const WorldPoint& center, DlInt32 size, bool fill) const;
	MGRect DrawRect(const WorldPoint& center, DlInt32 size, bool fill) const;
	MGRect DrawRect(const WorldRect& r, bool fill) const;

	void PlotIcon(DlInt32 id, const WorldPoint& center) const;

	static bool		FitsOnScreen(const DlScreenPoint2d& pt);
	static void		ClampToScreen(DlScreenPoint2d& pt);

//
//	conversions
//
	MGPoint			ToMGPoint(const WorldPoint& wp) const;
	MGRect			ToMGRect(const WorldRect& wr) const;
	MGRect			ToMGRect(const WorldPoint& p, DlInt32 size) const;

	WorldRect		ToWorld(const MGRect& r) const;
	WorldPoint		ToWorld(const MGPoint& pt) const;
	MGRect			ScreenNSRect() const;

	DlScreenPoint2d	ToScreen(const WorldPoint& wp) const;
	WorldPoint		ToWorld(const DlScreenPoint2d& sp) const;

	DlFloat64		WorldTolerance(DlInt32 screenTol=3) const;
//
//	access
//
	//	return the page scale in dpi
	DlInt32					GetPageScale() const { return currScale; }

	DlScreenPoint2d			GetScreenOrigin() const;
	DlScreenPoint2d			GetScreenSize() const;

	DlFloat64				GetXScale() const { return xScale; }
	DlFloat64				GetYScale() const { return yScale; }

	DlUInt32				GetFillColor() const { return fillColor; }
	DlUInt32				GetLineColor() const { return lineColor; }
	DlFloat32 				GetThickness() const { return thickness; }
    const WorldRect&		GetWorldRect() const { return wr; }

//	colors
	static DlUInt32	constexpr makeColor(DlUInt8 r, DlUInt8 g, DlUInt8 b, DlUInt8 a=0xff);

	static DlUInt8 constexpr getRed(DlUInt32 color);
	static DlUInt8 constexpr getGreen(DlUInt32 color);
	static DlUInt8 constexpr getBlue(DlUInt32 color);
	static DlUInt8 constexpr getAlpha(DlUInt32 color);

	// return the minumum (x) and maximum (y) allowable scale factor for wr given that the smallest
    //	mapped screen is smallSize
	static WorldPoint getScaleLimits(const DlScreenPoint2d& smallSize, const WorldRect& wr);

private:

	graphics(const DlScreenRect2d& sr, const WorldRect& wr);
	graphics(DlFloat64 scale, const WorldRect& wr);
	graphics(StrInputStream& s);

	void	initScale();
	void	updateScreenRect();

	DlScreenRect2d	sr;
	WorldRect		wr;
	DlFloat64		xScale;
	DlFloat64		yScale;
	DlInt32			currScale;
	DlFloat32		thickness;
	DlUInt32		fillColor;
	DlUInt32		backColor;
	DlUInt32		lineColor;
};

//----------------------------------------------------------------------------------------
//  graphics::makeColor                                                            inline
//
//      combine byte elements to make a color.
//
//  DlUInt8 r                  -> red component.
//  DlUInt8 g                  -> green component.
//  DlUInt8 b                  -> blue component.
//  DlUInt8 a                  -> alpha component.
//
//  returns DlUInt32 constexpr <- 
//----------------------------------------------------------------------------------------
inline DlUInt32 constexpr
graphics::makeColor(DlUInt8 r, DlUInt8 g, DlUInt8 b, DlUInt8 a)
{
	return a << 24 | r << 16 | g << 8 | b;
}

//----------------------------------------------------------------------------------------
//  graphics::getRed                                                               inline
//
//      return the red component from a color.
//
//  DlUInt32 color    -> the color
//
//  returns DlUInt8   <- the blue component.
//----------------------------------------------------------------------------------------
inline DlUInt8 constexpr
graphics::getRed(DlUInt32 color)
{
	return DlUInt8(color >> 16);
}

//----------------------------------------------------------------------------------------
//  graphics::getGreen                                                             inline
//
//      return the green component from a color.
//
//  DlUInt32 color    -> the color
//
//  returns DlUInt8   <- the green component.
//----------------------------------------------------------------------------------------
inline DlUInt8 constexpr
graphics::getGreen(DlUInt32 color)
{
	return DlUInt8(color >> 8);
}

//----------------------------------------------------------------------------------------
//  graphics::getBlue                                                              inline
//
//      return the blue component from a color.
//
//  DlUInt32 color    -> the color
//
//  returns DlUInt8   <- the blue component.
//----------------------------------------------------------------------------------------
inline DlUInt8 constexpr
graphics::getBlue(DlUInt32 color)
{
	return DlUInt8(color);
}

//----------------------------------------------------------------------------------------
//  graphics::getAlpha                                                             inline
//
//      return the alpha component from a color.
//
//  DlUInt32 color    -> the color
//
//  returns DlUInt8   <- the alpha component.
//----------------------------------------------------------------------------------------
inline DlUInt8 constexpr
graphics::getAlpha(DlUInt32 color)
{
	return DlUInt8(color >> 24);
}

//----------------------------------------------------------------------------------------
//  graphics::ToScreen                                                             inline
//
//      convert the point to screen coordinates.
//
//  const WorldPoint& wp       -> the world point.
//
//  returns DlScreenPoint2d    <- the screen point.
//----------------------------------------------------------------------------------------
inline DlScreenPoint2d
graphics::ToScreen(const WorldPoint& wp) const 
{
#if SCREEN_FLIPPED
	return DlScreenPoint2d(
			sr.l + (wp.x() - wr.left()) * xScale,
			sr.t + (wp.y() - wr.bottom()) * yScale);
#else
	return DlScreenPoint2d(
		   sr.l + (wp.x() - wr.left()) * xScale,
		   sr.b + (wp.y() - wr.bottom()) * yScale);
#endif
}

//----------------------------------------------------------------------------------------
//  graphics::ToWorld                                                              inline
//
//      convert the point to world coordinates.
//
//  const DlScreenPoint2d& sp  -> the screen point.
//
//  returns WorldPoint         <- the world point.
//----------------------------------------------------------------------------------------
inline WorldPoint
graphics::ToWorld(const DlScreenPoint2d& sp) const
{
#if SCREEN_FLIPPED
	return WorldPoint(
			wr.left() + (sp.x - sr.l) / xScale,
			wr.bottom() + (sp.y + sr.t) / yScale);
#else
	return WorldPoint(
			wr.left() + (sp.x - sr.l) / xScale,
			wr.bottom() + (sp.y - sr.b) / yScale);
#endif
}

//----------------------------------------------------------------------------------------
//  graphics::WorldTolerance                                                       inline
//
//      return a tolerance in world coordinates given the tolerance on the screen.
//
//  const DlInt32 screenTol    -> the screen tolerance.
//
//  returns WorldPoint         <- the world tolerance.
//----------------------------------------------------------------------------------------

inline DlFloat64
graphics::WorldTolerance(DlInt32 screenTol) const
{
	return screenTol / xScale;
}

//----------------------------------------------------------------------------------------
//  graphics::GetScreenOrigin                                                      inline
//
//      return the screen origin in world coordinates.
//
//  returns DlScreenPoint2d    <- the point.
//----------------------------------------------------------------------------------------
inline DlScreenPoint2d
graphics::GetScreenOrigin() const 
{
#if SCREEN_FLIPPED
	return DlScreenPoint2d(sr.l, sr.t);
#else
	return DlScreenPoint2d(sr.l, sr.b);
#endif
}

//----------------------------------------------------------------------------------------
//  graphics::GetScreenSize                                                        inline
//
//      return the screen size in world coordinates.
//
//  returns DlScreenPoint2d    <- the point.
//----------------------------------------------------------------------------------------
inline DlScreenPoint2d
graphics::GetScreenSize() const 
{
#if SCREEN_FLIPPED
	return DlScreenPoint2d(sr.r - sr.l, sr.b - sr.t);
#else
	return DlScreenPoint2d(sr.r - sr.l, sr.t - sr.b);
#endif
}

//----------------------------------------------------------------------------------------
//  printWorldRect                                                                 inline
//
//      print the specified rectangle.
//
//  const char* message    -> the message.
//  const WorldRect& r     -> the rectangle.
//
//  returns nothing
//----------------------------------------------------------------------------------------
inline void
printWorldRect(const char* message, const WorldRect& r)
{
	printf("%s (%f, %f, %f, %f)\n", message, r.left(), r.bottom(), r.right(), r.top());
}

//----------------------------------------------------------------------------------------
//  printWorldPoint                                                                inline
//
//      print the specified rectangle.
//
//  const char* message    -> the message to precede the coordinates.
//  const WorldPoint& r    -> the rectangle.
//
//  returns nothing
//----------------------------------------------------------------------------------------
inline void
printWorldPoint(const char* message, const WorldPoint& r)
{
	printf("%s (%f, %f)\n", message, r.x(), r.y());
}

#	include "quartzgraphics.h"

#endif
//	eof
