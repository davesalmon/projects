/*+
 *	File:		FrameView.h
 *
 *	Contains:	NSView subclass for frame.
 *
 *	Written by:	David C. Salmon
 *
 *	Copyright:	COPYRIGHT (C) 2000 By David C. Salmon.  *WORLDWIDE RIGHTS RESERVED*
 *
 *	Discussion:
 *
 *	The FrameView class acts as the NSView and the NSResponder for frame. Drawing 
 *	events are filtered and passed to the FrameDrawing delegate. Events are 
 *	filtered and passed to the FrameEvents delegate. This class handles view 
 *	independent chores like drawing selection rectangles and keeping the tool
 *	window coordinates up-to-date.
 *
 *	To Do:
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

#import "FrameDrawing.h"
#import "FrameEvents.h"

@interface FrameView : NSView
{
    IBOutlet NSTextField*	_xCoordBox;
    IBOutlet NSTextField*	_yCoordBox;
    id <FrameDrawing>		_drawDelegate;
    id <FrameEvents>		_eventDelegate;
    
    NSCursor* 	_currToolCursor;
    
    NSRect		_selectionRect;
    bool		_selecting;
    int			_phase;
    NSPoint		_lastWhere;
    NSTimer*	_timer;
}


- (void) setCursor:(NSCursor*) cursor;
- (void) setDrawDelegate: (id <FrameDrawing>) drawer andEventDelegate: (id <FrameEvents>) handler;
- (void) timerFired: (id) timer;

- (void) stopTimer;

// NSView overrides
- (id) initWithFrame:(NSRect)frameRect;
- (void) drawRect:(NSRect)rect;
- (void) resetCursorRects;

// NSResponder overrides
- (BOOL) acceptsFirstResponder;
- (void) flagsChanged:(NSEvent *)theEvent;
- (void) mouseDown: (NSEvent*) theEvent;

// internal private methods
- (BOOL) performKeyEquivalent:(NSEvent *)theEvent;
- (bool) mouseClicked: (NSPoint) location withFlags: (unsigned int) modifierFlags;
- (void) mouseDraggedFrom: (NSPoint)start to: (NSPoint)current withFlags: (unsigned int)modifierFlags andDist: (float) d;

@end

inline bool IsShiftKey(unsigned int modFlags)
{
	return (modFlags & NSShiftKeyMask) != 0;
}

inline bool IsOptionKey(unsigned int modFlags) 
{
	return (modFlags & NSAlternateKeyMask) != 0;
}