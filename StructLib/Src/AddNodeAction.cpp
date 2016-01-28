/*+
 *	File:		AddNodeAction.cpp
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
#include "AddNodeAction.h"
#include "NodeImp.h"
#include "NodeList.h"

//+--------------------------------- Methods -----------------------------------

//----------------------------------------------------------------------------------------
//  AddNodeAction::AddNodeAction                                              constructor
//
//      Construct action for adding nodes.
//
//  NodeImp* theNode   -> the node to add.
//  NodeList* theList  -> the list of add to.
//
//  returns nothing
//----------------------------------------------------------------------------------------
AddNodeAction::AddNodeAction(NodeImp* theNode, NodeList* theList)
	: Action("Add Node")
	, itsList(theList)
	, itsNode(theNode)
{
//	theList->Add(theNode);
}

//----------------------------------------------------------------------------------------
//  AddNodeAction::~AddNodeAction                                              destructor
//
//      Destructor.
//
//  returns nothing
//----------------------------------------------------------------------------------------
AddNodeAction::~AddNodeAction()
{
	if (!PerformIsUndo())
		delete itsNode;
}

//----------------------------------------------------------------------------------------
//  AddNodeAction::Undo
//
//      Undo the addition of a node.
//
//  returns nothing
//----------------------------------------------------------------------------------------
void 
AddNodeAction::Undo()
{
	itsList->Remove(itsNode);
}

//----------------------------------------------------------------------------------------
//  AddNodeAction::Redo
//
//      Redo the addition of a node.
//
//  returns nothing
//----------------------------------------------------------------------------------------
void 
AddNodeAction::Redo()
{
	itsList->Add(itsNode);
}

//----------------------------------------------------------------------------------------
//  SetRestraintsAction::SetRestraintsAction                                  constructor
//
//      Construct action for setting nodal restraints.
//
//  const NodeEnumerator& theList  -> the list of nodes.
//  DlUInt32 restCode              -> the restraint to set.
//
//  returns nothing
//----------------------------------------------------------------------------------------
SetRestraintsAction::SetRestraintsAction(const NodeEnumerator& theList, DlUInt32 restCode)
	: Action("Set Node Restraint")
	, itsList(theList)
	, itsRestCode(restCode)
	, itsOldCodes(0)
{
}

//----------------------------------------------------------------------------------------
//  SetRestraintsAction::~SetRestraintsAction                                  destructor
//
//      Destructor.
//
//  returns nothing
//----------------------------------------------------------------------------------------
SetRestraintsAction::~SetRestraintsAction() 
{
	delete itsOldCodes;
}

//----------------------------------------------------------------------------------------
//  SetRestraintsAction::Redo
//
//      Redo the constraints.
//
//  returns nothing
//----------------------------------------------------------------------------------------
void
SetRestraintsAction::Redo()
{
	int numNodes = itsList.Length();
	NodeList* l = NodeList::GetList(itsList);
	
	if (itsOldCodes == 0) {
		itsOldCodes = (DlUInt8*)OPERATORNEW(numNodes);
	}
	
	for (int i = 0; i < numNodes; i++)
	{
		NodeImp* n = l->ElementAt(i);
		itsOldCodes[i] = n->SetRestraint(itsRestCode);
	}
}

//----------------------------------------------------------------------------------------
//  SetRestraintsAction::Undo
//
//      Undo the constraints.
//
//  returns nothing
//----------------------------------------------------------------------------------------
void
SetRestraintsAction::Undo()
{
	int numNodes = itsList.Length();
	NodeList* l = NodeList::GetList(itsList);

	for (int i = 0; i < numNodes; i++)
	{
		NodeImp* n = l->ElementAt(i);
		n->SetRestraint(itsOldCodes[i]);
	}
}

//----------------------------------------------------------------------------------------
//  ToggleRestraintsAction::ToggleRestraintsAction                            constructor
//
//      Construct action for toggling restraints.
//
//  const NodeEnumerator& theList  -> the list of nodes.
//  int whichDof                   -> the dof to toggle between fixed and free.
//
//  returns nothing
//----------------------------------------------------------------------------------------
ToggleRestraintsAction::ToggleRestraintsAction(const NodeEnumerator& theList, int whichDof)
	: Action("Toggle Node Restraint")
	, itsList(theList)
	, itsDof(whichDof)
{
}

//----------------------------------------------------------------------------------------
//  ToggleRestraintsAction::Redo
//
//      Redo action.
//
//  returns nothing
//----------------------------------------------------------------------------------------
void
ToggleRestraintsAction::Redo()
{
	int numNodes = itsList.Length();
	NodeList* l = NodeList::GetList(itsList);

	for (int i = 0; i < numNodes; i++)
	{
		NodeImp* n = l->ElementAt(i);
		n->SetRestrained(itsDof, !n->GetRestrained(itsDof));
	}
}

//----------------------------------------------------------------------------------------
//  ToggleRestraintsAction::Undo
//
//      Undo action.
//
//  returns nothing
//----------------------------------------------------------------------------------------
void
ToggleRestraintsAction::Undo()
{
	Redo();
}


//	eof

