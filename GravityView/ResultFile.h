//
//  ResultFile.h
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
#import <Cocoa/Cocoa.h>

#include "GPoint.h"

@interface ResultFile : NSObject {
	NSString* _path;
	unsigned _nPoints;
	unsigned _nMasses;
	unsigned _currentReadPoint;
	FILE* _file;
	BOOL _writing;
	
	GPointListPtr _lastPoint;	// for reading
	double _lastTime;
	
	GPointListPtr _currPoint;	// for reading
	double _currTime;
	
	double _nextSaveTime;
	double _saveStep;
}

+ (ResultFile*)createForReadFrom: (NSString*) path forCount: (unsigned) nMasses;
+ (ResultFile*)createForWriteFrom: (NSString*) path forCount: (unsigned) nMasses;
+ (ResultFile*)createForAppendFrom: (NSString*) path forCount: (unsigned) nMasses;
+ (BOOL) checkValidFileFrom: (NSString*) path forCount: (unsigned) nMasses;

- (id)initForReadFrom: (NSString*) path forCount: (unsigned) nMasses;
- (id)initForWriteFrom: (NSString*) path forCount: (unsigned) nMasses;
- (id)initForAppendFrom: (NSString*) path forCount: (unsigned) nMasses;

- (BOOL) isWriting;

- (BOOL) writePoint: (const GPointList&) point atTime: (double) time;

- (BOOL) hasMorePoints;
- (BOOL) playback;

- (unsigned) count;

- (GPointListPtr) readNextPointAtTime:(double) time;

- (double) nextSaveTime;
- (void) setNextSaveTime: (double) saveTime withStep: (double) step;

@end
