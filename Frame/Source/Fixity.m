/*+
 *
 *  Fixity.m
 *
 *  Copyright © 2004 David C. Salmon. All Rights Reserved.
 *
 *  Generate bezier paths for the various joint fixity icons.
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

#import <Cocoa/Cocoa.h>

#include "DlPlatform.h"
#import "Fixity.h"
#include "Node.h"

static NSArray* _fixityArray = nil;


@implementation Fixity

//----------------------------------------------------------------------------------------
//  drawRollerWithAngle:
//
//      Draw a roller with the specified angle.
//
//  drawRollerWithAngle: float angle   -> angle for graphic
//
//  returns NSBezierPath*              <- graphic
//----------------------------------------------------------------------------------------
+ (NSBezierPath*)drawRollerWithAngle: (float) angle
{
	NSBezierPath* path =  [NSBezierPath bezierPath];
	NSPoint startPt;
	NSPoint lineSize;
	NSRect ovalRect;
	
	ovalRect.origin.y = -9;
	ovalRect.origin.x = -4;
	ovalRect.size.width = ovalRect.size.height = 2;
	[path appendBezierPathWithOvalInRect:ovalRect];
	ovalRect.origin.x += 6;
	[path appendBezierPathWithOvalInRect:ovalRect];

	startPt.y = -7;
	startPt.x = -8;	
	[path moveToPoint: startPt];
	startPt.x += 16;
	[path lineToPoint: startPt];
	
	startPt.y = -10;
	startPt.x = -8;	
	[path moveToPoint: startPt];
	startPt.x += 16;
	[path lineToPoint: startPt];

	startPt.y = -12;
	startPt.x = -7;
	lineSize.y = 2;
	lineSize.x = 2;
	for (int i = 0; i < 5; i++) {
		[path moveToPoint: startPt];
		[path relativeLineToPoint: lineSize];
		startPt.x += 3;
	}
	 
	if (angle != 0) {
		NSAffineTransform* t = [NSAffineTransform transform];
		[t rotateByDegrees: angle];
		[path transformUsingAffineTransform: t];
	}
	return path;
}

//----------------------------------------------------------------------------------------
//  drawFixedWithAngle:
//
//      Draw a fixed edge with the specified angle.
//
//  drawFixedWithAngle: float angle    -> angle for graphic
//
//  returns NSBezierPath*              <- graphic
//----------------------------------------------------------------------------------------
+ (NSBezierPath*)drawFixedWithAngle: (float) angle
{
	NSBezierPath* path =  [NSBezierPath bezierPath];
	NSPoint startPt;
	NSPoint lineSize;

	startPt.y = -7;
	startPt.x = -8;	
	[path moveToPoint: startPt];
	startPt.x += 16;
	[path lineToPoint: startPt];

	startPt.y = -9;
	startPt.x = -7;
	lineSize.y = 2;
	lineSize.x = 2;
	for (int i = 0; i < 5; i++) {
		[path moveToPoint: startPt];
		[path relativeLineToPoint: lineSize];
		startPt.x += 3;
	}
	
	if (angle != 0) {
		NSAffineTransform* t = [NSAffineTransform transform];
		[t rotateByDegrees: angle];
		[path transformUsingAffineTransform: t];
	}

	return path;
}

//----------------------------------------------------------------------------------------
//  drawTriangle
//
//      Draw a triangle.
//
//  returns NSBezierPath*  <- graphic
//----------------------------------------------------------------------------------------
+ (NSBezierPath*)drawTriangle
{
	NSBezierPath* path =  [NSBezierPath bezierPath];
	[path moveToPoint: NSMakePoint(0,0)];
	[path relativeLineToPoint: NSMakePoint(7, -7)];
	[path relativeLineToPoint: NSMakePoint(-14, 0)];
	[path closePath];
	return path;
}

//----------------------------------------------------------------------------------------
//  drawBox
//
//      Draw a box.
//
//  returns NSBezierPath*  <- graphic
//----------------------------------------------------------------------------------------
+ (NSBezierPath*)drawBox
{
	return [NSBezierPath bezierPathWithRect:NSMakeRect(-7, -7, 14, 14)];
}

//----------------------------------------------------------------------------------------
//  drawRockerWithAngle:
//
//      Draw a rocker fixity at the specified angle.
//
//  drawRockerWithAngle: float angle   -> the angle
//
//  returns NSBezierPath*              <- graphic
//----------------------------------------------------------------------------------------
+ (NSBezierPath*)drawRockerWithAngle: (float) angle
{
	NSBezierPath* path = [self drawRollerWithAngle: 0];
	[path appendBezierPath: [self drawTriangle]];
	if (angle != 0) {
		NSAffineTransform* t = [NSAffineTransform transform];
		[t rotateByDegrees: angle];
		[path transformUsingAffineTransform: t];
	}
	return path;
//	[self drawPath: path at:n withAngle: angle];
}

//----------------------------------------------------------------------------------------
//  drawPinned
//
//      draw a pinned joint.
//
//  returns NSBezierPath*  <- graphic
//----------------------------------------------------------------------------------------
+ (NSBezierPath*)drawPinned
{
	NSBezierPath* path = [self drawFixedWithAngle: 0];
	[path appendBezierPath: [self drawTriangle]];
	return path;
	//[self drawPath: path at:n withAngle: angle];
}

//----------------------------------------------------------------------------------------
//  drawRotationFixed
//
//      draw a rotationally fixed joint.
//
//  returns NSBezierPath*  <- graphic
//----------------------------------------------------------------------------------------
+ (NSBezierPath*)drawRotationFixed
{
	NSBezierPath* path = [self drawRollerWithAngle: -90];
	[path appendBezierPath: [self drawRollerWithAngle: 0]];
	[path appendBezierPath: [self drawBox]];
	return path;
//	[self drawPath: path at: n withAngle: angle];
}

//----------------------------------------------------------------------------------------
//  drawFixedRollerX
//
//      Draw a rotationally fixed joint with rollers in the X direction.
//
//  returns NSBezierPath*  <- graphic
//----------------------------------------------------------------------------------------
+ (NSBezierPath*)drawFixedRollerX
{
	NSBezierPath* path = [self drawRollerWithAngle: -90];
//	[path appendBezierPath: [self drawFixedWithAngle: 0]];
	[path appendBezierPath: [self drawBox]];
	return path;
//	[self drawPath: path at: n withAngle: angle-90];
}

//----------------------------------------------------------------------------------------
//  drawFixedRollerY
//
//      Draw a rotationally fixed joint with rollers in the Y direction.
//
//  returns NSBezierPath*  <- graphic
//----------------------------------------------------------------------------------------
+ (NSBezierPath*)drawFixedRollerY
{
	NSBezierPath* path = [self drawRollerWithAngle: 0];
//	[path appendBezierPath: [self drawFixedWithAngle: -90]];
	[path appendBezierPath: [self drawBox]];
	return path;
//	[self drawPath: path at: n withAngle: angle-90];
}


//----------------------------------------------------------------------------------------
//  drawFixedAll
//
//      Draw completely fixed joint.
//
//  returns NSBezierPath*  <- graphic
//----------------------------------------------------------------------------------------
+ (NSBezierPath*)drawFixedAll
{
	NSBezierPath* path  = [self drawFixedWithAngle: -90];
	[path appendBezierPath: [self drawFixedWithAngle: 0]];
	[path appendBezierPath: [self drawBox]];
	return path;
}

//----------------------------------------------------------------------------------------
//  drawFixity:
//
//      Draw fixity for a node based on the restraint. Used to build the fixity
//		graphic array.
//
//  drawFixity: DlUInt32 fixity    -> the code
//
//  returns NSBezierPath*          <- the graphic.
//----------------------------------------------------------------------------------------
+ (NSBezierPath*) drawFixity:(DlUInt32)fixity
{
	NSBezierPath* path = nil;
	switch(fixity) {
	case Node::Free:
		break;
	case Node::FixX:
		path = [self drawRockerWithAngle: -90];
		break;
	case Node::FixY:
		path = [self drawRockerWithAngle: 0];
		break;
	case Node::FixTheta:
		path = [self drawRotationFixed];
		break;
	case (Node::FixX | Node::FixY):
		path = [self drawPinned];
		break;
	case (Node::FixX | Node::FixTheta):
		path = [self drawFixedRollerX];
		break;
	case (Node::FixY | Node::FixTheta):
		path = [self drawFixedRollerY];
		break;
	default: // everything fixed
		path = [self drawFixedAll];
		break;
	}
		
	return path;
}


//----------------------------------------------------------------------------------------
//  makeFixityArray
//
//      Build an array of NSBezierPath elements for each fixity condition.
//
//  returns nothing
//----------------------------------------------------------------------------------------
+ (void) makeFixityArray {
	_fixityArray = [[NSArray arrayWithObjects: 
						[Fixity drawFixity:1],
						[Fixity drawFixity:2],
						[Fixity drawFixity:3],
						[Fixity drawFixity:4],
						[Fixity drawFixity:5],
						[Fixity drawFixity:6],
						[Fixity drawFixity:7],
						nil] retain];
}

//----------------------------------------------------------------------------------------
//  fixityWithCode:
//
//      Get fixity graphic for the given code.
//
//  fixityWithCode: DlUInt32 code  -> the fixity code
//
//  returns NSBezierPath*          <- the graphic.
//----------------------------------------------------------------------------------------
+ (NSBezierPath*) fixityWithCode: (DlUInt32) code {
	if (_fixityArray == nil) {
		[Fixity makeFixityArray];
	}
	
	code &= 0x7;

	if (code == 0)
		return nil;
	
	code -= 1;
	if (code < [_fixityArray count])
		return [_fixityArray objectAtIndex: code];
	return nil;
}


@end
