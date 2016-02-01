/*+
 *
 *  FrameView.mm
 *
 *  Copyright © 2003 David C. Salmon. All Rights Reserved.
 *
 *  display the main frame view.
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

#import "FrameView.h"
#import "FrameDocument.h"
#include "UnitTable.h"
#include "DlString.h"

#include "graphics.h"

static NSRect constrain(const NSRect& r);

@implementation FrameView

//----------------------------------------------------------------------------------------
//  initWithFrame:
//
//      initialize the view.
//
//  initWithFrame: NSRect frameRect    -> the frame rectangle.
//
//  returns id                         <- 
//----------------------------------------------------------------------------------------
- (id)initWithFrame:(NSRect)frameRect
{
	self = [super initWithFrame:frameRect];
	_drawDelegate = nil;
	_eventDelegate = nil;
	_currToolCursor = nil;
	_selecting = false;
	_phase = 0;
	_timer = 0;
	return self;
}

- (void) setDrawDelegate: (id <FrameDrawing>) drawer andEventDelegate: (id <FrameEvents>) handler
{
	_drawDelegate = drawer;
	_eventDelegate = handler;
}

//----------------------------------------------------------------------------------------
//  awakeFromNib
//
//      post nib setup.
//
//  returns nothing
//----------------------------------------------------------------------------------------
- (void) awakeFromNib
{
	[[self window] makeFirstResponder:self];
	_timer = [NSTimer scheduledTimerWithTimeInterval: 0.10 target:self selector: @selector(timerFired:) 
		userInfo: nil repeats: YES];
}

//----------------------------------------------------------------------------------------
//  drawRect:
//
//      redraw the portion of the view in rect. First call the delegate to draw the
//		data. If there is a selection, we draw it.
//
//  drawRect: NSRect rect  -> the rectangle to draw.
//
//  returns nothing
//----------------------------------------------------------------------------------------
- (void)drawRect:(NSRect)rect
{
	[_drawDelegate drawRect:rect];
	if (_selecting) {
		NSBezierPath* path = [NSBezierPath bezierPathWithRect:constrain(_selectionRect)];
		CGFloat theArray[4];
		theArray[0] = 2;
		theArray[1] = 2;
//		[path setLineWidth:.75];
		//	for some reason, changing the phase does not cause the pattern to march.
		[path setLineDash:theArray count:2 phase: 2 * _phase + 0.5];
		[[NSColor grayColor]set];
		[path stroke];
		_phase = (_phase + 1) % 2;
		//printf("phase = %d\n", _phase);
//		[self setNeedsDisplayInRect: NSInsetRect(_selectionRect, -1, -1)];
	}
}

//----------------------------------------------------------------------------------------
//  resetCursorRects
//
//      reset cursor rectangles.
//
//  returns nothing
//----------------------------------------------------------------------------------------
- (void)resetCursorRects 
{
	if (_currToolCursor != nil) {
		NSRect cursRect = [(NSScrollView*)[self superview] documentVisibleRect];
		[self addCursorRect: cursRect cursor:_currToolCursor];
	}
}

//----------------------------------------------------------------------------------------
//  setCursor:
//
//      set the cursor.
//
//  setCursor: NSCursor* cursor    -> the cursor
//
//  returns nothing
//----------------------------------------------------------------------------------------
- (void)setCursor: (NSCursor*) cursor
{
	if (cursor != _currToolCursor) {
		//NSRect cursRect = [(NSScrollView*)[self superview] documentVisibleRect];
		[[self window] invalidateCursorRectsForView: self];
		_currToolCursor = cursor;
	}
}

//----------------------------------------------------------------------------------------
//  timerFired:
//
//      The timer fired. Handle periodic tasks for the window. This just updates the
//		coordinate box in the tool window.
//
//  timerFired: id timer   -> the timer that fired
//
//  returns nothing
//----------------------------------------------------------------------------------------
- (void) timerFired: (id) timer 
{
	NSWindow* w = [self window];
	if ([w isMainWindow]) {
		
		NSPoint where = [NSEvent mouseLocation];
		if ([_eventDelegate mouseInWindow: where]) {
			
			where = [self convertPoint: convertScreenToBase(w, where) fromView:nil];
//			where = [self convertPoint: [w convertScreenToBase: where] fromView:nil];
			NSRect bounds = [(NSScrollView*)[self superview] documentVisibleRect];
			
            if (!NSEqualPoints(where, _lastWhere))
            {
            	_lastWhere = where;
            
                if ([self mouse: where inRect:bounds]) {
                    graphics* g = [_drawDelegate graphics];
                    WorldPoint p = g->ToWorld(where);
                    DlString xval = UnitTable::FormatValue(p.x(), UnitsLength, DlFloatFormat());
                    DlString yval = UnitTable::FormatValue(p.y(), UnitsLength, DlFloatFormat());
                    [_xCoordBox setStringValue : [NSString stringWithCString: xval.get()
																	encoding: NSUTF8StringEncoding]];
                    [_yCoordBox setStringValue : [NSString stringWithCString: yval.get()
																	encoding: NSUTF8StringEncoding]];
                    return;
                }
            }
            else
            {
            	return;
            }
		}
		
		if ([[_xCoordBox stringValue] length] != 0) {
			[_xCoordBox setStringValue:@""];
			[_yCoordBox setStringValue:@""];
		}
	}
}

//----------------------------------------------------------------------------------------
//  stopTimer
//
//      stop the timer
//
//  returns nothing
//----------------------------------------------------------------------------------------
- (void) stopTimer 
{
	if (_timer)
		[_timer invalidate];
	_timer = 0;
}

//----------------------------------------------------------------------------------------
//  acceptsFirstResponder
//
//      set accepts first responder.
//
//  returns BOOL   <- returns YES
//----------------------------------------------------------------------------------------
- (BOOL) acceptsFirstResponder
{
	return YES;
}

//----------------------------------------------------------------------------------------
//  flagsChanged:
//
//      tells us when the keyboard modifier flags have changed. We inform our event
//		delegate that the flags have changed.
//
//  flagsChanged: NSEvent * theEvent   -> the event
//
//  returns nothing
//----------------------------------------------------------------------------------------
- (void)flagsChanged:(NSEvent *)theEvent {
	if ([_eventDelegate mouseInWindow: [NSEvent mouseLocation]]) {
		[_eventDelegate flagsChanged: [theEvent modifierFlags]];
	} else {
		[_eventDelegate flagsChanged: 0];
	}
}

//----------------------------------------------------------------------------------------
//  performKeyEquivalent:
//
//      perform the specified key equivalent event. Pass to event delegate.
//
//  performKeyEquivalent: NSEvent * theEvent   -> the event
//
//  returns BOOL                               <- true if we did it.
//----------------------------------------------------------------------------------------
- (BOOL)performKeyEquivalent:(NSEvent *)theEvent 
{
	return [_eventDelegate performKeyEquivalent: theEvent];
}

//----------------------------------------------------------------------------------------
//  mouseClicked:withFlags:
//
//      handle clicks.
//
//  mouseClicked: NSPoint location         -> click location.
//  withFlags: unsigned int modifierFlags  -> modifier flags
//
//  returns bool                           <- true if selection event.
//----------------------------------------------------------------------------------------
- (bool) mouseClicked: (NSPoint) location withFlags: (unsigned int) modifierFlags
{
	graphics* g = [_drawDelegate graphics];
	return [_eventDelegate beginClick:g->ToWorld(location) withFlags:modifierFlags];
}

//----------------------------------------------------------------------------------------
//  mouseDraggedFrom:to:withFlags:
//
//      handle drags.
//
//  mouseDraggedFrom: NSPoint start        -> initial location
//  to: NSPoint current                    -> current location
//  withFlags: unsigned int modifierFlags  -> modifiers.
//	andDist: float                         -> maximum distance dragged while down
//
//  returns nothing
//----------------------------------------------------------------------------------------
- (void) mouseDraggedFrom: (NSPoint)start to: (NSPoint)current
				withFlags: (unsigned int)modifierFlags andDist: (float) d
{
	graphics* g = [_drawDelegate graphics];
	bool isSelection = [_eventDelegate handleDragFrom: g->ToWorld(start) to: g->ToWorld(current) 
						withFlags: modifierFlags andDist: d];
	if (isSelection) {
		if (_selecting)
			[self setNeedsDisplayInRect: NSInsetRect(_selectionRect, -2, -2)];
	
		float l, w, b, h;
		if (start.x > current.x) {
			l = current.x;
			w = start.x - l;
		} else {
			l = start.x;
			w = current.x - l;
		}
		if (start.y > current.y) {
			b = current.y;
			h = start.y - b;
		} else {
			b = start.y;
			h = current.y - b;
		}
		_selectionRect.origin.x = l;
		_selectionRect.origin.y = b;
		_selectionRect.size.width = w;
		_selectionRect.size.height = h;
		
		[self setNeedsDisplayInRect:NSInsetRect(_selectionRect, -2, -2)];
		_selecting = true;
	}
}

//----------------------------------------------------------------------------------------
//  mouseDown:
//
//      handle mouse down event.
//
//  mouseDown: NSEvent* theEvent   -> the event
//
//  returns nothing
//----------------------------------------------------------------------------------------
- (void) mouseDown: (NSEvent*) theEvent
{
	BOOL keepOn = YES;
//    BOOL isInside = YES;
    NSPoint mouseLoc;
	NSPoint	startLoc = [self convertPoint:[theEvent locationInWindow] fromView:nil];
	float maxDist = 0;
	float currDist;
	_selecting = false;
	try {
		if (![self mouseClicked: startLoc withFlags: [theEvent modifierFlags]]) {
			[_eventDelegate endClick];
			return;
		}

	    while (keepOn) {
	        theEvent = [[self window] nextEventMatchingMask: NSLeftMouseUpMask |
	                NSLeftMouseDraggedMask];
	        mouseLoc = [self convertPoint:[theEvent locationInWindow] fromView:nil];
//	        isInside = [self mouse:mouseLoc inRect:[self bounds]];
			

	        switch ([theEvent type]) {
            case NSLeftMouseDragged:
        		currDist = fabs(mouseLoc.x - startLoc.x);
        		if (currDist > maxDist)
        			maxDist = currDist;
         		currDist = fabs(mouseLoc.y - startLoc.y);
        		if (currDist > maxDist)
        			maxDist = currDist;
                [self mouseMoved: theEvent];
                [self mouseDraggedFrom: startLoc to: mouseLoc 
                		withFlags: [theEvent modifierFlags] andDist: maxDist];
                break;
            case NSLeftMouseUp:
//                    if (isInside) 
//                    	[self mouseClicked: mouseLoc withFlags: [theEvent modifierFlags]];
//                    [self highlight:NO];
                keepOn = NO;
                break;
            default:
                /* Ignore any other kind of event. */
                break;
	        }
	    }
	    
	    if (_selecting) {
	    	[self setNeedsDisplayInRect: NSInsetRect(_selectionRect, -2, -2)];
	    	_selecting = false;
	    }
	    
	    [_eventDelegate endClick];
    } catch (...) {
    }
} 

@end

//----------------------------------------------------------------------------------------
//  constrain                                                                      static
//
//      constrain rectangle to drawing grid.
//
//  const NSRect& r    -> the rectangle to constrain to drawing coords.
//
//  returns NSRect     <- the constrained rect.
//----------------------------------------------------------------------------------------
static NSRect
constrain(const NSRect& r)
{
	NSRect cr = r;
	cr.origin.x = (int)r.origin.x + 0.5;
	cr.origin.y = (int)r.origin.y - 0.5;
	return cr;
}

