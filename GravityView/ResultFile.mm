//
//  ResultFile.m
//  GravityView
//
//  Created by David Salmon on 12/29/08.
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
#import "ResultFile.h"

#include <sys/stat.h>


#define PointSize(n) (sizeof(double) + n * sizeof(GPoint))

@implementation ResultFile

//----------------------------------------------------------------------------------------
//  createForReadFrom:forCount:
//
//      create a result file for reading from the specified path. Returns nil if the 
//		file cannot be opened or if it contains the wrong amount of data.
//
//  createForReadFrom: NSString* path  -> the path.
//  forCount: unsigned nMasses         -> the number of masses
//
//  returns ResultFile*                <- the result file.
//----------------------------------------------------------------------------------------
+ (ResultFile*) createForReadFrom: (NSString*) path forCount: (unsigned) nMasses
{
	return [[[ResultFile alloc] initForReadFrom: path forCount: nMasses] autorelease];
}

//----------------------------------------------------------------------------------------
//  createForWriteFrom:forCount:
//
//      Create a ResultFile discarding any results already in the file.
//
//  createForWriteFrom: NSString* path -> the path to the file
//  forCount: unsigned nMasses         -> the number of masses.
//
//  returns ResultFile*                <- the file object.
//----------------------------------------------------------------------------------------
+ (ResultFile*) createForWriteFrom: (NSString*) path forCount: (unsigned) nMasses
{
	return [[[ResultFile alloc] initForWriteFrom: path forCount: nMasses] autorelease];
}

//----------------------------------------------------------------------------------------
//  createForAppendFrom:forCount:
//
//      create a ResultFile object for appending using the specified path.
//
//  createForAppendFrom: NSString* path    -> the path
//  forCount: unsigned nMasses             -> the number of masses.
//
//  returns ResultFile*                    <- the file object.
//----------------------------------------------------------------------------------------
+ (ResultFile*) createForAppendFrom: (NSString*) path forCount: (unsigned) nMasses
{
	return [[[ResultFile alloc] initForAppendFrom: path forCount: nMasses] autorelease];
}

//----------------------------------------------------------------------------------------
//  checkValidFileFrom:forCount:
//
//      check the specified path to see if its valid.
//
//  checkValidFileFrom: NSString* path -> the path
//  forCount: unsigned nMasses         -> the number of masses
//
//  returns BOOL                       <- YES if success
//----------------------------------------------------------------------------------------
+ (BOOL) checkValidFileFrom: (NSString*) path forCount: (unsigned) nMasses
{
	NSFileManager* mgr = [NSFileManager defaultManager];
	// check if the new directory exists.
	if ([mgr fileExistsAtPath: path]) {
		ResultFile* f = [[ResultFile alloc] initForReadFrom: path forCount: nMasses];
		if (f != 0)
		{
			[f release];
			return true;
		}
	}
	return false;
}

//----------------------------------------------------------------------------------------
//  initForWriteFrom:forCount:
//
//      initialize the file for overwrite with the specified number of masses.
//
//  initForWriteFrom: NSString* path   -> the path to the file
//  forCount: unsigned nMasses         -> the number of masses
//
//  returns id                         <- self
//----------------------------------------------------------------------------------------
- (id) initForWriteFrom: (NSString*) path forCount: (unsigned) nMasses
{
	self = [super init];
	if (self)
	{
		_writing = true;
		_path = path;
		_currentReadPoint = 0;
		_nPoints = 0;
		_nMasses = nMasses;
		
		_file = fopen([path UTF8String], "wb");
		
		if (_file != 0)
		{
			int c = fwrite(&_nMasses, sizeof(_nMasses), 1, _file);
			if (c != 1)
			{
				perror("Failed to write to file");
				fclose(_file);
				_file = 0;
			}
		}
		else
		{
			perror("Failed to open file for writing.");
		}
	}
	
	if (_file == 0)
	{
		[self release];
		return 0;
	}
	
	return self;
}

//----------------------------------------------------------------------------------------
//  initForAppendFrom:forCount:
//
//      initialize the file for append for the specified number of masses.
//
//  initForAppendFrom: NSString* path  -> the path to the file
//  forCount: unsigned nMasses         -> the number of masses
//
//  returns id                         <- self
//----------------------------------------------------------------------------------------
- (id) initForAppendFrom: (NSString*) path forCount: (unsigned) nMasses
{
	self = [super init];
	if (self)
	{
		_writing = true;
		_path = path;
		_currentReadPoint = 0;
		_nPoints = 0;
		_nMasses = nMasses;
		
		_file = fopen([path UTF8String], "ab");
		
		if (_file == 0)
		{
			perror("Failed to open file for writing.");
		}
	}
	
	if (_file == 0)
	{
		[self release];
		return 0;
	}
		
	return self;
}

//----------------------------------------------------------------------------------------
//  dealloc
//
//      deallocate the data.
//
//  returns nothing
//----------------------------------------------------------------------------------------
- (void) dealloc
{
	if (_file != 0)
	{
		fclose(_file);
	}
	[super dealloc];
}

//----------------------------------------------------------------------------------------
//  isWriting
//
//      return YES if we are writing.
//
//  returns BOOL   <- 
//----------------------------------------------------------------------------------------
-(BOOL) isWriting
{
	return _writing;
}

//----------------------------------------------------------------------------------------
//  writePoint:atTime:
//
//      write the specified point and the time.
//
//  writePoint: const GPointListPtr& point -> the point
//  atTime: double t                       -> the time
//
//  returns BOOL                           <- YES if success
//----------------------------------------------------------------------------------------
- (BOOL) writePoint: (const GPointList&) point atTime: (double) t
{
	if (_nMasses == point.size())
	{
		if (fwrite(&t, sizeof(t), 1, _file) != 1)
			return NO;
			
		for (int i = 0; i < _nMasses; i++)
		{
			if (fwrite(&point[i], sizeof(GPoint), 1, _file) != 1)
				return NO;
		}
		
		_nextSaveTime += _saveStep;
		
		return YES;
	}
	return NO;
}

//----------------------------------------------------------------------------------------
//  hasMorePoints
//
//      return true if the file contains more points to display.
//
//  returns BOOL   <- 
//----------------------------------------------------------------------------------------
- (BOOL) hasMorePoints
{
	return _currentReadPoint < _nPoints;
}

//----------------------------------------------------------------------------------------
//  playback
//
//      return true if we are in playback.
//
//  returns BOOL   <- 
//----------------------------------------------------------------------------------------
- (BOOL) playback
{
	return !_writing && _currentReadPoint > 0;
}

//----------------------------------------------------------------------------------------
//  count
//
//     return the number of points in the file.
//
//  returns unsigned   <- 
//----------------------------------------------------------------------------------------
- (unsigned) count
{
	return _nPoints;
}

- (double) nextSaveTime
{
	return _nextSaveTime;
}

- (void) setNextSaveTime: (double) saveTime withStep: (double) step
{
	_nextSaveTime = saveTime;
	_saveStep = step;
}

//----------------------------------------------------------------------------------------
//  readNextPointWithTime:
//
//      read the next point and return the time.
//
//  readNextPointWithTime: double& t   <-> the current time
//
//  returns GPointListPtr              <- the new point
//----------------------------------------------------------------------------------------
- (GPointListPtr) readNextPointWithTime: (double&) t
{
	if (fread(&t, sizeof(double), 1, _file) != 1)
		return GPointListPtr();
	
	GPointListPtr pts = GPointListPtr(new GPointList(_nMasses));
	for (int i = 0; i < _nMasses; i++)
	{
		if (fread(&(*pts)[i], sizeof(GPoint), 1, _file) != 1)
			return GPointListPtr();
	}
	
	_currentReadPoint++;
	return pts;
}

// interpolate between a and b, storing the result in c.
static void interpolate(double fract, const std::vector<GPoint>& a,
			const std::vector<GPoint>& b, std::vector<GPoint>& c)
{
	for (int i = 0; i < a.size(); i++)
    {
    	c[i] = a[i] + (b[i] - a[i]) * fract;
    }
}


- (GPointListPtr) readNextPointAtTime:(double) time
{
	while (time > _currTime)
	{
		if (![self hasMorePoints])
			return GPointListPtr();
	
		_lastTime =_currTime;
		_lastPoint = _currPoint;
		
		_currPoint = [self readNextPointWithTime:_currTime];
		
		if (_currPoint.get() == 0)
			return _currPoint;		
	}
	
	// now interpolate between the _lastTime and _currTime.
	
	if (_currTime == _lastTime)
		return _currPoint;
	
	GPointListPtr pts = GPointListPtr(new GPointList(_nMasses));
	double fract = (time - _lastTime) / (_currTime - _lastTime);
	interpolate(fract, *_lastPoint, *_currPoint, *pts);

	return pts;
}

//----------------------------------------------------------------------------------------
//  initForReadFrom:forCount:
//
//      init the file for reading.
//
//  initForReadFrom: NSString* path    -> the path to the file
//  forCount: unsigned nMasses         -> the number of masses
//
//  returns id                         <- self
//----------------------------------------------------------------------------------------
- (id) initForReadFrom: (NSString*) path forCount: (unsigned) nMasses
{
	self = [super init];
	if (self)
	{
		_writing = false;
		_path = path;
		_currentReadPoint = 0;
		_file = fopen([path UTF8String], "rb");
		if (_file != 0)
		{
			int c = fread(&_nMasses, sizeof(_nMasses), 1, _file);
			if (c != 1)
			{
				perror("failed to read from file");
				fclose(_file);
				_file = 0;
			}
			else if (_nMasses != nMasses)
			{
				fprintf(stderr, "number of masses does not match\n");
				fclose(_file);
				_file = 0;
			}
			else
			{
				struct stat statbuf;
				if (stat([path UTF8String], &statbuf) == 0)
				{
					off_t fSize = statbuf.st_size;
					double ptCount = (fSize - sizeof(_nMasses)) / PointSize(_nMasses);
					if (floor(ptCount) != ptCount)
					{
						perror("invalid point count");
						_file = 0;
					}
					else
					{
						_nPoints = ptCount;
					}
				}
				else
				{
					perror("failed to stat file");
				}
			}
		}
		else
		{
			perror("failed to open file for read");
		}
	}
	
	if (_file == 0)
	{
		[self release];
		return 0;
	}
	else
	{
		_lastPoint = [self readNextPointWithTime: _lastTime];
		_currPoint = [self readNextPointWithTime: _currTime];
	}
	
	return self;
}


@end
