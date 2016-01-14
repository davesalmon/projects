/*+
 *
 *  GravityView.mm
 *
 *  Copyright © 2008 David C. Salmon. All Rights Reserved.
 *
 *  The view class for gravity simulator. This class performs the simulation and displays
 *  the results.
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

#import "GravityView.h"

#import "GravityDocument.h"
#import "ObjMass.h"
#import "Simulation.h"
//#import "ResultQueue.h"
//#import "ResultFile.h"
//#import "ResultPoly.h"
//
//#include "System.h"
//#include <sys/time.h>
//#include <queue>

@implementation GravityView

//const double POINT_FACTOR = 1.0;

//----------------------------------------------------------------------------------------
//  initWithFrame:
//
//      initialize the view with the specified frame.
//
//  initWithFrame: NSRect frame    -> 
//
//  returns id                     <- 
//----------------------------------------------------------------------------------------
- (id)initWithFrame:(NSRect)frame 
{
    self = [super initWithFrame:frame];
    if (self) {
        // Initialization code here.
		//double wFactor = 20.0 / frame.size.width;
		_timer = 0;
		_system = 0;
		//_scale = 20.0;
//		_results = 0;
//		//_displayFactor = 1.0;
//		_currentTime = 0;
//		//_currentStep = 2;
//		_resultFile = 0;
		[self adjustBounds];
		
//		_curves = 0;
//		_computing = NO;
    }
    return self;
}

//----------------------------------------------------------------------------------------
//  dealloc
//
//      delete locals.
//
//  returns nothing
//----------------------------------------------------------------------------------------
- (void) dealloc
{	
//	[_mutex release];
//	if (_system != 0)
//		delete _system;
//	if (_results != 0)
//		[_results release];
//	if (_curves != 0)
//		[_curves release];

	[super dealloc];
}


//----------------------------------------------------------------------------------------
//  adjustBounds
//
//      adjust the bounds values.
//
//  returns nothing
//----------------------------------------------------------------------------------------
- (void)adjustBounds
{
	NSRect frame = [self frame];
	_origin.x = frame.size.width / 2;
	_origin.y = frame.size.height / 2;
	
	[[_doc simulation] resetCurves];
//	if (_curves)
//	{
//		[_curves clearPoints];
//	}
}

//----------------------------------------------------------------------------------------
//  setFrame:
//
//      override setFrame to track the scale data.
//
//  setFrame: NSRect frameRect -> the new frame
//
//  returns nothing
//----------------------------------------------------------------------------------------
- (void)setFrame:(NSRect)frameRect
{
	[super setFrame: frameRect];
	[self adjustBounds];
}

- (double) stepSize
{
	return _updateFrequency * [_doc displayStepSize];
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
//  toScreen:
//
//      convert the coordinate to the frame coords.
//
//  toScreen: const GPoint& p  -> the point
//
//  returns NSPoint            <- the frame point
//----------------------------------------------------------------------------------------
- (NSPoint) toScreen: (const GPoint&) p
{
	double scale = [_doc scale];
	int x = p.getX() * scale + _origin.x + 0.5;
	int y = p.getY() * scale + _origin.y + 0.5;
	return NSMakePoint(x, y);
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
	NSPoint loc = [self toScreen: p];
		
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
//  drawRect:
//
//      draw the specified rect.
//
//  drawRect: NSRect rect  -> the rect to draw.
//
//  returns nothing
//----------------------------------------------------------------------------------------
- (void)drawRect:(NSRect)rect 
{
	Simulation* s = [[_doc simulation] retain];

    if (s != 0 && ([s computing] || [s replay] || [s inReplay]))
	{
		if ([s computing])
		{
			if (![s drawNextSimulationInRect:rect])
			{
				[_timer invalidate];
				_timer = 0;
			}
			[_doc setCurrentStep: [s currentTime]];
		}
		else
		{
			if (![s drawNextReplayInRect:rect])
			{
				[_timer invalidate];
				_timer = 0;
				[_doc stopReplay];
				[_doc resetReplay];
				[_doc startReplay];
			}
		}
		
		[s release];
	}
	else
	{
		unsigned count = [_doc countMasses];
		
		GPoint cm;
		double tm = 0;
		for (int i = 0; i < count; i++)
		{
			ObjMass* m = [_doc getMass:i];
			double mm = [m mg];
			cm += [m location] * mm;
			tm += mm;
		}
		
		cm *= (1.0 / tm);
		
		for (unsigned i = 0; i < count; i++)
		{
			ObjMass* m = [_doc getMass:i];
			[self drawPoint: [m location] - cm inRect: &rect 
				atSize:[m displaySize]
				inColor:[m color]];
		}
	}
//	else 
//	{
//		GPointListPtr pts = _prevPoint;
//		if (_results)
//		{
//			//[_mutex lock];
//			if (![_results empty])
//			{
//				pts = [_results pop];
//				_prevPoint = pts;
//			}
//			else
//			{
//				fprintf(stderr, "lag\n");
//			}
//		} 
//		else if (_resultFile && [_resultFile hasMorePoints])
//		{
//			pts = [_resultFile readNextPointWithTime:_currentTime];
//			if (pts.get() != 0)
//			{
//				_prevPoint = pts;
//			}
////			else
////			{
////				[_doc sendStopSimultation];
////				delete _system;
////				_system = 0;
////				[_resultFile release];
////				_resultFile = 0;
////				return;
////			}
//		}
//
//		[_doc setCurrentStep: _currentTime];			
//		
//		GPoint cm;
//		double tm = 0;
//		for (int i = 0; i < pts->size(); i++)
//		{
//			double mm = _system->getMasses()[i];
//			cm += (*pts)[i] * mm;
//			tm += mm;
//		}
//		
//		cm *= (1.0 / tm);
//		
//		// make points relative to center of mass
//		//GPoint zPoint = (*points)[0];
//		for (int i = 0; i < pts->size(); i++)
//			(*pts)[i] -= cm;
//		
//		//fprintf(stderr, "update\n");
//		unsigned count = [_doc countMasses];
//		for (unsigned i = 0; i < count; i++)
//		{
//			ObjMass* m = [_doc getMass:i];
//			NSPoint p = [self drawPoint: (*pts)[i] inRect: &rect 
//				atSize:[m displaySize]
//				inColor:[m color]];
//			if (_curves != 0)
//			{
//				[_curves addPoint:p withLimit: [m tailLength] forCurve:i];
//				[_curves strokeCurve:i inColor:[m color]];
//			}
//		}				
//	}
}

//- (void) initSystem
//{
//	_currentTime = 0;
////	_iteration = 0;
////	[self adjustBounds];
//
//	// start the timer
//	_system = new System();
//	unsigned count = [_doc countMasses];
//	
//	// initialize the previous point.
//	_prevPoint = GPointListPtr(new GPointList(count));
//	
//	for (unsigned i = 0; i < count; i++)
//	{
//		ObjMass* m = [_doc getMass:i];
//		_system->addMass([m location],[m velocity],[m mg]);
//		(*_prevPoint)[i] = [m location];
//	}
//	
//	[_curves release];
//	_curves = 0;
//	
//	if ([_doc drawPolys] != 0)
//	{
//		_curves = [[ResultPoly createForCount: count] retain];
//	}
//}

//- (void) initResultFile: (ResultFile*) file
//{
//	if (_resultFile)
//	{
//		[_resultFile release];
//		_resultFile = 0;
//	}
//	
//	_resultFile = [file retain];
//}

- (void) startTimer: (double) interval
{
//	gettimeofday(&_lasttime, 0);

	_updateFrequency = interval;
	_timer = [NSTimer scheduledTimerWithTimeInterval:interval 
		target:self 
		selector:@selector(timerFired:) 
		userInfo:nil repeats:YES];

//	_maxdiff = 0;

	fprintf(stderr, "Started timer\n");
}

//- (void) clearSimulation
//{
//	_prevPoint = GPointListPtr();
//}

//----------------------------------------------------------------------------------------
//  startSimulation
//
//      start the simulation.
//
//  returns nothing
//----------------------------------------------------------------------------------------
//- (void) startSimulation: (ResultFile*) file
//{
//	[self initSystem];
//	[self initResultFile: file];
//	
//	_results = [[ResultQueue createWithCapacity: 10000] retain];
//	//_running = YES;
//
//	// fire off the computation thread
//	[NSThread detachNewThreadSelector:@selector(computeThread:) toTarget:self withObject:_results];
//
//	struct timespec sleepTime;
//	sleepTime.tv_sec = 1;
//	sleepTime.tv_nsec = 0;
//	nanosleep(&sleepTime, 0);
//
////	gettimeofday(&_lasttime, 0);
//
//	_timer = [NSTimer scheduledTimerWithTimeInterval:1.0/30.0 
//		target:self 
//		selector:@selector(timerFired:) 
//		userInfo:nil repeats:YES];
//
////	_maxdiff = 0;
//
//	fprintf(stderr, "Started simulation\n");
//}
//
//- (void) replay: (ResultFile*) file
//{
//	[self initSystem];
//	[self initResultFile: file];
//	_timer = [NSTimer scheduledTimerWithTimeInterval:1.0/60.0 
//			target:self 
//			selector:@selector(timerFired:) 
//			userInfo:nil repeats:YES];
//}

- (void)keyDown:(NSEvent *)theEvent
{
	[_doc keyDown: theEvent];
}

//----------------------------------------------------------------------------------------
//  computeThread:
//
//      function to compute the simulation.
//
//  computeThread: void* arg   -> ignored
//
//  returns nothing
//----------------------------------------------------------------------------------------
//- (void) computeThread: (void*) arg
//{
//	_computing = YES;
//	while([_doc running])
//	{
//		if (![_results full])
//		{
//			_currentTime += [_doc stepSize];
//			//fprintf(stderr, "point\n");
//			_system->runSimulation(_currentTime);
//			GPointListPtr points = GPointListPtr(new std::vector<GPoint>([_doc countMasses]));
//			_system->interpolateLocation(_currentTime, *points);
//
//			if (_resultFile)
//			{
//				[_resultFile writePoint: points atTime: _currentTime];
//			}
//
//			[_results push: points];			
//		}
//		else
//		{
//			//fprintf(stderr, "full\n");
//			struct timespec sleepTime;
//			sleepTime.tv_sec = 0;
//			sleepTime.tv_nsec = 1000000;
//			nanosleep(&sleepTime, 0);
//		}
//	}
//	
//	_computing = NO;
//	
////	[_results release];
////	_results = 0;
////	delete _system;
////	_system = 0;
////	
////	[_resultFile release];
////	_resultFile = 0;
//
//}

//----------------------------------------------------------------------------------------
//  timerFired:
//
//      The timer fired. Just refresh the view.
//
//  timerFired: id timer   -> the timer that fired
//
//  returns nothing
//----------------------------------------------------------------------------------------
- (void) timerFired: (id) timer 
{
	[self setNeedsDisplay: YES];
	
	if (!([_doc running] || [_doc replaying]))
	{
		fprintf(stderr, "stopping timer\n");
		
		[_timer invalidate];
		_timer = 0;
		[_doc listChanged:NO];
	}
	
}


@end
