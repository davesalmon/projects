/*+
 *
 *  FrameDocument.mm
 *
 *  Copyright © 2003 David C. Salmon. All Rights Reserved.
 *
 *  Contains the implementation of the frame document class. This class acts as the
 *  controller for the main frame window.
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

#include "DlPlatform.h"
#import "FrameDocument.h"
#import "FrameView.h"
#import "FrameAction.h"
#import "FrameGrid.h"
#import "FrameGraphics.h"
#import "FrameClipboard.h"
#import "NodeLoadController.h"
#import "ElemLoadController.h"
#import "PropertyController.h"
#import "AnalysisPanelController.h"
#import "ResultTableController.h"
#import "WorkspaceController.h"
#import "ElementInspectorController.h"
#import "NodeInspectorController.h"
#import "WorkspaceData.h"
#import "LoadCaseCombinationController.h"
#import "LoadCaseController.h"

#include "graphics.h"
#include "FrameStructure.h"
#include "Action.h"
#include "Element.h"
#include "FrameListener.h"
#include "NodeLoad.h"
#include "ElementLoad.h"
#include "FrameIO.h"
#include "LoadCaseCombination.h"
#include "StrErrCode.h"

const NSPoint kHotSpots[3] = 
{
	{11,  0},
	{ 8,  8},
	{ 8,  8},
//	{ 8, 10},
//	{ 3, 12},
//	{ 8,  8},
//	{ 8,  8},
//	{ 8,  9}	 
};

@implementation FrameDocument

enum {
	  ToolSelect 			= 0
	, ToolAddNode 			= 1
	, ToolAddElement		= 2
};

// number of pixels to move before a drag is processed. This
// prevents a small movement of the mouse while clicking from
// moving the points clicked on.
const int kDragPixelLimit = 4;

//----------------------------------------------------------------------------------------
//  init
//
//      initialize frame to default values.
//
//  returns id <- self
//----------------------------------------------------------------------------------------
- (id)init
{
	//	called for new documents
	self = [super init];
	
	_windowFrameStr = 0;
	_currTool = 0;
	for (int i = 0; i < DlArrayElements(_cursors); i++)
		_cursors[i] = 0;
	_graphics = 0;
	_structure = 0;
	_listener = 0;
	_grid = 0;
	_drawing = 0;
	_nodeLoadPanel = 0;
	_elemLoadPanel = 0;
	_propertyPanel = 0;
    _workspacePanel = 0;
	_elementInspectorPanel = 0;
	_nodeInspectorPanel = 0;
	_resultsPanel = 0;
	
//	_currLoadCase = 0;
	_selectedNodes = 0;
	_selectedElements = 0;
//	_activeElementType = 0;
	_drawingFlags = -1;

//	_draggedNode = Node(0);
	_isDrag = false;
	_clickNodeSelection = 0;
	_clickElemSelection = 0;
	
	_drawing = [[FrameGraphics alloc] initWithDocument: self];
	
	_changedCount = 0;
	
//	{
//		DlUInt32 theColor = colorFromNSColor([NSColor selectedControlColor]);
//		kDefaultColors._nodeSelectionColor = theColor;
//		kDefaultColors._elementSelectionColor = theColor;
//		
//		kDefaultColors._activeColor = colorFromNSColor([NSColor alternateSelectedControlColor]);
//	}
	
	_nodeLoadIsVisible = false;
	_elemLoadIsVisible = false;
	_propertyIsVisible = false;
	_workspaceIsVisible = false;
	_elementInspectorIsVisible = false;
	_resultsIsVisible = false;
	_nodeInspectorIsVisible = false;

	_currUnits = -1;
	
	return self;
}

//----------------------------------------------------------------------------------------
//  dealloc
//
//      release memory used by document
//
//  returns nothing
//----------------------------------------------------------------------------------------
- (void)dealloc
{
	for (int i = 0; i < DlArrayElements(_cursors); i++)
		[_cursors[i] release];
	delete _graphics;
	delete _structure;
	delete _listener;
	[_frameView stopTimer];
	[_grid release];
	[_drawing release];
	delete _selectedNodes;
	delete _selectedElements;
	[_nodeLoadPanel release];
	[_elemLoadPanel release];
	[_propertyPanel release];
    [_workspacePanel release];
	[_elementInspectorPanel release];
	[_nodeInspectorPanel release];
	[_resultsPanel release];
	[super dealloc];
}

#pragma mark -
#pragma mark =============== accessors ===============
#pragma mark -

//----------------------------------------------------------------------------------------
//  structure
//
//      return the structure object.
//
//  returns FrameStructure*    <- structure
//----------------------------------------------------------------------------------------
- (FrameStructure*) structure
{
	return _structure;
}

//----------------------------------------------------------------------------------------
//  graphics
//
//      return the graphics object.
//
//  returns graphics*  <- grahics
//----------------------------------------------------------------------------------------
- (graphics*)graphics
{
	return _graphics;
}

//----------------------------------------------------------------------------------------
//  nodeSelection
//
//      return the current node selection.
//
//  returns NodeEnumerator*    <- selection
//----------------------------------------------------------------------------------------
- (NodeEnumerator*) nodeSelection
{
	return _selectedNodes;
}

//----------------------------------------------------------------------------------------
//  nodeSelection
//
//      return the current node selection.
//
//  returns NodeEnumerator*    <- selection
//----------------------------------------------------------------------------------------
- (ElementEnumerator*) elementSelection
{
	return _selectedElements;
}

//----------------------------------------------------------------------------------------
//  hilitedNodeLoad
//
//      return the node load that should be hilited.
//
//  returns NodeLoad   <- 
//----------------------------------------------------------------------------------------
- (NodeLoad) hilitedNodeLoad
{
	return [[_nodeLoadPanel window] isKeyWindow] ?
		[_nodeLoadPanel hiliteLoad] : NodeLoad(0);
}

//----------------------------------------------------------------------------------------
//  hilitedElementLoad
//
//      return the hilited element load
//
//  returns ElementLoad    <- the load
//----------------------------------------------------------------------------------------
- (ElementLoad) hilitedElementLoad
{
	return [[_elemLoadPanel window] isKeyWindow] ?
		[_elemLoadPanel hiliteLoad] : ElementLoad(0);
}

//----------------------------------------------------------------------------------------
//  hilitedProperty
//
//      return the property that should be hilited.
//
//  returns Property   <- 
//----------------------------------------------------------------------------------------
- (Property) hilitedProperty
{
	if (_propertyPanel != nil && [[_propertyPanel window] isKeyWindow]) {
		int selProp = [_propertyPanel selectedRow];
        if (selProp >= 0) {
            return _structure->GetProperties().At(selProp);
        }
    }
    
    return Property(0);
}

//----------------------------------------------------------------------------------------
//  activeElementType
//
//      return the name of the active element type.
//
//  returns const char*    <- the type name
//----------------------------------------------------------------------------------------
- (const char*) activeElementType 
{
	return [[[_elementTypePopup selectedItem] title] UTF8String];
}


//----------------------------------------------------------------------------------------
//  refresh
//
//      refresh the entire display
//
//  returns nothing
//----------------------------------------------------------------------------------------
- (void) refresh
{
	[_frameView setNeedsDisplay:YES];
}

//----------------------------------------------------------------------------------------
//  refreshRect:
//
//      refresh the specified rectangle
//
//  refreshRect: NSRect r  -> teh rect to refresh
//
//  returns nothing
//----------------------------------------------------------------------------------------
- (void) refreshRect: (NSRect) r
{
	[_frameView setNeedsDisplayInRect: r];
}

//----------------------------------------------------------------------------------------
//  frameView
//
//      return the frame view.
//
//  returns NSView*    <- frame view
//----------------------------------------------------------------------------------------
- (NSView*) frameView
{
	return _frameView;
}

//----------------------------------------------------------------------------------------
//  scrollView
//
//      return the scroll view enclosing the document
//
//  returns NSScrollView*  <- the scroll view
//----------------------------------------------------------------------------------------
- (NSScrollView*) scrollView
{
	return (NSScrollView*)[[_frameView superview] superview];
}

//----------------------------------------------------------------------------------------
//  mainWindow
//
//      return the main window.
//
//  returns NSWindow*  <- 
//----------------------------------------------------------------------------------------
- (NSWindow*) mainWindow
{
	return [_frameView window];
}

//----------------------------------------------------------------------------------------
//  grid
//
//      return the grid.
//
//  returns FrameGrid* <- 
//----------------------------------------------------------------------------------------
- (FrameGrid*) grid
{
	return _grid;
}

//----------------------------------------------------------------------------------------
//  workspace
//
//      return the workspace.
//
//  returns WorkspaceData* <- 
//----------------------------------------------------------------------------------------
- (WorkspaceData*) workspace
{
	return [WorkspaceData workspaceDataWithRect: _graphics->GetWorldRect() grid: _grid scale:_graphics->GetXScale()];
}

//----------------------------------------------------------------------------------------
//  setWorkspace
//
//      set the workspace.
//
//  setWorkspace: (WorkspaceData*) workspace   -> the workspace
//
//----------------------------------------------------------------------------------------
- (void) setWorkspace: (WorkspaceData*) workspace
{
	_graphics->SetScale([workspace scale]);
	_graphics->SetWorldRect([workspace worldRect]);
    [_grid setSpacing: [workspace gridSpacing]];
    [_grid setSnapOn: [workspace gridSnapOn]];
    [_grid setGridOn: [workspace gridVisible]];

    [self updateFrame];
}

//----------------------------------------------------------------------------------------
//  updateFrame
//
//      update the current frame, based on changes to the workspace.
//
//  returns nothing
//----------------------------------------------------------------------------------------
- (void) updateFrame
{
	NSRect existingViewRect = [_frameView frame];

//	printNSRect("frame rect is", [_frameView frame]);
//	printNSRect("content rect is", [[[self scrollView] contentView] frame]);
//	printNSSize("content size is", [[self scrollView] contentSize]);

    NSRect newViewRect = _graphics->ScreenNSRect();
	newViewRect.size.width += 3;
	newViewRect.size.height += 3;

	if (!NSEqualRects(existingViewRect, newViewRect)) {
//		printNSRect("adjusted view rect is ", newViewRect);

		[_frameView setFrame: newViewRect];
	}
	
	[self refresh];
}

//----------------------------------------------------------------------------------------
//  updateWorkspace
//
//      update the current workspace, based on changes to the frame.
//
//  returns nothing
//----------------------------------------------------------------------------------------
- (void) updateWorkspace
{
    NSSize viewSize = [_frameView frame].size;
	NSPoint p { viewSize.width-3, viewSize.height-3 };

	DlScreenPoint2d currSize = _graphics->GetScreenSize();

	if (currSize.x < p.x || currSize.y < p.y)
	{
		printWorldRect("original world is", _graphics->GetWorldRect());
		printNSSize("view size is", [_frameView frame].size);
		
		WorldPoint topRight = _graphics->ToWorld(p);

		DlFloat64 newWid = topRight.x();
		DlFloat64 newHgt = topRight.y();

		WorldRect w { _graphics->GetWorldRect() };

		DlFloat64 exWid = w.right() - w.left();
		DlFloat64 exHgt = w.top() - w.bottom();

		if (newWid > exWid)
			w.right() = w.left() + topRight.x();// - 0.1;

		if (newHgt > exHgt)
			w.top() = w.bottom() + topRight.y();// - 0.1;

		_graphics->SetWorldRect(w);
		printWorldRect("modified world is", _graphics->GetWorldRect());
	}
}

//----------------------------------------------------------------------------------------
//  setUnits:
//
//      update the units used for the structure.
//
//  setUnits: NSNumber* units  -> 
//
//  returns nothing
//----------------------------------------------------------------------------------------
- (void) setUnits: (NSNumber*) units
{
	_currUnits = [units intValue];
    UnitTable::SetUnits(_currUnits);
    [_unitsPopup selectItemAtIndex:_currUnits];
    
    [self refreshWindows];
}

//----------------------------------------------------------------------------------------
//  setScale:
//
//      update the scale factor for screen drawing.
//
//  setScale: NSNumber* scale  -> 
//
//  returns nothing
//----------------------------------------------------------------------------------------
- (void) setScale: (NSNumber*) scale
{
	double theScale = [scale doubleValue];
    _graphics->SetScale(theScale);
    
    [self updateFrame];
}

//----------------------------------------------------------------------------------------
//  scale
//
//      return the current scale.
//
//  returns NSNumber*  <- 
//----------------------------------------------------------------------------------------
- (NSNumber*) scale
{
	double currScale = _graphics->GetXScale();
    return [NSNumber numberWithDouble: currScale];
}

//----------------------------------------------------------------------------------------
//  drawingFlags
//
//      return the drawing flags.
//
//  returns DlUInt32   <- 
//----------------------------------------------------------------------------------------
- (DlUInt32)drawingFlags 
{
	return _drawingFlags;
}

//----------------------------------------------------------------------------------------
//  setDrawingFlags:
//
//      set the drawing flags.
//
//  setDrawingFlags: DlUInt32 flags    -> 
//
//  returns nothing
//----------------------------------------------------------------------------------------
- (void) setDrawingFlags : (DlUInt32) flags 
{
	_drawingFlags = flags;
	if (_structure->Analyzed()) {
		[_drawing updateElementBounds];
	}
	[self refresh];
}

//----------------------------------------------------------------------------------------
//  changeCount
//
//      return the number of times changes have been made. If this number has not changed
//		then there have been no changes to the data.
//
//  returns DlInt32    <- 
//----------------------------------------------------------------------------------------
- (DlInt32) changeCount 
{
	return _changedCount;
}

//----------------------------------------------------------------------------------------
//  elementInspectorIsClosing
//
//      handle the closing of the element inspector.
//
//  returns nothing
//----------------------------------------------------------------------------------------
- (void) elementInspectorIsClosing
{
	_elementInspectorIsVisible = false;
}

//----------------------------------------------------------------------------------------
//  elementLoadPanelIsClosing
//
//      handle closing of element load panel.
//
//  returns nothing
//----------------------------------------------------------------------------------------
- (void) elementLoadPanelIsClosing
{
	_elemLoadIsVisible = false;
}

//----------------------------------------------------------------------------------------
//  propertyPanelIsClosing
//
//      handle closing of property panel.
//
//  returns nothing
//----------------------------------------------------------------------------------------
- (void) propertyPanelIsClosing
{
	_propertyIsVisible = false;
}

//----------------------------------------------------------------------------------------
//  nodeLoadPanelIsClosing
//
//      handle closing of node load panel.
//
//  returns nothing
//----------------------------------------------------------------------------------------
- (void) nodeLoadPanelIsClosing
{
	_nodeLoadIsVisible = false;
}

//----------------------------------------------------------------------------------------
//  resultsPanelIsClosing
//
//      handle closing of results.
//
//  returns nothing
//----------------------------------------------------------------------------------------
- (void) resultsPanelIsClosing
{
	_resultsIsVisible = false;
}

//----------------------------------------------------------------------------------------
//  nodeInspectorIsClosing
//
//      handle closing of node inspector.
//
//  returns nothing
//----------------------------------------------------------------------------------------
- (void) nodeInspectorIsClosing
{
	_nodeInspectorIsVisible = false;
}

#pragma mark -
#pragma mark =============== Window ===============
#pragma mark -

//----------------------------------------------------------------------------------------
//  windowNibName
//
//      return the nib name for this document type.
//
//  returns NSString*  <- window nib name
//----------------------------------------------------------------------------------------
- (NSString*)windowNibName 
{
	return @"FrameDocument";
}

//----------------------------------------------------------------------------------------
//  viewDidResize:
//
//      handle view resize.
//
//  viewDidResize: NSNotification * notif  -> 
//
//  returns nothing
//----------------------------------------------------------------------------------------
- (void) viewDidResize:(NSNotification *)notif
{
	id view = [notif object];
	if (view == _frameView)
	{
//		printf("got notification %s\n", [[notif name] UTF8String]);
		[self updateWorkspace];
	}
}

//----------------------------------------------------------------------------------------
//  visibleSize
//
//      return the visible size.
//
//  returns NSSize <- 
//----------------------------------------------------------------------------------------
- (NSSize) visibleSize
{
	NSSize s = [[[self scrollView] contentView] frame].size;
	s.width -= 3;
	s.height -= 3;
	return s;
}

//----------------------------------------------------------------------------------------
//  buildLoadCaseMenu
//
//      build the load case menu,
//
//  returns nothing
//----------------------------------------------------------------------------------------
- (void) buildLoadCaseMenu
{
	//	setup the load case menu
	[_loadCasePopup removeAllItems];
	
	for (auto lc = 0; lc < _structure->GetLoadCaseCount(); lc++) {
		
		NSMenuItem* item = [[NSMenuItem alloc] initWithTitle:
							[NSString stringWithUTF8String: _structure->GetLoadCaseName(lc)]
													  action: nil
											   keyEquivalent: [NSString string]];

		item.tag = _loadCasePopup.itemArray.count;
		
		[[_loadCasePopup menu] addItem: item];
	}

	[_loadCasePopup selectItemAtIndex: _structure->GetActiveLoadCase()];
}

//----------------------------------------------------------------------------------------
//  windowControllerDidLoadNib:
//
//      called after the nib file loaded.
//
//  windowControllerDidLoadNib: NSWindowController * aController   -> this controller.
//
//  returns nothing
//----------------------------------------------------------------------------------------
- (void)windowControllerDidLoadNib:(NSWindowController *) aController
{
    [super windowControllerDidLoadNib:aController];
    
    [aController setShouldCloseDocument: YES];
    
    // Add any code here that need to be executed once the windowController
    // has loaded the document's window.

	// Create graphics and empty structure if necessary
	if (!_graphics) {
		_graphics = graphics::createGraphics(20.0, WorldRect(0, 0, 40, 40));
	}
	if (!_structure) {
		_structure = NEW FrameStructure([NSLocalizedString(@"default", nil) UTF8String]);
	}
	if (!_grid)
		_grid = [[FrameGrid createWithSpacing: 1 on: YES] retain];
	
	// Create the element type popup
	[_elementTypePopup removeAllItems];
	
	StringEnumerator elemTypes(_structure->GetElementTypes());
	while (elemTypes.HasMore()) {
		[_elementTypePopup addItemWithTitle: [NSString stringWithCString: elemTypes.Next()
															   encoding : NSUTF8StringEncoding]];
	}
	
	[_elementTypePopup selectItemWithTitle: [NSString stringWithCString: _structure->GetActiveElementType()
															  encoding : NSUTF8StringEncoding]];
	
	if (elemTypes.Length() == 1)
		[_elementTypePopup setEnabled: NO];
	
	for (int item = 1; item < [_displayPopup numberOfItems]; item++) {
		DlUInt32 mask = 1 << (item - 1);
		[[_displayPopup itemAtIndex: item] setState: mask & _drawingFlags ? NSOnState : NSOffState];
	}
    
    // Create the units popup
	if (_currUnits == -1)
   		_currUnits = UnitTable::GetDefaultTable();

	[self setUnits: [NSNumber numberWithInt: _currUnits]];
	
	//	now hook everything up

	_listener = NEW FrameListener(self);
	_structure->AddListener(_listener);
	_selectedNodes = NEW NodeEnumerator();
	_selectedElements = NEW ElementEnumerator();

    [_toolWindow setHidesOnDeactivate: YES];
    [_toolWindow setFloatingPanel: YES];
    for (int i = 0; i < DlArrayElements(_cursors); i++) {
    	NSString* imName = [NSString stringWithFormat: @"tool%d", i];
    	NSImage* im = [NSImage imageNamed : imName];
    	_cursors[i] = [[NSCursor alloc] initWithImage:im hotSpot: kHotSpots[i]];
    }

	[_frameView setCursor: _cursors[0]];
	[_frameView setDrawDelegate:_drawing andEventDelegate: self];
	[[_frameView window] setInitialFirstResponder: _frameView];

	// Setting the frame here overrides defaults, but does not change the window size
//	NSRect viewRect = NSInsetRect(_graphics->ScreenNSRect(), -20, -20);
	
//	[_frameView setFrame: viewRect];
//	[_frameView setBounds: viewRect];

	if (_windowFrameStr) {
		[[_frameView window] setFrameFromString: _windowFrameStr];
		[_windowFrameStr release];
		_windowFrameStr = 0;
	} else {
		[self updateWorkspace];
	}
	
	[self buildLoadCaseMenu];
	
	[self enableFixityButtons];
	[self resetPasteOffset];

	[_frameView setPostsBoundsChangedNotifications: YES];

	NSNotificationCenter *ctr = [NSNotificationCenter defaultCenter];
	[ctr addObserver:self
			selector:@selector(viewDidResize:)
				name:NSViewFrameDidChangeNotification
			  object:nil
	];
	
//	[[self undoManager] setGroupsByEvent:NO];
}

//----------------------------------------------------------------------------------------
//  windowDidResignMain:
//
//      called when the window resigned as main window.
//
//  windowDidResignKey: NSNotification * aNotification -> 
//
//  returns nothing
//----------------------------------------------------------------------------------------
- (void) windowDidResignMain: (NSNotification *) aNotification 
{
	[_toolWindow orderOut: self];
	
	_nodeLoadIsVisible = [[_nodeLoadPanel window] isVisible];
	if (_nodeLoadIsVisible)
		[[_nodeLoadPanel window] orderOut: self];
	
	_elemLoadIsVisible = [[_elemLoadPanel window] isVisible];
	if (_elemLoadIsVisible)
		[[_elemLoadPanel window] orderOut: self];
	
	_propertyIsVisible = [[_propertyPanel window] isVisible];
	if (_propertyIsVisible)
		[[_propertyPanel window] orderOut: self];

	_workspaceIsVisible = [[_workspacePanel window] isVisible];
	if (_workspaceIsVisible)
		[[_workspacePanel window] orderOut: self];

	_elementInspectorIsVisible = [[_elementInspectorPanel window] isVisible];
	if (_elementInspectorIsVisible)
		[[_elementInspectorPanel window] orderOut: self];

	_nodeInspectorIsVisible = [[_nodeInspectorPanel window] isVisible];
	if (_nodeInspectorIsVisible)
		[[_nodeInspectorPanel window] orderOut: self];

	_resultsIsVisible = [[_resultsPanel window] isVisible];
	if (_resultsIsVisible)
		[[_resultsPanel window] orderOut: self];
}

//----------------------------------------------------------------------------------------
//  windowDidBecomeMain:
//
//      called when the window becomes the main window.
//
//  windowWillResize: NSWindow * sender -> the window
//	toSize: NSSize						-> the new size
//
//  returns size
//----------------------------------------------------------------------------------------
- (NSSize)windowWillResize:(NSWindow *)sender toSize:(NSSize)proposedFrameSize 
{
	bool updateFrame = false;
    NSScrollView* superView = [self scrollView];
	NSSize viewSize = [superView contentSize];
    NSSize currSize = [sender frame].size;
    NSSize deltaSize = NSMakeSize(currSize.width - viewSize.width, currSize.height - viewSize.height);

//	fprintf(stderr, "(%f, %f)\n", deltaSize.width, deltaSize.height);

    NSSize proposeViewSize = NSMakeSize(proposedFrameSize.width - deltaSize.width,
    							proposedFrameSize.height - deltaSize.height);
	
//	NSSize proposeViewSize = proposedFrameSize;
    viewSize = [_frameView frame].size;

    if (viewSize.width < proposeViewSize.width)
    {
    	viewSize.width = proposeViewSize.width;
        updateFrame = true;
    }

    if (viewSize.height < proposeViewSize.height)
    {
    	viewSize.height = proposeViewSize.height;
        updateFrame = true;
    }

    if (updateFrame)
    {
//    	[_frameView setBoundsSize:viewSize];
        [_frameView setFrameSize: viewSize];
        [self refresh];
    }
	
    return proposedFrameSize;
}

//----------------------------------------------------------------------------------------
//  windowDidBecomeMain:
//
//      called when the window becomes the main window.
//
//  windowDidBecomeKey: NSNotification * aNotification -> 
//
//  returns nothing
//----------------------------------------------------------------------------------------
- (void) windowDidBecomeMain: (NSNotification *) aNotification 
{
    [_toolWindow orderFront: self];
	[_frameView setCursor: _cursors[_currTool]];
	if (_nodeLoadIsVisible)
		[self showNodeLoadPanel: self];
	
	if (_elemLoadIsVisible)
		[self showElementLoadPanel: self];
	
	if (_propertyIsVisible)
		[self showPropertyPanel: self];
        
	if (_workspaceIsVisible)
		[self showWorkspacePanel: self];
		
	if (_elementInspectorIsVisible)
		[self showElementInspector: self];
	
	if (_nodeInspectorIsVisible)
		[self showNodeInspector: self];

	if (_resultsIsVisible)
		[self showAnalysisResults: self];

    UnitTable::SetUnits(_currUnits);
}

//----------------------------------------------------------------------------------------
//  windowWillClose:
//
//      called when the window is closing. This closes the document and so should nuke
//		the clipboard if it came from this document.
//
//  windowWillClose: NSNotification * notification ->
//
//  returns nothing
//----------------------------------------------------------------------------------------
- (void) windowWillClose: (NSNotification *)notification
{
	FrameClipboard* cb = [FrameClipboard sharedClipboard];
	if (![cb empty] && [cb document] == self)
		[cb clear];
}

//----------------------------------------------------------------------------------------
//  refreshWindows
//
//      refresh all the windows with unit related data.
//
//  returns nothing
//----------------------------------------------------------------------------------------
- (void)refreshWindows
{
	NSArray* cList = [self windowControllers];
    for (size_t i = 0; i < [cList count]; i++)
    {
    	NSWindowController* c = [cList objectAtIndex:i];
		if ([c conformsToProtocol: @protocol(FramePanel)])
		{
			[(NSWindowController<FramePanel>*)c refresh];
//        	fprintf(stderr, "%s\n", [[c windowNibName] UTF8String]);
        }
    }
}

#pragma mark -
#pragma mark =============== Actions ===============
#pragma mark -

//----------------------------------------------------------------------------------------
//  performKeyEquivalent:
//
//      perform an action for the specified keystroke.
//
//  performKeyEquivalent: NSEvent * theEvent   ->
//
//  returns BOOL                               <-
//----------------------------------------------------------------------------------------
- (BOOL)performKeyEquivalent:(NSEvent *)theEvent
{
	return [[_displayPopup menu] performKeyEquivalent: theEvent];
}


//----------------------------------------------------------------------------------------
//  performAction:
//
//      perform the specified action to support undo.
//
//  performAction: FrameAction* action -> the action
//
//  returns nothing
//----------------------------------------------------------------------------------------
- (void) performAction:(FrameAction*)action
{
	if ([action clearSelection])
		[self clearSelections];

#if DlDebugging
	printf("processing action %s\n", [[action description] UTF8String]);
#endif
	
	NSUndoManager* m = [self undoManager];
	
//	[m beginUndoGrouping];
	
	[m registerUndoWithTarget:self selector:@selector(performAction:) object:action];
	[m setActionName:[action title]];
	
//	[m endUndoGrouping];
	try {
		
		bool isUndo = [action perform : [self structure]];
		
		if ([action checkDuplicates])
			[self checkDuplicates];
		
		if ([self structure]->GetActiveLoadCase() >= kMaxLoadCases) {
			[_loadCasePopup selectItemWithTag: 0];
		}
		
	//	if (![action savePasteOffset])
	//		[self resetPasteOffset];

		if (isUndo)
			_changedCount--;
		else
			_changedCount++;
		
		if (_elementInspectorIsVisible) {
			[_elementInspectorPanel buildList];
		}
		
		if (_nodeInspectorIsVisible) {
			[_nodeInspectorPanel buildList];
		}

		if (_resultsIsVisible)
			[_resultsPanel refresh];
		
		if (_propertyIsVisible)
			[_propertyPanel refresh];

		[_displacementScaleSlider setEnabled: _structure->Analyzed()];
		
		if ([action updateLoadCaseMenu])
			[self buildLoadCaseMenu];
		
	} catch(DlException& ex) {
		ex.Display();
	}
}

#pragma mark -
#pragma mark ------ tool panel items
#pragma mark -

//----------------------------------------------------------------------------------------
//  toggleFixity:
//
//      toggle the fixity of the selected nodes.
//
//  toggleFixity: DlInt32 dir  -> the direction
//
//  returns nothing
//----------------------------------------------------------------------------------------
- (void) toggleFixity: (DlInt32) dir
{
	[self performAction: [FrameAction create: _structure->ToggleRestraints(*_selectedNodes, dir)
									withView: _frameView]];
	
}

//----------------------------------------------------------------------------------------
//  setFixity:
//
//      specify the fixity of the selected nodes.
//
//  setFixity: DlUInt32 restCode  -> the new restraint code
//
//  returns nothing
//----------------------------------------------------------------------------------------
- (void) setFixity: (DlUInt32) restCode
{
	[self performAction: [FrameAction create: _structure->SetRestraints(*_selectedNodes, restCode)
									withView: _frameView]];
}

//----------------------------------------------------------------------------------------
//  fixInXDirection:
//
//      fix the node in the x direction.
//
//  fixInXDirection: id sender -> the menu item
//
//  returns nothing
//----------------------------------------------------------------------------------------
- (IBAction) fixInXDirection: (id) sender
{
	[self toggleFixity: XCoord];
}

//----------------------------------------------------------------------------------------
//  fixInYDirection:
//
//      toggle fixity in the y direction.
//
//  fixInYDirection: id sender -> the menu item
//
//  returns nothing
//----------------------------------------------------------------------------------------
- (IBAction) fixInYDirection: (id) sender
{
	[self toggleFixity: YCoord];
}

//----------------------------------------------------------------------------------------
//  fixInTDirection:
//
//      toggle fixity in the z direction.
//
//  fixInTDirection: id sender -> the menu item
//
//  returns nothing
//----------------------------------------------------------------------------------------
- (IBAction) fixInTDirection: (id) sender
{
	[self toggleFixity: TCoord];
}

//----------------------------------------------------------------------------------------
//  fixityAction:
//
//      handle the set fixity action. This comes from the buttons.
//
//  fixityAction: id sender    -> 
//
//  returns nothing
//----------------------------------------------------------------------------------------
- (IBAction) fixityAction: (id) sender {
	[self setFixity: [[sender selectedCell] tag]];
}

//----------------------------------------------------------------------------------------
//  toolSelect:
//
//      select a new tool.
//
//  toolSelect: id sender  -> select tool element
//
//  returns nothing
//----------------------------------------------------------------------------------------
- (IBAction) toolSelect: (id)sender
{
	_currTool = [[sender selectedCell] tag];
	[_frameView setCursor: _cursors[_currTool]];
}

//----------------------------------------------------------------------------------------
//  setItemDisplay:
//
//      update the displayed items from the display menu.
//
//  setItemDisplay: id sender  ->
//
//  returns nothing
//----------------------------------------------------------------------------------------
- (IBAction)setItemDisplay:(id)sender
{
	DlUInt32 mask = [[(NSPopUpButton*)sender selectedItem] tag];
	if (mask) {
		if (_drawingFlags & mask) {
			_drawingFlags &= ~mask;
		} else {
			_drawingFlags |= mask;
		}
		
		[[(NSPopUpButton*)sender selectedItem] setState: mask & _drawingFlags ? NSOnState : NSOffState];
		[self refresh];
	}
}

//----------------------------------------------------------------------------------------
//  setElementType:
//
//      set the current element type.
//
//  setElementType: id sender  ->
//
//  returns nothing
//----------------------------------------------------------------------------------------
- (IBAction)setElementType:(id)sender
{
	// will need to update properties window on this message.
	_structure->SetActiveElementType([[[_elementTypePopup selectedItem] title] UTF8String]);
	[_propertyPanel refresh];
}

//----------------------------------------------------------------------------------------
//  setUnitType:
//
//      set the current unit type.
//
//  setUnitType: id sender ->
//
//  returns nothing
//----------------------------------------------------------------------------------------
- (IBAction)setUnitType:(id) sender
{
	[self changeUnits: [_unitsPopup indexOfSelectedItem]];
}

//----------------------------------------------------------------------------------------
//  selectLoadCase:
//
//      menu selection handler for load case menu.
//
//  selectLoadCase: id sender  -> the menu item
//
//  returns nothing
//----------------------------------------------------------------------------------------
- (IBAction) selectLoadCase: (id) sender {
	_structure->SetActiveLoadCase([_loadCasePopup selectedTag]);
//	if (_structure->Analyzed()) {
		[self refresh];
		
		if (_elementInspectorIsVisible)
			[_elementInspectorPanel buildList];
		
		if (_nodeInspectorIsVisible)
			[_nodeInspectorPanel buildList];

		if (_resultsIsVisible)
			[_resultsPanel refresh];
//	}
}

//----------------------------------------------------------------------------------------
//  updateAnalysisScale:
//
//      update the analysis scale from the value in the sender
//
//  doubleScale: id sender -> NSSlider
//
//  returns nothing
//----------------------------------------------------------------------------------------
- (IBAction)updateAnalysisScale: (id)sender
{
	DlFloat64 factor = [sender doubleValue];
	
	_structure->SetDisplacementMagnifier(factor);
	
	[self refresh];
}

#pragma mark -
#pragma mark ------ edit menu items
#pragma mark -

//----------------------------------------------------------------------------------------
//  selectAll:
//
//      select all joints and members.
//
//  selectAll: id sender   -> 
//
//  returns nothing
//----------------------------------------------------------------------------------------
- (IBAction) selectAll: (id) sender {
	*_selectedNodes = _structure->GetNodes();
	*_selectedElements = _structure->GetElements();
	[self refresh];
}

//----------------------------------------------------------------------------------------
//  clear:
//
//      delete selected joints and members.
//
//  clear: id sender   -> 
//
//  returns nothing
//----------------------------------------------------------------------------------------
- (IBAction) clear: (id) sender {
	[self performAction: [FrameAction 
			createClearingSelection: _structure->RemoveNodes(*_selectedNodes, *_selectedElements) 
			withView: _frameView]];
}

//----------------------------------------------------------------------------------------
//  copy:
//
//      copy selected joints and members to the clipboard.
//
//  copy: id sender    -> 
//
//  returns nothing
//----------------------------------------------------------------------------------------
- (IBAction) copy: (id) sender 
{
	// when a new selection is copied, reset the paste offset.
	[self resetPasteOffset];

	[[FrameClipboard sharedClipboard] setNodes: *_selectedNodes
								   andElements: *_selectedElements
								   forDocument: self];
}

//----------------------------------------------------------------------------------------
//  paste:
//
//      paste contents of the clipboard.
//
//  paste: id sender   -> 
//
//  returns nothing
//----------------------------------------------------------------------------------------
- (IBAction) paste: (id) sender 
{
	const FrameStructure* s = [[FrameClipboard sharedClipboard] contents];

	_pasteOffset += _pasteDelta;

	if (![[FrameClipboard sharedClipboard] validPasteOffset:_pasteOffset
												   forWorld:_graphics->GetWorldRect()] )
	{
		[self resetPasteOffset];
		_pasteOffset += _pasteDelta;
	} else if (_pasteOffset == WorldPoint()) {
		[self resetPasteOffset];
		_pasteOffset += _pasteDelta;
	}

	_updatePasteOffset = true;
	
#if DlDebugging
		printWorldPoint("paste offset for paste is ", _pasteOffset);
#endif

	FrameAction* action = [FrameAction
						   createClearingSelection: (_structure->PasteNew(s, _pasteOffset))
						   withView: _frameView];
	
//	FrameAction* action = [FrameAction 
//			createClearingSelection: (sameDocument ?
//								_structure->PasteSame(s, _pasteOffset) :
//								_structure->PasteNew(s, _pasteOffset))
//			withView: _frameView];
	[action setCheckDuplicates: true];
//	[action setSavePasteOffset: true];

	[self performAction: action];
}

//----------------------------------------------------------------------------------------
//  showWorkspacePanel:
//
//      show the workspace panel.
//
//  showWorkspacePanel: id sender  ->
//
//  returns nothing
//----------------------------------------------------------------------------------------
- (IBAction)showWorkspacePanel: (id)sender
{
	if (_workspacePanel == nil) {
		_workspacePanel = [[WorkspaceController createWorkspacePanel: self] retain];
	}
	
	[_workspacePanel showWindow:self];
	[[_workspacePanel window] makeKeyAndOrderFront: self];
	_workspaceIsVisible = true;
}

//----------------------------------------------------------------------------------------
//  doubleScale:
//
//      double the current scale
//
//  doubleScale: id sender -> unused
//
//  returns nothing
//----------------------------------------------------------------------------------------
- (IBAction)doubleScale: (id)sender
{
	[self changeScale: _graphics->GetXScale() * 2];
}

//----------------------------------------------------------------------------------------
//  halveScale:
//
//      cut the current scale in half
//
//  halveScale: id sender  -> unused
//
//  returns nothing
//----------------------------------------------------------------------------------------
- (IBAction)halveScale: (id)sender
{
	[self changeScale: _graphics->GetXScale() / 2];
}

#pragma mark -
#pragma mark ------ structure menu items
#pragma mark -

//----------------------------------------------------------------------------------------
//  showNodeLoadPanel:
//
//      display a panel showing the defined joint loads.
//
//  showNodeLoadPanel: id sender   -> the menu item
//
//  returns nothing
//----------------------------------------------------------------------------------------
- (IBAction) showNodeLoadPanel: (id) sender
{
	if (_nodeLoadPanel == nil) {
		_nodeLoadPanel = [[NodeLoadController createNodeLoadPanel: self] retain];
	}
    [_nodeLoadPanel showWindow:self];
	[[_nodeLoadPanel window] makeKeyAndOrderFront: self];
	_nodeLoadIsVisible = true;
}

//----------------------------------------------------------------------------------------
//  showElemLoadPanel:
//
//      display a panel showing the defined element loads.
//
//  showElemLoadPanel: id sender   -> the menu item
//
//  returns nothing
//----------------------------------------------------------------------------------------
- (IBAction) showElementLoadPanel: (id) sender
{
	if (_elemLoadPanel == nil) {
		_elemLoadPanel = [[ElemLoadController createElemLoadPanel: self] retain];
	}
    [_elemLoadPanel showWindow:self];
	[[_elemLoadPanel window] makeKeyAndOrderFront: self];
	_elemLoadIsVisible = true;
}

//----------------------------------------------------------------------------------------
//  showPropertiesPanel:
//
//      display a panel showing the defined element loads.
//
//  showPropertiesPanel: id sender   -> the menu item
//
//  returns nothing
//----------------------------------------------------------------------------------------
- (IBAction) showPropertyPanel: (id) sender
{
	if (_propertyPanel == nil) {
		_propertyPanel = [[PropertyController createPropertyPanel: self] retain];
	}
    [_propertyPanel showWindow:self];
	[[_propertyPanel window] makeKeyAndOrderFront: self];
	_propertyIsVisible = true;
//	printf("showPropertiesPanel: not implemented.\n");
}

//----------------------------------------------------------------------------------------
//  analyze:
//
//      analyze the structure.
//
//  analyze: id sender -> 
//
//  returns nothing
//----------------------------------------------------------------------------------------
- (IBAction) analyze: (id) sender
{
	[_toolMatrix setSelectionFrom: 0 to: 0 anchor: 0 highlight: true];
	[self toolSelect: _toolMatrix];
	
	if ([AnalysisPanelController runDialogWithDocument: self]) {
		[_drawing updateElementBounds];
		[_displacementScaleSlider setEnabled: YES];
		[_displacementScaleSlider setDoubleValue: 1.0];

		[self refresh];
		
		if (_elementInspectorIsVisible)
			[_elementInspectorPanel buildList];
		
		if (_nodeInspectorIsVisible)
			[_nodeInspectorPanel buildList];
		
		if (_resultsIsVisible)
			[_resultsPanel refresh];
	}
}

//----------------------------------------------------------------------------------------
//  showAnalysisResults:
//
//      display the results window.
//
//  showAnalysisResults: id sender -> 
//
//  returns nothing
//----------------------------------------------------------------------------------------
- (IBAction)showAnalysisResults: (id)sender
{
	if (!_resultsPanel) {
		_resultsPanel = [[ResultTableController createResultTable: self] retain];
	}
	
	[_resultsPanel showWindow: self];
	[[_resultsPanel window] makeKeyAndOrderFront: self];
	_resultsIsVisible = true;
}

//----------------------------------------------------------------------------------------
//  showElementInspector:
//
//      show the element inspection window.
//
//  showElementInspector: id sender    -> 
//
//  returns nothing
//----------------------------------------------------------------------------------------
- (IBAction)showElementInspector: (id) sender
{
	if (_elementInspectorPanel == nil) {
    	_elementInspectorPanel = 
			[[ElementInspectorController createElementPanel: self] retain];
    }
    
    [_elementInspectorPanel showWindow:self];
	[[_elementInspectorPanel window] makeKeyAndOrderFront: self];
	_elementInspectorIsVisible = true;
}

//----------------------------------------------------------------------------------------
//  showElementInspector:
//
//      show the element inspection window.
//
//  showElementInspector: id sender    ->
//
//  returns nothing
//----------------------------------------------------------------------------------------
- (IBAction)showNodeInspector: (id) sender
{
	if (_nodeInspectorPanel == nil) {
		_nodeInspectorPanel =
		[[NodeInspectorController createNodePanel: self] retain];
	}
	
	[_nodeInspectorPanel showWindow:self];
	[[_nodeInspectorPanel window] makeKeyAndOrderFront: self];
	_nodeInspectorIsVisible = true;
}

- (IBAction) createLoadCase: (id) sender
{
	if ([LoadCaseController runDialogWithDocument: self andName: nullptr]) {
		_structure->SetActiveLoadCase(_structure->GetLoadCaseCount()-1);
		[self buildLoadCaseMenu];
	}
}

- (IBAction) updateLoadCase: (id) sender
{
	if (_structure->ActiveLoadCaseIsEditable()) {
		
		if ([LoadCaseController runDialogWithDocument: self andName:
			 	_structure->GetLoadCaseName(_structure->GetActiveLoadCase())]) {
			_structure->SetActiveLoadCase(_structure->GetLoadCaseCount()-1);
			[self buildLoadCaseMenu];
		}
	}
}

//----------------------------------------------------------------------------------------
//  createLoadCombo:
//
//      create a load case combination.
//
//  createLoadCombo: id sender -> 
//
//  returns nothing
//----------------------------------------------------------------------------------------
- (IBAction) createLoadCombo: (id) sender
{
	LoadCaseCombination tlc;
	[LoadCaseCombinationController runDialogWithDocument: self andCombo: &tlc];
}

//----------------------------------------------------------------------------------------
//  updateLoadCombo:
//
//      update a load case combination.
//
//  updateLoadCombo: id sender -> 
//
//  returns nothing
//----------------------------------------------------------------------------------------
- (IBAction) updateLoadCombo: (id) sender
{
	NSMenuItem* item = [_loadCasePopup selectedItem];
	int theTag = item.tag;
	
	const LoadCaseCombination* exLoadCase = _structure->GetLoadCaseCombination(theTag);
	
	if (exLoadCase) {
		LoadCaseCombination tlc(*exLoadCase);
		[LoadCaseCombinationController runDialogWithDocument: self andCombo: &tlc];
	}
}

#pragma mark -
#pragma mark ------ menu utilities
#pragma mark -

//----------------------------------------------------------------------------------------
//  changeScale:
//
//      change the scale on the screen to the new scale
//
//  changeScale: double newScale   -> the new scale
//
//  returns nothing
//----------------------------------------------------------------------------------------
- (void)changeScale: (double) newScale
{
	[self updateScale:[NSNumber numberWithDouble: newScale]];
}

//----------------------------------------------------------------------------------------
//  updateScale:
//
//      update the scale on the screen to the new scale with undo
//
//  updateScale: (NSNumber*) newScale   -> the new scale as object
//
//  returns nothing
//----------------------------------------------------------------------------------------
- (void)updateScale: (NSNumber*) newScale
{
	NSUndoManager* m = [self undoManager];
	//	[m beginUndoGrouping];
	[m registerUndoWithTarget:self selector:@selector(updateScale:) object:[self scale]];
	[m setActionName: NSLocalizedString(@"Set Scale", nil)];
	//	[m endUndoGrouping];
	
	[self setScale: newScale];
}

//----------------------------------------------------------------------------------------
//  updateWorkspace:
//
//      <Specify function description>
//
//  updateWorkspace: WorkspaceData* w  -> 
//
//  returns nothing
//----------------------------------------------------------------------------------------
- (void)updateWorkspace: (WorkspaceData*)w
{
	NSUndoManager* m = [self undoManager];
	[m registerUndoWithTarget:self selector:@selector(updateWorkspace:) object:[self workspace]];
	[m setActionName: NSLocalizedString(@"Update Workspace", nil)];
	[self setWorkspace: w];
}

//----------------------------------------------------------------------------------------
//  updateWorkspace:
//
//      <Specify function description>
//
//  updateWorkspace: WorkspaceData* w  -> 
//
//  returns nothing
//----------------------------------------------------------------------------------------
- (void)changeUnits: (DlInt32)units
{
	NSUndoManager* m = [self undoManager];
    NSNumber* value = [NSNumber numberWithInt:_currUnits];
	[m registerUndoWithTarget:self selector:@selector(changeUnits:) object:value];
	[m setActionName: NSLocalizedString(@"Change Units", nil)];
	[self setUnits: [NSNumber numberWithInt:units]];
}

//----------------------------------------------------------------------------------------
//  validateMenuItem:
//
//      validate menu items.
//
//  validateMenuItem: NSMenuItem* anItem   -> the menu item
//
//  returns BOOL                           <- true if valid.
//----------------------------------------------------------------------------------------
- (BOOL) validateMenuItem: (NSMenuItem*) anItem
{
	SEL act = [anItem action];

	if (
			act == @selector(fixInXDirection:) || 
			act == @selector(fixInYDirection:) || 
			act == @selector(fixInTDirection:) ||
			act == @selector(copy:)
		) {
		// fixity or copy requires selected nodes.
		return _selectedNodes->Length() != 0;
	} else if (act == @selector(paste:)) {
		// paste require clipboard
		return ![[FrameClipboard sharedClipboard] empty];
	} else if (act == @selector(analyze:)) {
		// analysis requires loads
		return _structure->CanAnalyze();
	} else if (act == @selector(setItemDisplay:)) {
		// can't select result displays
		return anItem.tag < ShowDisplacement || _structure->Analyzed();
    } else if (act == @selector(showAnalysisResults:)) {
		// results view
        return _structure->Analyzed();
	} else if (act == @selector(selectLoadCase:)) {
		// and load cases.
		return anItem.tag < kMaxLoadCases || _structure->Analyzed();
	} else if (act == @selector(updateLoadCombo:)) {
		return !_structure->ActiveLoadCaseIsEditable();
	} else if (act == @selector(updateLoadCombo:)) {
		return !_structure->ActiveLoadCaseIsEditable();
	} else if (act == @selector(updateLoadCase:)) {
		return _structure->ActiveLoadCaseIsEditable();
	} else {
		return [super validateMenuItem: anItem];
	}
}

//----------------------------------------------------------------------------------------
//  enableFixityButtons
//
//      enable the fixity buttons, based on the selection.
//
//  returns nothing
//----------------------------------------------------------------------------------------
- (void) enableFixityButtons 
{
	[_fixityMatrix setEnabled:_selectedNodes->Length() != 0];
}

//----------------------------------------------------------------------------------------
//  selectionChanged
//
//      handle selection change.
//
//  returns nothing
//----------------------------------------------------------------------------------------
- (void) selectionChanged
{
	[self enableFixityButtons];
	[_elementInspectorPanel updateSelection: true];
	[_nodeInspectorPanel updateSelection: true];
	[_resultsPanel updateSelection: true];
}

//----------------------------------------------------------------------------------------
//  setActiveProperty:
//
//      set the active property.
//
//  setActiveProperty: Property p  -> 
//
//  returns nothing
//----------------------------------------------------------------------------------------
- (void) setActiveProperty: (Property) p
{
	if (!p.Empty())
		_structure->SetActiveProperty(p);
}

//----------------------------------------------------------------------------------------
//  doStitchDups:returnCode:contextInfo:
//
//      Handle duplicate nodes modal sheet callback.
//
//  doStitchDups: NSWindow * sheet     -> 
//  returnCode: int returnCode         -> 
//  contextInfo: void * contextInfo    -> 
//
//  returns nothing
//----------------------------------------------------------------------------------------
- (void)doStitchDups:(NSWindow *)sheet returnCode:(int)returnCode contextInfo:(void *)contextInfo 
{
	if (returnCode == NSAlertDefaultReturn) {
		FrameAction* action = [FrameAction create:
					   _structure->StitchNodes(
											   *_selectedNodes,
											   _graphics->WorldTolerance(6))];
//		[action setSavePasteOffset: true];
		[self performAction: action];
	}
}

//----------------------------------------------------------------------------------------
//  checkDuplicates
//
//      Check the structure for duplicates and ask the user about stitch if there are
//      duplicates.
//
//  returns nothing
//----------------------------------------------------------------------------------------
- (void) checkDuplicates {
	if (_structure->DuplicateNodes(*_selectedNodes, _graphics->WorldTolerance(6))) {
		NSBeginAlertSheet(
				NSLocalizedString(@"Combine overlapping nodes?", nil),
				NSLocalizedString(@"Combine", nil),
				NSLocalizedString(@"Cancel", nil), nil,
				[_frameView window], 
				self, 
				@selector(doStitchDups:returnCode:contextInfo:), 
				nil, 
				nil, 
				NSLocalizedString(@"Some nodes occupy the same locations. Combine them?", nil));
	}
}

//----------------------------------------------------------------------------------------
//  resetPasteOffset
//
//      Reset the paste offset to default.
//
//  returns nothing
//----------------------------------------------------------------------------------------
- (void) resetPasteOffset 
{
	_pasteOffset = WorldPoint();
	_pasteDelta = [_grid spacing];
//#if DlDebugging
//	printWorldPoint("paste offset for paste is ", _pasteOffset);
//#endif
	_updatePasteOffset = false;
}

#pragma mark -
#pragma mark =============== Selection ===============
#pragma mark -

//----------------------------------------------------------------------------------------
//  clearSelections
//
//      clear the node and element selections.
//
//  returns nothing
//----------------------------------------------------------------------------------------
- (void) clearSelections
{
//	printf("clear\n");
	[self invalSelections];
	*_selectedNodes = NodeEnumerator();
	*_selectedElements = ElementEnumerator();
	[self selectionChanged];
}

//----------------------------------------------------------------------------------------
//  selectNode:withFlags:
//
//      select a node.
//
//  selectNode: Node n             -> the node to select
//  withFlags: unsigned int flags  -> selection flags (key modifier flags)
//
//  returns nothing
//----------------------------------------------------------------------------------------
- (void) selectNode: (Node)n withFlags:(unsigned int) flags
{
	if (!IsShiftKey(flags)) {
		if (_selectedNodes->Contains(n)) 
			return;
		[self clearSelections];
	}
	
	if (!n.Empty()) {
		if (!_selectedNodes->Contains(n))
			_selectedNodes->Add(n);
		else
			_selectedNodes->Remove(n);
		[_drawing invalNode: n];
		
//		*_selectedElements = _structure->SelectElements(*_selectedNodes);
//		[_drawing invalElems: *_selectedElements];
	}
	
	[self selectionChanged];
}

//----------------------------------------------------------------------------------------
//  setSelectedNodes:
//
//      set the node selection to the specified list.
//
//  setSelectedNodes: NodeEnumerator nodes     -> the nodes to select
//
//  returns nothing
//----------------------------------------------------------------------------------------
- (void) setSelectedNodes: (NodeEnumerator) nodes
{
	[_drawing invalNodes: *_selectedNodes];
	*_selectedNodes = nodes;
//	printf("set selection to %d nodes", _selectedNodes->Length());
	[_drawing invalNodes: *_selectedNodes];
	[self selectionChanged];
}

//----------------------------------------------------------------------------------------
//  selectElement:withFlags:
//
//      select an element.
//
//  selectElement: Element n             -> the node to select
//  withFlags: unsigned int flags  -> selection flags (key modifier flags)
//
//  returns nothing
//----------------------------------------------------------------------------------------
- (void) selectElement: (Element)e withFlags:(unsigned int) flags
{
//	printf("select %f\n", e.Length());

	if (!IsShiftKey(flags)) {
		if (_selectedElements->Contains(e))
			return;
		[self clearSelections];
	}
		
	if (!e.Empty()) {
		if (!_selectedElements->Contains(e)) {
			_selectedElements->Add(e);
			if (!IsOptionKey(flags)) {
				if (_selectedNodes->Add(e.StartNode()))
					[_drawing invalNode: e.StartNode()];
				if (_selectedNodes->Add(e.EndNode()))
					[_drawing invalNode: e.EndNode()];
			}
		} else {
			_selectedElements->Remove(e);
		}
		[_drawing invalElement: e];
	}
	
	[self selectionChanged];
}


//----------------------------------------------------------------------------------------
//  invalSelections
//
//      invalidate selection.
//
//  returns nothing
//----------------------------------------------------------------------------------------
- (void)invalSelections
{
	[_drawing invalNodes: *_selectedNodes];
	[_drawing invalElems: *_selectedElements];
	[self selectionChanged];
}

//----------------------------------------------------------------------------------------
//  elementSelected:
//
//      return true if the specified element is selected.
//
//  elementSelected: DlInt32 index -> 
//
//  returns bool                   <- 
//----------------------------------------------------------------------------------------
- (bool)elementSelected: (DlInt32) index
{
	if (_selectedElements == nil)
		return false;
	return _selectedElements->Contains(_structure->GetElements().At(index));
}

//----------------------------------------------------------------------------------------
//  nodeSelected:
//
//      return true if the specified element is selected.
//
//  elementSelected: DlInt32 index ->
//
//  returns bool                   <-
//----------------------------------------------------------------------------------------
- (bool)nodeSelected: (DlInt32) index
{
	if (_selectedNodes == nil)
		return false;
	return _selectedNodes->Contains(_structure->GetNodes().At(index));
}

#pragma mark -
#pragma mark =============== Mouse Handling ===============
#pragma mark -

//----------------------------------------------------------------------------------------
//  moveSelection:
//
//      move the selected nodes.
//
//  moveSelection: const WorldPoint& offset    -> offset to move selection.
//
//  returns nothing
//----------------------------------------------------------------------------------------
- (void) moveSelection: (const WorldPoint&)offset
{
	_selectedNodes->Reset();
	while(_selectedNodes->HasMore())
	{
		Node n = _selectedNodes->Next();
		ElementEnumerator attached(_structure->AttachedElements(n));
		[_drawing invalElems: attached];
		[_drawing invalNode: n];
		WorldPoint where = n.GetCoords() + offset;
		n.SetCoords(where);
		[_drawing updateElementBounds];
		[_drawing invalNode: n];
		[_drawing invalElems: attached];
	}
}

//----------------------------------------------------------------------------------------
//  handleDragFrom:to:withFlags:andDist:
//
//      handle drag.
//
//  handleDragFrom: const WorldPoint& start    -> original point
//  to: const WorldPoint& end                  -> current point
//  withFlags: unsigned int flags              -> modifier flags
//  andDist: float maxDist                     -> maximum distance dragged while down
//
//  returns bool                               <- true if selection.
//----------------------------------------------------------------------------------------
- (bool)handleDragFrom: (const WorldPoint&)start to:(const WorldPoint&)end 
			withFlags:(unsigned int)flags andDist: (float)maxDist
{
	bool rValue = false;
//	bool shiftDown = IsShiftKey(flags);
	if (_isDrag) {
		if (maxDist > kDragPixelLimit) {
			WorldPoint newCoords([_grid snapTo: end]); 
			[self moveSelection: newCoords-_dragCurrCoords];
			_dragCurrCoords = newCoords;
		}
	} else {
		switch (_currTool) {
			case ToolSelect:
			{
				WorldRect r(start, end);
				[self invalSelections];

				if (IsOptionKey(flags)) {
					ElementEnumerator eSel = _structure->SelectElements(r);
					if (_clickElemSelection == 0) {
						*_selectedElements = eSel;
					} else {
						eSel.Reset();
						*_selectedElements = *_clickElemSelection;
						while(eSel.HasMore()) {
							Element e = eSel.Next();
							if (_clickElemSelection->Contains(e))
								_selectedElements->Remove(e);
							else
								_selectedElements->Add(e);
						}
					}
				} else {
					
					NodeEnumerator nSel = _structure->SelectNodes(r);
					if (_clickNodeSelection == 0) {
						*_selectedNodes = nSel;
					} else {
						nSel.Reset();
						*_selectedNodes = *_clickNodeSelection;
						while(nSel.HasMore()) {
							Node n = nSel.Next();
							if (_clickNodeSelection->Contains(n))
								_selectedNodes->Remove(n);
							else
								_selectedNodes->Add(n);
						}
					}
					*_selectedElements = _structure->SelectElements(*_selectedNodes);
				}
				[self invalSelections];
				rValue = true;
			}	break;
			case ToolAddNode:
				break;
			case ToolAddElement:
				break;
		}
	}
			
	[self selectionChanged];
	return rValue;
}

//----------------------------------------------------------------------------------------
//  flagsChanged:
//
//      change the add element cursor to the select cursor when the option key is
//      depressed.
//
//  flagsChanged: unsigned int flags   -> 
//
//  returns nothing
//----------------------------------------------------------------------------------------
- (void)flagsChanged:(unsigned int) flags 
{
	if (_currTool == ToolAddElement) {
		[_frameView setCursor: 
			_cursors[IsOptionKey(flags) ? ToolSelect : ToolAddElement]];
	} 
}

//----------------------------------------------------------------------------------------
//  mouseInWindow:
//
//      handle mouse click
//
//  mouseInWindow: (NSPoint)globalPoint    -> global location of click
//
//  returns bool                           <- true if not in floating window.
//----------------------------------------------------------------------------------------
- (bool)mouseInWindow: (NSPoint)globalPoint 
{
	if (NSPointInRect(globalPoint, [_toolWindow frame]))
		return false;
	
	NSWindow* panelWin = [_nodeLoadPanel window];
	if (panelWin && [panelWin isVisible] && NSPointInRect(globalPoint, [panelWin frame]))
		return false;
	
	panelWin = [_elemLoadPanel window];
	if (panelWin && [panelWin isVisible] && NSPointInRect(globalPoint, [panelWin frame]))
		return false;

	panelWin = [_propertyPanel window];
	if (panelWin && [panelWin isVisible] && NSPointInRect(globalPoint, [panelWin frame]))
		return false;

	panelWin = [_workspacePanel window];
	if (panelWin && [panelWin isVisible] && NSPointInRect(globalPoint, [panelWin frame]))
		return false;

	panelWin = [_elementInspectorPanel window];
	if (panelWin && [panelWin isVisible] && NSPointInRect(globalPoint, [panelWin frame]))
		return false;

	panelWin = [_nodeInspectorPanel window];
	if (panelWin && [panelWin isVisible] && NSPointInRect(globalPoint, [panelWin frame]))
		return false;

	return true;
}

//----------------------------------------------------------------------------------------
//  beginClick:withFlags:
//
//      handle mouse click
//
//  beginClick: const WorldPoint& point    -> location of click
//  withFlags: unsigned int flags          -> modifier flags.
//
//  returns bool                           <- continue handling
//----------------------------------------------------------------------------------------
- (bool) beginClick: (const WorldPoint&)point withFlags:(unsigned int) flags
{
//	printf("Clicked: (%f, %f)\n", point.x(), point.y());
	Node hitNode = _structure->SelectOneNode(WorldRect(point, 3.0 / _graphics->GetXScale()));
	WorldPoint snapPoint = [_grid snapTo: point];
	if (hitNode.Empty()) {
		hitNode = _structure->SelectOneNode(WorldRect(snapPoint, 3.0 / _graphics->GetXScale()));
	}

	_isDrag = false;

	_dragStartCoords = point;
	_dragCurrCoords = point;

	int activeTool = _currTool;
	if (IsOptionKey(flags) && activeTool == ToolAddElement)
		activeTool = ToolSelect;

	double elemLoc = 0;
	
	switch (activeTool) {
		case ToolSelect:
			//	if we hit a node
			if (!hitNode.Empty()) {
				//	if the shift key is down, just unselect it
				if (!IsShiftKey(flags)) {
					_dragStartCoords = hitNode.GetCoords();
					_dragCurrCoords = _dragStartCoords;
					_isDrag = true;
				}
				[self selectNode: hitNode withFlags: flags];
				return _isDrag;
			} else {
				Element hitElem = _structure->SelectOneElement(
									WorldRect(point, 3.0 / _graphics->GetXScale()), elemLoc);
				if (!hitElem.Empty()) {
					if (!IsShiftKey(flags))
						_isDrag = true;
					[self selectElement: hitElem withFlags: flags];
					return _isDrag;
				} else if (!IsShiftKey(flags)) {
					[self clearSelections];
				} else {
					//	unselect drag? save the current selection
					_clickNodeSelection = NEW NodeEnumerator(*_selectedNodes);
					_clickElemSelection = NEW ElementEnumerator(*_selectedElements);
				}
			}
			break;
		case ToolAddNode:
			if (!hitNode.Empty())
			{
				NSBeginAlertSheet(
						  NSLocalizedString(@"Error", nil),
						  NSLocalizedString(@"Ok", nil),
						  nil, nil, [_frameView window], nil, nil, nil,
						  nil, NSLocalizedString(@"A node already exists at this location", nil));
					return false;
			}
			else
			{
				// check if we hit an element
				Element hitElem = _structure->SelectOneElement(
										WorldRect(point, 5.0 / _graphics->GetXScale()), elemLoc);
				
				if (hitElem.Empty() && [_grid snapOn]) {
					hitElem = _structure->SelectOneElement(
										WorldRect(snapPoint, 2.0 / _graphics->GetXScale()), elemLoc);
				}

				if (hitElem.Empty()) {
					//	add a new node and select it. This is drag node
					[self performAction: [FrameAction createClearingSelection:
										  _structure->AddNode(snapPoint, hitNode)]];
				} else {
					// split the element
					[self performAction: [FrameAction createClearingSelection:
										  _structure->SplitElement(hitElem, hitNode, elemLoc)]];
				}
				
				_dragStartCoords = hitNode.GetCoords();
				_dragCurrCoords = _dragStartCoords;
				_isDrag = true;
			}
			break;
		case ToolAddElement:
		{
			//	if there is only one node in the selection,
			//	add a new element to it
			if (_selectedNodes->Length() == 1) {
				_selectedNodes->Reset();
				Node n1 = _selectedNodes->Next();
				Element created(0);
				if (hitNode.Empty()) {
#if DlDebugging
					printf("creating new node\n");
#endif
					if (!(snapPoint == n1.GetCoords())) {
						
						//	add a new node and select it. This is drag node
						
						// check if we hit an element
						Element hitElem = _structure->SelectOneElement(
										WorldRect(point, 5.0 / _graphics->GetXScale()), elemLoc);
						
						if (hitElem.Empty()  && [_grid snapOn]) {
							hitElem = _structure->SelectOneElement(
										WorldRect(snapPoint, 2.0 / _graphics->GetXScale()), elemLoc);
						}
						
						if (hitElem.Empty()) {
							// no element hit. Add the node.
							[self performAction: [FrameAction createClearingSelection:
												  _structure->AddNode(snapPoint, hitNode)]];
						} else {
							// element hit, split it.
							[self performAction: [FrameAction createClearingSelection:
												  _structure->SplitElement(hitElem, hitNode, elemLoc)]];
						}
						
						// and add the new element.
						[self performAction: [FrameAction create:
								_structure->AddElement(n1, hitNode, [self activeElementType], created)]];
						_dragStartCoords = snapPoint;
						_dragCurrCoords = _dragStartCoords;
						_isDrag = true;
					} else {
						// do we want to ding here?
						NSBeep();
					}
				} else {
					if (!(n1 == hitNode)) {
#if DlDebugging
						printf("using existing node\n");
#endif
						// check for existing element
						if (!_structure->DuplicateElement(n1, hitNode)) {
							
							[self performAction: [FrameAction createClearingSelection: 
									_structure->AddElement(n1, hitNode, [self activeElementType], 
										created)]];
							_dragStartCoords = hitNode.GetCoords();
							_dragCurrCoords = _dragStartCoords;
							_isDrag = true;
						} else {
							// do we want to ding here?
							NSBeep();
						}

						[self clearSelections];
						_selectedNodes->Add(hitNode);

						if (!created.Empty())
							_selectedElements->Add(created);
					}
				}

				// allow dragging the node point hit?
				if (!created.Empty()) {
					_isDrag = true;
				}
			} else if (!hitNode.Empty()) {
				[self selectNode: hitNode withFlags: 0];
				_dragStartCoords = hitNode.GetCoords();
				_dragCurrCoords = _dragStartCoords;
				_isDrag = true;
			}
		}
			break;
	}
	[self selectionChanged];
	return true;
}

//----------------------------------------------------------------------------------------
//  endClick
//
//      Handle the mouse-up event after a click.
//
//  returns nothing
//----------------------------------------------------------------------------------------
-(void)endClick
{
	if (_isDrag && !(_dragCurrCoords == _dragStartCoords)) {
		
		WorldPoint moveDist(_dragCurrCoords - _dragStartCoords);
		FrameAction* action = [FrameAction create:
				_structure->MoveNodes(*_selectedNodes, moveDist)
				withView: _frameView];
		[action setCheckDuplicates: true];
//		[action setSavePasteOffset: true];
		// we  need a moved node action here.
		[self performAction: action];

		if (_updatePasteOffset) {
#if DlDebugging
			printWorldPoint("paste offset before move is ", _pasteOffset);
			printWorldPoint("move distance is ", moveDist);
#endif
			_pasteOffset += moveDist;
			_pasteDelta = _pasteOffset;
#if DlDebugging
			printWorldPoint("paste offset after move is ", _pasteOffset);
			printWorldPoint("paste delta after move is ", _pasteDelta);
#endif
		}
	} 
//	else if (!_draggedNode.Empty()) {
//		if (!_selectedNodes->Contains(_draggedNode))
//			[self selectNode: _draggedNode withFlags: 0];
//	}

	if (_clickNodeSelection) {
		delete _clickNodeSelection;
		_clickNodeSelection = 0;
	}
	
	if (_clickElemSelection) {
		delete _clickElemSelection;
		_clickElemSelection = 0;
	}
	
//	_draggedNode = Node(0);
	_isDrag = false;
	[self selectionChanged];
}

#pragma mark -
#pragma mark =============== List Changes ===============
#pragma mark -


//----------------------------------------------------------------------------------------
//  nodeChanged:removed:
//
//      handle change event from the node list.
//
//  nodeChanged: Node* nd  -> the node
//  removed: BOOL rem      -> true if removed
//
//  returns nothing
//----------------------------------------------------------------------------------------
- (void)nodeChanged:(Node*) nd removed: (BOOL)rem
{
	[_drawing invalNode:*nd];
	
	if (rem) {
		_selectedNodes->Remove(*nd);
	} else {
		_selectedNodes->Add(*nd);
	}
}

//----------------------------------------------------------------------------------------
//  nodesChanged:removed:
//
//      handle chage event from the node list.
//
//  nodesChanged: const NodeEnumerator& nodes  -> list of nodes
//  removed: BOOL rem                          -> true if removed.
//
//  returns nothing
//----------------------------------------------------------------------------------------
- (void)nodesChanged: (const NodeEnumerator&)nodes removed: (BOOL)rem
{
	nodes.Reset();
	while(nodes.HasMore()) {
		Node nn(nodes.Next());
		[self nodeChanged: &nn removed: rem];
	}
}

//----------------------------------------------------------------------------------------
//  elemChanged:removed:
//
//      handle change event from the element list.
//
//  elemChanged: Element* el   -> element
//  removed: BOOL rem          -> true if removed
//
//  returns nothing
//----------------------------------------------------------------------------------------
- (void)elemChanged:(Element*) el removed: (BOOL)rem
{
	[_drawing invalElement:*el];
	if (rem)
		_selectedElements->Remove(*el);
	else
		_selectedElements->Add(*el);
}

//----------------------------------------------------------------------------------------
//  elemsChanged:removed:
//
//      handle change event from the element list.
//
//  elemsChanged: const ElementEnumerator& elems   -> list of elements
//  removed: BOOL rem                              -> true if removed
//
//  returns nothing
//----------------------------------------------------------------------------------------
- (void)elemsChanged: (const ElementEnumerator&)elems removed: (BOOL)rem
{
	elems.Reset();
	while(elems.HasMore()) {
		Element ee(elems.Next());
		[self elemChanged: &ee removed: rem];
	}
}

//----------------------------------------------------------------------------------------
//  nodeLoadChanged:removed:
//
//      handle change event from the node load list.
//
//  nodeLoadChanged: NodeLoad* n   -> node load
//  removed: BOOL rem              -> true if removed
//
//  returns nothing
//----------------------------------------------------------------------------------------
- (void)nodeLoadChanged: (NodeLoad*)n removed: (BOOL)rem
{
	[_nodeLoadPanel listChanged];
}

//----------------------------------------------------------------------------------------
//  nodeLoadsChanged:removed:
//
//      handle change event from the node load list.
//
//  nodeLoadsChanged: const NodeLoadEnumerator& nodes  -> list of node loads
//  removed: BOOL rem                                  -> true if removed
//
//  returns nothing
//----------------------------------------------------------------------------------------
- (void)nodeLoadsChanged: (const NodeLoadEnumerator&)nodes removed: (BOOL)rem
{
	[_nodeLoadPanel listChanged];
}

//----------------------------------------------------------------------------------------
//  elemLoadChanged:removed:
//
//      handle change event from the element load list.
//
//  elemLoadChanged: ElementLoad* n    -> element load
//  removed: BOOL rem                  -> true if removed
//
//  returns nothing
//----------------------------------------------------------------------------------------
- (void)elemLoadChanged: (ElementLoad*)n removed: (BOOL)rem
{
	[_elemLoadPanel listChanged];
}

//----------------------------------------------------------------------------------------
//  elemLoadsChanged:removed:
//
//      handle change event from the element load list.
//
//  elemLoadsChanged: const ElementLoadEnumerator& nodes   -> list of element loads
//  removed: BOOL rem                                      -> true if removed
//
//  returns nothing
//----------------------------------------------------------------------------------------
- (void)elemLoadsChanged: (const ElementLoadEnumerator&)nodes removed: (BOOL)rem
{
	[_elemLoadPanel listChanged];
}

//----------------------------------------------------------------------------------------
//  propChanged:removed:
//
//      handle change event from the properties list.
//
//  propChanged: Property* n   -> property
//  removed: BOOL rem          -> true if removed
//
//  returns nothing
//----------------------------------------------------------------------------------------
- (void)propChanged: (Property*)n removed: (BOOL)rem
{
	[_propertyPanel listChanged];
}

//----------------------------------------------------------------------------------------
//  propsChanged:removed:
//
//      handle change event from the properties list.
//
//  propsChanged: const PropertyEnumerator& nodes  -> list of properties
//  removed: BOOL rem                              -> true if removed
//
//  returns nothing
//----------------------------------------------------------------------------------------
- (void)propsChanged: (const PropertyEnumerator&)nodes removed: (BOOL)rem
{
	[_propertyPanel listChanged];
}

//- (void)updateLoadCounts
//{
//	// tell the structure to update the load counts.
//	_structure->UpdateLoadCaseCount();
//}

#pragma mark -
#pragma mark =============== Data Load/Save ===============
#pragma mark -

NSString* kDocType = @"dcs.plane-frame-data";

//----------------------------------------------------------------------------------------
//  dataOfType:error
//
//      return the data as an NSData object.
//
//  dataOfType: NSString * type  -> document type.
//	error: (NSError**)			 <- the error.
//
//  returns NSData *                           <- data
//----------------------------------------------------------------------------------------
- (NSData *)dataOfType: (NSString *)typeName
				 error: (NSError **)outError
{
	NSLog(@"Serializing document of type %@", typeName);
	if ([[typeName lowercaseString] isEqualToString: kDocType]) {
		try
		{
			FrameWriter writer(0);
			//	document data
			writer.PutString([[[_frameView window] stringWithSavedFrame] UTF8String]);
			writer.PutInt(_structure->GetActiveLoadCase());
			writer.PutInt(_drawingFlags);		
			writer.PutString([self activeElementType]);
			
			_graphics->Save(writer);
			[_grid saveToFile: writer];
            writer.PutInt(_currUnits);

			_structure->Save(writer);
			
			writer.PutInt(_currUnits);
			
			return writer.GetBytes();
		} 
		catch(DlException& ex)
		{
			NSString* msg = [NSString stringWithUTF8String: ex.what()];
			NSDictionary* userInfo = [NSDictionary dictionaryWithObject: msg
																 forKey: NSLocalizedDescriptionKey];
			*outError = [NSError errorWithDomain: NSCocoaErrorDomain
											code: NSFileReadUnknownError
										userInfo: userInfo];
		}
		
	} else {
	}
	
	return nil;
}

//----------------------------------------------------------------------------------------
//  readFromData:ofType:error
//
//      parse data from NSData into structure.
//
//  readFromData: NSData * data  -> data
//  ofType: NSString * typeName  -> document type
//	error: NSError ** outError   <- outError
//
//  returns BOOL                 <- true if success
//----------------------------------------------------------------------------------------
- (BOOL)readFromData: (NSData *)data
			  ofType: (NSString *)typeName
			   error: (NSError **)outError
{
	NSLog(@"Opening document of type %@", typeName);
	if ([[typeName lowercaseString] isEqualToString: kDocType]) {
		try
		{
			FrameReader reader(data);
			_windowFrameStr = [[NSString stringWithCString: reader.GetString().get()
												 encoding : NSUTF8StringEncoding] retain];
			LoadCase act = reader.GetInt();
			_drawingFlags = reader.GetInt();
			reader.GetString();
			_graphics = graphics::createGraphics(reader);
			_grid = [[FrameGrid createFromFile: reader] retain];
            _currUnits = reader.GetInt();
			
			_structure = NEW FrameStructure([NSLocalizedString(@"default", nil) UTF8String]);
			_structure->Load(reader);
			_structure->SetActiveLoadCase(act);
			
			if (!reader.eof()) {
				_currUnits = reader.GetInt();
			}
			
//			[self setLoadCase: currLC];
			return YES;
		}
		catch(DlException& ex)
		{
			
			NSString* msg = [NSString stringWithUTF8String: ex.what()];
			NSLog(@"exception opening document: %@", msg);

			NSDictionary* userInfo = [NSDictionary dictionaryWithObject: msg
																 forKey: NSLocalizedFailureReasonErrorKey];
			
			
			*outError = [NSError errorWithDomain:NSCocoaErrorDomain
											code:NSFileReadUnknownError
										userInfo:userInfo];
			
			return NO;

//			NSBeginAlertSheet(@"Error", @"Ok", nil, nil, [_frameView window], nil, nil, nil, 
//							  nil, @"%@",
//							  [NSString stringWithCString: ex.what()
//												encoding : NSUTF8StringEncoding]);
		} catch(StrErrCode err) {
			
			char str[5];
			str[4] = 0;
			str[3] = DlByte0(err);
			str[2] = DlByte1(err);
			str[1] = DlByte2(err);
			str[0] = DlByte3(err);
			
			NSLog(@"error (%s) opening document", str);
			
			NSDictionary* userInfo = [NSDictionary dictionaryWithObject: @"Failed to open document"
																 forKey: NSLocalizedDescriptionKey];
			
			
			*outError = [NSError errorWithDomain:NSCocoaErrorDomain
											code:NSFileReadUnknownError
										userInfo:userInfo];
			
			return NO;
		}
	}
	return NO;
}

@end
