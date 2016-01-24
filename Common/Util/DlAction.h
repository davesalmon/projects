/*!
 *  \file DlAction.h
 *  \author David C. Salmon
 *	\date 5/11/2002
 *
 *  Copyright © 2002-2016 David C. Salmon. All Rights Reserved.
 *
 * Follows similar class from CodeWarrior PowerPlant.
 */
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

#ifndef _H_DlAction
#define _H_DlAction

#include "CFStringTracker.h"
#include <string>

// Base class for supporting Undo and Redo in an application. This is
//	an abstract base class for supporting undoable actions. Concrete
//	subclasses must implement the RedoSelf() and UndoSelf() methods.
//
// The default behavior of DlAction is to extract the menu string from
//	a resource file. The menu string is defined by the #resId and #resIndex
//	constructor arguments. If this is not the desired behavior, override the
// GetDescription() method.
class DlAction {
public:

	/// Destruct action.
	virtual 			~DlAction();

	/// Redo the action.
	void Redo();
	/// Undo the action.
	void Undo();

	/// return true if the action has been done.
	/// @return true if the action has been done.
	bool IsDone() const;

	/// return true if this action supports redo. The default is true. If your
	///	action does not support redo, override this to return false.
	///	@return true if the action supports redo.
	virtual Boolean CanRedo() const;
	
	/// return true if this action supports undo. The default is true. If your
	///	action does not support undo, override this to return false.
	///	@return true if the action supports undo.
	virtual Boolean CanUndo() const;
	
	/// Copy the title(s).
	/// @param name the name to lookup or null for the name of this object
	CFStringTracker CopyCFTitle(const char* name = 0) const;
	std::string CopyTitle(const char* name = 0) const;
	
	static CFStringTracker CopyCFActionString(const char* name);

protected:
	/// Construct an action.
	/// name is used to lookup the action title in actions.string
	DlAction(const char* name, bool alreadyDone = false);

	bool				_isDone;			///< Is Action done or redone?
	const char*			_name;				///< The action name.
	
	///	Redo the operations. Subclasses must override this method to perform
	/// whatever operations are necessary to redo the operation.
	virtual void		RedoSelf() = 0;
	
	///	Undo the operations. Subclasses must override this method to perform
	/// whatever operations are necessary to undo the operation.
	virtual void		UndoSelf() = 0;
private:
	DlAction(const DlAction&);
	DlAction& operator =(const DlAction&);
};


//----------------------------------------------------------------------------------------
//  DlAction::DlAction                                                 constructor inline
//
//      construct an action.
//
//  DlInt32 resID      -> the resource id (ala STR#)
//  DlInt16 resIndex   -> the resource index
//  bool alreadyDone   -> true if the tracked action has been performed.
//
//  returns nothing
//----------------------------------------------------------------------------------------
inline 
DlAction::DlAction(const char* name, bool alreadyDone)
		: _isDone(alreadyDone)
		, _name(name)
{
}

//----------------------------------------------------------------------------------------
//  DlAction::Redo                                                                 inline
//
//      redo an action.
//
//  returns nothing
//----------------------------------------------------------------------------------------
inline void 
DlAction::Redo() 
{
	if (CanRedo()) {
		RedoSelf();
		_isDone = !_isDone;
	}
}

//----------------------------------------------------------------------------------------
//  DlAction::Undo                                                                 inline
//
//      undo an action.
//
//  returns nothing
//----------------------------------------------------------------------------------------
inline void 
DlAction::Undo()
{
	if (CanUndo()) {
		UndoSelf();
		_isDone = !_isDone;
	}
}

//----------------------------------------------------------------------------------------
//  DlAction::IsDone                                                               inline
//
//      return true if the action has been performed.
//
//  returns bool   <- true if the action has been performed.
//----------------------------------------------------------------------------------------
inline bool 
DlAction::IsDone() const {
	return _isDone;	
}

#endif


