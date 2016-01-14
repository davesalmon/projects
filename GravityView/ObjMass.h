//
//  ObjMass.h
//  GravityView
//
//  Created by David Salmon on 12/24/08.
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

#import <Cocoa/Cocoa.h>

#include "GPoint.h"

@interface ObjMass : NSObject <NSCoding>
{
	GPoint	_loc;
	GPoint	_vel;
	double	_mg;
	int		_displaySize;
	unsigned _tailLength;
	NSColor* _color;
}

+ (ObjMass*) create;

- (id)init;

- (double)mass;
- (void)setMass:(double) mass;

- (const GPoint&)location;
- (void)setLocation: (const GPoint&) location;
- (void)setXCoord: (double) x;
- (void)setYCoord: (double) y;

- (const GPoint&)velocity;
- (void) setVelocity: (const GPoint&) velocity;
- (void)setVXCoord: (double) x;
- (void)setVYCoord: (double) y;

- (int) tailLength;
- (void) setTailLength: (int) size;

- (int) displaySize;
- (void) setDisplaySize: (int) size;

- (NSColor*) color;
- (void)setColor: (NSColor*) color;

- (double)mg;

@end
