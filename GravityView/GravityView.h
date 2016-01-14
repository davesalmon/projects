//
//  GravityView.h
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

#include <vector>
#include "GPoint.h"

@class GravityDocument;
@class ResultQueue;
@class ResultFile;
@class ResultPoly;

class System;
struct timeval;

@interface GravityView : NSView {
	IBOutlet GravityDocument* _doc;
	System*		_system;
	NSTimer*	_timer;
	double		_updateFrequency;
	
//	double		_scale;
	//double		_displayFactor;
	//double		_lineFactor;
	
	//std::vector<GPoint>* _locations;
//	ResultQueue* _results;
//	ResultFile* _resultFile;
//	GPointListPtr _prevPoint;
	
//	double _currentTime;

	NSPoint _origin;
	
//	ResultPoly* _curves;
//	BOOL _computing;
	
//	struct timeval _lasttime;
//	int32_t _maxdiff;
	
	//int32_t _iteration;
	//BOOL _running;
}

- (id)initWithFrame:(NSRect)frame;

- (NSPoint) toScreen: (const GPoint&) pt;

- (void)adjustBounds;
- (void)setFrame:(NSRect)frameRect;
- (void) drawRect:(NSRect)rect;
- (double) stepSize;
//- (void) resetCursorRects;

// start or restart the simulation
- (void) startTimer: (double) interval;
// clear the simulation
//- (void) clearSimulation;

//- (void) replay: (ResultFile*) file;


// handle keyboard events
//- (BOOL)performKeyEquivalent:(NSEvent *)theEvent;

//- (void) computeThread: (void*) arg;

@end
