/*+
 *
 *  RemoveElementAction.cpp
 *
 *  Copyright © 2014 David C. Salmon. All Rights Reserved.
 *
 *  Class to track the removal of an element from the structure.
 *
-*/

#include "RemoveElementAction.h"
#include "ElementImp.h"
#include "ElementList.h"

//----------------------------------------------------------------------------------------
//  RemoveElementAction::RemoveElementAction                                  constructor
//
//      construct an action to remove an element from the specified list.
//
//  ElementImp* theElem    -> 
//  ElementList* theList   -> 
//
//  returns nothing
//----------------------------------------------------------------------------------------
RemoveElementAction::RemoveElementAction(ElementImp* theElem, ElementList* theList)
	: Action("Remove Element")
	,itsElem(theElem)
	, itsList(theList)
{
}

//----------------------------------------------------------------------------------------
//  RemoveElementAction::~RemoveElementAction                                  destructor
//
//      destruct action.
//
//  returns nothing
//----------------------------------------------------------------------------------------
RemoveElementAction::~RemoveElementAction()
{
	// only delete the element if the next perform is an undo.
	if (PerformIsUndo())
		delete itsElem;
}

//----------------------------------------------------------------------------------------
//  RemoveElementAction::Redo
//
//      redo the action.
//
//  returns nothing
//----------------------------------------------------------------------------------------
void
RemoveElementAction::Redo()
{
	itsList->Remove(itsElem);
}

//----------------------------------------------------------------------------------------
//  RemoveElementAction::Undo
//
//      undo the action.
//
//  returns nothing
//----------------------------------------------------------------------------------------
void
RemoveElementAction::Undo()
{
	itsList->Add(itsElem);
}
