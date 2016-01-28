/*+
 *	File:		RemoveNodeAction.cpp
 *
 *	Contains:	Action interface for removing nodes
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
#include "DlPlatform.h"
#include "RemoveNodeAction.h"
#include "NodeImp.h"
#include "NodeList.h"
#include "ElementList.h"


//----------------------------------------------------------------------------------------
//  RemoveNodeAction::RemoveNodeAction                                        constructor
//
//      construct an action to remove nodes and/or elements from the specified node
//		or element list. All elements in elemList are removed, along with all elements
//		attached to the nodes.
//
//  const NodeEnumerator& nodeList     -> the nodes to remove
//  const ElementEnumerator& elemList  -> the elements to remove
//  NodeList * theNodeList             -> the current node list.
//  ElementList * theElemList          -> the current element list.
//
//  returns nothing
//----------------------------------------------------------------------------------------
RemoveNodeAction::RemoveNodeAction(const NodeEnumerator& nodeList, 
		const ElementEnumerator& elemList,
		NodeList * theNodeList, ElementList * theElemList)
	: Action(nodeList.Length() == 0 ? "Remove Elements" :
			 elemList.Length() == 0 ? "Remove Nodes" : "Remove Selected")
	, itsNodeList(theNodeList)
	, itsElemList(theElemList)
	, itsRemovedNodes(nodeList)
	, itsRemovedElements(itsElemList->Attached(NodeList::GetList(itsRemovedNodes)))
{
	elemList.Reset();
	while(elemList.HasMore()) {
		itsRemovedElements.Add(elemList.Next());
	}
//	Redo();
}

//----------------------------------------------------------------------------------------
//  RemoveNodeAction::~RemoveNodeAction                                        destructor
//
//      destruct action
//
//  returns nothing
//----------------------------------------------------------------------------------------
RemoveNodeAction::~RemoveNodeAction()
{
	if (PerformIsUndo())
	{
		NodeList::GetList(itsRemovedNodes)->Erase();
		ElementList::GetList(itsRemovedElements)->Erase();
	}
}

//----------------------------------------------------------------------------------------
//  RemoveNodeAction::Undo
//
//      undo remove nodes
//
//  returns nothing
//----------------------------------------------------------------------------------------
void
RemoveNodeAction::Undo()
{
	itsNodeList->Add(NodeList::GetList(itsRemovedNodes));
	itsElemList->Add(ElementList::GetList(itsRemovedElements));
}

//----------------------------------------------------------------------------------------
//  RemoveNodeAction::Redo
//
//      redo remove nodes
//
//  returns nothing
//----------------------------------------------------------------------------------------
void
RemoveNodeAction::Redo()
{
	itsElemList->Remove(ElementList::GetList(itsRemovedElements));
	itsNodeList->Remove(NodeList::GetList(itsRemovedNodes));
}




