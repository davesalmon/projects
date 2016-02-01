/*+
 *	File:		FrameDocument.h
 *
 *	Contains:	document object for frame.
 *
 *	Written by:	David C. Salmon
 *
 *	Copyright:	COPYRIGHT (C) 2000 By David C. Salmon.  *WORLDWIDE RIGHTS RESERVED*
 *
 *	Discussion:
 *
 *	The FrameDocument class manages a single frame for analysis. All of the internal
 *	frame data is handled by the FrameStructure instance. Drawing is handled by
 *	by FrameView using two delegates:  The drawing delegate and the event deletage.
 *
 *	FrameView: This is the Cocoa view and the first responder for the document.
 *	All events pass through this window. Events are passed on to the event
 *	delegate (FrameDocument) and drawing to the drawing delegate (FrameDrawing).
 *
 *	The graphics state is maintained by the graphics object. This object contains
 *	methods for drawing lines and curves in a platform independent way.
 *
 *	To Do:
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


#import <Cocoa/Cocoa.h>
#import "FrameDrawing.h"
#import "FrameEvents.h"

#include "FrameStructure.h"
#include "Property.h"
#include "Node.h"
#include "WorldPoint.h"
#include "WorldRect.h"
class graphics;

@class FrameView;
@class FrameAction;
@class FrameGrid;
@class FrameGraphics;
@class NodeLoadController;
@class ElemLoadController;
@class PropertyController;
@class WorkspaceController;
@class ElementInspectorController;
@class NodeInspectorController;
@class WorkspaceData;
@class ResultTableController;
class FrameListener;



@interface FrameDocument : NSDocument <FrameEvents>
{
    // tool window objects
    IBOutlet NSMatrix*		_toolMatrix;
    IBOutlet NSMatrix*		_fixityMatrix;
    IBOutlet NSPanel*		_toolWindow;
	
    IBOutlet NSPopUpButton* _loadCasePopup;
    IBOutlet NSPopUpButton* _displayPopup;
    IBOutlet NSPopUpButton* _elementTypePopup;
    IBOutlet NSPopUpButton* _unitsPopup;
    IBOutlet NSSlider*		_displacementScaleSlider;
	
	IBOutlet FrameView*		_frameView;

    NSString*			_windowFrameStr;
    
    int					_currTool;
    NSCursor*			_cursors[3];
    graphics*			_graphics;
    FrameStructure* 	_structure;
    FrameListener*		_listener;
    FrameGrid*			_grid;
	FrameGraphics*		_drawing;

	NodeLoadController*			_nodeLoadPanel;
	ElemLoadController*			_elemLoadPanel;
	PropertyController*			_propertyPanel;
    WorkspaceController*		_workspacePanel;
	ElementInspectorController*	_elementInspectorPanel;
	ResultTableController*		_resultsPanel;
	NodeInspectorController*	_nodeInspectorPanel;
	
	bool				_nodeLoadIsVisible;
	bool				_elemLoadIsVisible;
	bool				_propertyIsVisible;
    bool				_workspaceIsVisible;
	bool				_elementInspectorIsVisible;
	bool				_resultsIsVisible;
	bool				_nodeInspectorIsVisible;

//	state variables
//	LoadCase			_currLoadCase;
    NodeEnumerator*		_selectedNodes;
    ElementEnumerator*	_selectedElements;
    DlUInt32			_drawingFlags;
    DlInt32				_currUnits;

	//	dragging and clicking
//    Node				_draggedNode;
    bool				_isDrag;
    NodeEnumerator*		_clickNodeSelection;	//	selection saved on click
    ElementEnumerator*	_clickElemSelection;	//	selection saved on click
    WorldPoint			_dragStartCoords;	// for move action
    WorldPoint			_dragCurrCoords;	//	for move action
    WorldPoint			_pasteOffset;
    WorldPoint			_pasteDelta;
    bool				_updatePasteOffset;
	DlInt32				_changedCount;
}

- (id)init;
- (void)dealloc;

//	-- accessors

- (graphics*)graphics;
- (FrameStructure*) structure;
- (NodeEnumerator*) nodeSelection;
- (ElementEnumerator*) elementSelection;
- (NSView*) frameView;
- (NSScrollView*) scrollView;
- (NSWindow*) mainWindow;
- (FrameGrid*) grid;

//	just an increment count whenever the data changes
- (DlInt32) changeCount;

- (WorkspaceData*) workspace;
- (void) setWorkspace: (WorkspaceData*) workspace;
- (void) setUnits: (NSNumber*) units;
- (void) setScale: (NSNumber*) scale;
- (NSNumber*) scale;

- (DlUInt32)drawingFlags;
- (void) setDrawingFlags : (DlUInt32) flags;

// accessors
//- (LoadCase) loadCase;
//- (void) setLoadCase: (LoadCase) theCase;

- (NodeLoad) hilitedNodeLoad;
- (ElementLoad) hilitedElementLoad;
- (Property) hilitedProperty;

- (const char*) activeElementType;

- (void) refresh;
- (void) refreshRect: (NSRect) r;

// panel visibility
- (void) elementInspectorIsClosing;
- (void) elementLoadPanelIsClosing;
- (void) resultsPanelIsClosing;
- (void) nodeInspectorIsClosing;
- (void) propertyPanelIsClosing;
- (void) nodeLoadPanelIsClosing;

// -- window handling

- (NSString*) windowNibName;
- (void) windowControllerDidLoadNib:(NSWindowController *)aController;
- (void) windowDidResignMain: (NSNotification *)aNotification;
- (void) windowDidBecomeMain: (NSNotification *)aNotification;
- (NSSize) windowWillResize:(NSWindow *)sender toSize:(NSSize)proposedFrameSize;

// return the size of the visible portion of the window.
- (NSSize) visibleSize;

- (void)refreshWindows;

// -- actions

- (void)performAction:(FrameAction*) action;

- (void)toggleFixity: (DlInt32) dir;
- (void)setFixity: (DlUInt32) restCode;

- (IBAction) fixInXDirection: (id) sender;
- (IBAction) fixInYDirection: (id) sender;
- (IBAction) fixInTDirection: (id) sender;
- (IBAction) fixityAction: (id) sender;
- (IBAction) selectAll: (id) sender;
- (IBAction) clear: (id) sender;
- (IBAction) copy: (id) sender;
- (IBAction) paste: (id) sender;

// show the node load panel
- (IBAction)showNodeLoadPanel: (id)sender;
// show the element load panel
- (IBAction)showElementLoadPanel: (id)sender;
// show the property panel
- (IBAction)showPropertyPanel: (id)sender;
// show the workspace panel
- (IBAction)showWorkspacePanel: (id)sender;
// select the load case
- (IBAction)selectLoadCase: (id)sender;
// select the tool
- (IBAction)toolSelect: (id)sender;
// run an analysis
- (IBAction)analyze:(id)sender;
// hande the display menu on the toolbar
- (IBAction)setItemDisplay: (id)sender;
// handle element type menu
- (IBAction)setElementType: (id)sender;
// handle units menu
- (IBAction)setUnitType: (id)sender;
// show the analysis results
- (IBAction)showAnalysisResults: (id)sender;

- (IBAction)showElementInspector: (id) sender;
- (IBAction)showNodeInspector: (id) sender;

- (IBAction) createLoadCombo: (id) sender;
- (IBAction) updateLoadCombo: (id) sender;

- (IBAction)updateAnalysisScale: (id)sender;

- (IBAction)doubleScale: (id)sender;
- (IBAction)halveScale: (id)sender;
- (void)changeScale: (double)newScale;

// update the workspace
- (void)updateWorkspace: (WorkspaceData*)w;
- (void)changeUnits: (DlInt32)units;

// miscellaneous
- (void) enableFixityButtons;
- (void) checkDuplicates;
- (void) resetPasteOffset;
- (void) selectionChanged;
- (void) setActiveProperty: (Property) index;

//	selection
- (void) clearSelections;
- (void) selectNode: (Node)n withFlags:(unsigned int) flags;
- (void) setSelectedNodes: (NodeEnumerator) nodes;

- (void) selectElement: (Element)e withFlags:(unsigned int) flags;
- (void) invalSelections;

- (bool) elementSelected: (DlInt32) index;
- (bool) nodeSelected: (DlInt32) index;

// -- mouse handling

- (void)flagsChanged:(unsigned int) flags;

//- (void)selectInRect:(const NSRect&) r;
- (bool)mouseInWindow: (NSPoint)globalPoint;

- (bool)beginClick: (const WorldPoint&)where withFlags: (unsigned int)flags;

//	handle the drag. return true if selection drag
- (bool)handleDragFrom: (const WorldPoint&)start to:(const WorldPoint&)end 
			withFlags:(unsigned int)flags andDist: (float) maxDist;
- (void)endClick;

//	-- listener

- (void)nodeChanged: (Node*)n removed: (BOOL)rem;
- (void)nodesChanged: (const NodeEnumerator&)nodes removed: (BOOL)rem;
- (void)elemChanged: (Element*)n removed: (BOOL)rem;
- (void)elemsChanged: (const ElementEnumerator&)nodes removed: (BOOL)rem;
- (void)nodeLoadChanged: (NodeLoad*)n removed: (BOOL)rem;
- (void)nodeLoadsChanged: (const NodeLoadEnumerator&)nodes removed: (BOOL)rem;
- (void)elemLoadChanged: (ElementLoad*)n removed: (BOOL)rem;
- (void)elemLoadsChanged: (const ElementLoadEnumerator&)nodes removed: (BOOL)rem;
- (void)propChanged: (Property*)n removed: (BOOL)rem;
- (void)propsChanged: (const PropertyEnumerator&)nodes removed: (BOOL)rem;
//- (void)updateLoadCounts;

@end
