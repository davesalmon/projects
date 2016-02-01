/*+
 *
 *  FrameEvents.h
 *
 *  Copyright © 2014 David C. Salmon. All Rights Reserved.
 *
 *  Defines a protocol for implementation of frame program events.
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

class WorldPoint;

//-------------------------------------------------------
// FrameEvents
//
//	protocol implementing the following methods:
//
//	flagsChanged -- message to inform object that the
//		keyboard flags have changed.
//
//	mouseInWindow -- message to inform object of the
//		location of the mouse in the window.
//
//	beginClick: withFlags: -- tell the object the mouse
//		button was clicked.
//
//	handleDragFrom -- message the object that the mouse
//		click was dragged across the screen.
//
//	endClick -- specify that the button was released.
//
//	performKeyEquivalend -- handle keyboard event.
//
@protocol FrameEvents  <NSObject>

- (void) flagsChanged: (unsigned int) flags;

- (bool) mouseInWindow: (NSPoint) pt;

- (bool) beginClick: (const WorldPoint&)where
		  withFlags: (unsigned int)flags;

- (bool) handleDragFrom: (const WorldPoint&)start
					 to:(const WorldPoint&)end
			  withFlags:(unsigned int)flags
				andDist: (float) maxDist;

- (void) endClick;

- (BOOL) performKeyEquivalent:(NSEvent *)theEvent;
@end
