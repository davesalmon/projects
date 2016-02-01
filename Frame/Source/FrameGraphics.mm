/*+
 *
 *  FrameGraphics.mm
 *
 *  Copyright © 2004 David C. Salmon. All Rights Reserved.
 *
 *  Draw joints and elements. We keep a map of the current bounds for elements,
 *	based on the currently selected drawing environment. This map is used to 
 * 	determine which elements actually need to be drawn.
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

#import "FrameGraphics.h"
#import "FrameDocument.h"
#import "FrameGrid.h"
#import "Fixity.h"
#import "FrameColor.h"

#include "graphics.h"
#include "FrameStructure.h"
#include "ElementLoad.h"
#include "NodeLoad.h"
#include "PointEnumerator.h"
#include "LoadCaseResults.h"

typedef struct DrawElementInfo
{
	NSRect					r;
	FrameGraphics* 			doc;
	FrameStructure*			s;
	class graphics*				graphics;
//	const DocumentMetadata*	metadata;
	
	bool					moment;
	bool					shear;
	bool					axial;
	bool					disp;
	bool					elements;
	bool					loads;
	const LoadCaseResults*	results;
	PointEnumerator			pts;
}	DrawElementInfo;

typedef struct DrawNodeInfo
{
	NSRect					r;
	FrameGraphics* 			doc;
	FrameStructure*			s;
	class graphics*				graphics;
	
	bool					nodes;
	bool					loads;
	
//	const DocumentMetadata*	metadata;
}	DrawNodeInfo;

@interface FrameGraphics(Internal)
- (NSRect)boundsForElement: (Element)e;
- (NSRect)boundsForNode: (Node)n;
- (bool)createElemInfo: (DrawElementInfo*)info withRect: (NSRect)r;
- (bool)createNodeInfo: (DrawNodeInfo*)info withRect: (NSRect)r;
- (void)drawFixity: (Node)n;
- (void)drawElement: (Element)e atIndex: (DlUInt32)index withInfo: (DrawElementInfo*)info;
- (void)drawNode: (Node)n withInfo: (DrawNodeInfo*)info;
- (void)computeElementBounds: (Element)e atIndex: (DlUInt32)index withInfo: (DrawElementInfo*)info;
@end

@interface FrameGraphics(NodeLoads)

- (void)drawLoad: (const DlFloat32[3]) vals
		andTypes: (const NodeLoadType[3]) types
			  at: (const WorldPoint&)where hilited: (bool)hilited;

- (NSBezierPath*) drawArrowWithLength: (float)length thickness: (float)thickness
					headLength: (float)headLength headWidth: (float)headWidth 
					offset: (float)offset;
- (NSBezierPath*) drawCurvedArrowFromAngle: (float)angle thickness: (float)thickness
					headLength: (float)headLength headWidth: (float)headWidth 
					radius: (float)radius;
- (NSBezierPath*) makeArrowAt: (const WorldPoint&)pt withAngle: (float)angle;
- (NSBezierPath*) makeCurvedArrowAt: (const WorldPoint&)pt clockwise: (bool)clockwise;
@end

inline DlUInt32 fillColor(DlUInt32 color) 
{
	return (color & 0x00ffffff) | 0x33000000;
}

//----------------------------------------------------------------------------------------
//  nodeKey                                                                        inline
//
//      Get the lookup key for the node.
//
//  Node n             -> 
//
//  returns DlUInt32   <- 
//----------------------------------------------------------------------------------------
inline DlUInt64 
nodeKey(Node n) 
{
	return (DlUInt64)((NodeImp*)n);
}

//----------------------------------------------------------------------------------------
//  elemKey                                                                        inline
//
//      Get the lookup key for the element.
//
//  Element e          -> 
//
//  returns DlUInt32   <- 
//----------------------------------------------------------------------------------------
inline DlUInt64 
elemKey(Element e) 
{
	return (DlUInt64)((ElementImp*)e);
}

//----------------------------------------------------------------------------------------
//  addNodeToMap                                                                   static
//
//      Add the node to the map.
//
//  Node n         -> 
//  DlInt32 index  -> 
//  void* ctx      -> 
//
//  returns nothing
//----------------------------------------------------------------------------------------
static void 
addNodeToMap(Node n, DlInt32 index, void* ctx)
{
	BoundsMap* m = (BoundsMap*)ctx;
	(*m)[nodeKey(n)] = NSMakeRect(0,0,0,0);
}

//----------------------------------------------------------------------------------------
//  addElemToMap                                                                   static
//
//      Add the element to the map.
//
//  Element e      -> 
//  DlInt32 index  -> 
//  void* ctx      -> 
//
//  returns nothing
//----------------------------------------------------------------------------------------
static void 
addElemToMap(Element e, DlInt32 index, void* ctx)
{
	BoundsMap* m = (BoundsMap*)ctx;
	(*m)[elemKey(e)] = NSMakeRect(0,0,0,0);
}

//----------------------------------------------------------------------------------------
//  doDrawElement                                                                  static
//
//      draw the element.
//
//  Element e      -> 
//  DlInt32 index  -> 
//  void* ctx      -> 
//
//  returns nothing
//----------------------------------------------------------------------------------------
static void 
doDrawElement(Element e, DlInt32 index, void* ctx)
{
	[((DrawElementInfo*)ctx)->doc drawElement:e atIndex: index withInfo:(DrawElementInfo*)ctx];
}

//----------------------------------------------------------------------------------------
//  doUpdateBounds                                                                 static
//
//      update the structure bounds.
//
//  Element e      -> 
//  DlInt32 index  -> 
//  void* ctx      -> 
//
//  returns nothing
//----------------------------------------------------------------------------------------
static void
doUpdateBounds(Element e, DlInt32 index, void* ctx) 
{
	[((DrawElementInfo*)ctx)->doc computeElementBounds:e atIndex: index withInfo:(DrawElementInfo*)ctx];
}

//----------------------------------------------------------------------------------------
//  doDrawNode                                                                     static
//
//      draw the node.
//
//  Node n         -> 
//  DlInt32 index  -> 
//  void* ctx      -> 
//
//  returns nothing
//----------------------------------------------------------------------------------------
static void 
doDrawNode(Node n, DlInt32 index, void* ctx)
{
	DrawNodeInfo* info = (DrawNodeInfo*)ctx;
	[info->doc drawNode:n withInfo:info];
}


@implementation FrameGraphics

//----------------------------------------------------------------------------------------
//  drawPath:at:withAngle:
//
//      Draw the given Bezier path at the specified angle.
//
//  drawPath: NSBezierPath* path   -> 
//  at: NSPoint pt                 -> 
//  withAngle: float angle         -> 
//
//  returns NSRect                 <- 
//----------------------------------------------------------------------------------------
+ (NSRect)drawPath: (NSBezierPath*)path at: (NSPoint)pt withAngle: (float)angle
{
	NSAffineTransform* t = [NSAffineTransform transform];
//	NSPoint nodePt = _graphics->ToMGPoint(n.GetCoords());
	if (pt.x != 0 || pt.y != 0)
		[t translateXBy: pt.x yBy: pt.y];
	if (angle != 0)
		[t rotateByDegrees: angle];
	[path setLineWidth: 0];
	path = [t transformBezierPath: path];
	[path stroke];
	return [path bounds];
}

//----------------------------------------------------------------------------------------
//  initWithDocument:
//
//      initialize for the specified document.
//
//  initWithDocument: FrameDocument* doc   -> 
//
//  returns id                             <- 
//----------------------------------------------------------------------------------------
- (id)initWithDocument: (FrameDocument*)doc
{
	self = [super init];
	_frameDocument = doc;
//	_nodeBounds = new BoundsMap;
	_elemBounds = NEW BoundsMap;
	
	FrameStructure *s = [_frameDocument structure];
	if (s) {
//		s->DoForEachNode(addNodeToMap, _nodeBounds);
		s->DoForEachElement(addElemToMap, _elemBounds);
	}
	
	return self;
}

//----------------------------------------------------------------------------------------
//  dealloc
//
//      free
//
//  returns nothing
//----------------------------------------------------------------------------------------
- (void)dealloc {
//	delete _nodeBounds;
	delete _elemBounds;
    [super dealloc];
}

//----------------------------------------------------------------------------------------
//  drawRect:
//
//      draw a rectangle.
//
//  drawRect: const NSRect& r  -> 
//
//  returns nothing
//----------------------------------------------------------------------------------------
- (void)drawRect: (const NSRect&)r
{
	[[_frameDocument grid] drawRect: r withGraphics: [_frameDocument graphics]];
	[self drawElems: r];
	[self drawNodes: r];
}

//----------------------------------------------------------------------------------------
//  graphics
//
//      return the graphics object.
//
//  returns graphics*  <- 
//----------------------------------------------------------------------------------------
- (graphics*)graphics {
	return [_frameDocument graphics];
}

//----------------------------------------------------------------------------------------
//  drawElems:
//
//      draw the elements.
//
//  drawElems: const NSRect& r -> 
//
//  returns nothing
//----------------------------------------------------------------------------------------
- (void)drawElems: (const NSRect&)r
{
	DrawElementInfo i;
	if ([self createElemInfo: &i withRect: r]) {
		i.graphics->SetLineColor(0xff000000);
		i.graphics->SetThickness(3);

		i.s->DoForEachElement(doDrawElement, &i);
	}
}

//----------------------------------------------------------------------------------------
//  drawNodes:
//
//      draw the nodes.
//
//  drawNodes: const NSRect& r -> 
//
//  returns nothing
//----------------------------------------------------------------------------------------
- (void)drawNodes: (const NSRect&)r
{
	DrawNodeInfo i;
	if ([self createNodeInfo: &i withRect:r]) {
		
		i.graphics->SetLineColor(0xff000000);
		i.graphics->SetThickness(1);

		i.s->DoForEachNode(doDrawNode, &i);
	}
}


//----------------------------------------------------------------------------------------
//  updateElementBounds
//
//      update the bounds for elements.
//
//  returns nothing
//----------------------------------------------------------------------------------------
- (void)updateElementBounds {
	DrawElementInfo i;
	_elemBounds->clear();
	if ([self createElemInfo: &i withRect: NSMakeRect(0,0,0,0)]) {
		i.s->DoForEachElement(doUpdateBounds, &i);
	}
}

//----------------------------------------------------------------------------------------
//  invalNode:
//
//      invalidate the node.
//
//  invalNode: Node n  -> 
//
//  returns nothing
//----------------------------------------------------------------------------------------
- (void)invalNode: (Node)n {
	[_frameDocument refreshRect: [self boundsForNode: n]];
}

//----------------------------------------------------------------------------------------
//  invalElement:
//
//      invalidate the element.
//
//  invalElement: Element e    -> 
//
//  returns nothing
//----------------------------------------------------------------------------------------
- (void)invalElement:(Element) e {
	[_frameDocument refreshRect: [self boundsForElement:e]];
}

//----------------------------------------------------------------------------------------
//  invalNodes:
//
//      invalidate the node list.
//
//  invalNodes: const NodeEnumerator& nodes    -> 
//
//  returns nothing
//----------------------------------------------------------------------------------------
- (void)invalNodes: (const NodeEnumerator&)nodes 
{
	nodes.Reset();
	MGRect bounds {{0, 0}, {0, 0}};
	bool first = true;
	while(nodes.HasMore()) {
		NSRect nodeBounds = [self boundsForNode: nodes.Next()];
		
		if (first) {
			bounds = nodeBounds;
			first = false;
		} else {
			bounds = NSUnionRect(bounds, nodeBounds);
		}
	}
	
	[_frameDocument refreshRect: bounds];
}

//----------------------------------------------------------------------------------------
//  invalElems:
//
//      invalidate the element list.
//
//  invalElems: const ElementEnumerator& elems -> 
//
//  returns nothing
//----------------------------------------------------------------------------------------
- (void)invalElems: (const ElementEnumerator&)elems 
{
	elems.Reset();
	while(elems.HasMore())
		[self invalElement: elems.Next()];
}

@end

@implementation FrameGraphics(NodeLoads)

//----------------------------------------------------------------------------------------
//  drawLoad:at:
//
//      draw the node load at the specified location.
//
//  drawLoad: NodeLoad load        -> 
//  at: const WorldPoint& where    -> 
//
//  returns nothing
//----------------------------------------------------------------------------------------
- (void)drawLoad: (const DlFloat32[3]) vals andTypes: (const NodeLoadType[3]) types
			  at: (const WorldPoint&)where hilited: (bool)hilited
{
	if (vals[0] != 0)
	{
		NSBezierPath* path = [self makeArrowAt: where withAngle: vals[0] > 0 ? 0 : 180];
		if (types[0] == NodeLoadIsForce) {
			[(hilited ? [NSColor blueColor] : [NSColor blackColor]) set];
			[path setLineWidth: 0];
			[path fill];
		}
		[path stroke];
	}
	
	if (vals[1] != 0)
	{
		NSBezierPath* path = [self makeArrowAt: where withAngle: vals[1] > 0 ? 90 : 270];
		if (types[1] == NodeLoadIsForce) {
			[(hilited ? [NSColor blueColor] : [NSColor blackColor]) set];
			[path setLineWidth: 0];
			[path fill];
		}
		[path stroke];
	}
	
	if (vals[2] != 0)
	{
		NSBezierPath* path = [self makeCurvedArrowAt: where clockwise: vals[2] > 0];
		if (types[2] == NodeLoadIsForce) {
			[(hilited ? [NSColor blueColor] : [NSColor blackColor]) set];
			[path setLineWidth: 0];
			[path fill];
		}
		[path stroke];
	}
}

//          .
//         |  .
// --------|    .
// --------|  .
//          .
//
//
//----------------------------------------------------------------------------------------
//  drawArrowWithLength:thickness:headLength:headWidth:offset:
//
//      draw an arrow.
//
//  drawArrowWithLength: float length  -> 
//  thickness: float thickness         -> 
//  headLength: float headLength       -> 
//  headWidth: float headWidth         -> 
//  offset: float offset               -> 
//
//  returns NSBezierPath*              <- 
//----------------------------------------------------------------------------------------
- (NSBezierPath*)drawArrowWithLength: (float)length thickness: (float)thickness
	headLength: (float)headLength headWidth: (float)headWidth offset: (float)offset
{
	NSBezierPath* path = [NSBezierPath bezierPath];
	[path moveToPoint: NSMakePoint(offset,0)];
	[path relativeLineToPoint: NSMakePoint(0, thickness/2)];
	if (length != headLength)
		[path relativeLineToPoint: NSMakePoint(length - headLength, 0)];
	[path relativeLineToPoint: NSMakePoint(0, (headWidth-thickness)/2)];
	[path relativeLineToPoint: NSMakePoint(headLength, -headWidth/2)];
	[path relativeLineToPoint: NSMakePoint(-headLength, -headWidth/2)];
	[path relativeLineToPoint: NSMakePoint(0, (headWidth-thickness)/2)];
	if (length != headLength)
		[path relativeLineToPoint: NSMakePoint(headLength - length, 0)];
	[path closePath];
	return path;
}

//----------------------------------------------------------------------------------------
//  drawCurvedArrowFromAngle:thickness:headLength:headWidth:radius:
//
//      draw a curved arrow.
//
//  drawCurvedArrowFromAngle: float angle  -> 
//  thickness: float thickness             -> 
//  headLength: float headLength           -> 
//  headWidth: float headWidth             -> 
//  radius: float radius                   -> 
//
//  returns NSBezierPath*                  <- 
//----------------------------------------------------------------------------------------
- (NSBezierPath*)drawCurvedArrowFromAngle: (float)angle thickness: (float)thickness
	headLength: (float)headLength headWidth: (float)headWidth radius: (float)radius 
{
	NSBezierPath* path = [NSBezierPath bezierPath];
	[path appendBezierPathWithArcWithCenter : NSMakePoint(0,0) radius: radius - thickness / 2
			startAngle: 90 endAngle: angle];
	[path appendBezierPathWithArcWithCenter : NSMakePoint(0,0) radius: radius + thickness / 2
			startAngle: angle endAngle: 90 clockwise: YES];
	[path relativeLineToPoint: NSMakePoint(0, (headWidth-thickness)/2)];
	[path relativeLineToPoint: NSMakePoint(headLength, -headWidth/2)];
	[path relativeLineToPoint: NSMakePoint(-headLength, -headWidth/2)];
	[path relativeLineToPoint: NSMakePoint(0, (headWidth-thickness)/2)];
	[path closePath];
	return path;
}

//----------------------------------------------------------------------------------------
//  makeArrowAt:withAngle:
//
//      make an arrow
//
//  makeArrowAt: const WorldPoint& pt  -> 
//  withAngle: float angle             -> 
//
//  returns NSBezierPath*              <- 
//----------------------------------------------------------------------------------------
- (NSBezierPath*)makeArrowAt: (const WorldPoint&)pt withAngle: (float)angle
{
	NSBezierPath* path = [self drawArrowWithLength: 14 thickness:4 headLength: 6 headWidth: 8 offset: 13];
	NSAffineTransform* t = [NSAffineTransform transform];
	NSPoint nodePt = [_frameDocument graphics]->ToMGPoint(pt);
	[t translateXBy: nodePt.x yBy: nodePt.y];
	if (angle != 0)
		[t rotateByDegrees: angle];
	[path transformUsingAffineTransform: t];
	return path;
}

//----------------------------------------------------------------------------------------
//  makeCurvedArrowAt:clockwise:
//
//      make a curved arrow.
//
//  makeCurvedArrowAt: const WorldPoint& pt    -> 
//  clockwise: bool clockwise                  -> 
//
//  returns NSBezierPath*                      <- 
//----------------------------------------------------------------------------------------
- (NSBezierPath*)makeCurvedArrowAt: (const WorldPoint&)pt clockwise: (bool)clockwise
{
	NSBezierPath* path = [self drawCurvedArrowFromAngle: 200 thickness:4 headLength: 6 headWidth: 8 
							radius: 16];
	NSAffineTransform* t = [NSAffineTransform transform];
	NSPoint nodePt = [_frameDocument graphics]->ToMGPoint(pt);
	[t translateXBy: nodePt.x yBy: nodePt.y];
	if (!clockwise)
		[t scaleXBy: -1 yBy: 1];
	[t rotateByDegrees: 45];
	[path transformUsingAffineTransform: t];
	return path;
}

@end

@implementation FrameGraphics(Internal)

//----------------------------------------------------------------------------------------
//  boundsForElement:
//
//      return the bounds for the specified element.
//
//  boundsForElement: Element e    -> 
//
//  returns NSRect                 <- 
//----------------------------------------------------------------------------------------
- (NSRect)boundsForElement: (Element)e 
{
	NSRect	bounds = NSInsetRect([_frameDocument graphics]->ToMGRect(e.GetBounds()), -2, -2);
	BoundsMapIter i = _elemBounds->find(elemKey(e));
	if (i != _elemBounds->end()) {
		if (!NSContainsRect(i->second, bounds))
			i->second = bounds;
		else
			return i->second;
	}
	return bounds;
}

//----------------------------------------------------------------------------------------
//  boundsForNode:
//
//      return the bounds for the specified node.
//
//  boundsForNode: Node n  -> 
//
//  returns NSRect         <- 
//----------------------------------------------------------------------------------------
- (NSRect)boundsForNode: (Node) n {
	return [_frameDocument graphics]->ToMGRect(n.GetCoords(), NodeRefreshSize);
}

//----------------------------------------------------------------------------------------
//  createElemInfo:withRect:
//
//      create element drawing info.
//
//  createElemInfo: DrawElementInfo* info  -> 
//  withRect: NSRect r                     -> 
//
//  returns bool                           <- 
//----------------------------------------------------------------------------------------
- (bool)createElemInfo: (DrawElementInfo*)info withRect:(NSRect)r {
	FrameStructure* s = [_frameDocument structure];
	if (s) {
		DlUInt32 flags = [_frameDocument drawingFlags];
		info->s = s;
		info->doc = self;
		info->r = r;
		info->elements = (flags & ShowElements) != 0;
		info->loads = (flags & ShowLoads) != 0;
		info->graphics = [_frameDocument graphics];
		if (s->Analyzed()) {
			info->results = s->GetResults();
			info->axial = (flags & ShowAxial) != 0;
			info->moment = (flags & ShowMoment) != 0;
			info->shear = (flags & ShowShear) != 0;
			info->disp = (flags & ShowDisplacement) != 0;
		} else {
			info->results = 0;
			info->axial = info->shear = info->moment = info->disp = false;
		}
		return true;
	}
	
	return false;
}

//----------------------------------------------------------------------------------------
//  createNodeInfo:withRect:
//
//      create node drawing info.
//
//  createNodeInfo: DrawNodeInfo* info -> 
//  withRect: NSRect r                 -> 
//
//  returns bool                       <- 
//----------------------------------------------------------------------------------------
- (bool)createNodeInfo: (DrawNodeInfo*)info withRect: (NSRect)r {
	FrameStructure* s = [_frameDocument structure];
	if (s) {
		DlUInt32 flags = [_frameDocument drawingFlags];
		info->s = s;
		info->r = r;
		info->doc = self;
		info->nodes = (flags & ShowNodes) != 0;
		info->loads = (flags & ShowLoads) != 0;
		info->graphics = [_frameDocument graphics];
		return true;
	}
	return false;
}

//----------------------------------------------------------------------------------------
//  drawFixity:
//
//      draw fixity graphic for node.
//
//  drawFixity: Node n -> 
//
//  returns nothing
//----------------------------------------------------------------------------------------
- (void)drawFixity: (Node)n
{
	NSBezierPath *p = [Fixity fixityWithCode: n.GetRestraint()];
	if (p) {
		[FrameGraphics drawPath: p at: [_frameDocument graphics]->ToMGPoint(n.GetCoords()) withAngle: 0];
	}
}

inline bool signsMatch(DlFloat64 a, DlFloat64 b)
{
	return (a > 0 && b >= 0) || (a < 0 && b <= 0);
}

- (DlInt32)getElementLoadCoordsForElement: (Element) e withInfo: (DrawElementInfo*)info
{
	DlInt32 count = 0;
	
	DlFloat32 vals[ElementLoadCount];
	
	if (info->s->GetElementLoads(vals, e))
	{
		DlFloat64 factor = 10 / info->graphics->GetXScale();

		DlFloat64 l = vals[ElementLoadLateral];
		if (l != 0)
		{
			DlFloat64 f = copysign(factor, vals[ElementLoadLateral]);
			
			WorldPoint cosines(e.Cosines());
			WorldPoint offset(-f * cosines.y(), f * cosines.x());
			
			WorldPoint n1(e.StartNode().GetCoords());
			WorldPoint n2(e.EndNode().GetCoords());
			
			info->pts.Set(count++, n1);
			info->pts.Set(count++, n1 + offset);
			info->pts.Set(count++, n2 + offset);
			info->pts.Set(count++, n2);
		}
		
		DlFloat64 a = vals[ElementLoadAxial];
		if (!signsMatch(a, l))
		{
			DlFloat64 f = copysign(factor, a);
				
			WorldPoint cosines(e.Cosines());
			WorldPoint offset(-f * cosines.y(), f * cosines.x());
			
			WorldPoint n1(e.StartNode().GetCoords());
			WorldPoint n2(e.EndNode().GetCoords());
			
			info->pts.Set(count++, n1);
			info->pts.Set(count++, n1 + offset);
			info->pts.Set(count++, n2 + offset);
			info->pts.Set(count++, n2);
		}
	}
	
	return count;
}


- (void)drawElementLoad: (DlFloat64)l forElement: (Element)e withInfo: (DrawElementInfo*)info inColor: (DlUInt32) color
{
//	ElementLoad theLoad(e.GetLoad([_frameDocument loadCase]));
	if (l != 0) {

		DlFloat64 factor = 10 / info->graphics->GetXScale();
		if (l < 0)
			factor = -factor;
		
		WorldPoint cosines(e.Cosines());
		WorldPoint offset(-factor * cosines.y(), factor * cosines.x());
		
		WorldPoint n1(e.StartNode().GetCoords());
		WorldPoint n2(e.EndNode().GetCoords());

		info->pts.Set(0, n1);
		info->pts.Set(1, n1 + offset);
		info->pts.Set(2, n2 + offset);
		info->pts.Set(3, n2);
	
		info->graphics->DrawCurve(4, info->pts, color, 1.0, true, fillColor(color));
	}
}

constexpr int kPinnedOffset {5};

//----------------------------------------------------------------------------------------
//  drawElement:atIndex:withInfo:
//
//      draw the specified element.
//
//  drawElement: Element e             -> 
//  atIndex: DlUInt32 index            -> 
//  withInfo: DrawElementInfo* info    -> 
//
//  returns nothing
//----------------------------------------------------------------------------------------
- (void)drawElement: (Element)e atIndex: (DlUInt32)index withInfo: (DrawElementInfo*)info
{
	NSRect elementBounds = [self boundsForElement: e];
	if (NSIntersectsRect(elementBounds, info->r)) {
		DlInt32 count = 0;
	
		if (info->elements) {
			count = e.GetCoords(info->pts);
			if (count) {
				DlUInt32 color = [FrameColor colorForElement: e inDocument: _frameDocument];
                
				graphics* g = info->graphics;
				
				g->DrawCurve(count, info->pts, color, 3.0);
				
				// draw little indicators for pinned connections.
				if (e.GetProperty().GetBoolValue(kPropertyStartPinned))
				{
					WorldPoint p { e.StartNode().GetCoords() };
					
					// move back 4 pixels along the beam
					WorldPoint cosines(e.Cosines());
					DlFloat64 factor = kPinnedOffset / info->graphics->GetXScale();
					WorldPoint offset(factor * cosines.x(), factor * cosines.y());

					//g->SetLineColor(0);
					g->SetThickness(1);
					g->SetFillColor(-1);
					g->DrawCircle(p + offset, 2, true);
				}
				
				if (e.GetProperty().GetBoolValue(kPropertyEndPinned))
				{
					WorldPoint p { e.EndNode().GetCoords() };
					
					// move back 4 pixels along the beam
					WorldPoint cosines(e.Cosines());
					DlFloat64 factor = -kPinnedOffset / info->graphics->GetXScale();
					WorldPoint offset(factor * cosines.x(), factor * cosines.y());
					
					g->SetThickness(1);
					g->SetFillColor(-1);
					g->DrawCircle(p + offset, 2, true);
				}
			}
		}
		
		if (info->loads) {
			
			LoadCase actLC = info->s->GetActiveLoadCase();
			ElementLoad theLoad(actLC < kMaxLoadCases ? e.GetLoad(actLC): 0);
			DlFloat32 vals[ElementLoadCount];
			
			if (info->s->GetElementLoads(vals, e)) {
			
				if (vals[ElementLoadLateral] != 0) {
					
					DlUInt32 theColor = [FrameColor lateralLoadColor];
					if (theLoad && [_frameDocument hilitedElementLoad] == theLoad)
						theColor |= 0x000000ff;
					[self drawElementLoad: vals[ElementLoadLateral] forElement: e withInfo: info
						inColor: theColor];
				}
				
				if (vals[ElementLoadAxial] != 0) {
				
					DlUInt32 theColor = [FrameColor axialLoadColor];
					if (theLoad && [_frameDocument hilitedElementLoad] == theLoad)
						theColor |= 0x000000ff;
					[self drawElementLoad: vals[ElementLoadAxial] forElement: e withInfo: info
						inColor: theColor];
				}
			}
		}
		
		if (info->results) {
			if (info->axial) {
				count = e.ForceDOFCoords(XCoord, index, info->pts, *info->results);
				if (count) {
					DlUInt32 color = [FrameColor axialColor];
					info->graphics->DrawCurve(count, info->pts, color, 1.0,
						true, fillColor(color));
				}
			}
			
			if (info->shear) {
				count = e.ForceDOFCoords(YCoord, index, info->pts, *info->results);
				if (count) {
					DlUInt32 color = [FrameColor shearColor];
					info->graphics->DrawCurve(count, info->pts, color, 1.0,
							true, fillColor(color));
				}
			}
			
			if (info->moment) {
				count = e.ForceDOFCoords(TCoord, index, info->pts, *info->results);
				if (count) {
					DlUInt32 color = [FrameColor momentColor];
					info->graphics->DrawCurve(count, info->pts, color, 1.0, 
							true, fillColor(color));
				}
			}
			
			if (info->disp) {
				count = e.GetDisplacedCoords(index, info->pts, *info->results);
				if (count) {
					info->graphics->DrawCurve(count, info->pts, [FrameColor displacementColor], 1.5);
				}
			}
		}
	}
}

//----------------------------------------------------------------------------------------
//  drawNode:withInfo:
//
//      draw the specified node.
//
//  drawNode: Node n               -> 
//  withInfo: DrawNodeInfo* info   -> 
//
//  returns nothing
//----------------------------------------------------------------------------------------
- (void)drawNode: (Node)n withInfo: (DrawNodeInfo*)info
{
	NSRect ndBounds = [self boundsForNode: n];
	
	if (NSIntersectsRect(ndBounds, info->r)) {
		graphics* g = info->graphics;
	
		[[NSColor blackColor] set];
		if (info->loads) {
			NodeLoadType types[DOF_PER_NODE];
			DlFloat32 vals[DOF_PER_NODE];
			
			if (info->s->GetNodeLoads(vals, types, n)) {
				
				LoadCase actLC = info->s->GetActiveLoadCase();
				NodeLoad theLoad(actLC < kMaxLoadCases ? n.GetLoad(actLC) : 0);
			
				[self drawLoad: vals
					  andTypes: types
							at: n.GetCoords()
					   hilited: theLoad && [_frameDocument hilitedNodeLoad] == theLoad];
			}
		}
		
		if (info->nodes) {

			[self drawFixity: n];

			g->SetFillColor([FrameColor colorForNode: n inDocument: _frameDocument]);
			g->DrawCircle(n.GetCoords(), 3, true);

#if DlDebugging
			NSString* s = [[NSNumber numberWithInt:n.id()] stringValue];

			NSMutableDictionary* attrs = [NSMutableDictionary dictionaryWithCapacity: 10];
			NSFont* fnt = [NSFont userFontOfSize : 10];
			DlScreenPoint2d p = g->ToScreen(n.GetCoords());
			NSPoint pt { static_cast<CGFloat>(p.x) + 2, static_cast<CGFloat>(p.y) + 2 };
			[attrs setObject: fnt forKey: NSFontAttributeName];
			[s drawAtPoint:pt withAttributes:attrs];
#endif
		}
	}
}

//----------------------------------------------------------------------------------------
//  addPointsToBounds                                                              static
//
//      add a point to the bounds.
//
//  graphics* g            -> 
//  NSRect& bounds         -> 
//  DlInt32 count          -> 
//  PointEnumerator pts    -> 
//
//  returns nothing
//----------------------------------------------------------------------------------------
static void
addPointsToBounds(graphics* g, NSRect& bounds, DlInt32 count, PointEnumerator pts) {
	for (DlInt32 i = 0; i < count; i++)
		bounds = NSUnionRect(bounds, g->ToMGRect(pts.ElementAt(i), 2));
}

//----------------------------------------------------------------------------------------
//  computeElementBounds:atIndex:withInfo:
//
//      compute element bounds.
//
//  computeElementBounds: Element e    -> 
//  atIndex: DlUInt32 index            -> 
//  withInfo: DrawElementInfo* info    -> 
//
//  returns nothing
//----------------------------------------------------------------------------------------
- (void)computeElementBounds: (Element)e atIndex: (DlUInt32)index withInfo: (DrawElementInfo*)info 
{
	NSRect elementBounds = NSInsetRect(info->graphics->ToMGRect(e.GetBounds()), -1, -1);
	//DlInt32 count = 0;
	if (info->elements) {
		addPointsToBounds(info->graphics, elementBounds, e.GetCoords(info->pts), info->pts);	
	}
		
	if (info->loads) {
		addPointsToBounds(info->graphics, elementBounds,
				[self getElementLoadCoordsForElement: e withInfo: info],
				info->pts);
	}
	
	if (info->results) {
		if (info->axial) {
			addPointsToBounds(info->graphics, elementBounds,  
				e.ForceDOFCoords(XCoord, index, info->pts, *info->results), 
				info->pts);	
		}
		
		if (info->shear) {
			addPointsToBounds(info->graphics, elementBounds,  
				e.ForceDOFCoords(YCoord, index, info->pts, *info->results), 
				info->pts);	
		}
		
		if (info->moment) {
			addPointsToBounds(info->graphics, elementBounds,  
				e.ForceDOFCoords(TCoord, index, info->pts, *info->results), 
				info->pts);	
		}
		
		if (info->disp) {
			addPointsToBounds(info->graphics, elementBounds,  
				e.GetDisplacedCoords(index, info->pts, *info->results), 
				info->pts);	
		}
	}

	(*_elemBounds)[elemKey(e)] = elementBounds;
}


@end
