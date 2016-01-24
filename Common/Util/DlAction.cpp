/*+
 *  DlAction.cpp
 *
 *  Created by David Salmon on Sat May 11 2002.
 *  Copyright © 2002-2016 David C. Salmon. All Rights Reserved.
 *
 *  Class to track action state.
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

#if !defined(__PowerPlant__)

#include "DlPlatform.h"
#include "DlAction.h"
#include "DlCFUtilities.h"

//----------------------------------------------------------------------------------------
//  DlAction::~DlAction                                                        destructor
//
//      clean up
//
//  returns nothing
//----------------------------------------------------------------------------------------
DlAction::~DlAction()
{
}

//----------------------------------------------------------------------------------------
//  DlAction::CanRedo
//
//      return true if the action can be redone.
//
//  returns Boolean    <- true if redo possible.
//----------------------------------------------------------------------------------------
Boolean
DlAction::CanRedo() const
{
	return !IsDone();
}

//----------------------------------------------------------------------------------------
//  DlAction::CanUndo
//
//      return true if the action can be undone.
//
//  returns Boolean    <- true if undo possible.
//----------------------------------------------------------------------------------------
Boolean
DlAction::CanUndo() const
{
	return IsDone();
}

//----------------------------------------------------------------------------------------
//  DlAction::CopyCFActionString
//
//      copy the specified action string from actions.strings and return a tracked
//      reference to it.
//
//  const char* name           -> the key
//
//  returns CFStringTracker    <- the corresponding string.
//----------------------------------------------------------------------------------------
CFStringTracker
DlAction::CopyCFActionString(const char* name)
{
	return CFStringTracker(name, CFSTR("actions"));
}

//----------------------------------------------------------------------------------------
//  DlAction::CopyCFTitle
//
//      return the value from action.strings for either the key passed in or the action
//      name if the key is null.
//
//  const char* name           -> the key
//
//  returns CFStringTracker    <- the corresponding string.
//----------------------------------------------------------------------------------------
CFStringTracker 
DlAction::CopyCFTitle(const char* name) const
{
	if (name == 0)
		name = _name;
	return CFStringTracker(name, CFSTR("actions"));
}

//----------------------------------------------------------------------------------------
//  DlAction::CopyTitle
//
//      similar to CopyCFTitle except that a std:string is returned.
//
//  const char* name       -> the key
//
//  returns std::string    <- the corresponding string.
//----------------------------------------------------------------------------------------
std::string 
DlAction::CopyTitle(const char* name) const
{
	CFStringTracker val(CopyCFTitle(name));
	return GetCFStringUTF8Value(val.get());
}

//----------------------------------------------------------------------------------------
//  DlAction::GetDescription
//
//      return the title for the action.
//
//  Str255 outRedoString   <-> redo string
//  Str255 outUndoString   <-> undo string
//
//  returns nothing
//----------------------------------------------------------------------------------------
//void
//DlAction::GetDescription(Str255 outRedoString, Str255 outUndoString) const
//{
//	::GetIndString(outRedoString, _stringResID, _stringIndex);
//	::GetIndString(outUndoString, (SInt16) (_stringResID + 1), _stringIndex);
//}

#endif


