//
//  ResultPoly.mm
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

#import "ResultPoly.h"


@implementation ResultPoly

+ (ResultPoly*) createForCount: (unsigned) count
{
	return [[[ResultPoly alloc] initForCount:count] autorelease];
}

- (id) initForCount: (unsigned) count
{
	self = [super init];
	if (self != 0)
	{
		if (count != 0)
		{
			_count = count;
//			_limit = limit;
			_queues = new PointQueue[count];
		}
		else
		{
			_count = 0;
//			_limit = 0;
		}
	}
	return self;
}

- (void) dealloc
{
	delete [] _queues;
	[super dealloc];
}

- (void) clearPoints
{
	for (int i = 0; i < _count; i++)
	{
		PointQueue& q = _queues[i];
		q.erase(q.begin(), q.end());
	}
}

- (void) addPoint: (const NSPoint&) p withLimit: (unsigned) limit forCurve:(unsigned) curve
{
	if (limit > 0 && curve < _count)
	{
		PointQueue& q = _queues[curve];
		q.push_back(p);
		while (q.size() > limit)
			q.pop_front();
	}
}

- (void) strokeCurve: (unsigned)curve inColor: (NSColor*)color
{
	if (curve < _count)
	{
		const PointQueue& q = _queues[curve];
		if (q.size() > 0)
		{
			NSBezierPath* path = [NSBezierPath bezierPath];
			
			for (PIter j = q.begin(); j != q.end(); j++)
			{
				if (j == q.begin())
					[path moveToPoint:(*j)];
				else
					[path lineToPoint:(*j)];
			}
			[color set];
			[path stroke];
		}
	}
}


@end
