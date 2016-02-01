/*+
 *
 *  FrameGraphics
 *
 *  Copyright © 2008 David C. Salmon. All Rights Reserved.
 *
 *  Implements FrameDrawing protocol. Handles drawing for frames.
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
//
//
//

#import <Cocoa/Cocoa.h>
#import "FrameDrawing.h"

#include "NodeEnumerator.h"
#include "ElementEnumerator.h"
#include "Element.h"

#include <map>

struct DrawElementInfo;
struct DrawNodeInfo;
@class FrameDocument;

const DlInt32 NodeRefreshSize = 28;

//	drawing flags
enum {
	  ShowNodes 		= (1 << 0)
	, ShowElements 		= (1 << 1)
	, ShowLoads			= (1 << 2)
	, ShowDisplacement 	= (1 << 3)
	, ShowMoment 		= (1 << 4)
	, ShowShear 		= (1 << 5)
	, ShowAxial 		= (1 << 6)
};

typedef std::map<DlUInt64, NSRect>	BoundsMap;
typedef BoundsMap::iterator			BoundsMapIter;
typedef BoundsMap::const_iterator	BoundsMapConstIter;

@interface FrameGraphics : NSObject <FrameDrawing>
{
    FrameDocument*	_frameDocument;
//    BoundsMap*		_nodeBounds;
    BoundsMap*		_elemBounds;
}

+ (NSRect)drawPath: (NSBezierPath*)path at: (NSPoint)pt withAngle: (float)angle;

- (id)initWithDocument: (FrameDocument*)doc;

//	FrameDrawing protocol methods
- (void)drawRect:(const NSRect&)rect;
- (graphics*) graphics;

//	extras
- (void)drawElems: (const NSRect&)r;
- (void)drawNodes: (const NSRect&)r;

- (void)invalNode: (Node) n;
- (void)invalElement: (Element)e;
- (void)invalNodes: (const NodeEnumerator&)nodes;
- (void)invalElems: (const ElementEnumerator&)elems;

- (void)updateElementBounds;
//- (void)invalAll;

@end

