/*+
 *
 *  Simulation.mm
 *
 *  Copyright © 2015 David C. Salmon. All Rights Reserved.
 *
 *  control the simulation computation and display.
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


#import "Simulation.h"

#import "GravityDocument.h"
#import "GravityView.h"
#import "ObjMass.h"
#import "ResultQueue.h"
#import "ResultFile.h"
#import "ResultPoly.h"

#include "System.h"
#include <sys/time.h>
//#include <queue>


@implementation Simulation

//----------------------------------------------------------------------------------------
//  createWithDocument:
//
//      create a simulation for the specified date document.
//
//  createWithDocument: GravityDocument* doc   -> 
//
//  returns Simulation*                        <- 
//----------------------------------------------------------------------------------------
+ (Simulation*) createWithDocument: (GravityDocument*)doc
{
	return [[[Simulation alloc] initWithDocument:doc] autorelease];
}

//----------------------------------------------------------------------------------------
//  offsetToOrigin:
//
//      offset the location of each of the display points relative to the display origin.
//		this puts the origin of the display at the current display origin.
//
//  offsetToOrigin: const GPointListPtr& pts   -> the list of locations.
//
//  returns GPointListPtr                      <- the new points.
//----------------------------------------------------------------------------------------
- (GPointListPtr) offsetToOrigin: (const GPointListPtr&) pts
{
	GPoint cm;
	GPointListPtr curr = pts;
	if (pts.get() != 0)
	{
		GPoint cm;
		int oi = [_doc originIndex];
		if (oi >= pts->size())
			oi = -1;
		if (oi < 0)
		{
		
			double tm = 0;
			for (int i = 0; i < pts->size(); i++)
			{
				double mm = _system->getMasses()[i];
				cm += (*pts)[i] * mm;
				tm += mm;
			}
			
			cm *= (1.0 / tm);
		}
		else
		{
			cm = (*pts)[oi];
		}
		
		// make points relative to center of mass
		//GPoint zPoint = (*points)[0];
		for (int i = 0; i < pts->size(); i++)
			(*pts)[i] -= cm;
		
		_lastPoint = pts;
	}
	else
	{
		curr = _lastPoint;
	}
	
	return curr;
}

//----------------------------------------------------------------------------------------
//  updateOrigin
//
//      update the origin.
//
//  returns nothing
//----------------------------------------------------------------------------------------
- (void) updateOrigin
{
	[self resetCurves];
	[self offsetToOrigin: _lastPoint];
}

//----------------------------------------------------------------------------------------
//  initSystem
//
//      initialize the system.
//
//  returns nothing
//----------------------------------------------------------------------------------------
- (void) initSystem
{
	delete _system;
	
	// create the system
	_system = new System();
	unsigned count = [_doc countMasses];

	// initialize the previous point.
	_lastPoint = GPointListPtr(new GPointList(count));

	for (unsigned i = 0; i < count; i++)
	{
		ObjMass* m = [_doc getMass:i];
		_system->addMass([m location],[m velocity],[m mg]);
		(*_lastPoint)[i] = [m location];
	}

	[self offsetToOrigin:_lastPoint];

	[_curves release];
	_curves = 0;
	
	if ([_doc drawPolys] != 0)
	{
		_curves = [[ResultPoly createForCount: count] retain];
	}
}

//----------------------------------------------------------------------------------------
//  initWithDocument:
//
//      intialize for the document.
//
//  initWithDocument: GravityDocument* doc -> 
//
//  returns id                             <- 
//----------------------------------------------------------------------------------------
- (id) initWithDocument: (GravityDocument*)doc
{
	self = [super init];
	
	if (self)
	{
		_doc = doc;
		_view = [_doc view];
		_queue = 0;
		_file = 0;
		_curves = 0;
		_currentTime = 0;
		_replayTime = 0;
		_system = 0;
		[self initSystem];
	}
	
	return self;
}

//----------------------------------------------------------------------------------------
//  dealloc
//
//      free memoty.
//
//  returns nothing
//----------------------------------------------------------------------------------------
- (void) dealloc
{
	[_queue release];
	[_file release];
	[_curves release];
	delete _system;
	
	[super dealloc];
}

//----------------------------------------------------------------------------------------
//  computing
//
//      return true if computing.
//
//  returns BOOL   <- 
//----------------------------------------------------------------------------------------
- (BOOL) computing
{
	return _computing;
}

//----------------------------------------------------------------------------------------
//  hasSimulation
//
//      return true if there is a simulation.
//
//  returns BOOL   <- 
//----------------------------------------------------------------------------------------
- (BOOL) hasSimulation
{
	return _queue != 0;
}

//----------------------------------------------------------------------------------------
//  startComputation
//
//      start the computation.
//
//  returns nothing
//----------------------------------------------------------------------------------------
- (void) startComputation
{
	// make sure we do not go away during the compute
	[self retain];
	
	// if there is both a file and a queue and the file is writing
	//	this is a restart.
	if (_queue == 0)
	{
		[_file release]; _file = 0;
		[_queue release]; _queue = 0;
		_currentTime = 0;
		_replayTime = 0;
		[_doc setCurrentStep:_currentTime];

		if ([_doc currentPath] != 0)
		{
			_file = [[ResultFile createForWriteFrom:[_doc currentPath] forCount:[_doc countMasses]]retain];
			[_file setNextSaveTime: _currentTime withStep: [_doc saveStepSize]];
			GPointListPtr points = GPointListPtr(new std::vector<GPoint>([_doc countMasses]));
			// write out the computed points and the time.
			double t = _system->getTimeAndLocation(*points);
			[_file writePoint: *points atTime: t];
		}
		_queue = [[ResultQueue createWithCapacity:2000] retain];
		
		[self initSystem];
	}
	else
	{
		if (_file != 0)
		{
			[_file release]; _file = 0;
		}
		
		if ([self canReplay])
		{
			_file = [[ResultFile createForAppendFrom:[_doc currentPath] 
				forCount:[_doc countMasses]]retain];
			[_file setNextSaveTime: _currentTime withStep: [_doc saveStepSize]];
		}
		[_doc setCurrentStep:_currentTime];
	}
	
	// Now launch the thread
		// fire off the computation thread
	[NSThread detachNewThreadSelector:@selector(computeThread:) toTarget:self withObject:nil];

}

//----------------------------------------------------------------------------------------
//  stopComputation
//
//      stop the computation.
//
//  returns nothing
//----------------------------------------------------------------------------------------
- (void) stopComputation
{
	_computing = NO;
}

//----------------------------------------------------------------------------------------
//  replay
//
//      return true if replay is running.
//
//  returns BOOL   <- 
//----------------------------------------------------------------------------------------
- (BOOL) replay
{
	return _replay;
}

//----------------------------------------------------------------------------------------
//  canReplay
//
//      return true if there is a simulation to replay.
//
//  returns BOOL   <- 
//----------------------------------------------------------------------------------------
- (BOOL) canReplay
{
	NSString* path = [_doc currentPath];
	if (path == 0)
		return NO;
	return [ResultFile checkValidFileFrom:path forCount:[_doc countMasses]];
//	return NO;
}

//----------------------------------------------------------------------------------------
//  inReplay
//
//      return true if we are replaying.
//
//  returns BOOL   <- 
//----------------------------------------------------------------------------------------
- (BOOL) inReplay
{
	if (_file != 0)
	{
		return [_file playback];
	}
	return NO;
}

//----------------------------------------------------------------------------------------
//  resetCurves
//
//      reset the tails curves.
//
//  returns nothing
//----------------------------------------------------------------------------------------
- (void) resetCurves
{
	if (_curves != 0)
		[_curves clearPoints];
}

//----------------------------------------------------------------------------------------
//  setDrawPolys:
//
//      toggle the curves.
//
//  setDrawPolys: BOOL doDraw  -> true to draw the curves.
//
//  returns nothing
//----------------------------------------------------------------------------------------
- (void) setDrawPolys: (BOOL) doDraw
{
	if (doDraw != 0)
	{
		_curves = [[ResultPoly createForCount: [_doc countMasses]] retain];
	}
	else
	{
		ResultPoly* tmp = _curves;
		_curves = 0;
		[tmp release];
	}
}

//----------------------------------------------------------------------------------------
//  drawPoint:inRect:atSize:inColor:
//
//      draw the mass.
//
//  drawPoint: const GPoint& p -> the location
//  inRect: NSRect * r         -> the draw rect
//  atSize: int size           -> the mass screen size
//  inColor: NSColor* color    -> the color for the mass
//
//  returns nothing
//----------------------------------------------------------------------------------------
- (NSPoint)drawPoint:(const GPoint&)p inRect:(NSRect *)r atSize:(int)size inColor:(NSColor*)color
{
	NSPoint loc = [_view toScreen: p];

	NSRect mRect = NSMakeRect(loc.x - size, loc.y - size, 2* size, 2* size);
	if (r != 0 && NSIntersectsRect(*r, mRect))
	{
		[color set];
		NSBezierPath* path = [NSBezierPath bezierPathWithOvalInRect: mRect];
		[path fill];
	}
	
	return loc;
}

//----------------------------------------------------------------------------------------
//  drawNextStep:inRect:
//
//      draw the next step in the simulation.
//
//  drawNextStep: const GPointListPtr& pts -> the point list for this step
//  inRect: NSRect rect                    -> the rect to draw into.
//
//  returns nothing
//----------------------------------------------------------------------------------------
- (void) drawNextStep: (const GPointListPtr&) pts inRect:(NSRect) rect
{
	GPointListPtr curr = [self offsetToOrigin: pts];
	
	//fprintf(stderr, "update\n");
	unsigned count = [_doc countMasses];
	for (unsigned i = 0; i < count; i++)
	{
		ObjMass* m = [_doc getMass:i];
		NSPoint p = [self drawPoint: (*curr)[i] inRect: &rect 
			atSize:[m displaySize]
			inColor:[m color]];

		[_curves retain];
		if (_curves != 0)
		{
			[_curves addPoint:p withLimit: [m tailLength] forCurve:i];
			[_curves strokeCurve:i inColor:[m color]];
		}
		[_curves release];
	}				

}

//----------------------------------------------------------------------------------------
//  drawNextReplayInRect:
//
//      draw the next replay step.
//
//  drawNextReplayInRect: NSRect r -> the rect to draw in.
//
//  returns BOOL                   <- true if there is simuation.
//----------------------------------------------------------------------------------------
- (BOOL) drawNextReplayInRect:(NSRect) r
{
	if (_file == 0)
	{
		[self drawNextStep: GPointListPtr() inRect: r];
		return NO;
	}
	else
	{
		
		if (_replay)
			_replayTime += [_view stepSize];
//		_replayTime += [_doc displayStepSize];
		
		GPointListPtr p = [_file readNextPointAtTime:_replayTime];
		if (p.get() == 0)
			return NO;

		// here we read the next point from the file.
		[self drawNextStep: p inRect: r];
		
		[_doc setCurrentStep:_replayTime];
		return YES;
	}
}

//----------------------------------------------------------------------------------------
//  drawNextSimulationInRect:
//
//      Draw the next available point in the simulation. If there is no point
//		available then draw the previous.
//
//  drawNextSimulationInRect: NSRect r -> the rect to draw in
//
//  returns BOOL                       <- true if the simulation is still running.
//----------------------------------------------------------------------------------------
- (BOOL) drawNextSimulationInRect:(NSRect) r
{
	if (!_computing)
	{
		[self drawNextStep: GPointListPtr() inRect: r];
		return NO;
	}
	else if (![_queue empty])
	{
		[self drawNextStep: [_queue pop] inRect: r];
	}
	else
	{
		[self drawNextStep: GPointListPtr() inRect: r];
	}
	return YES;
}

//- (void) setStepSize: (double) step
//{
//	_stepSize = step;
//}
//
//- (double) stepSize
//{
//	return _stepSize;
//}
//
//----------------------------------------------------------------------------------------
//  currentTime
//
//      return the current simulation time.
//
//  returns double <- 
//----------------------------------------------------------------------------------------
- (double) currentTime
{
	return _currentTime;
}

//----------------------------------------------------------------------------------------
//  startReplay
//
//      start the replay
//
//  returns nothing
//----------------------------------------------------------------------------------------
- (void) startReplay
{
	if (_file == 0 || [_file isWriting])
	{
		[_file release];
		_file = [[ResultFile createForReadFrom:[_doc currentPath] forCount:[_doc countMasses]] retain];
		//_replayTime = 0;
		[self resetCurves];
	}
	_replay = YES;
}

//----------------------------------------------------------------------------------------
//  stopReplay
//
//      stop the replay.
//
//  returns nothing
//----------------------------------------------------------------------------------------
- (void) stopReplay
{
	_replay = NO;
}

//----------------------------------------------------------------------------------------
//  resetReplay
//
//      reset the replay back to the start.
//
//  returns nothing
//----------------------------------------------------------------------------------------
- (void) resetReplay
{
	[self resetCurves];
	[_file release];
	_file = [[ResultFile createForReadFrom:[_doc currentPath] forCount:[_doc countMasses]] retain];
	_replayTime = 0;
	[_doc setCurrentStep:_replayTime];
}

//----------------------------------------------------------------------------------------
//  writeToFile                                                                    static
//
//      methdo to write a data point to the file.
//
//  void *dataFile                  -> the data file
//  double t                        -> the time.
//  const std::vector<GPoint>& loc  -> the locations of all the masses at t.
//
//  returns nothing
//----------------------------------------------------------------------------------------
static void
writeToFile(void *dataFile, double t, const std::vector<GPoint>& loc)
{
	if (dataFile)
	{
		ResultFile* f = (ResultFile*)dataFile;
		if (t > [f nextSaveTime])
			[f writePoint: loc atTime: t];
	}
}

// thread for computation
//----------------------------------------------------------------------------------------
//  computeThread:
//
//      the thread method that does the actual computation.
//
//  computeThread: void* arg   -> no arg.
//
//  returns nothing
//----------------------------------------------------------------------------------------
- (void) computeThread: (void*) arg
{
	_computing = YES;
	while(_computing)
	{
		if (![_queue full])
		{
			_currentTime += [_doc saveStepSize];
			//fprintf(stderr, "point\n");
			_system->runSimulation(_currentTime, writeToFile, _file);
			GPointListPtr points = GPointListPtr(new std::vector<GPoint>([_doc countMasses]));

//			if (_file)
//			{
//				// write out the computed points and the time.
//				double t = _system->getTimeAndLocation(*points);
//				[_file writePoint: *points atTime: t];
//			}

			_system->interpolateLocation(_currentTime, *points);
			[_queue push: points];			
		}
		else
		{
			//fprintf(stderr, "full\n");
			struct timespec sleepTime;
			sleepTime.tv_sec = 0;
			sleepTime.tv_nsec = 1000000;
			nanosleep(&sleepTime, 0);
		}
	}
	
//	[self resetReplay];
	
	[self release];

//	_computing = NO;
}

@end
