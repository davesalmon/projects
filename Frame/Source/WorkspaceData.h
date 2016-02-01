/*+
 *
 *  WorkspaceData.h
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

//
//  WorkspaceData.h
//  Frame
//
//  Created by David Salmon on 7/4/07.
//  Copyright 2007 __MyCompanyName__. All rights reserved.
//

#import <Cocoa/Cocoa.h>

#include "WorldRect.h"
#include "WorldPoint.h"

@class FrameGrid;

@interface WorkspaceData : NSObject {
	WorldRect _worldRect;
    WorldPoint _gridSpacing;
    bool		_gridVisible;
    bool		_gridSnapOn;
    DlFloat64	_scale;
}

+ (WorkspaceData*) workspaceDataWithRect: (const WorldRect&) wr grid: (FrameGrid*)grid scale: (DlFloat64)scale;

- (id) initWithRect: (const WorldRect&) wr grid: (FrameGrid*)grid scale: (DlFloat64)scale;

- (const WorldRect&) worldRect;
- (const WorldPoint&) gridSpacing;
- (bool) gridVisible;
- (bool) gridSnapOn;
- (DlFloat64) scale;

- (void) setWorldRect: (const WorldRect&) wr;
- (void) setGridSpacing: (const WorldPoint&) spacing;
- (void) setGridVisible: (bool) visible;
- (void) setGridSnapOn: (bool) on;
- (void) setScale: (DlFloat64) scale;

@end

// eof
