//
//  PasteNewStructureAction.cpp
//  StructLib
//
//	Paste the given nodes and elements into a different structure.
//
//	Copyright (c) 2004, David C. Salmon. All rights reserved.
//

#include "DlPlatform.h"
#include "PasteNewStructureAction.h"
#include "frame_data.h"
#include "StitchAction.h"

PasteNewStructureAction::PasteNewStructureAction(const frame_data* src, frame_data* dest,
										   const WorldPoint& ofs)
		: Action("Paste")
		, _dest(dest)
{
	NodeCloneMap	theClonedNodes;

	_propsCount = dest->Properties().Length();
	_nodeLoadCount = dest->NodeLoads().Length();
	_elemLoadCount = dest->ElementLoads().Length();
	
	src->Nodes().Clone(NodeList::GetList(_pastedNodes),
					   &dest->NodeLoads(),
					   theClonedNodes);
	src->Elements().Clone(ElementList::GetList(_pastedElements), theClonedNodes,
						  &dest->Properties(),
						  &dest->ElementLoads(),
						  NodeList::GetList(_pastedNodes));

	while(_pastedNodes.HasMore()) {
		NodeImp* n = _pastedNodes.Next();
		n->SetCoords(n->GetCoords() + ofs);
	}

	_pastedNodes.Reset();
	
	// and grab any created properties
	for (auto i = _propsCount; i < dest->Properties().Length(); i++) {
		_addedProps.Add(dest->Properties().ElementAt(i));
	}
	
	// or created loads
	for (auto i = _nodeLoadCount; i < dest->NodeLoads().Length(); i++) {
		_addedNodeLoads.Add(dest->NodeLoads().ElementAt(i));
	}

	// or created element loads
	for (auto i = _elemLoadCount; i < dest->ElementLoads().Length(); i++) {
		_addedElementLoads.Add(dest->ElementLoads().ElementAt(i));
	}

}

PasteNewStructureAction::~PasteNewStructureAction()
{
	if (!PerformIsUndo()) {
		//	we cloned these and they are not part of the
		//	structure.
		ElemLoadList::GetList(_addedElementLoads)->Erase();
		NodeLoadList::GetList(_addedNodeLoads)->Erase();
		ElementList::GetList(_pastedElements)->Erase();
		
		PropertyList::GetList(_addedProps)->Erase();
		NodeList::GetList(_pastedNodes)->Erase();
	}

	//	delete _stitcher;
}

void
PasteNewStructureAction::Redo()
{
	_dest->Nodes().Add(NodeList::GetList(_pastedNodes));
	_dest->Properties().Add(PropertyList::GetList(_addedProps));
	_dest->Elements().Add(ElementList::GetList(_pastedElements));

	_dest->NodeLoads().Add(NodeLoadList::GetList(_addedNodeLoads));
	_dest->ElementLoads().Add(ElemLoadList::GetList(_addedElementLoads));
}

void
PasteNewStructureAction::Undo()
{
	_dest->Properties().Remove(PropertyList::GetList(_addedProps));
	_dest->Nodes().Remove(NodeList::GetList(_pastedNodes));
	_dest->Elements().Remove(ElementList::GetList(_pastedElements));

	_dest->NodeLoads().Remove(NodeLoadList::GetList(_addedNodeLoads));
	_dest->ElementLoads().Remove(ElemLoadList::GetList(_addedElementLoads));
}


