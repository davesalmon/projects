/*+
 *
 *  ObjCAction.mm
 *
 *  Copyright © 2014 David C. Salmon. All Rights Reserved.
 *
 *  wrapper code for DlAction that can be used in objective C containers.
 *
-*/
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

#import "ObjCAction.h"

#include "DlPlatform.h"
#include "DlAction.h"

@implementation ObjCAction

//----------------------------------------------------------------------------------------
//  create:
//
//      create an ObjCAction object.
//
//  create: DlAction* action   -> 
//
//  returns ObjCAction*        <- 
//----------------------------------------------------------------------------------------
+ (ObjCAction*)create:(DlAction*) action
{
	return [[[ObjCAction alloc]init:action]autorelease];
}

//----------------------------------------------------------------------------------------
//  actionString:
//
//      return a string looked up in actions.strings.
//
//  actionString: const char* key  -> the lookup key
//
//  returns NSString*              <- the string.
//----------------------------------------------------------------------------------------
+ (NSString*) actionString: (const char*) key
{
	return [[(NSString*) DlAction::CopyCFActionString(key).get() retain] autorelease];
}

//----------------------------------------------------------------------------------------
//  init:
//
//      initialize the object.
//
//  init: DlAction* action -> the underlying action.
//
//  returns id             <- self
//----------------------------------------------------------------------------------------
- (id)init:(DlAction*)action
{
	_action = action;
	return self;
}

//----------------------------------------------------------------------------------------
//  dealloc
//
//      free the object.
//
//  returns nothing
//----------------------------------------------------------------------------------------
- (void) dealloc
{
	delete _action;
    [super dealloc];
}

//----------------------------------------------------------------------------------------
//  perform
//
//      perform the action.
//
//  returns nothing
//----------------------------------------------------------------------------------------
- (void)perform
{
	if (_action) {
		if (_action->CanUndo()) {
			_action->Undo();
		} else if (_action->CanRedo()) {
			_action->Redo();
		}
	}
}

//----------------------------------------------------------------------------------------
//  canUndo
//
//      return true if the action is undoable.
//
//  returns bool   <- true if action is undoable.
//----------------------------------------------------------------------------------------
- (bool)canUndo
{
	return _action->CanUndo();
}

//----------------------------------------------------------------------------------------
//  title
//
//      return the title for the action.
//
//  returns NSString*  <- the title.
//----------------------------------------------------------------------------------------
- (NSString*) title
{
	if (_action->CanUndo() || _action->CanRedo())
	{
		return [[(NSString*)_action->CopyCFTitle().get() retain] autorelease];
	}
	else
	{
		return [[(NSString*)_action->CopyCFTitle("NoUndo").get() retain] autorelease];
	}	
}


@end



