/*+
 *	File:		AddElemLoadAction.cpp
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

//---------------------------------- Includes ----------------------------------

#include "DlPlatform.h"
#include "AddElemLoadAction.h"
#include "ElemLoadImp.h"
#include "ElemLoadList.h"
#include "frame_data.h"

//---------------------------------- Methods -----------------------------------

//----------------------------------------------------------------------------------------
//  AddElemLoadAction::AddElemLoadAction                                      constructor
//
//      Construct action for adding element loads.
//
//  ElemLoadImp* theElem   -> the element load to add
//  ElemLoadList* theList  -> the load list.
//
//  returns nothing
//----------------------------------------------------------------------------------------
AddElemLoadAction::AddElemLoadAction(ElemLoadImp* theElem, ElemLoadList* theList)
	: Action("Add Element Load")
	, itsList(theList)
	, itsElem(theElem)
{
	//theList->Add(theElem);
}

//----------------------------------------------------------------------------------------
//  AddElemLoadAction::~AddElemLoadAction                                      destructor
//
//      destruct action for adding element loads.
//
//----------------------------------------------------------------------------------------
AddElemLoadAction::~AddElemLoadAction()
{
	if (!PerformIsUndo())
		delete itsElem;
}

//----------------------------------------------------------------------------------------
//  AddElemLoadAction::Undo
//
//      Undo action for adding element loads.
//
//  returns nothing
//----------------------------------------------------------------------------------------
void 
AddElemLoadAction::Undo()
{
	//	if there is a node list, then we need to remove the nodes
	itsList->Remove(itsElem);
}

//----------------------------------------------------------------------------------------
//  AddElemLoadAction::Redo
//
//      Redo action for adding element loads.
//
//  returns nothing
//----------------------------------------------------------------------------------------
void 
AddElemLoadAction::Redo()
{
	//	and if there is a node list, we have to add them back in
	itsList->Add(itsElem);
}

//----------------------------------------------------------------------------------------
//  RemoveElemLoadAction::RemoveElemLoadAction                                constructor
//
//      Construct action for removing element loads.
//
//  ElemLoadImp* theElem   -> the element load to remove
//  ElemLoadList* theList  -> the load list.
//
//  returns nothing
//----------------------------------------------------------------------------------------
RemoveElemLoadAction::RemoveElemLoadAction(ElemLoadImp* theElem, ElemLoadList* theList)
	: Action("Delete Element Load")
	, itsList(theList)
	, itsElem(theElem)
{
}
	
//----------------------------------------------------------------------------------------
//  RemoveElemLoadAction::~RemoveElemLoadAction                                destructor
//
//      destruct action for removing element loads.
//
//----------------------------------------------------------------------------------------
RemoveElemLoadAction::~RemoveElemLoadAction()
{
	if (PerformIsUndo())
		delete itsElem;
}
	
//----------------------------------------------------------------------------------------
//  AddElemLoadAction::Redo
//
//      Redo action for removing element loads.
//
//  returns nothing
//----------------------------------------------------------------------------------------
void
RemoveElemLoadAction::Redo()
{
	//	remove the load from the list
	itsList->Remove(itsElem);
}

//----------------------------------------------------------------------------------------
//  AddElemLoadAction::Undo
//
//      Undo action for removing element loads.
//
//  returns nothing
//----------------------------------------------------------------------------------------
void
RemoveElemLoadAction::Undo()
{
	//	add the load back into the list
	itsList->Add(itsElem);
}
	
//----------------------------------------------------------------------------------------
//  AssignElemLoadAction::AssignElemLoadAction                                constructor
//
//      Construct action for assigning element loads.
//
//  ElemLoadImp* theLoad             -> the load
//  const ElementEnumerator& theList -> the list
//  LoadCase lc						 -> the load case
//
//  returns nothing
//----------------------------------------------------------------------------------------
AssignElemLoadAction::AssignElemLoadAction(ElemLoadImp* theLoad, const ElementEnumerator& theList, LoadCase lc)
	: Action(theLoad == 0 ? "Assign Element Load" : "Remove Element Load")
	, itsList(theList)
	, itsLoad(theLoad)
	, itsLoadCase(lc)
	, savedLoads(0)
{
	int numLoads = itsList.Length();
	ElementList* l = ElementList::GetList(itsList);
	savedLoads = (ElemLoadImp**)OPERATORNEW(numLoads * sizeof(ElemLoadImp*));
	for (int i = 0; i < numLoads; i++)
		savedLoads[i] = l->ElementAt(i)->GetLoad(lc);
}

//----------------------------------------------------------------------------------------
//  AssignElemLoadAction::~AssignElemLoadAction
//
//      destruct action
//
//----------------------------------------------------------------------------------------
AssignElemLoadAction::~AssignElemLoadAction()
{
	delete savedLoads;
}

//----------------------------------------------------------------------------------------
//  AssignElemLoadAction::Redo
//
//      Redo action for assigning element loads.
//
//  returns nothing
//----------------------------------------------------------------------------------------
void 
AssignElemLoadAction::Redo()
{
	int numLoads = itsList.Length();
	ElementList* l = ElementList::GetList(itsList);
	for (int i = 0; i < numLoads; i++)
		l->ElementAt(i)->AssignLoad(itsLoadCase, itsLoad);
	
	l->BroadcastMessage(MessageElemLoadAssignmentChanged, nullptr);
}

//----------------------------------------------------------------------------------------
//  AssignElemLoadAction::Undo
//
//      Undo action for assigning element loads.
//
//  returns nothing
//----------------------------------------------------------------------------------------
void 
AssignElemLoadAction::Undo()
{
	int numLoads = itsList.Length();
	ElementList* l = ElementList::GetList(itsList);
	for (int i = 0; i < numLoads; i++)
		l->ElementAt(i)->AssignLoad(itsLoadCase, savedLoads[i]);
	l->BroadcastMessage(MessageElemLoadAssignmentChanged, nullptr);
}

//----------------------------------------------------------------------------------------
//  ChangeElemLoadAction::ChangeElemLoadAction                                constructor
//
//      action for changing element loads.
//
//  ElemLoadImp* theLoad   -> the load to change
//  DlInt32 dof            -> the component
//  const char* theValue   -> the new value
//
//  returns nothing
//----------------------------------------------------------------------------------------
ChangeElemLoadAction::ChangeElemLoadAction(ElemLoadImp* theLoad, ElementLoadType dof,
						const char* theValue, frame_data* data)
	: Action("Change Element Load Value")
	, itsLoad(theLoad)
	, itsOldValue(0)
	, itsType(dof)
	, itsData(data)
{
	// first get the value
	itsOldValue = ElementLoad::ConvertElemLoadValue(theLoad, dof, theValue);
}

//----------------------------------------------------------------------------------------
//  ChangeElemLoadAction::Redo
//
//      Redo the change of load
//
//  returns nothing
//----------------------------------------------------------------------------------------
void 
ChangeElemLoadAction::Redo() 
{
	DlFloat64 curr = itsLoad->GetValue(itsType);
	itsLoad->SetValue(itsType, itsOldValue);
	itsOldValue = curr;
	itsData->ListenToMessage(MessageElemLoadAssignmentChanged, nullptr);
}

//----------------------------------------------------------------------------------------
//  ChangeElemLoadAction::Undo
//
//      Undo the change of load.
//
//  returns nothing
//----------------------------------------------------------------------------------------
void
ChangeElemLoadAction::Undo()
{
	Redo();
}


//	eof

