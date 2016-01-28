//+
//	PasteSameStructureAction.cpp
//
//	Paste the given nodes and elements into the structure.
//
//	Copyright (c) 2004, David C. Salmon. All rights reserved.
//-

#include "DlPlatform.h"
#include "PasteSameStructureAction.h"
#include "frame_data.h"
#include "StitchAction.h"


PasteSameStructureAction::PasteSameStructureAction(const frame_data* src, frame_data* dest,
	const WorldPoint& ofs) 
	: Action("Paste")
	, _dest(dest) 
{
	NodeCloneMap	theClonedNodes;
	
	src->Nodes().ShallowClone(NodeList::GetList(_pastedNodes),
					   theClonedNodes);

	src->Elements().ShallowClone(ElementList::GetList(_pastedElements), theClonedNodes,
						NodeList::GetList(_pastedNodes));

	while(_pastedNodes.HasMore()) {
		NodeImp* n = _pastedNodes.Next();
		n->SetCoords(n->GetCoords() + ofs);
	}
	
	_pastedNodes.Reset();
}

PasteSameStructureAction::~PasteSameStructureAction()
{
	if (!PerformIsUndo()) {
		//	we cloned these and they are not part of the 
		//	structure.
		ElementList::GetList(_pastedElements)->Erase();
		NodeList::GetList(_pastedNodes)->Erase();
	}	
}

void 
PasteSameStructureAction::Redo()
{
	_dest->Nodes().Add(NodeList::GetList(_pastedNodes));
	_dest->Elements().Add(ElementList::GetList(_pastedElements));
}

void
PasteSameStructureAction::Undo()
{
	_dest->Nodes().Remove(NodeList::GetList(_pastedNodes));
	_dest->Elements().Remove(ElementList::GetList(_pastedElements));
}

 
