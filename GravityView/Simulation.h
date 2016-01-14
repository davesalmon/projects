//
//  Simulation.h
//  GravityView
//
//  Created by David Salmon on 12/30/08.
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

@class GravityDocument;
@class ResultQueue;
@class ResultFile;
@class ResultPoly;
@class GravityView;

class System;

const int CENTER_OF_MASS = -1;

@interface Simulation : NSObject {

//	NSString* _path;
	GravityDocument*	_doc;
	GravityView*		_view;
	
	ResultQueue*		_queue;
	ResultFile*			_file;
	ResultPoly*			_curves;
	
	System*				_system;
	
	BOOL				_computing;
	BOOL				_replay;
	
	double				_currentTime;
//	double				_stepSize;
	double				_replayTime;
		
	GPointListPtr		_lastPoint;
}

+ (Simulation*) createWithDocument: (GravityDocument*)doc;

- (id) initWithDocument: (GravityDocument*)doc;
- (void) dealloc;

- (void) updateOrigin;

- (BOOL) computing;		// true if the simulation is running.
- (BOOL) hasSimulation;	// true if there is a simulation started.
- (void) startComputation;	// start/restart the computation
- (void) stopComputation;	// pause the computation


- (BOOL) replay;		// true if the replay is running.
- (BOOL) canReplay;		// true if there is a valid replay
- (BOOL) inReplay;		// true if the replay has started.
- (void) startReplay;	// start/restart the replay
- (void) stopReplay;	// pause the replay
- (void) resetReplay;	// stop the replay

- (void) resetCurves;	// reset the curves
- (void) setDrawPolys: (BOOL) doDraw;

// return the next replay point or the same point if
//	none
- (BOOL) drawNextReplayInRect:(NSRect)r;
- (BOOL) drawNextSimulationInRect:(NSRect)r;

//- (void) setStepSize: (double) step;
//- (double) stepSize;
- (double) currentTime;

//- (void) setOrigin: (int) which;
//- (int) origin;

// thread for computation
- (void) computeThread: (void*) arg;

@end
