/*+
 *	File:		AddElementAction.cpp
 *
 *	Contains:	Action interface for adding nodes
 *
 *	Written by:	David C. Salmon
 *
 *	Copyright:	COPYRIGHT (C) 2000 By David C. Salmon.  *WORLDWIDE RIGHTS RESERVED*
 *
 *
 *	To Do:
-*/

//+--------------------------------- Includes ----------------------------------

#include "DlPlatform.h"
#include "AddElementAction.h"
#include "ElementImp.h"
#include "ElementList.h"

//+--------------------------------- Methods -----------------------------------

//----------------------------------------------------------------------------------------
//  AddElementAction::AddElementAction                                        constructor
//
//      Construct action for adding elements. The associated node action keeps track
//		of nodes added while adding the element.
//
//  ElementImp* theElem    -> the element to add
//  ElementList* theList   -> the list to add it to
//  Action* node1Action    -> the associated node action
//
//  returns nothing
//----------------------------------------------------------------------------------------
AddElementAction::AddElementAction(ElementImp* theElem, ElementList* theList, 
		Action* node1Action)
	: Action("Add Element")
	, itsList(theList)
{
	itsElems.push_back(theElem);
	if (node1Action)
		itsOtherActions.push_back(node1Action);
	//theList->Add(theElem);
}

AddElementAction::AddElementAction(ElementImp* e1, ElementImp*e2, ElementList* theList,
								   Action* rmElemAction, Action* node1Action)
	: Action("Split Element")
	, itsList(theList)
{
	itsElems.push_back(e1);
	itsElems.push_back(e2);
	
	if (node1Action)
		itsOtherActions.push_back(node1Action);
	
	itsOtherActions.push_back(rmElemAction);
}

//----------------------------------------------------------------------------------------
//  AddElementAction::~AddElementAction                                        destructor
//
//      Destructor
//
//  returns nothing
//----------------------------------------------------------------------------------------
AddElementAction::~AddElementAction()
{
	if (!PerformIsUndo()) {
		for (auto e : itsElems)
			delete e;
	}
	
	for (auto a : itsOtherActions)
		delete a;
}

//----------------------------------------------------------------------------------------
//  AddElementAction::Undo
//
//      Undo the addition of elements.
//
//  returns nothing
//----------------------------------------------------------------------------------------
void
AddElementAction::Undo()
{
	//	remove the elements
	for (auto e : itsElems)
		itsList->Remove(e);
	
	// and possible a created node.
	for (auto a : itsOtherActions)
		a->Perform();
}

//----------------------------------------------------------------------------------------
//  AddElementAction::Redo
//
//      Redo the addition of elements.
//
//  returns nothing
//----------------------------------------------------------------------------------------
void
AddElementAction::Redo()
{
	//	and if there are other actions, perform them
	for (auto a : itsOtherActions)
		a->Perform();
	
	for (auto e : itsElems)
		itsList->Add(e);
}

//	eof

