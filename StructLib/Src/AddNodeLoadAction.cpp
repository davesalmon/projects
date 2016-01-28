/*+
 *	File:		AddNodeLoadAction.cpp
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
#include "AddNodeLoadAction.h"
#include "NodeLoadImp.h"
#include "NodeLoadList.h"
#include "frame_data.h"

//+--------------------------------- Methods -----------------------------------

//----------------------------------------------------------------------------------------
//  AddNodeLoadAction::AddNodeLoadAction                                      constructor
//
//      Construct action for adding joint loads.
//
//  NodeLoadImp* theElem   -> the load
//  NodeLoadList* theList  -> the load list
//
//  returns nothing
//----------------------------------------------------------------------------------------
AddNodeLoadAction::AddNodeLoadAction(NodeLoadImp* theElem, NodeLoadList* theList)
	: Action("Add Node Load")
	, itsList(theList)
	, itsElem(theElem)
{
	//theList->Add(theElem);
}

//----------------------------------------------------------------------------------------
//  AddNodeLoadAction::~AddNodeLoadAction                                      destructor
//
//      destructor
//
//  returns nothing
//----------------------------------------------------------------------------------------
AddNodeLoadAction::~AddNodeLoadAction()
{
	if (!PerformIsUndo())
		delete itsElem;
}

//----------------------------------------------------------------------------------------
//  AddNodeLoadAction::Undo
//
//      Undo add of joint load
//
//  returns nothing
//----------------------------------------------------------------------------------------
void 
AddNodeLoadAction::Undo()
{
	//	if there is a node list, then we need to remove the nodes
	itsList->Remove(itsElem);
}

//----------------------------------------------------------------------------------------
//  AddNodeLoadAction::Redo
//
//      Redo add of joint load
//
//  returns nothing
//----------------------------------------------------------------------------------------
void 
AddNodeLoadAction::Redo()
{
	//	and if there is a node list, we have to add them back in
	itsList->Add(itsElem);
}

//----------------------------------------------------------------------------------------
//  AddNodeLoadAction::AddNodeLoadAction                                      constructor
//
//      Construct action for removing joint loads.
//
//  NodeLoadImp* theElem   -> the load
//  NodeLoadList* theList  -> the load list
//
//  returns nothing
//----------------------------------------------------------------------------------------
RemoveNodeLoadAction::RemoveNodeLoadAction(NodeLoadImp* theElem, NodeLoadList* theList)
: Action("Remove Node Load")
, itsList(theList)
, itsElem(theElem)
{
	//theList->Add(theElem);
}

//----------------------------------------------------------------------------------------
//  AddNodeLoadAction::~AddNodeLoadAction                                      destructor
//
//      destructor
//
//  returns nothing
//----------------------------------------------------------------------------------------
RemoveNodeLoadAction::~RemoveNodeLoadAction()
{
	if (PerformIsUndo())
		delete itsElem;
}

//----------------------------------------------------------------------------------------
//  AddNodeLoadAction::Undo
//
//      Undo remove of joint load
//
//  returns nothing
//----------------------------------------------------------------------------------------
void
RemoveNodeLoadAction::Undo()
{
	//	if there is a node list, then we need to remove the nodes
	itsList->Add(itsElem);
}

//----------------------------------------------------------------------------------------
//  AddNodeLoadAction::Redo
//
//      Redo remove of joint load
//
//  returns nothing
//----------------------------------------------------------------------------------------
void
RemoveNodeLoadAction::Redo()
{
	//	and if there is a node list, we have to add them back in
	itsList->Remove(itsElem);
}

//----------------------------------------------------------------------------------------
//  AssignNodeLoadAction::AssignNodeLoadAction                                constructor
//
//      Action for assigning joint loads to joint.
//
//  NodeLoadImp* theLoad           -> the load to assign.
//  const NodeEnumerator& theList  -> the nodes to assign to.
//  LoadCase lc                    -> the load case.
//
//  returns nothing
//----------------------------------------------------------------------------------------
AssignNodeLoadAction::AssignNodeLoadAction(NodeLoadImp* theLoad, const NodeEnumerator& theList, LoadCase lc)
	: Action(theLoad ? "Assign Node Load" : "Remove Node Load")
	, itsList(theList)
	, itsLoad(theLoad)
	, itsLoadCase(lc)
	, savedLoads(0)
{
	int numLoads = itsList.Length();
	NodeList* l = NodeList::GetList(itsList);
	savedLoads = (NodeLoadImp**)OPERATORNEW(numLoads * sizeof(NodeLoadImp*));
	for (int i = 0; i < numLoads; i++)
		savedLoads[i] = l->ElementAt(i)->GetLoad(lc);
}

//----------------------------------------------------------------------------------------
//  AssignNodeLoadAction::~AssignNodeLoadAction                                destructor
//
//      destructor
//
//  returns nothing
//----------------------------------------------------------------------------------------
AssignNodeLoadAction::~AssignNodeLoadAction()
{
	delete savedLoads;
}

//----------------------------------------------------------------------------------------
//  AssignNodeLoadAction::Redo
//
//      redo joint load assign
//
//  returns nothing
//----------------------------------------------------------------------------------------
void 
AssignNodeLoadAction::Redo()
{
	int numLoads = itsList.Length();
	NodeList* l = NodeList::GetList(itsList);
	for (int i = 0; i < numLoads; i++)
		l->ElementAt(i)->AssignLoad(itsLoadCase, itsLoad);
	l->BroadcastMessage(MessageNodeLoadAssignmentChanged, nullptr);
}

//----------------------------------------------------------------------------------------
//  AssignNodeLoadAction::Undo
//
//      undo joint load assign
//
//  returns nothing
//----------------------------------------------------------------------------------------
void 
AssignNodeLoadAction::Undo()
{
	int numLoads = itsList.Length();
	NodeList* l = NodeList::GetList(itsList);
	for (int i = 0; i < numLoads; i++)
		l->ElementAt(i)->AssignLoad(itsLoadCase, savedLoads[i]);
	l->BroadcastMessage(MessageNodeLoadAssignmentChanged, nullptr);
}

//----------------------------------------------------------------------------------------
//  ChangeNodeLoadAction::ChangeNodeLoadAction                                constructor
//
//      Action for changing the values of a joint load. Only one of theType or theValue
//		are changed.
//
//  NodeLoadImp* theLoad   -> the load to change
//  DlInt32 dof            -> the dof affected
//  const char* theValue   -> the new value
//  DlInt32 theType        -> the new type
//
//  returns nothing
//----------------------------------------------------------------------------------------
ChangeNodeLoadAction::ChangeNodeLoadAction(NodeLoadImp* theLoad, DlInt32 dof, 
						const char* theValue, NodeLoadType theType, frame_data* data)
	: Action("Change Node Load Value")
	, itsLoad(theLoad)
	, itsOldValue(0)
	, itsOldType(NodeLoadUndefined)
	, itsDof(dof)
	, itsIsValue(theValue != 0)
	, itsData(data)
{
	// first get the value
	if (theValue) {
		_DlAssert(theType == NodeLoadUndefined);
		itsOldValue = NodeLoad::ConvertNodeLoadValue(theLoad, dof, theValue);
	} else {
		_DlAssert(theType != NodeLoadUndefined);
		itsOldType = theType;
	}
}

//----------------------------------------------------------------------------------------
//  ChangeNodeLoadAction::Redo
//
//      redo the change.
//
//  returns nothing
//----------------------------------------------------------------------------------------
void 
ChangeNodeLoadAction::Redo() 
{
	if (itsIsValue) {
		DlFloat64 curr = itsLoad->GetValue(itsDof);
		itsLoad->SetValue(itsDof, itsOldValue);
		itsOldValue = curr;
	} else {
		NodeLoadType curr = itsLoad->GetType(itsDof);
		itsLoad->SetType(itsDof, itsOldType);
		itsOldType = curr;
	}
	
	itsData->ListenToMessage(MessageNodeLoadAssignmentChanged, nullptr);
}

//----------------------------------------------------------------------------------------
//  ChangeNodeLoadAction::Undo
//
//      undo the change.
//
//  returns nothing
//----------------------------------------------------------------------------------------
void
ChangeNodeLoadAction::Undo()
{
	Redo();
}


//	eof

