//
//  ColorCell.m
//  GravityView
//
//  Created by David Salmon on 12/26/08.
//  Copyright 2008 David Salmon. All rights reserved.
//
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
#import "ColorCell.h"


@implementation ColorCell

//----------------------------------------------------------------------------------------
//  hasValidObjectValue
//
//      return true if the object data is valid.
//
//  returns BOOL   <- true if value is NSColor
//----------------------------------------------------------------------------------------
- (BOOL) hasValidObjectValue
{
	return [[self objectValue] isKindOfClass:[NSColor class]];
}
//----------------------------------------------------------------------------------------
//  drawInteriorWithFrame:inView:
//
//      draw the color inside the cell.
//
//  drawInteriorWithFrame: NSRect cellFrame    -> the frame to draw.
//  inView: NSView * controlView               -> the view
//
//  returns nothing
//----------------------------------------------------------------------------------------
- (void)drawInteriorWithFrame:(NSRect)cellFrame inView:(NSView *)controlView
{
	NSColor* color = [self objectValue];
	[color set];
	[NSBezierPath fillRect:cellFrame];
}

@end
