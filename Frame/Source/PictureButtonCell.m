/*+
 *
 *  PictureButtonCell.m
 *
 *  Copyright © 2003 David C. Salmon. All Rights Reserved.
 *
 *  override button to create a button containing a diagram.
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

#import "PictureButtonCell.h"
#import "Fixity.h"

@implementation PictureButtonCell

//----------------------------------------------------------------------------------------
//  drawInteriorWithFrame:inView:
//
//      draw the interior of the button. This draws the node fixity diagrams over the
//		selector buttons in the toolbar.
//
//  drawInteriorWithFrame: NSRect cellFrame    -> the button frame.
//  inView: NSView * controlView               -> the button view.
//
//  returns nothing
//----------------------------------------------------------------------------------------
- (void)drawInteriorWithFrame:(NSRect)cellFrame inView:(NSView *)controlView 
{
	[super drawInteriorWithFrame: cellFrame inView:controlView];

	NSBezierPath* p = [Fixity fixityWithCode: [self tag]];
	NSBezierPath* n = [NSBezierPath bezierPathWithOvalInRect: NSMakeRect(-3,-3,6,6)];
	
	NSRect r = p ? [p bounds] : [n bounds];
				
	NSPoint cellCenter = NSMakePoint(cellFrame.origin.x + cellFrame.size.width/2.0,
						cellFrame.origin.y + cellFrame.size.height/2.0);

	// compute the offset. Add y coords since we need to invert.
	NSPoint offset = NSMakePoint(cellCenter.x - (r.origin.x + r.size.width/2.0),
						 cellCenter.y + (r.origin.y + r.size.height/2.0));
	offset.x = (int)offset.x + 0.5;
	offset.y = (int)offset.y + 0.5;
	
	NSAffineTransform* t = [NSAffineTransform transform];
	[t translateXBy: offset.x yBy: offset.y];
	[t scaleXBy:1 yBy:-1];
	
	if (p) {
		p = [t transformBezierPath: p];
		[[self isEnabled] ? [NSColor blackColor] : [NSColor grayColor] set];
		[p stroke];
	}
	
	n = [t transformBezierPath: n];
	[[NSColor whiteColor] set];
	[n fill];
	[[self isEnabled] ? [NSColor blackColor] : [NSColor grayColor] set];
	[n stroke];
}

@end
