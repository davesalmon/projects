/*+
 *
 *  GraphView.mm
 *
 *  Copyright © 2015 David C. Salmon. All Rights Reserved.
 *
 *  Contains the definition of the graph view object. This object can draw
 *  x-y, line, error-bar, and horizontal and vertical bar graphs.
 *
 -*/

#import "GraphView.h"

const int kLongTickSize = 5;
const int kShortTickSize = 3;
const int kTextOffset = 4;
const int kTotalOffset = (kLongTickSize + kTextOffset);

//------------------------------------------
//	class GraphDrawView
//
//	draws the graph.
//

@interface GraphDrawView : CPView {
@public
	GraphView*		_container;
	id 				_delegate;
	int				_mouseGraph;
	NSString*		_lastMouseString;
	CGRect			_lastMouseRect;
}

#if !TARGET_OS_IPHONE
- (void) mouseMoved:(NSEvent *)theEvent;
#endif

- (void) drawXYGraph:(int) which forRect: (GraphViewRect*) r;
- (void) drawXYRangeGraph:(int) which forRect: (GraphViewRect*) r;
- (void) drawLineGraph:(int) which forRect: (GraphViewRect*) r;
- (void) drawBarYGraph:(int) which forRect: (GraphViewRect*) r;
- (void) drawBarXGraph:(int) which forRect: (GraphViewRect*) r;

@end

// -----------------------------------------------------------------------------------------
// ---------------------------------------- GraphView Impl ---------------------------------
// -----------------------------------------------------------------------------------------

@implementation GraphView

//------------------------------------------------------------------------------
//  initWithFrame:
//
//      Initilize the graph view with the specified frame.
//
//  initWithFrame: NSRect frame ->
//
//  returns id                  <-
//------------------------------------------------------------------------------
- (id)initWithFrame:(CPRect)frame
{
	self = [super initWithFrame:frame];
	if (self) {
		_graphCount = 0;
		_rectsForGraphs = 0;
		_typesForGraphs = 0;
		
		// Initialization code here.
		frame.origin.x = 0;
		frame.origin.y = 0;
		_drawView = [[GraphDrawView alloc]initWithFrame:frame];
		[self addSubview:_drawView];
		_drawView->_container = self;
		
		CPFont* theFont = [CPFont systemFontOfSize:[CPFont smallSystemFontSize]-2];
		
#if !__has_feature(objc_arc)
		_labelAttrs = [[NSDictionary dictionaryWithObject: theFont
												   forKey: NSFontAttributeName] retain];
#else
		_labelAttrs = [NSDictionary dictionaryWithObject: theFont
												  forKey: NSFontAttributeName];
#endif
		
		for (int i = 0; i < 4; i++) {
			_insets[i] = 0;
			_tickSpacing[i] = 0;
		}
		
		_delegate = nil;
	}
	return self;
}

//------------------------------------------------------------------------------
//  dealloc
//
//      free local storage
//
//  returns nothing
//------------------------------------------------------------------------------
- (void) dealloc {
	delete _rectsForGraphs;
	delete _typesForGraphs;
	
#if !__has_feature(objc_arc)
	
	[_labelAttrs release];
	[_drawView release];
	[super dealloc];
	
#endif
	
}

//----------------------------------------------------------------------------------------
//  frameChanged:
//
//      handle change to graph frame.
//
//  frameChanged: id sender    -> 
//
//  returns nothing
//----------------------------------------------------------------------------------------
- (void) frameChanged: (id)sender
{
	[self noteGraphDataChanged];
}

//------------------------------------------------------------------------------
//  awakeFromNib
//
//      complete initialization after view is loaded.
//
//  returns nothing
//------------------------------------------------------------------------------
- (void) awakeFromNib {
	_drawView->_delegate = _delegate;
	
#if !TARGET_OS_IPHONE
	[[self window] makeFirstResponder:_drawView];
	
	[[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(frameChanged:)
												 name:NSViewFrameDidChangeNotification object:self];
	[self setPostsFrameChangedNotifications:YES];
#endif
	//	[self noteGraphDataChanged];
}

//----------------------------------------------------------------------------------------
//  noteGraphDataChanged
//
//      called to indicate that the number or type of the graphs has changed.
//
//  returns nothing
//----------------------------------------------------------------------------------------
- (void) noteGraphDataChanged
{
	if (_delegate != nil)
	{
		int nGraphs = [_delegate numberOfGraphsInGraphView: self];
		if (nGraphs != _graphCount)
		{
			delete _rectsForGraphs;
			delete _typesForGraphs;
			
			_rectsForGraphs = (GraphViewRect*)operator new
			(nGraphs * sizeof(GraphViewRect));
			_typesForGraphs = (GraphViewGraphType*)operator new
			(nGraphs * sizeof(GraphViewGraphType));
			
			_graphCount = nGraphs;
		}
		
		for (int i = 0; i < nGraphs; i++)
		{
			_typesForGraphs[i] = [_delegate graphView:self typeForGraph:i];
			_rectsForGraphs[i] = [_delegate graphView:self boundsForGraph:i];
			
			switch(_typesForGraphs[i])
			{
				case kGraphTypeNone:
					;
				case kGraphTypeXY:
				case kGraphTypeRangeXY:
				case kGraphTypeLine:
				case kGraphTypeBarY:
				case kGraphTypeBarX:
					;
			}
		}
	}
	
	//	NSRect r = [self frame];
	
#if TARGET_OS_IPHONE
	[self setNeedsDisplay];
#else
	[self setNeedsDisplay: YES];
#endif
}


//------------------------------------------------------------------------------
//  drawRect:
//
//      draw the view. This draw only the frame, labels and tick marks.
//
//  drawRect: NSRect rect   ->
//
//  returns nothing
//------------------------------------------------------------------------------
- (void)drawRect:(CPRect)rect
{
	// Drawing code here.
#if !TARGET_OS_IPHONE
	[CPBezierPath setDefaultLineWidth:0];
	[CPBezierPath setDefaultLineCapStyle:NSButtLineCapStyle];
	[CPBezierPath setDefaultLineJoinStyle:NSMiterLineJoinStyle];
#endif
	
	CPRect	graphFrame = [_drawView frame];
	graphFrame.origin.x += 0.5;
	graphFrame.origin.y += 0.5;
	
	[[CPColor whiteColor] set];
	[[CPBezierPath bezierPathWithRect:graphFrame] fill];
//	[CPBezierPath fillRect: graphFrame];
	
	[[CPColor blackColor] set];
	[[CPBezierPath bezierPathWithRect:graphFrame] stroke];
//	[CPBezierPath strokeRect: graphFrame];
	
	int count = 0;
	//	and draw ticks and labels for the graphs
	if (_tickSpacing[kGraphLeft] > 0)
	{
		int	loc = (int)graphFrame.origin.y;
		int	top = (int)(loc + graphFrame.size.height + 1);
		
		while (loc <= top) {
			NSString* s = [_delegate graphView:self labelForLeftTick: count];
			
			if (s) {
				CPSize labelSize = [self labelSize: s];
				CPPoint sLoc;
				sLoc.x = graphFrame.origin.x - kTotalOffset - labelSize.width;
				sLoc.y = loc - labelSize.height / 2;
				[s drawAtPoint:sLoc withAttributes: _labelAttrs];
			}
			
			CPPoint start { graphFrame.origin.x, static_cast<CGFloat>(loc + 0.5) };
			CPPoint last { graphFrame.origin.x - (s != nil ? kLongTickSize : kShortTickSize), static_cast<CGFloat>(loc + 0.5) };

			CPBezierPath* p = [CPBezierPath bezierPath];
			[p moveToPoint: start];
			
			[p addLineToPoint: last];
			[p stroke];
			
//			[CPBezierPath strokeLineFromPoint: start toPoint: last];
			count++;
			loc = (int)(graphFrame.origin.y + count * _tickSpacing[kGraphLeft]);
		}
	}
	
	count = 0;
	//	and draw ticks and labels for the right side
	if (_tickSpacing[kGraphRight] > 0)
	{
		int	loc = (int)graphFrame.origin.y;
		int	top = (int)(loc + graphFrame.size.height + 1);
		while (loc <= top) {
			CGFloat xLoc = graphFrame.origin.x + graphFrame.size.width;
			
			NSString* s = [_delegate graphView:self labelForRightTick: count];
			
			if (s) {
				CPSize labelSize = [self labelSize: s];
				CPPoint sLoc;
				sLoc.x = xLoc + kTotalOffset;
				sLoc.y = loc - labelSize.height / 2;
				[s drawAtPoint:sLoc withAttributes: _labelAttrs];
			}
			
			CPPoint start { xLoc, static_cast<CGFloat>(loc + 0.5) };
			CPPoint last { xLoc + (s != nil ? kLongTickSize : kShortTickSize), static_cast<CGFloat>(loc + 0.5) };

			CPBezierPath* p = [CPBezierPath bezierPath];
			[p moveToPoint: start];
			[p addLineToPoint: last];
			[p stroke];

//			[CPBezierPath strokeLineFromPoint: start toPoint: last];
			count++;
			loc =  (int)(graphFrame.origin.y + count * _tickSpacing[kGraphRight]);
		}
	}
	
	count = 0;
	if (_tickSpacing[kGraphBottom] > 0)
	{
		int	loc = (int)graphFrame.origin.x;
		int top = (int)(loc + graphFrame.size.width + 1);
		while (loc <= top) {
			NSString* s = [_delegate graphView:self labelForBottomTick: count];
			
			if (s) {
				CPSize labelSize = [self labelSize: s];
				CPPoint sLoc;
				sLoc.y = graphFrame.origin.y - kTotalOffset - labelSize.height;
				sLoc.x = loc - labelSize.width / 2;
				[s drawAtPoint:sLoc withAttributes: _labelAttrs];
			}
			
			CPPoint start { static_cast<CGFloat>(loc + 0.5), graphFrame.origin.y };
			CPPoint last { static_cast<CGFloat>(loc + 0.5), graphFrame.origin.y - kLongTickSize };

			CPBezierPath* p = [CPBezierPath bezierPath];
			[p moveToPoint: start];
			[p addLineToPoint: last];
			[p stroke];

//			[CPBezierPath strokeLineFromPoint: start toPoint: last];
			
//			loc += _tickSpacing[kGraphBottom];
			count++;
			loc = (int)(graphFrame.origin.x + count * _tickSpacing[kGraphBottom]);
		}
	}
}

//------------------------------------------------------------------------------
//  setDelegate:
//
//      set the delegate object for the graph.
//
//  setDelegate: id delegate    ->
//
//  returns nothing
//------------------------------------------------------------------------------
- (void) setDelegate: (id)delegate
{
	_delegate = delegate;
	_drawView->_delegate = delegate;
}

//------------------------------------------------------------------------------
//  delegate
//
//      return the current delegate.
//
//  returns id  <-
//------------------------------------------------------------------------------
- (id) delegate {
	return _delegate;
}

//----------------------------------------------------------------------------------------
//  showValue:forGraph:
//
//      turn on the cursor value display for the specified graph.
//
//  showValue: bool show   -> 
//  forGraph: int graph    -> 
//
//  returns nothing
//----------------------------------------------------------------------------------------
- (void) showValue: (bool) show forGraph: (int) graph {
#if !TARGET_OS_IPHONE
	if (show) {
		_drawView->_mouseGraph = graph;
		[[self window] setAcceptsMouseMovedEvents:YES];
	} else {
		[[self window] setAcceptsMouseMovedEvents:NO];
	}
#endif
}

//------------------------------------------------------------------------------
//  setViewInsets:
//
//      set the insets for the view. These need to be large enough to
//      accomodate the labels and ticks.
//
//  setViewInsets: const int* insets    ->
//
//  returns nothing
//------------------------------------------------------------------------------
- (void) setViewInsets: (const int*) insets
{
	for (int i = 0; i < 4; i++)
	{
		_insets[i] = insets[i];
		NSAssert(_insets[i] >= 0, @"Graph insets may not be negative");
	}
	CPRect frame = [self frame];
	frame.origin.x = _insets[kGraphLeft];
	frame.origin.y = _insets[kGraphBottom];
	frame.size.width -= (_insets[kGraphLeft] + _insets[kGraphRight]);
	frame.size.height -= (_insets[kGraphBottom] + _insets[kGraphTop]);
	
	[_drawView setFrame: frame];
}

//------------------------------------------------------------------------------
//  setTickSpacing:
//
//      set the tick spacing for the view. These spacings are used to draw the
//      tick marks.
//
//  setTickSpacing: const float* spacing  ->
//
//  returns nothing
//------------------------------------------------------------------------------
- (void) setTickSpacing: (const float*) spacing
{
	for (int i = 0; i < 4; i++)
	{
		_tickSpacing[i] = spacing[i];
		NSAssert(spacing[i] >= 0, @"Graph spacing may not be negative");
	}
}

//----------------------------------------------------------------------------------------
//  setDrawPoints:
//
//      set if the graph will draw points.
//
//  setDrawPoints: BOOL doDrawPoints   -> 
//
//  returns nothing
//----------------------------------------------------------------------------------------
- (void) setDrawPoints:(BOOL)doDrawPoints
{
	_drawPoints = doDrawPoints;
}

//------------------------------------------------------------------------------
//  labelSize:
//
//      return the size required to draw the specified label.
//
//  labelSize: NSString* label  ->
//
//  returns NSSize              <-
//------------------------------------------------------------------------------
- (CPSize) labelSize: (NSString*) label
{
	return [label sizeWithAttributes: _labelAttrs];
}

//------------------------------------------------------------------------------
//  offsetFromGraph
//
//      returns the label offset from the graph.
//
//  returns int <-
//------------------------------------------------------------------------------
- (int) offsetFromGraph
{
	return kTotalOffset;
}

@end

// -----------------------------------------------------------------------------------------
// -------------------------------------- Graph Draw View Impl -----------------------------
// -----------------------------------------------------------------------------------------

@implementation GraphDrawView

//------------------------------------------------------------------------------
//  initWithFrame:
//
//      Initialize the graph drawing view with the given frame.
//
//  initWithFrame: NSRect frame ->
//
//  returns id                  <-
//------------------------------------------------------------------------------
- (id)initWithFrame:(CPRect)frame {
	self = [super initWithFrame:frame];
	if (self) {
		// Initialization code here.
		_delegate = NULL;
		_lastMouseString = nil;
		_mouseGraph = -1;
#if !TARGET_OS_IPHONE
		[super setAutoresizingMask:NSViewWidthSizable|NSViewHeightSizable];
#endif
	}
	return self;
}

//------------------------------------------------------------------------------
//  dealloc
//
//      free local storage
//
//  returns nothing
//------------------------------------------------------------------------------
- (void) dealloc {
	
#if !__has_feature(objc_arc)
	[_lastMouseString release];
	[super dealloc];
#endif
}

//----------------------------------------------------------------------------------------
//  acceptsFirstResponder
//
//      return true so we get key strokes.
//
//  returns BOOL   <- 
//----------------------------------------------------------------------------------------
- (BOOL) acceptsFirstResponder
{
	return YES;
}

//------------------------------------------------------------------------------
//  drawRect:
//
//      draw the graphs
//
//  drawRect: NSRect rect   ->
//
//  returns nothing
//------------------------------------------------------------------------------
- (void)drawRect:(CPRect)rect {
	// Drawing code here.
	int count = [_delegate numberOfGraphsInGraphView: _container];
	if (count != _container->_graphCount)
	{
		[_container noteGraphDataChanged];
	}
	
	CPRect bounds = [self bounds];
	
	for (int i = 0; i < count; i++) {
		GraphViewGraphType theType = _container->_typesForGraphs[i];
		GraphViewRect r = _container->_rectsForGraphs[i];
		
		//GraphViewGraphType theType = [_delegate graphView:_container typeForGraph:i];
		//GraphViewRect r = [_delegate graphView:_container boundsForGraph:i];
		r.size.width /= bounds.size.width;
		r.size.height /= bounds.size.height;
		
#if !TARGET_OS_IPHONE
		float oldDefault = [CPBezierPath defaultLineWidth];
		[CPBezierPath setDefaultLineWidth:
			[_delegate graphView:_container lineThicknessForGraph:i]];
#endif
		[[_delegate graphView:_container colorForGraph:i] set];

		switch (theType) {
			case kGraphTypeXY:		[self drawXYGraph:i forRect: &r];		break;
			case kGraphTypeRangeXY:	[self drawXYRangeGraph:i forRect: &r];	break;
			case kGraphTypeLine:	[self drawLineGraph:i forRect: &r];		break;
			case kGraphTypeBarY:	[self drawBarYGraph:i forRect: &r];		break;
			case kGraphTypeBarX:	[self drawBarXGraph:i forRect: &r];		break;
			default: break;
		}
		
#if !TARGET_OS_IPHONE
		[CPBezierPath setDefaultLineWidth:oldDefault];
#endif
		[[CPColor blackColor] set];
	}
	
	if (_mouseGraph != -1 && _lastMouseString != nil) {
		[_lastMouseString drawInRect:_lastMouseRect withAttributes: _container->_labelAttrs];
	}
	
}

//------------------------------------------------------------------------------
//  drawXYGraph:forRect:
//
//      draw an x-y graph
//
//  drawXYGraph: int which      ->
//  forRect: GraphViewRect* r   ->
//
//  returns nothing
//------------------------------------------------------------------------------
- (void) drawXYGraph:(int) which forRect: (GraphViewRect*) r
{
	// NOTE: we need to do our own scaling!!!
	int points = [_delegate graphView:_container pointsForGraph:which];
	CPPoint	first;
	CPPoint second;
	first.x = ([_delegate graphView:_container xValueForGraph:which atPoint:0] - r->origin.x)/r->size.width;
	first.y = ([_delegate graphView:_container primaryValueForGraph:which atPoint:0] - r->origin.y)/r->size.height;
	
	float wid = [_delegate graphView:_container lineThicknessForGraph: which];
	if (wid == 0) wid = 1;
	
	BOOL drawPoints = _container->_drawPoints && [_delegate graphView: _container drawPointsForGraph: which];

	if (drawPoints) {
		CPRect r = NSMakeRect(first.x - wid, first.y - wid, 2*wid, 2*wid);
		CPBezierPath* p = [CPBezierPath bezierPathWithOvalInRect: r];
		[p fill];
		[p stroke];
	}
	
	for (int i = 1; i < points; i++) {
		second.x = ([_delegate graphView:_container xValueForGraph:which atPoint:i] - r->origin.x) / r->size.width;
		second.y = ([_delegate graphView:_container primaryValueForGraph:which atPoint:i] - r->origin.y) / r->size.height;
		
		CPBezierPath* p = [CPBezierPath bezierPath];
		[p moveToPoint: first];
		[p addLineToPoint: second];
#if TARGET_OS_IPHONE
		[p setLineWidth:
			[_delegate graphView:_container lineThicknessForGraph:i]];
#endif
		[p stroke];
		
		if (drawPoints) {
			CPRect r = NSMakeRect(second.x - wid, second.y - wid, 2*wid, 2*wid);
			CPBezierPath* p = [CPBezierPath bezierPathWithOvalInRect: r];
			[p fill];
			[p stroke];
		}

//		[NSBezierPath strokeLineFromPoint: first toPoint: second];
		
		first = second;
	}
}

//------------------------------------------------------------------------------
//  ToExactScreen:
//
//      convert the given point to a point exactly on the pixel grid.
//
//  ToExactScreen: NSPoint pt   ->
//
//  returns NSPoint             <-
//------------------------------------------------------------------------------
- (CPPoint) ToExactScreen:(CPPoint) pt
{
#if TARGET_OS_IPHONE
	return pt;
#else
	
#if 1
	NSRect r = NSMakeRect(pt.x, pt.y, 0, 0);
	
	// convert to the screen coordinate system
	r = [[self window]convertRectToScreen: [self convertRect: r toView:nil]];
	
	// round to exact screen values.
	r.origin.x = (int)(r.origin.x + 0.5) + 0.5;
	r.origin.y = (int)(r.origin.y + 0.5) + 0.5;
	
	// and convert back to window
	r = [self convertRect: [[self window]convertRectFromScreen: r] fromView:nil];
	
	return r.origin;
#else
	NSPoint scr = [[self window]convertBaseToScreen: [self convertPoint:pt toView:nil]];
	scr.x = (int)(scr.x + 0.5) + 0.5;
	scr.y = (int)(scr.y + 0.5) + 0.5;
	return [self convertPoint: [[self window]convertScreenToBase: scr] fromView: nil];
#endif
#endif
}

//------------------------------------------------------------------------------
//  drawXYRangeGraph:forRect:
//
//      draw a range graph. This consists of vertical lines.
//
//  drawXYRangeGraph: int which ->
//  forRect: GraphViewRect* r   ->
//
//  returns nothing
//------------------------------------------------------------------------------
- (void) drawXYRangeGraph:(int) which forRect: (GraphViewRect*) r
{
	int points = [_delegate graphView:_container pointsForGraph:which];
	for (int i = 0; i < points; i++) {
		CPPoint first, second;
		first.x = second.x = ([_delegate graphView:_container xValueForGraph:which atPoint:i] - r->origin.x) / r->size.width;
		first.y = ([_delegate graphView:_container lowValueForGraph:which atPoint:i] - r->origin.y) / r->size.height;
		second.y = ([_delegate graphView:_container highValueForGraph:which atPoint:i] - r->origin.y) / r->size.height;
		
		CPBezierPath* p = [CPBezierPath bezierPath];
		[p moveToPoint: [self ToExactScreen:first]];
		[p addLineToPoint: [self ToExactScreen:second]];
		[p stroke];
		
//		[CPBezierPath strokeLineFromPoint: [self ToExactScreen:first] toPoint: [self ToExactScreen:second]];
	}
}

//------------------------------------------------------------------------------
//  drawLineGraph:forRect:
//
//      draw a line graph. The x coordinates are to point numbers.
//
//  drawLineGraph: int which    ->
//  forRect: GraphViewRect* r   ->
//
//  returns nothing
//------------------------------------------------------------------------------
- (void) drawLineGraph:(int) which forRect: (GraphViewRect*) r
{
	int points = [_delegate graphView:_container pointsForGraph:which];
	CPPoint* thePoints = new CPPoint[points];
	for (int i = 0; i < points; i++) {
		thePoints[i].x = i;
		thePoints[i].y = ([_delegate graphView:_container primaryValueForGraph:which atPoint:i] - r->origin.y) / r->size.height;
	}
	
	CPBezierPath* path = [CPBezierPath bezierPath];
	
	[path moveToPoint: thePoints[0]];
	 
	for (int i = 1; i < points; i++) {
		[path addLineToPoint: thePoints[i]];
	}
	
//	[path appendBezierPathWithPoints:thePoints count:points];
	delete [] thePoints;
	
	[path stroke];
	
}

//------------------------------------------------------------------------------
//  drawBarYGraph:forRect:
//
//      draw a bar graph in the vertical direction.
//
//  drawBarYGraph: int which    -> the graph index.
//  forRect: GraphViewRect* r   -> the bounding rect.
//
//  returns nothing
//------------------------------------------------------------------------------
- (void) drawBarYGraph:(int) which forRect: (GraphViewRect*) r
{
	int points = [_delegate graphView:_container pointsForGraph:which];
	
#if !TARGET_OS_IPHONE
	double widPerPoint = [self bounds].size.width / points / 2.0;
	[CPBezierPath setDefaultLineWidth:widPerPoint];
#endif
	
	for (int i = 0; i < points; i++) {
		CPPoint first, second;
		first.x = second.x = ([_delegate graphView:_container xValueForGraph:which atPoint:i]
							  - r->origin.x) / r->size.width;
		first.y = 0;
		second.y = ([_delegate graphView:_container primaryValueForGraph:which atPoint:i]
					- r->origin.y) / r->size.height;
		
		CPBezierPath* p = [CPBezierPath bezierPath];
		[p moveToPoint: [self ToExactScreen:first]];
		[p addLineToPoint: [self ToExactScreen:second]];
		[p stroke];

//		[CPBezierPath strokeLineFromPoint: [self ToExactScreen:first]
//								  toPoint: [self ToExactScreen:second]];
	}
}

//------------------------------------------------------------------------------
//  drawBarXGraph:forRect:
//
//      draw a bar graph in the horizontal direction.
//
//  drawBarXGraph: int which    ->
//  forRect: GraphViewRect* r   ->
//
//  returns nothing
//------------------------------------------------------------------------------
- (void) drawBarXGraph:(int) which forRect: (GraphViewRect*) r
{
	// Not implemented.
}

#if !TARGET_OS_IPHONE
//----------------------------------------------------------------------------------------
//  mouseMoved:
//
//      handle cursor movement. Called if the view is set to track mouse movements.
//		see showValue:forGraph:.
//
//  mouseMoved: NSEvent * theEvent -> 
//
//  returns nothing
//----------------------------------------------------------------------------------------
- (void) mouseMoved:(NSEvent *)theEvent
{
	NSPoint where = [self convertPoint:[theEvent locationInWindow] fromView:nil];
	NSRect bounds = [self bounds];
	
	if (_lastMouseString != nil) {
		[self setNeedsDisplayInRect:_lastMouseRect];
#if !__has_feature(objc_arc)
		[_lastMouseString release];
#endif
		_lastMouseString = nil;
	}
	
	if ([self mouse: where inRect:bounds]) {
		[self setNeedsDisplayInRect:_lastMouseRect];
		GraphViewRect r = [_delegate  graphView:_container boundsForGraph:_mouseGraph];
		GraphViewPoint gLoc;
		gLoc.x = where.x * r.size.width/bounds.size.width + r.origin.x;
		gLoc.y = where.y * r.size.height/bounds.size.height + r.origin.y;
		
#if !__has_feature(objc_arc)
		_lastMouseString = [[_delegate graphView:_container labelForCursor: gLoc] retain];
#else
		_lastMouseString = [_delegate graphView:_container labelForCursor: gLoc];
#endif
		if (_lastMouseString) {
			
			NSSize sSize = [_lastMouseString sizeWithAttributes:_container->_labelAttrs];
			where.x -= sSize.width / 2;
			if (where.x < 0) {
				where.x = 0;
			} else if (where.x + sSize.width > bounds.size.width) {
				where.x = bounds.size.width - sSize.width;
			}
			
			if (where.y + sSize.height > bounds.size.height) {
				where.y = bounds.size.height - sSize.height;
			}
			
			_lastMouseRect.origin = where;
			_lastMouseRect.size = sSize;
			[self setNeedsDisplayInRect:_lastMouseRect];
		}
	}
}
#endif

@end
