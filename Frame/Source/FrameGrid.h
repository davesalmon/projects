/*+
 *
 *  FrameGrid.h
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

#import <Cocoa/Cocoa.h>

class StrInputStream;
class StrOutputStream;
class WorldPoint;
class graphics;

@interface FrameGrid : NSObject
{
	DlFloat32	_xSpacing;
	DlFloat32	_ySpacing;
	bool		_gridSnap;
	bool		_gridOn;
}

+ (FrameGrid*)createWithSpacing:(DlFloat32)spc on:(BOOL)on;
+ (FrameGrid*)createFromFile:(StrInputStream&) s;

- (WorldPoint) spacing;
- (void) setSpacing: (const WorldPoint&) point;
- (bool) snapOn;
- (void) setSnapOn: (bool) on;
- (bool) gridOn;
- (void) setGridOn: (bool) on;

- (WorldPoint) snapTo: (const WorldPoint&) point;
- (void) saveToFile:(StrOutputStream&) s;

- (void) drawRect: (const NSRect&) r withGraphics: (graphics*) g;

@end
