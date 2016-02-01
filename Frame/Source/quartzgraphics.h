/*+
 *
 *  quartzgraphics.h
 *
 *  Copyright © 2015 David C. Salmon. All Rights Reserved.
 *
 *  inline operations to implement graphics methods for Quartz.
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

//----------------------------------------------------------------------------------------
//  convertScreenToBase                                                            inline
//
//      convert from screen coordinates to window coordinates.
//
//  NSWindow* w            -> the window.
//  const NSPoint& where   -> the point.
//
//  returns NSPoint        <- the point in window coords.
//----------------------------------------------------------------------------------------
inline NSPoint
convertScreenToBase(NSWindow* w, const NSPoint& where) {
	
	return [w convertRectFromScreen: NSMakeRect(where.x, where.y, 1, 1)].origin;
}

//----------------------------------------------------------------------------------------
//  graphics::ToMGPoint                                                            inline
//
//      convert world to mac.
//
//  const WorldPoint& wp   -> world point
//
//  returns MGPoint        <- mac graphics point (NSPoint/CGPoint)
//----------------------------------------------------------------------------------------
inline MGPoint
graphics::ToMGPoint(const WorldPoint& wp) const
{
	DlScreenPoint2d sp = ToScreen(wp);
	MGPoint nsp;
	nsp.x = sp.x + 0.5;
	nsp.y = sp.y - 0.5;
	return nsp;
}

//----------------------------------------------------------------------------------------
//  graphics::ToWorld                                                              inline
//
//      convet to mac to world coordinates.
//
//  const MGPoint& pt  -> mac graphics point (NSPoint/CGPoint)
//
//  returns WorldPoint <- world point
//----------------------------------------------------------------------------------------
inline WorldPoint
graphics::ToWorld(const MGPoint& pt) const
{
	return ToWorld(DlScreenPoint2d(pt.x, pt.y));
}

//----------------------------------------------------------------------------------------
//  graphics::ToMGRect                                                             inline
//
//      convert to world to mac coordinates.
//
//  const WorldRect& wr_   -> world rect.
//
//  returns MGRect         <- mac graphics rect (NSRect/CGRect)
//----------------------------------------------------------------------------------------
inline MGRect
graphics::ToMGRect(const WorldRect& wr_) const
{
	MGRect r;
	r.origin = ToMGPoint(wr_.bottomLeft());
	MGPoint topPoint = ToMGPoint(wr_.topRight());
	r.size.width = topPoint.x - r.origin.x;
	r.size.height = topPoint.y  - r.origin.y;
	return r;
}

//----------------------------------------------------------------------------------------
//  graphics::ToWorld                                                              inline
//
//      convert to world coordinates.
//
//  const MGRect& r    -> mac graphics rect (NSRect/CGRect)
//
//  returns WorldRect  <- world rect.
//----------------------------------------------------------------------------------------
inline WorldRect
graphics::ToWorld(const MGRect& r) const
{
	WorldPoint	pt(ToWorld(r.origin));
	return WorldRect(pt.x(), pt.y(), 
				pt.x() + r.size.width / xScale, 
				pt.y() + r.size.height / yScale);
}

//----------------------------------------------------------------------------------------
//  graphics::ToMGRect                                                             inline
//
//      convert square at p with size to mac graphics.
//
//  const WorldPoint& p    -> world point
//  DlInt32 size           -> size
//
//  returns MGRect         <- mac graphics rect (NSRect/CGRect)
//----------------------------------------------------------------------------------------
inline MGRect
graphics::ToMGRect(const WorldPoint& p, DlInt32 size) const
{
	MGRect r;
	r.origin = ToMGPoint(p);
	r.origin.x -= size;
	r.origin.y -= size;
	r.size.width = r.size.height = 2 * size;
	return r;
}

//----------------------------------------------------------------------------------------
//  graphics::ScreenNSRect                                                         inline
//
//      return the screen rect as mac rect
//
//  returns MGRect <- 
//----------------------------------------------------------------------------------------
inline MGRect
graphics::ScreenNSRect() const {
	return NSMakeRect(sr.l, sr.b, sr.r-sr.l, sr.t-sr.b);
}

//----------------------------------------------------------------------------------------
//  colorFromNSColor                                                               inline
//
//      convert ns color to packed rgb coordinates.
//
//  const NSColor* color   -> color
//
//  returns DlUInt32       <- color
//----------------------------------------------------------------------------------------
inline DlUInt32
colorFromNSColor(const NSColor* color) {
	if ([color colorSpaceName] != NSCalibratedRGBColorSpace)
		color = [color colorUsingColorSpaceName: NSCalibratedRGBColorSpace];
	DlUInt8 r = ([color redComponent] * 255) + 0.5;
	DlUInt8 g = ([color greenComponent] * 255) + 0.5;
	DlUInt8 b = ([color blueComponent] * 255) + 0.5;
	
	return graphics::makeColor(r, g, b);
}

//----------------------------------------------------------------------------------------
//  printNSRect                                                                    inline
//
//      print rect.
//
//  const char* message    -> 
//  const NSRect& r        -> 
//
//  returns nothing
//----------------------------------------------------------------------------------------
inline void
printNSRect(const char* message, const NSRect& r)
{
	printf("%s (%f, %f, %f, %f)\n", message,
		   r.origin.x, r.origin.y,
		   r.origin.x + r.size.width, r.origin.y + r.size.height);
}

//----------------------------------------------------------------------------------------
//  printNSSize                                                                    inline
//
//      return size
//
//  const char* message    -> 
//  const NSSize& s        -> 
//
//  returns nothing
//----------------------------------------------------------------------------------------
inline void
printNSSize(const char* message, const NSSize& s)
{
	printf("%s (%f, %f)\n", message, s.width, s.height);
}
