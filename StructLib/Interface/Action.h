/*+
 *	File:		Action.h
 *
 *	Contains:	Interface for actions
 *
 *	Written by:	David C. Salmon
 *
 *	Copyright:	COPYRIGHT (C) 2000 By David C. Salmon.  *WORLDWIDE RIGHTS RESERVED*
 *
 *
 *	To Do:
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

#ifndef _H_Action
#define _H_Action

#include "DlString.h"

class Action
{
public:
	virtual 			~Action() = 0;
	
	void 				Perform();
	
	//	return the title
	const DlString&		Title() const;
	
	//	return true if Perform() will undo the operation.
	bool 		 		PerformIsUndo() const;
	
	//	set the title from a constant string
	void				SetTitle(const char* title);
	
	//	set the title
//	void				SetTitle(const DlString& title);

protected:

	Action(const char* title = 0);
	virtual void Redo() = 0;
	virtual void Undo() = 0;

	bool 		performed;
	DlString	title;
};

//----------------------------------------------------------------------------------------
//  Action::~Action                                                     destructor inline
//
//      destruct action.
//
//  returns nothing
//----------------------------------------------------------------------------------------
inline
Action::~Action()
{
}

//----------------------------------------------------------------------------------------
//  Action::Action                                                     constructor inline
//
//      construct action.
//
//  const char* s  -> the title for the action.
//
//  returns nothing
//----------------------------------------------------------------------------------------
inline 
Action::Action(const char* s)
		: title(s ? s : ""), performed(false) 
{
}

//----------------------------------------------------------------------------------------
//  Action::Perform                                                                inline
//
//      perform an action.
//
//  returns nothing
//----------------------------------------------------------------------------------------
inline void 
Action::Perform()
{
	if (!performed)
		Redo();
	else
		Undo();
		
	performed = !performed;
}

//----------------------------------------------------------------------------------------
//  Action::Title                                                                  inline
//
//      return the title for the action.
//
//  returns const DlString&    <- the action title.
//----------------------------------------------------------------------------------------
inline const DlString& 
Action::Title() const 
{
	return title;
}

//----------------------------------------------------------------------------------------
//  Action::PerformIsUndo                                                          inline
//
//      return true if the perform operation is an undo.
//
//  returns bool   <- true if undo.
//----------------------------------------------------------------------------------------
inline bool 
Action::PerformIsUndo() const 
{
	return performed;
}
	
//----------------------------------------------------------------------------------------
//  Action::SetTitle                                                               inline
//
//      set the title for the action.
//
//  const char* titleString    -> the new title.
//
//  returns nothing
//----------------------------------------------------------------------------------------
inline void 
Action::SetTitle(const char* titleString)
{
	title = titleString;
}

#endif