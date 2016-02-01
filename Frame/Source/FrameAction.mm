/*+
 *
 *  FrameAction.mm
 *
 *  Copyright © 2004 David C. Salmon. All Rights Reserved.
 *
 *  Encapsulate Action C++ objects in objective C class.
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

#import "FrameAction.h"

#include "DlPlatform.h"
#include "Action.h"
#include "FrameStructure.h"

@implementation FrameAction

//----------------------------------------------------------------------------------------
//  create:
//
//      Create the FrameAction object for the action.
//
//  create: Action* action -> the action to perform.
//
//  returns FrameAction*   <- self
//----------------------------------------------------------------------------------------
+ (FrameAction*)create: (Action*)action
{
	return [[[FrameAction alloc]init:action withView: NULL clearSelection: false] autorelease];
}

//----------------------------------------------------------------------------------------
//  create:withView:
//
//      Create FrameAction for the action, specifying the view. In this case, the view is
//      automatically invalidated when the action is done or undone.
//
//  create: Action* action -> the action to perform.
//  withView: NSView* vw   -> the view to update.
//
//  returns FrameAction*   <- self
//----------------------------------------------------------------------------------------
+ (FrameAction*)create: (Action*)action withView: (NSView*)vw
{
	return [[[FrameAction alloc]init:action withView: vw clearSelection: false] autorelease];
}

//----------------------------------------------------------------------------------------
//  createClearingSelection:
//
//      Create the action, clearing the selection when the action is performed.
//
//  createClearingSelection: Action* action    -> the action to perform.
//
//  returns FrameAction*                       <- self
//----------------------------------------------------------------------------------------
+ (FrameAction*)createClearingSelection: (Action*)action
{
	return [[[FrameAction alloc]init:action withView: NULL clearSelection: true] autorelease];
}

//----------------------------------------------------------------------------------------
//  createClearingSelection:withView:
//
//      Create the action. The selection is clear, and the view is invalidated when the
//      action is performed.
//
//  createClearingSelection: Action* action    -> the action to perform.
//  withView: NSView* vw                       -> the view to update.
//
//  returns FrameAction*                       <- 
//----------------------------------------------------------------------------------------
+ (FrameAction*)createClearingSelection: (Action*)action withView: (NSView*)vw
{
	return [[[FrameAction alloc]init:action withView: vw clearSelection: true]autorelease];
}

+ (FrameAction*) createUpdatingLoadCase: (Action*)action
{
	FrameAction* fa = [[[FrameAction alloc] init: action
										withView: nil
								  clearSelection: false] autorelease];
	fa->_updateLoadCase = true;
	return fa;
}

//----------------------------------------------------------------------------------------
//  init:withView:clearSelection:
//
//      Intialize the object. The view is the view to invalidate, and the selection will
//      be cleared if clear selection is true.
//
//  init: Action* action       -> the action to perform.
//  withView: NSView* vw       -> the view to update.
//  clearSelection: bool clr   -> true to clear the selection.
//
//  returns id                 <- self
//----------------------------------------------------------------------------------------
- (id)init: (Action*)action withView: (NSView*)vw  clearSelection: (bool)clr
{
	_action = action;
	_view = vw;
	_clearSelection = clr;
	_checkDuplicates = false;
//	_savePasteOffset = true;
	return self;
}

//----------------------------------------------------------------------------------------
//  dealloc
//
//      deallocate the action.
//
//  returns nothing
//----------------------------------------------------------------------------------------
- (void)dealloc
{
#if DlDebugging
	NSLog(@"destroying action %@", self);
#endif
	delete _action;
	
	_action = nullptr;
	
    [super dealloc];
}

- (void)setCheckDuplicates: (bool) doCheck {
	_checkDuplicates = doCheck;
}

//- (void)setSavePasteOffset: (bool) doSave {
//	_savePasteOffset = doSave;
//}

//----------------------------------------------------------------------------------------
//  perform
//
//      perform the action.
//
//  returns true if this was an undo
//----------------------------------------------------------------------------------------
- (bool) perform : (FrameStructure*) structure
{
	if (_action) {
		structure->ClearResults();
		bool isUndo = _action->PerformIsUndo();
		_action->Perform();
		if (_view != nil)
			[_view setNeedsDisplay:YES];
		return isUndo;
	}
	
	return false;
}

//----------------------------------------------------------------------------------------
//  canUndo
//
//      return true is this can be undone. Always returns true.
//
//  returns bool   <- true if can undo.
//----------------------------------------------------------------------------------------
- (bool) canUndo
{
	return true;
}

- (bool) checkDuplicates {
	return _checkDuplicates && _action && _action->PerformIsUndo();
}

- (bool) updateLoadCaseMenu {
	return _updateLoadCase;
}

//- (bool) savePasteOffset {
//	return _savePasteOffset;
//}

//----------------------------------------------------------------------------------------
//  clearSelection
//
//      return true if this action should clear the selection.
//
//  returns bool   <- true to clear.
//----------------------------------------------------------------------------------------
- (bool)clearSelection {
	return _clearSelection;
}

//----------------------------------------------------------------------------------------
//  title
//
//      get the action title.
//
//  returns NSString*  <- the menu title string.
//----------------------------------------------------------------------------------------
- (NSString*)title
{
	if (_action)
	{
		NSString* key = [NSString stringWithCString: _action->Title().get()
								 encoding : NSUTF8StringEncoding];
		return NSLocalizedString(key, nil);
	}
	return NSLocalizedString(@"Can't undo or redo", nil);
}

//----------------------------------------------------------------------------------------
//  description
//
//      get the action description for debugging.
//
//  returns NSString*  <- the description.
//----------------------------------------------------------------------------------------
- (NSString*) description
{
	return [NSString stringWithFormat: @"(%s) %@",
			_action->PerformIsUndo() ? "undo" : "redo", [self title]];
}

@end
