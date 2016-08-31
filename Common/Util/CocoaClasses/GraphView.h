//
//  GraphView.h
//
//  Created by David Salmon on 5/28/15.
//  Copyright (c) 2015 Dave Salmon. All rights reserved.
//

#ifndef _H_GraphView
#define _H_GraphView

#import "CPDefines.h"

#if TARGET_OS_IPHONE
#import <UIKit/UIKit.h>
#else
#import <Cocoa/Cocoa.h>
#endif

typedef enum {
	kGraphTypeNone,
	kGraphTypeXY,		// X-Y
	kGraphTypeRangeXY,	// range bars
	kGraphTypeLine,		// only Y
	kGraphTypeBarY,		// vertical bars
	kGraphTypeBarX		// horizontal bars
} GraphViewGraphType;

typedef enum {
	kGraphAxisTypeNone,
	kGraphAxisTypeLine
} GraphViewAxisType;

typedef enum {
	kGraphTicksNone,
	kGraphTicksInside,
	kGraphTicksOutside,
	kGraphTicksAxis
} GraphViewTicksType;

typedef enum {
	kGraphLeft,
	kGraphBottom,
	kGraphRight,
	kGraphTop
} GraphViewLabelLocation;

typedef struct GraphViewPoint {
	double x;
	double y;
} GraphViewPoint;

typedef struct GraphViewSize {
	double width;
	double height;
} GraphViewSize;

typedef struct GraphViewRect {
	GraphViewPoint 	origin;
	GraphViewSize	size;
} GraphViewRect;

typedef struct GraphViewTickData {
	int	minorTickSpacing;
	int minorTicksPerLabel;
	GraphViewTicksType	tickType;
} GraphViewTickData;


@class GraphDrawView;
@protocol GraphViewDataSource;

@interface GraphView : CPView
{
@public
	
	//	these are bottom, left, top, right
	int					_insets[4];
	GraphViewTicksType	_ticks[4];
	float				_tickSpacing[4];
	BOOL				_drawPoints;
	
	NSDictionary*_labelAttrs;
	
	GraphDrawView*		_drawView;
	IBOutlet id<GraphViewDataSource>	_delegate;
	
	// cached layout
	int					_graphCount;
	GraphViewRect*		_rectsForGraphs;
	GraphViewGraphType*	_typesForGraphs;
}

- (void) setDelegate:(id<GraphViewDataSource>) delegate;
- (id<GraphViewDataSource>) delegate;

// set true to show the value for the specified graph when the mouse is
//	in the pane.
- (void) showValue: (bool) show forGraph: (int) graph;

// recompute the graph layout.
- (void) noteGraphDataChanged;

//	set the insets to make room for tick labels
//	these are left, bottom, right, top

- (void)setViewInsets: (const int*)insets;
- (void)setTickSpacing: (const float*)spacing;
- (void)setDrawPoints: (BOOL) doDrawPoints;

- (CPSize) labelSize: (NSString*) label;
- (int) offsetFromGraph;

//- (GraphViewRect*) adjustGraphForTicks: (GraphViewRect*) r;

@end

@protocol GraphViewDataSource

//	returns the number of graphs
- (int)numberOfGraphsInGraphView:(GraphView *)graphView;

//	return the type for graph graph
- (GraphViewGraphType) graphView:(GraphView *)graphView typeForGraph:(int)graph;

//	return the bounds for the graph
- (GraphViewRect) graphView:(GraphView *)graphView boundsForGraph:(int)graph;

//	return the number of points in graph
- (int) graphView:(GraphView *)graphView pointsForGraph:(int)graph;

// the color for the graph

- (CPColor*) graphView:(GraphView *)graphView colorForGraph:(int)graph;

// the color for the graph
- (float) graphView:(GraphView *)graphView lineThicknessForGraph:(int)graph;

//	values
- (double) graphView:(GraphView *)graphView xValueForGraph:(int)graph atPoint:(int)point;
- (double) graphView:(GraphView *)graphView primaryValueForGraph:(int)graph atPoint:(int)point;
- (double) graphView:(GraphView *)graphView highValueForGraph:(int)graph atPoint:(int)point;
- (double) graphView:(GraphView *)graphView lowValueForGraph:(int)graph atPoint:(int)point;
- (BOOL)   graphView:(GraphView *)graphView drawPointsForGraph:(int)graph;

//	return the label for the cursor. p is in bounds coordinates
- (NSString*) graphView:(GraphView *)graphView labelForCursor:(GraphViewPoint)p;

- (NSString*)graphView:(GraphView *)graphView labelForLeftTick:(int)tick;
- (NSString*)graphView:(GraphView *)graphView labelForRightTick:(int)tick;
- (NSString*)graphView:(GraphView *)graphView labelForTopTick:(int)tick;
- (NSString*)graphView:(GraphView *)graphView labelForBottomTick:(int)tick;

@end

#endif

// eof
