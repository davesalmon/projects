/*+
 *
 *  quartzGraphics.mm
 *
 *  Copyright © 2016 David C. Salmon. All Rights Reserved.
 *
 *  implement graphics interface for quartz.
 *
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

//
//	graphicsQuartz.mm
//

#include "DlPlatform.h"
#include "graphics.h"

//----------------------------------------------------------------------------------------
//  makeNSColor                                                                    static
//
//      make an NSColor.
//
//  DlUInt32 color     -> packed color.
//
//  returns NSColor*   <- mac color.
//----------------------------------------------------------------------------------------
static NSColor*
makeNSColor(DlUInt32 color)
{
	
	return [NSColor colorWithDeviceRed : graphics::getRed(color)/255.0 
							green: graphics::getGreen(color)/255.0 
							blue: graphics::getBlue(color)/255.0 
							alpha: graphics::getAlpha(color)/255.0 ];
}

//----------------------------------------------------------------------------------------
//  graphics::DrawCurve
//
//      draw a curve connecting the specified points.
//
//  DlInt32 numPoint       -> number of points in curve.
//  PointEnumerator pts    -> point enumerator for world points.
//  DlUInt32 color         -> the color to draw with
//  DlFloat32 t            -> the thickness to draw with.
//  bool fill              -> true to fill the shape.
//  DlUInt32 fillColor_    -> the packed fill color (rgba).
//
//  returns MGRect         <- the enclosing rectangle.
//----------------------------------------------------------------------------------------
MGRect
graphics::DrawCurve(DlInt32 numPoint, PointEnumerator pts, DlUInt32 color,
	DlFloat32 t, bool fill, DlUInt32 fillColor_) const
{
	NSBezierPath* path = [NSBezierPath bezierPath];
	[path moveToPoint:ToMGPoint(pts.ElementAt(0))];
	for (DlInt32 i = 1; i < numPoint; i++)
		[path lineToPoint:ToMGPoint(pts.ElementAt(i))];
		
	if (fill) {
		[makeNSColor(fillColor_) set];
		[path fill];
	}

	[makeNSColor(color) set];
	[path setLineWidth : t];
	[path stroke];
	return [path bounds];
}

//----------------------------------------------------------------------------------------
//  graphics::DrawLine
//
//      draw a line.
//
//  const WorldPoint& p1   -> start
//  const WorldPoint& p2   -> end
//
//  returns MGRect         <- the enclosing rectangle.
//----------------------------------------------------------------------------------------
MGRect 
graphics::DrawLine(const WorldPoint& p1, const WorldPoint& p2) const
{
	NSBezierPath* path = [NSBezierPath bezierPath];
	[path moveToPoint:ToMGPoint(p1)];
	[path lineToPoint:ToMGPoint(p2)];

	[makeNSColor(GetLineColor()) set];
	[path setLineWidth : GetThickness()];
	[path stroke];
	return [path bounds];
}

//----------------------------------------------------------------------------------------
//  graphics::FitsOnScreen
//
//      true if on screen.
//
//  const DlScreenPoint2d& -> 
//
//  returns bool           <- always true.
//----------------------------------------------------------------------------------------
bool
graphics::FitsOnScreen(const DlScreenPoint2d& )
{
	return true;
}

//----------------------------------------------------------------------------------------
//  graphics::ClampToScreen
//
//      does nothing.
//
//  DlScreenPoint2d&   -> 
//
//  returns nothing
//----------------------------------------------------------------------------------------
void
graphics::ClampToScreen(DlScreenPoint2d&)
{
}

//----------------------------------------------------------------------------------------
//  graphics::DrawCircle
//
//      draw a circle.
//
//  const WorldPoint& center   -> the center of circle
//  DlInt32 size               -> the size of circle
//  bool fill                  -> true to fill with current fill color.
//
//  returns MGRect             <- bounds of circle.
//----------------------------------------------------------------------------------------
MGRect 
graphics::DrawCircle(const WorldPoint& center, DlInt32 size, bool fill) const
{
	NSBezierPath* path = [NSBezierPath bezierPathWithOvalInRect: ToMGRect(center, size)];
	if (fill) {
		[makeNSColor(GetFillColor()) set];
		[path fill];
	}
	[makeNSColor(GetLineColor()) set];
	[path setLineWidth:GetThickness()];
	[path stroke];
	return [path bounds];
}

//----------------------------------------------------------------------------------------
//  graphics::DrawRect
//
//      draw a square.
//
//  const WorldPoint& center   -> center of rect
//  DlInt32 size               -> size of rect
//  bool fill                  -> true to fill with current fill color.
//
//  returns MGRect             <- bounds of rect.
//----------------------------------------------------------------------------------------
MGRect 
graphics::DrawRect(const WorldPoint& center, DlInt32 size, bool fill) const
{
	NSBezierPath* path = [NSBezierPath bezierPathWithRect: ToMGRect(center, size)];
	if (fill) {
		[makeNSColor(GetFillColor()) set];
		[path fill];
	}
	[makeNSColor(GetLineColor()) set];
	[path setLineWidth:GetThickness()];
	[path stroke];
	return [path bounds];
}

//----------------------------------------------------------------------------------------
//  graphics::DrawRect
//
//      draw a rectangle.
//
//  const WorldRect& r -> the rectangle.
//  bool fill          -> true to fill with current fill color.
//
//  returns MGRect     <- bounds
//----------------------------------------------------------------------------------------
MGRect 
graphics::DrawRect(const WorldRect& r, bool fill) const
{
	NSBezierPath* path = [NSBezierPath bezierPathWithRect: ToMGRect(r)];
	if (fill) {
		[makeNSColor(GetFillColor()) set];
		[path fill];
	}
	[makeNSColor(GetLineColor()) set];
	[path setLineWidth:GetThickness()];
	[path stroke];
	return [path bounds];
}

//eof
