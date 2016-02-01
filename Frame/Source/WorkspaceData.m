/*+
 *
 *  WorkspaceData.m
 *
 *  Copyright © 2007 David C. Salmon. All Rights Reserved.
 *
 *  track data for the structure workspace used to map world coordinates into window
 *  coordinates.
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

#import "WorkspaceData.h"

#include "FrameGrid.h"

@implementation WorkspaceData

//----------------------------------------------------------------------------------------
//  workspaceDataWithRect:grid:
//
//      Create an object to hold the workspace data
//
//  workspaceDataWithRect: const WorldRect& wr -> the world rectangle
//  grid: FrameGrid* grid                      -> the grid
//
//  returns WorkspaceData*                     <- the workspace data
//----------------------------------------------------------------------------------------
+ (WorkspaceData*) workspaceDataWithRect: (const WorldRect&) wr grid: (FrameGrid*)grid scale: (DlFloat64)scale
{
	return [[[WorkspaceData alloc] initWithRect: wr grid: grid scale: scale] autorelease];
}

//----------------------------------------------------------------------------------------
//  initWithRect:grid:scale:
//
//      initialize
//
//  initWithRect: const WorldRect& wr  -> 
//  grid: FrameGrid* grid              -> 
//  scale: DlFloat64 scale             -> 
//
//  returns id                         <- 
//----------------------------------------------------------------------------------------
- (id) initWithRect: (const WorldRect&) wr grid: (FrameGrid*)grid scale: (DlFloat64)scale
{
	_worldRect = wr;
    _gridSpacing = [grid spacing];
    _gridVisible = [grid gridOn];
    _gridSnapOn = [grid snapOn];
    _scale = scale;
    return self;
}

//----------------------------------------------------------------------------------------
//  worldRect
//
//      return the world rect.
//
//  returns const WorldRect&   <- 
//----------------------------------------------------------------------------------------
- (const WorldRect&) worldRect
{
	return _worldRect;
}

//----------------------------------------------------------------------------------------
//  gridSpacing
//
//      return the grid spacing.
//
//  returns const WorldPoint&  <- 
//----------------------------------------------------------------------------------------
- (const WorldPoint&) gridSpacing
{
	return _gridSpacing;
}

//----------------------------------------------------------------------------------------
//  gridVisible
//
//      return true if the grid is visible.
//
//  returns bool   <- 
//----------------------------------------------------------------------------------------
- (bool) gridVisible
{
	return _gridVisible;
}

//----------------------------------------------------------------------------------------
//  gridSnapOn
//
//      return true if snap is on.
//
//  returns bool   <- 
//----------------------------------------------------------------------------------------
- (bool) gridSnapOn
{
	return _gridSnapOn;
}

//----------------------------------------------------------------------------------------
//  scale
//
//      return the current scale.
//
//  returns DlFloat64  <- 
//----------------------------------------------------------------------------------------
- (DlFloat64) scale
{
	return _scale;
}

//----------------------------------------------------------------------------------------
//  setWorldRect:
//
//      specify a new world rect.
//
//  setWorldRect: const WorldRect& wr  -> new rect
//
//  returns nothing
//----------------------------------------------------------------------------------------
- (void) setWorldRect: (const WorldRect&) wr
{
	_worldRect = wr;
}

//----------------------------------------------------------------------------------------
//  setGridSpacing:
//
//      specify new grid spacing.
//
//  setGridSpacing: const WorldPoint& spacing  -> new spacing.
//
//  returns nothing
//----------------------------------------------------------------------------------------
- (void) setGridSpacing: (const WorldPoint&) spacing
{
	_gridSpacing = spacing;
}

//----------------------------------------------------------------------------------------
//  setGridVisible:
//
//      specify grid visible.
//
//  setGridVisible: bool visible   -> new grid visible.
//
//  returns nothing
//----------------------------------------------------------------------------------------
- (void) setGridVisible: (bool) visible
{
	_gridVisible = visible;
}

//----------------------------------------------------------------------------------------
//  setGridSnapOn:
//
//      specify snap on.
//
//  setGridSnapOn: bool on -> true if on.
//
//  returns nothing
//----------------------------------------------------------------------------------------
- (void) setGridSnapOn: (bool) on
{
	_gridSnapOn = on;
}

//----------------------------------------------------------------------------------------
//  setScale:
//
//      specify a new scale.
//
//  setScale: DlFloat64 scale  -> the scale
//
//  returns nothing
//----------------------------------------------------------------------------------------
- (void) setScale: (DlFloat64) scale
{
	_scale = scale;
}

@end

//eof

