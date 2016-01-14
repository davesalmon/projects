/*+
 *
 *  ObjMass.mm
 *
 *  Copyright © 2008 David C. Salmon. All Rights Reserved.
 *
 *  class to describe a mass with location, velocity, mass, display size, and color.
 *
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
-*/

#import "ObjMass.h"

const double GG = 8.88712e-10;	// AU^3 / (Day^2 EM) where EM = earth mass

@implementation ObjMass

//----------------------------------------------------------------------------------------
//  create
//
//      create a new mass.
//
//  returns ObjMass*   <- 
//----------------------------------------------------------------------------------------
+ (ObjMass*) create
{
	return [[[ObjMass alloc] init] autorelease];
}

//----------------------------------------------------------------------------------------
//  init
//
//      initialize a default mass.
//
//  returns id <- 
//----------------------------------------------------------------------------------------
- (id)init
{
	self = [super init];
	if (self)
	{
		[self setMass: 1];
		[self setDisplaySize: 2];
		[self setColor: [NSColor blackColor]];
		[self setTailLength: 100];
	}
	return self;
}

//----------------------------------------------------------------------------------------
//  encodeWithCoder:
//
//      encode for output.
//
//  encodeWithCoder: NSCoder * encoder -> the encoder
//
//  returns nothing
//----------------------------------------------------------------------------------------
- (void)encodeWithCoder:(NSCoder *)encoder
{
//	[super encodeWithCoder: encoder];
	[encoder encodeDouble:_mg forKey:@"_mg"];

	[encoder encodeDouble:_loc.getX() forKey:@"_loc.x"];
	[encoder encodeDouble:_loc.getY() forKey:@"_loc.y"];
	
	[encoder encodeDouble:_vel.getX() forKey:@"_vel.x"];
	[encoder encodeDouble:_vel.getY() forKey:@"_vel.y"];

	[encoder encodeInt:_displaySize forKey:@"_displaySize"];
	
	[encoder encodeObject:_color forKey:@"_color"];
	[encoder encodeInt:_tailLength forKey:@"_tailLength"];
}

//----------------------------------------------------------------------------------------
//  initWithCoder:
//
//      initialize from encoding.
//
//  initWithCoder: NSCoder * encoder   -> the encoder.
//
//  returns id                         <- self.
//----------------------------------------------------------------------------------------
- (id)initWithCoder:(NSCoder *)encoder
{
	_mg = [encoder decodeDoubleForKey:@"_mg"];

	_loc.setX([encoder decodeDoubleForKey:@"_loc.x"]);
	_loc.setY([encoder decodeDoubleForKey:@"_loc.y"]);

	_vel.setX([encoder decodeDoubleForKey:@"_vel.x"]);
	_vel.setY([encoder decodeDoubleForKey:@"_vel.y"]);
	
	_displaySize = [encoder decodeIntForKey:@"_displaySize"];
	
	if ([encoder containsValueForKey:@"_color"])
	{
		_color = [[encoder decodeObjectForKey:@"_color"] retain];
	}
	
	if ([encoder containsValueForKey:@"_tailLength"])
		_tailLength = [encoder decodeIntForKey:@"_tailLength"];
	
	return self;
}

//----------------------------------------------------------------------------------------
//  dealloc
//
//      delete.
//
//  returns nothing
//----------------------------------------------------------------------------------------
- (void)dealloc
{
	[_color release];
	[super dealloc];
}

//----------------------------------------------------------------------------------------
//  mass
//
//      return the mass.
//
//  returns double <- mass.
//----------------------------------------------------------------------------------------
- (double)mass 
{ 
	return _mg / GG; 
}

//----------------------------------------------------------------------------------------
//  mg
//
//      return the mass multiplied by the gravitational constant G.
//
//  returns double <- m G
//----------------------------------------------------------------------------------------
- (double)mg 
{ 
	return _mg; 
}

//----------------------------------------------------------------------------------------
//  setMass:
//
//      set the mass.
//
//  setMass: double mass   -> the mass
//
//  returns nothing
//----------------------------------------------------------------------------------------
- (void)setMass: (double) mass 
{ 
	_mg = mass * GG; 
}

//----------------------------------------------------------------------------------------
//  location
//
//      return the initial location.
//
//  returns const GPoint&  <- the location.
//----------------------------------------------------------------------------------------
- (const GPoint&)location 
{ 
	return _loc; 
}

//----------------------------------------------------------------------------------------
//  setLocation:
//
//      set the initial location.
//
//  setLocation: const GPoint& location    -> the location.
//
//  returns nothing
//----------------------------------------------------------------------------------------
- (void)setLocation: (const GPoint&) location 
{ 
	_loc = location; 
}
//----------------------------------------------------------------------------------------
//  setXCoord:
//
//      set the X coordinate of the location.
//
//  setXCoord: double x    -> the x coordinate.
//
//  returns nothing
//----------------------------------------------------------------------------------------
- (void)setXCoord: (double) x 
{
	_loc.setX(x); 
}

//----------------------------------------------------------------------------------------
//  setYCoord:
//
//      set the Y coordiate of the location.
//
//  setYCoord: double y    -> the y coordinate.
//
//  returns nothing
//----------------------------------------------------------------------------------------
- (void)setYCoord: (double) y 
{
	_loc.setY(y); 
}

//----------------------------------------------------------------------------------------
//  velocity
//
//      return the velocity.
//
//  returns const GPoint&  <- the velocity.
//----------------------------------------------------------------------------------------
- (const GPoint&)velocity 
{
	return _vel; 
}

//----------------------------------------------------------------------------------------
//  setVelocity:
//
//      set the velocity.
//
//  setVelocity: const GPoint& velocity    -> the velocity.
//
//  returns nothing
//----------------------------------------------------------------------------------------
- (void) setVelocity: (const GPoint&) velocity 
{
	_vel = velocity;
}

//----------------------------------------------------------------------------------------
//  setVXCoord:
//
//      set the X coordinate for the velocity.
//
//  setVXCoord: double x   -> X coord of V.
//
//  returns nothing
//----------------------------------------------------------------------------------------
- (void)setVXCoord: (double) x
{ 
	_vel.setX(x); 
}

//----------------------------------------------------------------------------------------
//  setVYCoord:
//
//      set the Y coordinate for the velocity.
//
//  setVYCoord: double y   -> Y coord of V.
//
//  returns nothing
//----------------------------------------------------------------------------------------
- (void)setVYCoord: (double) y 
{ 
	_vel.setY(y); 
}

//----------------------------------------------------------------------------------------
//  displaySize
//
//      return the display size.
//
//  returns int    <- the size.
//----------------------------------------------------------------------------------------
- (int) displaySize 
{ 
	return _displaySize; 
}

//----------------------------------------------------------------------------------------
//  setDisplaySize:
//
//      set the display size.
//
//  setDisplaySize: int size   -> the size.
//
//  returns nothing
//----------------------------------------------------------------------------------------
- (void) setDisplaySize: (int) size 
{
	_displaySize = size; 
}

- (int) tailLength
{
	return _tailLength;
}

- (void) setTailLength: (int) size
{
	_tailLength = size;
}

//----------------------------------------------------------------------------------------
//  color
//
//      return the color.
//
//  returns NSColor*   <- the color.
//----------------------------------------------------------------------------------------
- (NSColor*) color
{
	return _color;
}

//----------------------------------------------------------------------------------------
//  setColor:
//
//      set the color.
//
//  setColor: NSColor* color   -> the color
//
//  returns nothing
//----------------------------------------------------------------------------------------
- (void)setColor: (NSColor*) color
{
	_color = [color retain];
}

@end
