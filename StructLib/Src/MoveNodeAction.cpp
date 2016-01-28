/*+
 *	File:		MoveNodeAction.cpp
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
#include "MoveNodeAction.h"
#include "NodeImp.h"

//---------------------------------- Methods -----------------------------------

MoveNodeAction::MoveNodeAction(const NodeEnumerator& nodes, const WorldPoint& offset)
	: Action("Move Nodes")
	, _list(nodes)
	, _offset(offset)
{
	moveBy(-offset);
//	theList->Add(theNode);
}

MoveNodeAction::~MoveNodeAction()
{
}

void MoveNodeAction::Undo()
{
	moveBy(-_offset);
}


void MoveNodeAction::Redo()
{
	moveBy(_offset);
}

void
MoveNodeAction::moveBy(const WorldPoint& offset)
{
	_list.Reset();
	while (_list.HasMore()) {
		NodeImp* n = _list.Next();
		n->SetCoords(n->GetCoords() + offset);
	}
}

/*
private:
	NodePtrList* itsList;
	NodeImp* itsNode; 
};
*/

//	eof

