/*+
 *
 *  FrameGrid.mm
 *
 *  Copyright © 2004 David C. Salmon. All Rights Reserved.
 *
 *  Coordinate grid object for frame.
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

#include "DlPlatform.h"
#import "FrameGrid.h"
#include "StrInputStream.h"
#include "StrOutputStream.h"
#include "WorldPoint.h"
#include "graphics.h"

const DlUInt32 kGridColor = 0xffdddddd;
const DlUInt32 kGridDarkColor = 0xffbbbbbb;

@implementation FrameGrid

//----------------------------------------------------------------------------------------
//  initWithSpacing:on:
//
//      init the grid with the spcified spacing and if its on.
//
//  initWithSpacing: DlFloat32 spc -> the spacing
//  on: BOOL on                    -> true for on.
//
//  returns id                     <- self
//----------------------------------------------------------------------------------------
- (id) initWithSpacing:(DlFloat32)spc on:(BOOL)on
{
//	[super init];
	
	if (spc < 0)
		spc = 1;
	
	_xSpacing = spc;
	_ySpacing = spc;
	_gridSnap = on;
	_gridOn = on;
	return self;
}

//----------------------------------------------------------------------------------------
//  initFromFile:
//
//      init the grid from the specified stream.
//
//  initFromFile: StrInputStream& s    -> the stream
//
//  returns id                         <- self
//----------------------------------------------------------------------------------------
- (id) initFromFile:(StrInputStream&) s
{
//	[super init];
	_xSpacing = s.GetDouble();
	_ySpacing = s.GetDouble();
	_gridSnap = s.GetBool();
	_gridOn = s.GetBool();
	return self;
}

//----------------------------------------------------------------------------------------
//  createWithSpacing:on:
//
//      create a grid object with the specified values.
//
//  createWithSpacing: DlFloat32 spc   -> the spacing
//  on: BOOL on                        -> true for on
//
//  returns FrameGrid*                 <- the grid.
//----------------------------------------------------------------------------------------
+ (FrameGrid*)createWithSpacing:(DlFloat32)spc on:(BOOL)on
{
	return [[[FrameGrid alloc] initWithSpacing: spc on: on] autorelease];
}

//----------------------------------------------------------------------------------------
//  createFromFile:
//
//      create a grid object from the stream.
//
//  createFromFile: StrInputStream& s  -> the stream
//
//  returns FrameGrid*                 <- the grid
//----------------------------------------------------------------------------------------
+ (FrameGrid*)createFromFile:(StrInputStream&) s
{
	return [[[FrameGrid alloc] initFromFile: s] autorelease];
}

//----------------------------------------------------------------------------------------
//  snapit                                                                         static
//
//      snap val to the nearest grid point defined by spc.
//
//  DlFloat64 val      -> the original value
//  DlFloat64 spc      -> the spacing
//
//  returns DlFloat64  <- rounded number of spc units
//----------------------------------------------------------------------------------------
static DlFloat64 
snapit(DlFloat64 val, DlFloat64 spc)
{
	DlFloat64 nGrids = val / spc;
	DlFloat64 iPart = floor(nGrids);
	DlFloat64 fPart = nGrids -  iPart;
	if (fPart < 0.25)
		return iPart * spc;
	if (fPart > 0.75)
		return (iPart + 1)*spc;
	return val;
}

//----------------------------------------------------------------------------------------
//  spacing
//
//      return the current spacing
//
//  returns WorldPoint <- the spacing.
//----------------------------------------------------------------------------------------
- (WorldPoint) spacing {
	return WorldPoint(_xSpacing, _ySpacing);
}

//----------------------------------------------------------------------------------------
//  snapTo:
//
//      snap the specified world coordiate to the grid.
//
//  snapTo: const WorldPoint& point    -> the point
//
//  returns WorldPoint                 <- the snapped point.
//----------------------------------------------------------------------------------------
- (WorldPoint) snapTo: (const WorldPoint&) point
{
	if (_gridSnap)
		return WorldPoint(snapit(point.x(), _xSpacing), snapit(point.y(), _ySpacing));
	return point;
}

//----------------------------------------------------------------------------------------
//  saveToFile:
//
//      save the grid to the specified stream.
//
//  saveToFile: StrOutputStream& s -> the stream
//
//  returns nothing
//----------------------------------------------------------------------------------------
- (void) saveToFile:(StrOutputStream&) s
{
	s.PutDouble(_xSpacing);
	s.PutDouble(_ySpacing);
	s.PutBool(_gridSnap);
	s.PutBool(_gridOn);
}

//----------------------------------------------------------------------------------------
//  drawRect:withGraphics:
//
//      draw the grid in the specified rectangle.
//
//  drawRect: const NSRect& r  -> the rect
//  withGraphics: graphics* g  -> the graphics object.
//
//  returns nothing
//----------------------------------------------------------------------------------------
- (void) drawRect: (const NSRect&) r withGraphics: (graphics*) g
{
	if (_gridOn) {
        
        WorldPoint botLeft(g->GetWorldRect().bottomLeft());
		WorldPoint topRight(g->GetWorldRect().topRight());

        
//		WorldPoint botLeft(g->ToWorld(DlScreenPoint2d(r.origin.x, r.origin.y)));
//		WorldPoint topRight(g->ToWorld(DlScreenPoint2d(r.origin.x + r.size.width, 
//						r.origin.y + r.size.height)));

		g->SetThickness(0);
		g->SetLineColor(kGridColor);
		
		{
			DlInt32	topMark = floor(topRight.y() / _ySpacing);
			DlInt32	botMark = floor(botLeft.y() / _ySpacing);

			// draw horizontal marks.
			WorldPoint p1(botLeft);
			WorldPoint p2(topRight);
			p1.x() -= g->GetYScale();
			for (DlInt32 i = topMark; i >= botMark; i--) {
				p1.y() = i * _ySpacing;
				p2.y() = p1.y();
				
				if (i == 0)
					g->SetLineColor(kGridDarkColor);

				g->DrawLine(p1, p2);

				if (i == 0)
					g->SetLineColor(kGridColor);
			}

			g->SetThickness(3.0);
			p1 = botLeft;
			p2 = WorldPoint(topRight.x(), botLeft.y());
			g->DrawLine(p1, p2);

			g->SetThickness(2.0);

			p1 = WorldPoint(botLeft.x(), topRight.y());
			p2 = topRight;
			g->DrawLine(p1, p2);

			g->SetThickness(0.0);
		}

		{
			DlInt32	rightMark = floor(topRight.x() / _xSpacing);
			DlInt32	leftMark = floor(botLeft.x() / _xSpacing);
		
			WorldPoint p1(botLeft);
			WorldPoint p2(topRight);
			p1.y() -= g->GetXScale();
			for (DlInt32 i = rightMark; i >= leftMark; i--) {
				p1.x() = i * _xSpacing;
				p2.x() = p1.x();

				if (i == 0)
					g->SetLineColor(kGridDarkColor);
                
				g->DrawLine(p1, p2);

				if (i == 0)
					g->SetLineColor(kGridColor);
			}

			g->SetThickness(3.0);
			p1 = botLeft;
			p2 = WorldPoint(botLeft.x(), topRight.y());
			g->DrawLine(p1, p2);

			g->SetThickness(2.0);

			p1 = WorldPoint(topRight.x(), botLeft.y());
			p2 = topRight;
			g->DrawLine(p1, p2);

			g->SetThickness(0.0);

		}
	}
}

//----------------------------------------------------------------------------------------
//  setGridOn:
//
//      turn the grid on or off
//
//  setGridOn: bool on -> true for on, false for off.
//
//  returns nothing
//----------------------------------------------------------------------------------------
- (void) setGridOn: (bool) on { _gridOn = on; }

//----------------------------------------------------------------------------------------
//  gridOn
//
//      return true if the grid is on
//
//  returns bool   <- true for on.
//----------------------------------------------------------------------------------------
- (bool) gridOn { return _gridOn; }

//----------------------------------------------------------------------------------------
//  setSnapOn:
//
//      turn snap on or off
//
//  setSnapOn: bool on -> true for on, false for off
//
//  returns nothing
//----------------------------------------------------------------------------------------
- (void) setSnapOn: (bool) on { _gridSnap = on; }

//----------------------------------------------------------------------------------------
//  snapOn
//
//      return snap
//
//  returns bool   <- true for on false for off. 
//----------------------------------------------------------------------------------------
- (bool) snapOn { return _gridSnap; }


//----------------------------------------------------------------------------------------
//  setSpacing:
//
//      set the grid spacing.
//
//  setSpacing: const WorldPoint& spacing  -> the spacing
//
//  returns nothing
//----------------------------------------------------------------------------------------
- (void) setSpacing: (const WorldPoint&) spacing { 
	_xSpacing = spacing.x(); 
	_ySpacing = spacing.y(); 
}

@end
