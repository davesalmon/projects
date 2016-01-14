/*
 *  ResultQueue.cpp
 *  GravityView
 *
 *  Created by David Salmon on 12/27/08.
 *  Copyright 2008 David Salmon. All rights reserved.
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
 */

#import "ResultQueue.h"

@implementation ResultQueue

//----------------------------------------------------------------------------------------
//  createWithCapacity:
//
//      create the result queue with the specified capacity.
//
//  createWithCapacity: unsigned capacity  -> 
//
//  returns ResultQueue*                   <- 
//----------------------------------------------------------------------------------------
+ (ResultQueue*) createWithCapacity:(unsigned) capacity
{
	return [[[ResultQueue alloc] initWithCapacity: capacity] autorelease];
}

//----------------------------------------------------------------------------------------
//  initWithCapacity:
//
//      initialize the result queue with the specified capacity.
//
//  initWithCapacity: unsigned capacity    -> 
//
//  returns id                             <- 
//----------------------------------------------------------------------------------------
- (id) initWithCapacity:(unsigned) capacity
{
	self = [super init];
	if (self)
	{
		_capacity = capacity;
		_lock = [[NSLock alloc] init];
	}
	return self;

}

//----------------------------------------------------------------------------------------
//  dealloc
//
//      delete.
//
//  returns nothing
//----------------------------------------------------------------------------------------
- (void) dealloc
{
	[_lock release];
	[super dealloc];
}

//----------------------------------------------------------------------------------------
//  empty
//
//      return true if empty.
//
//  returns BOOL   <- 
//----------------------------------------------------------------------------------------
- (BOOL)empty
{
	[_lock lock];
	BOOL val = _queue.empty();
	[_lock unlock];
	return val;
}
//----------------------------------------------------------------------------------------
//  full
//
//      return true if full.
//
//  returns BOOL   <- 
//----------------------------------------------------------------------------------------
- (BOOL)full
{
	[_lock lock];
	BOOL isFull = _queue.size() == _capacity;
	[_lock unlock];
	return isFull;
}

- (size_t) size
{
	[_lock lock];
	size_t size = _queue.size();
	[_lock unlock];
	return size;
}

//----------------------------------------------------------------------------------------
//  pop
//
//      pop and return an element off the queue. The caller is expected to delete the
//		element.
//
//  returns GPointList*    <- the element.
//----------------------------------------------------------------------------------------
- (GPointListPtr) pop
{
	GPointListPtr val = _queue.front();
	_queue.pop();
	return val;
}

//----------------------------------------------------------------------------------------
//  push:
//
//      put a new element onto the queue.
//
//  push: GPointList* pts  -> the element.
//
//  returns nothing
//----------------------------------------------------------------------------------------
- (void) push: (GPointListPtr) pts
{
	_queue.push(pts);
}

@end
